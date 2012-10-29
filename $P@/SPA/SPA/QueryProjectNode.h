#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"

class QueryProjectNode :
	public QueryTreeNode
{
public:
	QueryProjectNode();
	QueryProjectNode(vector<pair<int, string>>);
	QueryProjectNode(vector<pair<string, string>>);
	QueryProjectNode(vector<pair<string, int>>);
	QueryProjectNode(vector<pair<int, int>>);
	vector<string> getAnswer();
	void setAnswer(vector<string>);
private:
	vector<string> answer;
};