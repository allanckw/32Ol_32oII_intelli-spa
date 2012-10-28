#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryEnums.h"

class SelectNode :
	public QueryTreeNode
{
public:
	SelectNode(unordered_map<int, vector<string>>);
	unordered_map<int, vector<string>> getSelectVariables();
private:
	unordered_map<int, vector<string>> selectVariables;
	vector<string> getSelectVariableNames(QueryEnums::QueryVar t);
	QueryEnums::QueryVar getSelectVariableType(string);
};