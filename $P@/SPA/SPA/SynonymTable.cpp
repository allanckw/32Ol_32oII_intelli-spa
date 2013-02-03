#pragma once
#include "SynonymTable.h"
#include "SPAException.h"

/**
* This method Returns the number of synonyms in the SynonymTable
* @return the number of synonyms
*/
int SynonymTable::getSize() const
{
	return synName.size();
}

/**
* This method  Inserts the synonym in the SynonymTable if it is not already present.
* @param name name of synonym
* @param type type of variable
*/
void SynonymTable::insert(const string& name, RulesOfEngagement::QueryVar type)
{
	if (stringToIndex.count(name) > 0)
		return;
		//throw new SPAException("Double declaration of synonym");

	int index = synName.size();

	synName.push_back(name);
	synType.push_back(type);
	selected.push_back(false);
	synClassIndex.push_back(-1);
	synAttributesSpecific.push_back(unordered_map<string, string>());
	synAttributesGeneric.push_back(unordered_map<string, unordered_map<
		RulesOfEngagement::QueryVar, string>>());
	synSelfReference.push_back(unordered_set<RulesOfEngagement::QueryRelations>());
	synRelGenericFirst.push_back(unordered_set<RulesOfEngagement::QueryRelations>());
	synRelSpecificFirst.push_back(vector<pair<RulesOfEngagement::QueryRelations, string>>());
	synRelGenericSecond.push_back(unordered_set<RulesOfEngagement::QueryRelations>());
	synRelSpecificSecond.push_back(vector<pair<RulesOfEngagement::QueryRelations, string>>());

	stringToIndex[name] = index;
	if (typeToIndices.count(type) > 0)
		typeToIndices[type].push_back(index);
	else {
		vector<int> temp;
		temp.push_back(index);
		typeToIndices[type] = temp;
	}
}

/**
* This method Returns a vector of all synonyms in SynonymTable.
* @return the vector
*/
vector<string> SynonymTable::getAllNames() const
{
	return synName;
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
		ans.push_back(synName[*it]);
	return ans;
}

/**
* This method Returns the type of the given synonym.
* @param name name of synonym
* @return the type
*/
RulesOfEngagement::QueryVar SynonymTable::getType(const string& name) const
{
	return synType.at(stringToIndex.at(name));
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
	selected.at(stringToIndex.at(name)) = true;
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
		ans.push_back(synName[*it]);
	return ans;
}

/**
* This method Returns true if the given synonym is selected for.
* @param name name of synonym
* @return true if the synonym is selected for, and false otherwise.
*/
bool SynonymTable::isSelected(const string& name) const
{
	return selected.at(stringToIndex.at(name));
}

/**
* This method Sets the partition that the given synonym is in.
* @param name name of synonym
* @param classIndex index of the partition
*/
void SynonymTable::putIntoClass(const string& name, int classIndex)
{
	synClassIndex.at(stringToIndex.at(name)) = classIndex;
}

/**
* This method Returns the partition that the given synonym is in.
* @param name name of synonym
* @return the partition
*/
int SynonymTable::getClass(const string& name) const
{
	return synClassIndex.at(stringToIndex.at(name));
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
		synType.at(stringToIndex.at(name))].count(condition) == 0)
		throw new SPAException("Synonym " + name + " has no such condition " + condition);
	if (synAttributesSpecific.at(stringToIndex.at(name)).count(condition) > 0)
		return synAttributesSpecific.at(stringToIndex.at(name))[condition] == attribute;
	synAttributesSpecific.at(stringToIndex.at(name)).insert(
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
	return synAttributesSpecific.at(stringToIndex.at(name));
}

/**
* This method Sets a generic condition on the synonym. It must be of the form name.ownAttribute = type.attribute
* @param name name of own synonym
* @param ownAttribute name of own attribute
* @param otherVariable name of other synonym
* @param otherAttribute name of other attribute
*/
void SynonymTable::setGenericAttribute(const string& name, const string& ownAttribute,
	RulesOfEngagement::QueryVar otherVariable, const string& otherAttribute)
{
	//if (RulesOfEngagement::allowableConditions[otherVariable].count(otherAttribute) == 0)
		//throw new SPAException(variable + " has no such condition " + otherAttribute);
	synAttributesGeneric.at(stringToIndex.at(name))[ownAttribute].insert(
		pair<RulesOfEngagement::QueryVar, string>(otherVariable, otherAttribute));
}

/**
* This method Returns all generic conditions on the synonym.
* @param name name of own synonym
* @return an unordered map containing all the generic conditions
*/
unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>
	SynonymTable::getAllGenericAttributes(const string& name) const
{
	return synAttributesGeneric.at(stringToIndex.at(name));
}

/**
* This method Sets a self reference condition on the synonym. It must be of the form relation(name, name).
* @param name name of own synonym
* @param relation type of variable
*/
void SynonymTable::setSelfReference(const string& name,
	RulesOfEngagement::QueryRelations relation)
{
	synSelfReference.at(stringToIndex.at(name)).insert(relation);
}

/**
* This method Returns all self references on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the self references
*/
unordered_set<RulesOfEngagement::QueryRelations>
	SynonymTable::getAllSelfReferences(const string& name) const
{
	return synSelfReference.at(stringToIndex.at(name));
}

/**
* This method Sets a generic condition on the synonym (first argument). It must be of the form relation(name, _).
* @param name name of own synonym
* @param relation type of variable
*/
void SynonymTable::setFirstGeneric(const string& name,
	RulesOfEngagement::QueryRelations relation)
{
	synRelGenericFirst.at(stringToIndex.at(name)).insert(relation);
}

/**
* This method Returns all generic conditions (synonym is first argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the generic conditions (synonym is first argument)
*/
unordered_set<RulesOfEngagement::QueryRelations>
	SynonymTable::getAllFirstGeneric(const string& name) const
{
	return synRelGenericFirst.at(stringToIndex.at(name));
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
	synRelSpecificFirst.at(stringToIndex.at(name)).push_back(pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

/**
* This method Returns all specific conditions (synonym is first argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the specific conditions (synonym is first argument)
*/
vector<pair<RulesOfEngagement::QueryRelations, string>>
	SynonymTable::getAllFirstSpecific(const string& name) const
{
	return synRelSpecificFirst.at(stringToIndex.at(name));
}

/**
* This method Sets a generic condition on the synonym (second argument). It must be of the form relation(_, name).
* @param name name of own synonym
* @param relation type of variable
*/
void SynonymTable::setSecondGeneric(const string& name,
	RulesOfEngagement::QueryRelations relation)
{
	synRelGenericSecond.at(stringToIndex.at(name)).insert(relation);
}

/**
* This method Returns all generic conditions (synonym is second argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the generic conditions (synonym is second argument)
*/
unordered_set<RulesOfEngagement::QueryRelations>
	SynonymTable::getAllSecondGeneric(const string& name) const
{
	return synRelGenericSecond.at(stringToIndex.at(name));
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
	synRelSpecificSecond.at(stringToIndex.at(name)).push_back(pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

/**
* This method Returns all specific conditions (synonym is second argument) on the synonym.
* @param name name of own synonym
* @return an unordered set containing all the specific conditions (synonym is second argument)
*/
vector<pair<RulesOfEngagement::QueryRelations, string>>
	SynonymTable::getAllSecondSpecific(const string& name) const
{
	return synRelSpecificSecond.at(stringToIndex.at(name));
}