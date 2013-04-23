#pragma once
#include "SynonymTable.h"
#include "SPAException.h"

/**
* This method Returns the number of synonyms in the SynonymTable
* @return the number of synonyms
*/
int SynonymTable::getSize() const
{
	return synonyms.size();
}

/**
* This method  Inserts the synonym in the SynonymTable if it is not already present.
* @param name name of synonym
* @param type type of variable
*/
void SynonymTable::insert(const string& name, const RulesOfEngagement::QueryVar type)
{
	if (stringToIndex.count(name) > 0)
		return;
		//throw new SPAException("Double declaration of synonym");

	int index = synonyms.size();
	synonyms.push_back(Synonym(name, type));
	stringToIndex[name] = index;
	typeToIndices[type].push_back(index);
}

/**
* This method Returns the synonym information stored as Synonym.
* @param name name of synonym
* @return the Synonym information
*/
Synonym SynonymTable::getSynonym(const string& name) const
{
	return synonyms.at(stringToIndex.at(name));
}

/**
* This method Returns a vector of all synonyms in SynonymTable.
* @return the vector
*/
vector<string> SynonymTable::getAllNames() const
{
	vector<string> answers;
	for (auto it = synonyms.begin(); it != synonyms.end(); ++it)
		answers.push_back(it->synName);
	return answers;
}

/**
* This method Returns true if the synonym is in SynonymTable.
* @param name name of synonym
* @return true if the synonym is in SynonymTable, and false otherwise.
*/
bool SynonymTable::isInTable(const string& name) const
{
	return (stringToIndex.count(name) > 0);
}

/**
* This method Returns a vector of all synonyms of the given type.
* @param type type of variable
* @return the vector
*/
vector<string> SynonymTable::getAllOfType(RulesOfEngagement::QueryVar type) const
{
	vector<string> ans;
	const vector<int>& indices = typeToIndices.at(type);
	for (auto it = indices.begin(); it != indices.end(); it++)
		ans.push_back(synonyms[*it].synName);
	return ans;
}

/**
* This method Returns the type of the given synonym.
* @param name name of synonym
* @return the type
*/
RulesOfEngagement::QueryVar SynonymTable::getType(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synType;
}

/**
* This method Returns the index in the SynonymTable, of the given synonym.
* @param name name of synonym
* @return the index
*/
int SynonymTable::getSynonymIndex(const string& name) const
{
	return stringToIndex.at(name);
}

/**
* This method  Sets the synonym as one that is selected for.
* @param name name of synonym
*/
void SynonymTable::setSelected(const string& name)
{
	synonyms.at(stringToIndex.at(name)).selected = true;
	selectedIndices.push_back(stringToIndex.at(name));
}

/**
* This method Returns a vector of all selected for synonyms.
* @return the vector
*/
vector<string> SynonymTable::getAllSelected() const
{
	vector<string> ans;
	for (auto it = selectedIndices.begin(); it != selectedIndices.end(); it++)
		ans.push_back(synonyms[*it].synName);
	return ans;
}

/**
* This method Returns true if the given synonym is selected for.
* @param name name of synonym
* @return true if the synonym is selected for, and false otherwise.
*/
bool SynonymTable::isSelected(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).selected;
}

/**
* This method Sets the partition that the given synonym is in.
* @param name name of synonym
* @param classIndex index of the partition
*/
void SynonymTable::putIntoClass(const string& name, int classIndex)
{
	synonyms.at(stringToIndex.at(name)).synClassIndex = classIndex;
}

/**
* This method Returns the partition that the given synonym is in.
* @param name name of synonym
* @return the partition
*/
int SynonymTable::getClass(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synClassIndex;
}

/**
* This method Sets a specific condition on the synonym. It must be of the form name.condition = attribute.
* @param name name of synonym
* @param condition name of condition
* @param attribute name of attribute
* @return true if the condition has not been set before, and false otherwise.
*/
bool SynonymTable::setSpecificAttribute(const string& name,
	const string& condition, const string& attribute)
{
	if (RulesOfEngagement::allowableConditions[
		synonyms.at(stringToIndex.at(name)).synType].count(condition) == 0)
		throw new SPAException("Synonym " + name + " has no such condition " + condition);
	if (synonyms.at(stringToIndex.at(name)).synAttributesSpecific.count(condition) > 0)
		return synonyms.at(stringToIndex.at(name)).synAttributesSpecific[condition] == attribute;
	synonyms.at(stringToIndex.at(name)).synAttributesSpecific.insert(
		pair<string, string>(condition, attribute));
	return true;
}

