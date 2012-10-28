#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryProjectNode.h"

vector<string> QueryProjectNode::getAnswer()
{
	return answer;
}
	
void QueryProjectNode::setAnswer(vector<string> a)
{
	answer = a;
}