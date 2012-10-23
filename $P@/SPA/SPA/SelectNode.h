#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"

class SelectNode :
	public QueryTreeNode
{
public:
	SelectNode(vector<string>);
	vector<string> getSelectVariables();
private:
	vector<string> selectVariables;
};