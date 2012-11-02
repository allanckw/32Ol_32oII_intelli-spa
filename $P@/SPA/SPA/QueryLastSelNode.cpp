#pragma once
#include "StdAfx.h"
#include "QueryLastSelNode.h"
#include "QueryTreeNode.h"
#include "QueryEnums.h"

QueryLastSelNode::QueryLastSelNode()
{
	this->nodeType = LastSelect;
}
	
QueryLastSelNode::QueryLastSelNode(unordered_map<int, vector<string>> contents)
{
	this->nodeType = LastSelect;
	QueryEnums::QueryVar varType;
	for (auto it = contents.begin(); it != contents.end(); it++)
	{
		varType = QueryEnums::QueryVar ((*it).first);
		for (int i = 0; i < (*it).second.size(); i++)
			this->remaindingSelectedVariables.push_back(make_pair(varType, (*it).second.at(i)));
	}
}
	
vector<pair<QueryEnums::QueryVar, string>> QueryLastSelNode::getRemaindingSelectedVariables()
{
	return remaindingSelectedVariables;
}