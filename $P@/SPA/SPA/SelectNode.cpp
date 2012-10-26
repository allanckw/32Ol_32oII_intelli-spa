#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "SelectNode.h"

SelectNode::SelectNode(unordered_map<int, vector<string>> contents)
{
	this->nodeType = Select;
	this->selectVariables = contents;
}

vector<string> SelectNode::getSelectVariableNames(PalletTown::ashKetchum type)
{
	return selectVariables[type];
}

PalletTown::ashKetchum SelectNode::getSelectVariableType(string name)
{
	for (auto it = selectVariables.begin(); it != selectVariables.end(); it++)
	{
		for (int i = 0; i < (*it).second.size(); i++)
		{
			if ((*it).second.at(i).compare(name) == 0)
				return (PalletTown::ashKetchum) (*it).first;
		}
	}

	return PalletTown::WildCard;
}