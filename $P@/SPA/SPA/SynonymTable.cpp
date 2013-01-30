#pragma once
#include "SynonymTable.h"
#include "SPAException.h"

int SynonymTable::size() const
{
	return synName.size();
}

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

vector<string> SynonymTable::getAllNames() const
{
	return synName;
}

bool SynonymTable::isInTable(string name) const
{
	return (stringToIndex.count(name) > 0);
}

vector<string> SynonymTable::getAllOfType(RulesOfEngagement::QueryVar type) const
{
	vector<string> ans;
	const vector<int>& indices = typeToIndices.at(type);
	for (auto it = indices.begin(); it != indices.end(); it++)
		ans.push_back(synName[*it]);
	return ans;
}

RulesOfEngagement::QueryVar SynonymTable::getType(string name) const
{
	return synType.at(stringToIndex.at(name));
}

int SynonymTable::synonymIndex(string name) const
{
	return stringToIndex.at(name);
}

void SynonymTable::changeType(string name, RulesOfEngagement::QueryVar type)
{
	if (synType.at(stringToIndex.at(name)) == RulesOfEngagement::Statement && type == RulesOfEngagement::Assign)
		synType.at(stringToIndex.at(name)) = RulesOfEngagement::Assign;
}

void SynonymTable::setSelected(string name)
{
	selected.at(stringToIndex.at(name)) = true;
	selectedIndices.push_back(stringToIndex.at(name));
}

vector<string> SynonymTable::getAllSelected() const
{
	vector<string> ans;
	for (auto it = selectedIndices.begin(); it != selectedIndices.end(); it++)
		ans.push_back(synName[*it]);
	return ans;
}

bool SynonymTable::isSelected(string name) const
{
	return selected.at(stringToIndex.at(name));
}

void SynonymTable::putIntoClass(string name, int classIndex)
{
	synClassIndex.at(stringToIndex.at(name)) = classIndex;
}

int SynonymTable::inClass(string name) const
{
	return synClassIndex.at(stringToIndex.at(name));
}

bool SynonymTable::setSpecificAttribute(string name, string condition, string attribute)
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

/*string SynonymTable::getAttribute(string name, string condition) const
{
	if (synAttributes.at(stringToIndex.at(name)).count(condition) == 0)
		return "";
	return synAttributes.at(stringToIndex.at(name)).at(condition);
}*/

unordered_map<string, string> SynonymTable::getAllSpecificAttributes(const string& name) const
{
	return synAttributesSpecific.at(stringToIndex.at(name));
}

void SynonymTable::setGenericAttribute(string name, string ownAttribute,
	RulesOfEngagement::QueryVar otherVariable, string otherAttribute)
{
	//if (RulesOfEngagement::allowableConditions[otherVariable].count(otherAttribute) == 0)
		//throw new SPAException(variable + " has no such condition " + otherAttribute);
	synAttributesGeneric.at(stringToIndex.at(name))[ownAttribute].insert(
		pair<RulesOfEngagement::QueryVar, string>(otherVariable, otherAttribute));
}

unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>
	SynonymTable::getAllGenericAttributes(const string& name) const
{
	return synAttributesGeneric.at(stringToIndex.at(name));
}

void SynonymTable::setSelfReference(string name, RulesOfEngagement::QueryRelations relation)
{
	synSelfReference.at(stringToIndex.at(name)).insert(relation);
}

unordered_set<RulesOfEngagement::QueryRelations> SynonymTable::getAllSelfReferences(string name) const
{
	return synSelfReference.at(stringToIndex.at(name));
}

void SynonymTable::setFirstGeneric(string name, RulesOfEngagement::QueryRelations relation)
{
	synRelGenericFirst.at(stringToIndex.at(name)).insert(relation);
}

unordered_set<RulesOfEngagement::QueryRelations> SynonymTable::getAllFirstGeneric(string name) const
{
	return synRelGenericFirst.at(stringToIndex.at(name));
}

void SynonymTable::setFirstSpecific(string name, RulesOfEngagement::QueryRelations relation,
	string specific)
{
	synRelSpecificFirst.at(stringToIndex.at(name)).push_back(pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

vector<pair<RulesOfEngagement::QueryRelations, string>> SynonymTable::getAllFirstSpecific(string name) const
{
	return synRelSpecificFirst.at(stringToIndex.at(name));
}

void SynonymTable::setSecondGeneric(string name, RulesOfEngagement::QueryRelations relation)
{
	synRelGenericSecond.at(stringToIndex.at(name)).insert(relation);
}

unordered_set<RulesOfEngagement::QueryRelations> SynonymTable::getAllSecondGeneric(string name) const
{
	return synRelGenericSecond.at(stringToIndex.at(name));
}

void SynonymTable::setSecondSpecific(string name, RulesOfEngagement::QueryRelations relation,
	string specific)
{
	synRelSpecificSecond.at(stringToIndex.at(name)).push_back(pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

vector<pair<RulesOfEngagement::QueryRelations, string>> SynonymTable::getAllSecondSpecific(string name) const
{
	return synRelSpecificSecond.at(stringToIndex.at(name));
}