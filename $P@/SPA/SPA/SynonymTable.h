#pragma once
#include "stdAfx.h"
#include "QueryEnums.h"

class SynonymTable
{
private:
	vector<string> synName;
	vector<QueryEnums::QueryVar> synType;
	vector<bool> selected;
	vector<int> synClassIndex;
	vector<unordered_map<string, string>> synAttributes;
	vector<unordered_set<QueryEnums::QueryReladition>> synSelfReference;

	unordered_map<string, int> stringToIndex;
	unordered_map<QueryEnums::QueryVar, vector<int>> typeToIndices;

	vector<int> selectedIndices;
	
public:
	int size();

	void insert(string name, QueryEnums::QueryVar type);
	bool isInTable(string name);
	vector<string> getAllOfType(QueryEnums::QueryVar type);
	QueryEnums::QueryVar getType(string);
	int synonymIndex(string name);
	void changeType(string name, QueryEnums::QueryVar type);

	void setSelected(string name);
	vector<string> getAllSelected();
	bool isSelected(string name);

	void putIntoClass(string name, int classIndex);
	int inClass(string name);

	bool setAttribute(string name, string condition, string attribute);
	string getAttribute(string name, string condition);
	unordered_map<string, string> getAllAttributes(string name);

	void setSelfReference(string name, QueryEnums::QueryReladition relation);
	unordered_set<QueryEnums::QueryReladition> getAllSelfReferences(string name);
};