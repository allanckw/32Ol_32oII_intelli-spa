#pragma once
#include "StdAfx.h"
#include "QueryRelNode.h"
#include "QueryPreprocessor.h"
#include "QueryEnums.h"
#include "PKB.h"

QueryRelNode::QueryRelNode()
{
	this->nodeType = Dummy;
}

QueryRelNode::QueryRelNode(QueryEnums::QueryRel type, 
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> contents)
{
	this->nodeType = Relationship;
	this->relationshipType = type;
	this->firstVariableType = contents.first.first;
	this->firstVariableName = contents.first.second;
	this->secondVariableType = contents.second.first;
	this->secondVariableName = contents.second.second;
}

QueryEnums::QueryRel QueryRelNode::getRelationshipType()
{
	return this->relationshipType;
}

string QueryRelNode::getFirstVariableName()
{
	return this->firstVariableName;
}

QueryEnums::QueryVar QueryRelNode::getFirstVariableType()
{
	return this->firstVariableType;
}

string QueryRelNode::getSecondVariableName()
{
	return this->secondVariableName;
}

QueryEnums::QueryVar QueryRelNode::getSecondVariableType()
{
	return this->secondVariableType;
}
