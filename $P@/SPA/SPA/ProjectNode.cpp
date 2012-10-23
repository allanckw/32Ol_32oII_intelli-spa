#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "ProjectNode.h"

vector<string> ProjectNode::getAnswer()
{
	return answer;
}
	
void ProjectNode::setAnswer(vector<string> a)
{
	answer = a;
}