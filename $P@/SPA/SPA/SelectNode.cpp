#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "SelectNode.h"

SelectNode::SelectNode(vector<string> v)
{
	this->nodeType = Select;
	this->selectVariables = v;
}

vector<string> SelectNode::getSelectVariables()
{
	return selectVariables;
}
