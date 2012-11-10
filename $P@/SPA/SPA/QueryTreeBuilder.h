#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryEnums.h"

typedef vector<vector<QueryTreeNode*>> QueryTree;
class QueryTreeBuilder
{
public:
	void buildQueryTree(unordered_map<int, vector<string>> userVariables, 
	unordered_map<int, vector<string>> selectVariables, 
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> relationships,
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> conditions,
	vector<pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> patterns);
	vector<vector<QueryTreeNode*>> getQueryTree();
private:
	QueryTree queryTree;
};