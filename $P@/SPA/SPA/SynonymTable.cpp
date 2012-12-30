#pragma once
#include "SynonymTable.h"
#include "SPAException.h"

int SynonymTable::size()
{
	return synName.size();
}

void SynonymTable::insert(string name, QueryEnums::QueryVar type)
{
	if (stringToIndex.count(name) > 0)
		throw new SPAException("Double declaration of synonym");

	int index = synName.size();

	synName.push_back(name);
	synType.push_back(type);
	selected.push_back(false);
	synClassIndex.push_back(-1);
	synAttributes.push_back(unordered_map<string, string>());
	synSelfReference.push_back(unordered_set<QueryEnums::QueryReladition>());

	stringToIndex[name] = index;
	if (typeToIndices.count(type) > 0)
		typeToIndices[type].push_back(index);
	else {
		vector<int> temp;
		temp.push_back(index);
		typeToIndices[type] = temp;
	}
}

bool SynonymTable::isInTable(string name)
{
	return (stringToIndex.count(name) > 0);
}

vector<string> SynonymTable::getAllOfType(QueryEnums::QueryVar type)
{
	vector<string> ans;
	for (auto it = typeToIndices[type].begin(); it != typeToIndices[type].end(); it++)
		ans.push_back(synName[*it]);
	return ans;
}

QueryEnums::QueryVar SynonymTable::getType(string name)
{
	return synType[stringToIndex.at(name)];
}

int SynonymTable::synonymIndex(string name)
{
	return stringToIndex.at(name);
}

void SynonymTable::changeType(string name, QueryEnums::QueryVar type)
{
	if (synType[stringToIndex.at(name)] == QueryEnums::Stmt && type == QueryEnums::Assign)
		synType[stringToIndex.at(name)] = QueryEnums::Assign;
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

void SynonymTable::setSelfReference(string name, QueryEnums::QueryReladition relation)
{
	synSelfReference[stringToIndex.at(name)].insert(relation);
}

unordered_set<QueryEnums::QueryReladition> SynonymTable::getAllSelfReferences(string name)
{
	return synSelfReference[stringToIndex.at(name)];
}