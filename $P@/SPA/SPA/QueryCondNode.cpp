#pragma once
#include "StdAfx.h"
#include "QueryCondNode.h"
#include "QueryPreprocessor.h"
#include "QueryEnums.h"
#include "PKB.h"

QueryCondNode::QueryCondNode(QueryEnums::QueryCond attribute, pair<pair<QueryEnums::QueryVar, string>, string> contents)
{
	this->nodeType = Condition;
	this->conditionAttribute = attribute;
	this->conditionVariableType = contents.first.first;
	this->conditionVariableName = contents.first.second;
	this->conditionBoundary = contents.second;
}
	
string QueryCondNode::getConditionVariableName()
{
	return conditionVariableName;
}

QueryEnums::QueryVar QueryCondNode::getConditionVariableType()
{
	return conditionVariableType;
}
	
QueryEnums::QueryCond QueryCondNode::getConditionAttribute()
{
	return conditionAttribute;
}
	
string QueryCondNode::getConditionBoundary()
{
	return conditionBoundary;
}