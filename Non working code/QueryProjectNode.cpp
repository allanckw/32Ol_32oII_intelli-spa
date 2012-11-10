#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryProjectNode.h"

QueryProjectNode::QueryProjectNode()
{
	this->nodeType = Project;
}

QueryProjectNode::QueryProjectNode(QueryEnums::QueryVar t1, string s1, QueryEnums::QueryVar t2, string s2, vector<string> a1, vector<string> a2)
{
	this->nodeType = Project;
	this->projectionHeader.first.first = t1;
	this->projectionHeader.first.second = s1;
	this->projectionHeader.second.first = t2;
	this->projectionHeader.second.second = s2;
	for (int i  = 0; i < a1.size(); i++)
		firstProjectionAnswer.at(i) = atoi(a1.at(i).c_str());
	for (int j  = 0; j < a2.size(); j++)
		secondProjectionAnswer.at(j) = atoi(a2.at(j).c_str());
	this->projectionAnswer = make_pair(firstProjectionAnswer, secondProjectionAnswer);
}

pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> QueryProjectNode::getProjectionHeader()
{
	return projectionHeader;
}

QueryEnums::QueryVar QueryProjectNode::getFirstProjectionType()
{
	return projectionHeader.first.first;
}

QueryEnums::QueryVar QueryProjectNode::getSecondProjectionType()
{
	return projectionHeader.second.first;
}

string QueryProjectNode::getFirstProjectionName()
{
	projectionHeader.first.second;
}

string QueryProjectNode::getSecondProjectionName()
{
	projectionHeader.second.second;
}

vector<int> QueryProjectNode::getFirstProjectionAnswer()
{
	return firstProjectionAnswer;
}

vector<int> QueryProjectNode::getSecondProjectionAnswer()
{
	return secondProjectionAnswer;
}
	