/**
* This method Returns all specific conditions on the synonym.
* @param name name of synonym
* @return an unordered map containing all the specific conditions
*/
unordered_map<string, string> SynonymTable::getAllSpecificAttributes(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synAttributesSpecific;
}

/**
* This method Sets a generic condition on the synonym. It must be of the form name.ownAttribute = type.attribute
* @param name name of own synonym
* @param ownAttribute name of own attribute
* @param otherVariable name of other synonym
* @param otherAttribute name of other attribute
*/
/*void SynonymTable::setGenericAttribute(const string& name, const string& ownAttribute,
	RulesOfEngagement::QueryVar otherVariable, const string& otherAttribute)
{
	//if (RulesOfEngagement::allowableConditions[otherVariable].count(otherAttribute) == 0)
		//throw new SPAException(variable + " has no such condition " + otherAttribute);
	synonyms.at(stringToIndex.at(name)).synAttributesGeneric[ownAttribute].insert(
		pair<RulesOfEngagement::QueryVar, string>(otherVariable, otherAttribute));
}*/

/**
* This method Returns all generic conditions on the synonym.
* @param name name of own synonym
* @return an unordered map containing all the generic conditions
*/
/*unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>
	SynonymTable::getAllGenericAttributes(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synAttributesGeneric;
}*/

/**
* This method Sets a self reference condition on the synonym. It must be of the form relation(name, name).
* @param name name of own synonym
* @param relation type of variable
*/
void SynonymTable::setSelfReference(const string& name,
	RulesOfEngagement::QueryRelations relation)
{
	synonyms.at(stringToIndex.at(name)).synSelfReference.insert(relation);
}

/**
* This method Returns all self references on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the self references
*/
unordered_set<RulesOfEngagement::QueryRelations>
	SynonymTable::getAllSelfReferences(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synSelfReference;
}

/**
* This method Sets a generic condition on the synonym (first argument). It must be of the form relation(name, _).
* @param name name of own synonym
* @param relation type of variable
*/
void SynonymTable::setFirstGeneric(const string& name,
	RulesOfEngagement::QueryRelations relation)
{
	synonyms.at(stringToIndex.at(name)).synRelGenericFirst.insert(relation);
}

/**
* This method Returns all generic conditions (synonym is first argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the generic conditions (synonym is first argument)
*/
unordered_set<RulesOfEngagement::QueryRelations>
	SynonymTable::getAllFirstGeneric(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synRelGenericFirst;
}

