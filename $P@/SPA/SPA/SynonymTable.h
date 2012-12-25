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
	vector<string> synProcName;
	vector<string> synVarName;
	vector<int> synValue;
	vector<int> synStmtNo;

	unordered_map<string, int> stringToIndex;
	unordered_map<QueryEnums::QueryVar, vector<int>> typeToIndices;

	vector<int> selectedIndices;
	
public:
	SynonymTable(void);
	~SynonymTable(void);

	void insert(string name, QueryEnums::QueryVar type);
	bool isInTable(string name);
	vector<string> getAllOfType(QueryEnums::QueryVar type);
	QueryEnums::QueryVar getType(string);
	int size();
	int synonymIndex(string name);

	void setSelected(string name);
	vector<string> getAllSelected();
	bool isSelected(string name);

	void putIntoClass(string name, int classIndex);
	int inClass(string name);

	bool setProcName(string name, string procName);
	string getProcName(string name);
	bool setVarName(string name, string varName);
	string getVarName(string name);
	bool setValue(string name, int value);
	int getValue(string name);
	bool setStmtNo(string name, int stmtNo);
	int getStmtNo(string name);
};