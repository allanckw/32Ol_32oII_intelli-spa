#pragma once
#include "SynonymTable.h"
#include "SPAException.h"

SynonymTable::SynonymTable()
{
}

SynonymTable::~SynonymTable()
{
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
	synProcName.push_back("");
	synVarName.push_back("");
	synValue.push_back(-1);
	synStmtNo.push_back(-1);

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

int SynonymTable::size()
{
	return synName.size();
}

void SynonymTable::setSelected(string name)
{
	selected[stringToIndex.at(name)] = true;
	selectedIndices.push_back(stringToIndex.at(name));
}

int SynonymTable::synonymIndex(string name)
{
	return stringToIndex.at(name);
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

bool SynonymTable::setProcName(string name, string procName)
{
	if (synProcName[stringToIndex.at(name)] != "")
		return synProcName[stringToIndex.at(name)] == procName;
	synProcName[stringToIndex.at(name)] = procName;
	return true;
}

string SynonymTable::getProcName(string name)
{
	return synProcName[stringToIndex.at(name)];
}

bool SynonymTable::setVarName(string name, string varName)
{
	if (synVarName[stringToIndex.at(name)] != "")
		return synVarName[stringToIndex.at(name)] == varName;
	synVarName[stringToIndex.at(name)] = varName;
	return true;
}

string SynonymTable::getVarName(string name)
{
	return synVarName[stringToIndex.at(name)];
}

bool SynonymTable::setValue(string name, int value)
{
	if (synValue[stringToIndex.at(name)] != -1)
		return synValue[stringToIndex.at(name)] == value;
	synValue[stringToIndex.at(name)] = value;
	return true;
}

int SynonymTable::getValue(string name)
{
	return synValue[stringToIndex.at(name)];
}

bool SynonymTable::setStmtNo(string name, int stmtNo)
{
	if (synStmtNo[stringToIndex.at(name)] != -1)
		return synStmtNo[stringToIndex.at(name)] == stmtNo;
	synStmtNo[stringToIndex.at(name)] = stmtNo;
	return true;
}

int SynonymTable::getStmtNo(string name)
{
	return synStmtNo[stringToIndex.at(name)];
}