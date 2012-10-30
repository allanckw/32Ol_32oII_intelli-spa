#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"

class QueryProjectNode :
	public QueryTreeNode
{
public:
	QueryProjectNode();
	QueryProjectNode(vector<pair<string, string>>);
	QueryProjectNode(vector<string>);
	vector<string> getAnswer();
	vector<pair<string, string>> getPairAnswer();
	void setAnswer(vector<string>);
	void setAnswer(vector<pair<string, string>>);
private:
	vector<string> answer1;
	vector<pair<string, string>> answer2;
};