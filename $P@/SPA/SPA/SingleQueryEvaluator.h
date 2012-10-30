#pragma once
#include "stdafx.h"
#include "QueryEnums.h"
#include "QueryTreeNode.h"

class SingleQueryEvaluator
{
public:
	SingleQueryEvaluator();
	vector<string> returnAnswer(vector<vector<QueryTreeNode*>>);
	string intToString(int);
	string charToString(char c);

private:
	bool allStmtsFirst, allStmtsSecond, allProcsFirst, allProcsSecond, allVarsSecond;
	vector<string> answer;
	unordered_set<STMT> currentFirstIndices, currentSecondIndices;
	void populateFirstVariableIndices(QueryEnums::QueryVar);
	void populateSecondVariableIndices(QueryEnums::QueryVar);
};
