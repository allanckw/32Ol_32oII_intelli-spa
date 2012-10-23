#pragma once
#include "StdAfx.h"
#include "ConditionNode.h"
#include "QueryPreprocessor.h"
#include "PKB.h"

ConditionNode::ConditionNode(QueryPreprocessor::rorona attribute, vector<string> c)
{
	if (c.size() != 2)
		throw SPAException("Weird condition detected");
	this->nodeType = Condition;
	this->conditionAttribute = attribute;
	this->conditionVariable = c.at(0);
	this->conditionBoundary = c.at(1);
}
	
string ConditionNode::getConditionVariable()
{
	return conditionVariable;
}
	
string ConditionNode::getConditionAttribute()
{
	return conditionAttribute;
}
	
string ConditionNode::getConditionBoundary()
{
	return conditionBoundary;
}