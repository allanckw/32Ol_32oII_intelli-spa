#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"

class ProjectNode :
	public QueryTreeNode
{
public:
	vector<string> getAnswer();
	void setAnswer(vector<string>);
private:
	vector<string> answer;
};