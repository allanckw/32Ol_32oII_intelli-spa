#pragma once
#include "stdAfx.h"
#include "QueryEnums.h"
#include "QueryTreeNode.h"

class QueryPatternNode:
	public QueryTreeNode
{
private:
	QueryEnums::QueryVar patternVariableType;
	QueryEnums::QueryVar firstPatternParameterType;
	QueryEnums::QueryVar secondPatternParameterType;
	string patternVariableName, firstPatternParameterName, secondPatternParameterName;

public:
	QueryPatternNode();
	QueryPatternNode(pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>);
	QueryEnums::QueryVar getPatternVariableType();
	QueryEnums::QueryVar getFirstPatternParameterType();
	QueryEnums::QueryVar getSecondPatternParameterType();
	string getPatternVariableName();
	string getFirstPatternParameterName();
	string getSecondPatternParameterName();
};
