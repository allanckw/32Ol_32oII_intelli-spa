#pragma once
#include "SynonymTable.h"
#include "SPAException.h"

int SynonymTable::size()
{
	return synName.size();
}

void SynonymTable::insert(string name, RulesOfEngagement::QueryVar type)
{
	if (stringToIndex.count(name) > 0)
		throw new SPAException("Double declaration of synonym");

	int index = synName.size();

	synName.push_back(name);
	synType.push_back(type);
	selected.push_back(false);
	synClassIndex.push_back(-1);
	synAttributes.push_back(unordered_map<string, string>());
	synSelfReference.push_back(unordered_set<RulesOfEngagement::QueryRelations>());
	synRelGenericFirst.push_back(vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>>());
	synRelSpecificFirst.push_back(vector<pair<RulesOfEngagement::QueryRelations, string>>());
	synRelGenericSecond.push_back(vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>>());
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

vector<string> SynonymTable::getAllNames()
{
	return synName;
}

bool SynonymTable::isInTable(string name)
{
	return (stringToIndex.count(name) > 0);
}

vector<string> SynonymTable::getAllOfType(RulesOfEngagement::QueryVar type)
{
	vector<string> ans;
	for (auto it = typeToIndices[type].begin(); it != typeToIndices[type].end(); it++)
		ans.push_back(synName[*it]);
	return ans;
}

RulesOfEngagement::QueryVar SynonymTable::getType(string name)
{
	return synType[stringToIndex.at(name)];
}

int SynonymTable::synonymIndex(string name)
{
	return stringToIndex.at(name);
}

void SynonymTable::changeType(string name, RulesOfEngagement::QueryVar type)
{
	if (synType[stringToIndex.at(name)] == RulesOfEngagement::Statement && type == RulesOfEngagement::Assign)
		synType[stringToIndex.at(name)] = RulesOfEngagement::Assign;
}

void SynonymTable::setSelected(string name)
{
	selected[stringToIndex.at(name)] = true;
	selectedIndices.push_back(stringToIndex.at(name));
}

vector<string> SynonymTable::getAllSelected()
{
	vector<string> ans;
	for (auto it = selectedIndices.begin(); it != selectedIndices.end(); it++)
		ans.push_back(synName[*it]);
	return ans;
}

bool SynonymTable::isSelected(string name)
{
	return selected[stringToIndex.at(name)];
}

void SynonymTable::putIntoClass(string name, int classIndex)
{
	synClassIndex[stringToIndex.at(name)] = classIndex;
}

int SynonymTable::inClass(string name)
{
	return synClassIndex[stringToIndex.at(name)];
}

bool SynonymTable::setAttribute(string name, string condition, string attribute)
{
	if (RulesOfEngagement::allowableConditions[synType[stringToIndex.at(name)]].count(condition) == 0)
		throw new SPAException("Synonym " + name + " has no such condition " + condition);
	if (synAttributes[stringToIndex.at(name)].count(condition) > 0)
		return synAttributes[stringToIndex.at(name)][condition] == attribute;
	synAttributes[stringToIndex.at(name)][condition] = attribute;
	return true;
}

string SynonymTable::getAttribute(string name, string condition)
{
	if (synAttributes[stringToIndex.at(name)].count(condition) == 0)
		return "";
	return synAttributes[stringToIndex.at(name)][condition];
}

unordered_map<string, string> SynonymTable::getAllAttributes(string name)
{
	return synAttributes[stringToIndex.at(name)];
}

void SynonymTable::setSelfReference(string name, RulesOfEngagement::QueryRelations relation)
{
	synSelfReference[stringToIndex.at(name)].insert(relation);
}

unordered_set<RulesOfEngagement::QueryRelations> SynonymTable::getAllSelfReferences(string name)
{
	return synSelfReference[stringToIndex.at(name)];
}

void SynonymTable::setFirstGeneric(string name, RulesOfEngagement::QueryRelations relation,
		RulesOfEngagement::QueryVar specific)
{
	synRelGenericFirst[stringToIndex.at(name)].push_back(pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>(relation, specific));
}

vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>> SynonymTable::getAllFirstGeneric(string name)
{
	return synRelGenericFirst[stringToIndex.at(name)];
}

void SynonymTable::setFirstSpecific(string name, RulesOfEngagement::QueryRelations relation,
	string specific)
{
	synRelSpecificFirst[stringToIndex.at(name)].push_back(pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

vector<pair<RulesOfEngagement::QueryRelations, string>> SynonymTable::getAllFirstSpecific(string name)
{
	return synRelSpecificFirst[stringToIndex.at(name)];
}

void SynonymTable::setSecondGeneric(string name, RulesOfEngagement::QueryRelations relation,
		RulesOfEngagement::QueryVar specific)
{
	synRelGenericSecond[stringToIndex.at(name)].push_back(pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>(relation, specific));
}

vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>> SynonymTable::getAllSecondGeneric(string name)
{
	return synRelGenericSecond[stringToIndex.at(name)];
}

void SynonymTable::setSecondSpecific(string name, RulesOfEngagement::QueryRelations relation,
	string specific)
{
	synRelSpecificSecond[stringToIndex.at(name)].push_back(pair<RulesOfEngagement::QueryRelations, string>(relation, specific));
}

vector<pair<RulesOfEngagement::QueryRelations, string>> SynonymTable::getAllSecondSpecific(string name)
{
	return synRelSpecificSecond[stringToIndex.at(name)];
}