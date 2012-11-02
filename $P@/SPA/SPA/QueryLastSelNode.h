#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryEnums.h"

class QueryLastSelNode :
	public QueryTreeNode
{
public:
	QueryLastSelNode();
	QueryLastSelNode(unordered_map<int, vector<string>>);
	vector<pair<QueryEnums::QueryVar, string>> getRemaindingSelectedVariables();
private:
	vector<pair<QueryEnums::QueryVar, string>> remaindingSelectedVariables;
};