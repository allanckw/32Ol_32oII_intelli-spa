#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"
#include "QueryEnums.h"

class QueryCondNode :
	public QueryTreeNode
{
public:
	QueryCondNode(QueryEnums::QueryCond, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>);
	string getConditionVariableName();
	QueryEnums::QueryVar getConditionVariableType();
	QueryEnums::QueryCond getConditionAttribute();
	string getConditionBoundaryName();
	QueryEnums::QueryVar getConditionBoundaryType();
	
private:
	string conditionVariableName, conditionBoundaryName;
	QueryEnums::QueryVar conditionVariableType, conditionBoundaryType;
	QueryEnums::QueryCond conditionAttribute;
};