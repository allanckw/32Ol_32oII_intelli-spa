#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"
#include "QueryEnums.h"

class QueryCondNode :
	public QueryTreeNode
{
public:
	QueryCondNode(QueryEnums::QueryCond, pair<pair<QueryEnums::QueryVar, string>, string>);
	string getConditionVariableName();
	QueryEnums::QueryVar getConditionVariableType();
	QueryEnums::QueryCond getConditionAttribute();
	string getConditionBoundary();
private:
	string conditionVariableName;
	QueryEnums::QueryVar conditionVariableType;
	QueryEnums::QueryCond conditionAttribute;
	string conditionBoundary;
};