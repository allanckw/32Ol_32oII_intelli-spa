#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QuerySelNode.h"

QuerySelNode::QuerySelNode()
{
	this->nodeType = Select;
}

QuerySelNode::QuerySelNode(unordered_map<int, vector<string>> contents)
{
	this->nodeType = Select;
	QueryEnums::QueryVar varType;
	for (auto it = contents.begin(); it != contents.end(); it++)
	{
		varType = QueryEnums::QueryVar ((*it).first);
		for (int i = 0; i < (*it).second.size(); i++)
			this->selectedVariables.push_back(make_pair(varType, (*it).second.at(i)));
	}
}

vector<pair<QueryEnums::QueryVar, string>> QuerySelNode::getSelectedVariables()
{
	return this->selectedVariables;
}