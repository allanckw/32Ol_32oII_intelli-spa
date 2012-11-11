#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"
#include "QueryEnums.h"

class QueryRelNode :
	public QueryTreeNode
{
private:
	QueryEnums::QueryReladition relationshipType;
	QueryEnums::QueryVar firstVariableType, secondVariableType;
	string firstVariableName, secondVariableName;

public:
	QueryRelNode();
	QueryRelNode(QueryEnums::QueryReladition, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>);
	QueryEnums::QueryReladition getRelationshipType();
	string getFirstVariableName();
	QueryEnums::QueryVar getFirstVariableType();
	string getSecondVariableName();
	QueryEnums::QueryVar getSecondVariableType();
};