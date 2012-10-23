#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"

class QueryTreeBuilder
{
public:
	void buildQueryTree(unordered_map<int, vector<string>>, vector<string>, unordered_map<int, vector<vector<string>>>, 
		unordered_map<int, vector<vector<string>>>);
private:
	vector<vector<QueryTreeNode*>> queryTree;
};