#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryEnums.h"

class QuerySelNode :
	public QueryTreeNode
{
public:
	QuerySelNode();
	QuerySelNode(unordered_map<int, vector<string>>);
	vector<pair<QueryEnums::QueryVar, string>> getSelectedVariables();
private:
	vector<pair<QueryEnums::QueryVar, string>> selectedVariables;
};