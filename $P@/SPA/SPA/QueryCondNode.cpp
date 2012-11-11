#pragma once
#include "StdAfx.h"
#include "QueryCondNode.h"
#include "QueryPreprocessor.h"
#include "QueryEnums.h"
#include "PKB.h"

QueryCondNode::QueryCondNode(QueryEnums::QueryReladition attribute, 
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> contents)
{
	this->nodeType = Condition;
	this->conditionAttribute = attribute;
	this->conditionVariableType = contents.first.first;
	this->conditionVariableName = contents.first.second;
	this->conditionBoundaryType = contents.second.first;
	this->conditionBoundaryName = contents.second.second;
}
	
string QueryCondNode::getConditionVariableName()
{
	return conditionVariableName;
}

QueryEnums::QueryVar QueryCondNode::getConditionVariableType()
{
	return conditionVariableType;
}
	
QueryEnums::QueryReladition QueryCondNode::getConditionAttribute()
{
	return conditionAttribute;
}
	
string QueryCondNode::getConditionBoundaryName()
{
	return conditionBoundaryName;
}

QueryEnums::QueryVar QueryCondNode::getConditionBoundaryType()
{
	return conditionBoundaryType;
}