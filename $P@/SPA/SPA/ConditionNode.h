#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"

class ConditionNode :
	public QueryTreeNode
{
public:
	ConditionNode(QueryPreprocessor::rorona, vector<string>);
	string getConditionVariable();
	string getConditionAttribute();
	string getConditionBoundary();
private:
	string conditionVariable;
	string conditionAttribute;
	string conditionBoundary;
};