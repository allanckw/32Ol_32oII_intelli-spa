#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryProjectNode.h"

QueryProjectNode::QueryProjectNode()
{
	this->nodeType = Dummy;
}

QueryProjectNode::QueryProjectNode(vector<pair<string, string>> v)
{
	this->nodeType = Project;
	this->answer2 = v;
}

QueryProjectNode::QueryProjectNode(vector<string> v)
{
	this->nodeType = Project;
	this->answer1 = v;
}

vector<string> QueryProjectNode::getAnswer()
{
	return answer1;
}
	
void QueryProjectNode::setAnswer(vector<string> a)
{
	answer1 = a;
}

vector<pair<string, string>> QueryProjectNode::getPairAnswer()
{
	return answer2;
}
	
void QueryProjectNode::setAnswer(vector<pair<string, string>> a)
{
	answer2 = a;
}