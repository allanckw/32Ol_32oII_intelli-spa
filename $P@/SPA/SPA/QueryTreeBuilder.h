#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"

class QueryTreeBuilder
{
public:
	void buildQueryTree(unordered_map<int, vector<string>> userVariables, 
	unordered_map<int, vector<string>> selectVariables, 
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> relationships,
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, string>>> conditions);
	vector<vector<QueryTreeNode*>> getQueryTree();
private:
	vector<vector<QueryTreeNode*>> queryTree;
};