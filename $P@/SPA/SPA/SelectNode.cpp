#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "SelectNode.h"

SelectNode::SelectNode(unordered_map<int, vector<string>> contents)
{
	this->nodeType = Select;
	this->selectVariables = contents;
}

vector<string> SelectNode::getSelectVariableNames(QueryEnums::QueryVar type)
{
	return selectVariables[type];
}

QueryEnums::QueryVar SelectNode::getSelectVariableType(string name)
{
	for (auto it = selectVariables.begin(); it != selectVariables.end(); it++)
	{
		for (int i = 0; i < (*it).second.size(); i++)
		{
			if ((*it).second.at(i).compare(name) == 0)
				return (QueryEnums::QueryVar) (*it).first;
		}
	}

	return QueryEnums::WildCard;
}