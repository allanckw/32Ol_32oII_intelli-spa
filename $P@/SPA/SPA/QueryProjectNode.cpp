#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryProjectNode.h"
#include "PKB.h"

QueryProjectNode::QueryProjectNode()
{
	this->nodeType = Project;
}

QueryProjectNode::QueryProjectNode
	(QueryEnums::QueryReladition rcType, QueryEnums::QueryVar t1, string s1, QueryEnums::QueryVar t2, 
	string s2, vector<int> a1, vector<int> a2, bool boolAns)
{
	this->nodeType = Project;
	this->reladitionType = rcType;
	this->projectionHeader.first.first = t1;
	this->projectionHeader.first.second = s1;
	this->projectionHeader.second.first = t2;
	this->projectionHeader.second.second = s2;
	this->firstProjectionAnswer = a1;
	this->secondProjectionAnswer = a2;
	this->projectionAnswer = make_pair(firstProjectionAnswer, secondProjectionAnswer);
	this->boolAnswer = boolAns;
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
	return projectionHeader.first.second;
}

string QueryProjectNode::getSecondProjectionName()
{
	return projectionHeader.second.second;
}

vector<int> QueryProjectNode::getFirstProjectionAnswer()
{
	return firstProjectionAnswer;
}

vector<int> QueryProjectNode::getSecondProjectionAnswer()
{
	return secondProjectionAnswer;
}

bool QueryProjectNode::getBoolAnswer()
{
	return boolAnswer;
}

QueryEnums::QueryReladition QueryProjectNode::getProjectionReladitionType()
{
	return reladitionType;
}