#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"
#include "QueryEnums.h"

class QueryRelNode :
	public QueryTreeNode
{
private:
	QueryEnums::QueryRel relationshipType;
	QueryEnums::QueryVar firstVariableType, secondVariableType;
	string firstVariableName, secondVariableName;

public:
	QueryRelNode(QueryEnums::QueryRel, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>);
	QueryEnums::QueryRel getRelationshipType();
	string getFirstVariableName();
	QueryEnums::QueryVar getFirstVariableType();
	string getSecondVariableName();
	QueryEnums::QueryVar getSecondVariableType();
};