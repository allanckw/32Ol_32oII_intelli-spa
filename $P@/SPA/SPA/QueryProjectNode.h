#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"

class QueryProjectNode :
	public QueryTreeNode
{
public:
	QueryProjectNode();
	vector<string> getAnswer();
	void setAnswer(vector<string>);
private:
	vector<string> answer;
};