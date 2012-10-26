#pragma once
#include "StdAfx.h"
#include "ConditionNode.h"
#include "QueryPreprocessor.h"
#include "PalletTown.h"
#include "PKB.h"

ConditionNode::ConditionNode(PalletTown::mistyWaterflower attribute, pair<pair<PalletTown::ashKetchum, string>, string> contents)
{
	this->nodeType = Condition;
	this->conditionAttribute = attribute;
	this->conditionVariableType = contents.first.first;
	this->conditionVariableName = contents.first.second;
	this->conditionBoundary = contents.second;
}
	
string ConditionNode::getConditionVariableName()
{
	return conditionVariableName;
}

PalletTown::ashKetchum ConditionNode::getConditionVariableType()
{
	return conditionVariableType;
}
	
PalletTown::mistyWaterflower ConditionNode::getConditionAttribute()
{
	return conditionAttribute;
}
	
string ConditionNode::getConditionBoundary()
{
	return conditionBoundary;
}