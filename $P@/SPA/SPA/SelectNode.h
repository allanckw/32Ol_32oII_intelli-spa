#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "PalletTown.h"

class SelectNode :
	public QueryTreeNode
{
public:
	SelectNode(unordered_map<int, vector<string>>);
	unordered_map<int, vector<string>> getSelectVariables();
private:
	unordered_map<int, vector<string>> selectVariables;
	vector<string> getSelectVariableNames(PalletTown::ashKetchum t);
	PalletTown::ashKetchum getSelectVariableType(string);
};