/**
* This method Sets a specific condition on the synonym (first argument).
* It must be of the form relation(name, specific).
* @param name name of own synonym
* @param relation type of variable
* @param specific the specific attribute
*/
void SynonymTable::setFirstSpecific(const string& name,
	RulesOfEngagement::QueryRelations relation, const string& specific)
{
	synonyms.at(stringToIndex.at(name)).synRelSpecificFirst.push_back(pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

/**
* This method Returns all specific conditions (synonym is first argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the specific conditions (synonym is first argument)
*/
vector<pair<RulesOfEngagement::QueryRelations, string>>
	SynonymTable::getAllFirstSpecific(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synRelSpecificFirst;
}

/**
* This method Sets a generic condition on the synonym (second argument). It must be of the form relation(_, name).
* @param name name of own synonym
* @param relation type of variable
*/
void SynonymTable::setSecondGeneric(const string& name,
	RulesOfEngagement::QueryRelations relation)
{
	synonyms.at(stringToIndex.at(name)).synRelGenericSecond.insert(relation);
}

/**
* This method Returns all generic conditions (synonym is second argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the generic conditions (synonym is second argument)
*/
unordered_set<RulesOfEngagement::QueryRelations>
	SynonymTable::getAllSecondGeneric(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synRelGenericSecond;
}

/**
* This method Sets a specific condition on the synonym (second argument).
* It must be of the form relation(specific, name).
* @param name name of own synonym
* @param relation type of variable
* @param specific the specific attribute
*/
void SynonymTable::setSecondSpecific(const string& name,
	RulesOfEngagement::QueryRelations relation, const string& specific)
{
	synonyms.at(stringToIndex.at(name)).synRelSpecificSecond.push_back(
		pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

/**
* This method Returns all specific conditions (synonym is second argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the specific conditions (synonym is second argument)
*/
vector<pair<RulesOfEngagement::QueryRelations, string>>
	SynonymTable::getAllSecondSpecific(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synRelSpecificSecond;
}

/**
* This method Sets a pattern involving an assignment and an expression.
* It must be of the form name(*, expression).
* @param name name of synonym
* @param expression pattern expression
*/
void SynonymTable::setPattern(const string& name, const string& expression)
{
	synonyms.at(stringToIndex.at(name)).synPattern.push_back(expression);
}

/**
* This method Returns all assignment patterns on the synonym.
* @param name name of  synonym
* @return a vector containing all the assignment patterns
*/
vector<string> SynonymTable::getAllPattern(const string& name) const
{
	return synonyms.at(stringToIndex.at(name)).synPattern;
}

void SynonymTable::setRelation(const RulesOfEngagement::Relation rel)
{
	const string& firstRel = rel.firstSynonym;
	const string& secondRel = rel.secondSynonym;
	synonyms.at(stringToIndex.at(firstRel)).relations.push_back(rel);
	synonyms.at(stringToIndex.at(secondRel)).relations.push_back(rel);
}

void SynonymTable::doneRelation(const RulesOfEngagement::Relation rel)
{
	const string& firstRel = rel.firstSynonym;
	const string& secondRel = rel.secondSynonym;
	vector<RulesOfEngagement::Relation>& relsOne =
		synonyms.at(stringToIndex.at(firstRel)).relations;
	for (auto it = relsOne.begin(); it != relsOne.end(); ++it)
		if (it->firstSynonym == firstRel &&
			it->secondSynonym == secondRel && it->type == rel.type) {
				relsOne.erase(it);
				break;
		}
	vector<RulesOfEngagement::Relation>& relsTwo =
		synonyms.at(stringToIndex.at(secondRel)).relations;
	for (auto it = relsTwo.begin(); it != relsTwo.end(); ++it)
		if (it->firstSynonym == firstRel &&
			it->secondSynonym == secondRel && it->type == rel.type) {
				relsTwo.erase(it);
				break;
		}
}

void SynonymTable::setCondition(const RulesOfEngagement::Condition cond)
{
	const string& firstRel = cond.firstRel;
	const string& secondRel = cond.secondRel;
	synonyms.at(stringToIndex.at(firstRel)).conditions.push_back(cond);
	synonyms.at(stringToIndex.at(secondRel)).conditions.push_back(cond);
}

void SynonymTable::doneCondition(const RulesOfEngagement::Condition cond)
{
	const string& firstRel = cond.firstRel;
	const string& secondRel = cond.secondRel;
	vector<RulesOfEngagement::Condition>& condsOne =
		synonyms.at(stringToIndex.at(firstRel)).conditions;
	for (auto it = condsOne.begin(); it != condsOne.end(); ++it)
		if (it->firstRel == firstRel && it->firstCondition == cond.firstCondition &&
			it->secondRel == secondRel && it->secondCondition == cond.secondCondition) {
				condsOne.erase(it);
				break;
		}
	vector<RulesOfEngagement::Condition>& condsTwo =
		synonyms.at(stringToIndex.at(secondRel)).conditions;
	for (auto it = condsTwo.begin(); it != condsTwo.end(); ++it)
		if (it->firstRel == firstRel && it->firstCondition == cond.firstCondition &&
			it->secondRel == secondRel && it->secondCondition == cond.secondCondition) {
				condsTwo.erase(it);
				break;
		}
}

bool SynonymTable::stillActive(const string& name) const
{
	const Synonym& syno = synonyms.at(stringToIndex.at(name));
	return syno.selected || !syno.synPattern.empty() ||
		!syno.relations.empty() || !syno.conditions.empty();
}

unordered_map<string, string> SynonymTable::alias()
{
	unordered_map<string, unordered_map<string, bool>> aliasMap;
	for (auto it = typeToIndices.begin(); it != typeToIndices.end(); ++it) {
		const RulesOfEngagement::QueryVar type = it->first;
		const vector<int>& potentials = it->second;

		for (size_t i = 0; i < potentials.size(); ++i) {
			const Synonym& first = synonyms[potentials[i]];
			for (size_t j = i + 1; j < potentials.size(); ++j) {
				const Synonym& second = synonyms[potentials[j]];
				if (aliasMap.count(first.synName) > 0 &&
					aliasMap[first.synName].count(second.synName) > 0)
					continue;
				bool equal = true;

				{
					const auto& firstThing = first.synAttributesSpecific;
					const auto& secondThing = second.synAttributesSpecific;
					for (auto it2 = firstThing.begin(); it2 != firstThing.end(); ++it2) {
						if (secondThing.count(it2->first) == 0 ||
							it2->second != secondThing.at(it2->first)) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
					for (auto it2 = secondThing.begin(); it2 != secondThing.end(); ++it2) {
						if (firstThing.count(it2->first) == 0 ||
							it2->second != firstThing.at(it2->first)) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
				}

				{
					const auto& firstThing = first.synSelfReference;
					const auto& secondThing = second.synSelfReference;
					for (auto it2 = firstThing.begin(); it2 != firstThing.end(); ++it2) {
						if (secondThing.count(*it2) == 0) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
					for (auto it2 = secondThing.begin(); it2 != secondThing.end(); ++it2) {
						if (firstThing.count(*it2) == 0) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
				}

				{
					const auto& firstThing = first.synRelGenericFirst;
					const auto& secondThing = second.synRelGenericFirst;
					for (auto it2 = firstThing.begin(); it2 != firstThing.end(); ++it2) {
						if (secondThing.count(*it2) == 0) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
					for (auto it2 = secondThing.begin(); it2 != secondThing.end(); ++it2) {
						if (firstThing.count(*it2) == 0) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
				}

				{
					const auto& firstThing = first.synRelSpecificFirst;
					const auto& secondThing = second.synRelSpecificFirst;
					for (auto it2 = firstThing.begin(); it2 != firstThing.end(); ++it2) {
						const pair<RulesOfEngagement::QueryRelations, string>& thepair = *it2;
						bool match = false;
						for (auto it3 = secondThing.begin(); it3 != secondThing.end(); ++it3) {
							const pair<RulesOfEngagement::QueryRelations, string>& thepair2 = *it3;
							if (thepair.first == thepair2.first && thepair.first == thepair2.first) {
								match = true;
								break;
							}
						}
						if (!match) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
					for (auto it2 = secondThing.begin(); it2 != secondThing.end(); ++it2) {
						const pair<RulesOfEngagement::QueryRelations, string>& thepair = *it2;
						bool match = false;
						for (auto it3 = firstThing.begin(); it3 != firstThing.end(); ++it3) {
							const pair<RulesOfEngagement::QueryRelations, string>& thepair2 = *it3;
							if (thepair.first == thepair2.first && thepair.first == thepair2.first) {
								match = true;
								break;
							}
						}
						if (!match) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
				}

				{
					const auto& firstThing = first.synRelGenericSecond;
					const auto& secondThing = second.synRelGenericSecond;
					for (auto it2 = firstThing.begin(); it2 != firstThing.end(); ++it2) {
						if (secondThing.count(*it2) == 0) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
					for (auto it2 = secondThing.begin(); it2 != secondThing.end(); ++it2) {
						if (firstThing.count(*it2) == 0) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
				}

				{
					const auto& firstThing = first.synRelSpecificSecond;
					const auto& secondThing = second.synRelSpecificSecond;
					for (auto it2 = firstThing.begin(); it2 != firstThing.end(); ++it2) {
						const pair<RulesOfEngagement::QueryRelations, string>& thepair = *it2;
						bool match = false;
						for (auto it3 = secondThing.begin(); it3 != secondThing.end(); ++it3) {
							const pair<RulesOfEngagement::QueryRelations, string>& thepair2 = *it3;
							if (thepair.first == thepair2.first && thepair.first == thepair2.first) {
								match = true;
								break;
							}
						}
						if (!match) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
					for (auto it2 = secondThing.begin(); it2 != secondThing.end(); ++it2) {
						const pair<RulesOfEngagement::QueryRelations, string>& thepair = *it2;
						bool match = false;
						for (auto it3 = firstThing.begin(); it3 != firstThing.end(); ++it3) {
							const pair<RulesOfEngagement::QueryRelations, string>& thepair2 = *it3;
							if (thepair.first == thepair2.first && thepair.first == thepair2.first) {
								match = true;
								break;
							}
						}
						if (!match) {
							equal = false;
							aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
							break;
						}
					}
					if (!equal)
						continue;
				}

				if (first.relations.size() != second.relations.size()) {
					equal = false;
					aliasMap[first.synName].insert(pair<string, bool>(second.synName, false));
					continue;
				}
				//equal = tryMatch(first.synName, first.relations, first.conditions, second.synName, second.relations, second.conditions)
			}
		}
	}
	return unordered_map<string, string>();
}

bool tryMatch(const string& firstName, const vector<RulesOfEngagement::Relation> firstRelations,
	const vector<RulesOfEngagement::Condition> firstConditions, const string& secondName,
	const vector<RulesOfEngagement::Relation> secondRelations,
	const vector<RulesOfEngagement::Condition> secondConditions)
{
	if (firstRelations.empty()) {
		if (firstConditions.empty()) {

		} else {

		}
	} else {

	}
	return true;
}