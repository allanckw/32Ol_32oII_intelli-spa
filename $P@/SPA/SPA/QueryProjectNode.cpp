#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryProjectNode.h"
#include "PKB.h"

QueryProjectNode::QueryProjectNode()
{
	this->nodeType = Project;
}

QueryProjectNode::QueryProjectNode(QueryEnums::QueryVar t1, string s1, QueryEnums::QueryVar t2, string s2, vector<string> a1, vector<string> a2, bool boolAns)
{
	this->nodeType = Project;
	this->projectionHeader.first.first = t1;
	this->projectionHeader.first.second = s1;
	this->projectionHeader.second.first = t2;
	this->projectionHeader.second.second = s2;
	for (int i  = 0; i < a1.size(); i++)
	{
		if (this->projectionHeader.first.first == QueryEnums::Procedure)
			firstProjectionAnswer.push_back(PKB::procedures.getPROCIndex(a1.at(i)));
		else if (this->projectionHeader.first.first == QueryEnums::Variable)
			firstProjectionAnswer.push_back(PKB::variables.getVARIndex(a1.at(i)));
		else
			firstProjectionAnswer.push_back(atoi(a1.at(i).c_str()));
	}
	for (int j  = 0; j < a2.size(); j++)
	{
		if (this->projectionHeader.second.first == QueryEnums::Procedure)
			secondProjectionAnswer.push_back(PKB::procedures.getPROCIndex(a2.at(j)));
		else if (this->projectionHeader.second.first == QueryEnums::Variable)
			secondProjectionAnswer.push_back(PKB::variables.getVARIndex(a2.at(j)));
		else
			secondProjectionAnswer.push_back(atoi(a2.at(j).c_str()));
	}
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