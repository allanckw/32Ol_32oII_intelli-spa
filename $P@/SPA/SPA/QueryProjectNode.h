#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryEnums.h"

class QueryProjectNode :
	public QueryTreeNode
{
public:
	QueryProjectNode();
	QueryProjectNode(QueryEnums::QueryReladition, QueryEnums::QueryVar, string, QueryEnums::QueryVar, string, vector<string>, vector<string>, bool);
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> getProjectionHeader();
	QueryEnums::QueryVar getFirstProjectionType();
	QueryEnums::QueryVar getSecondProjectionType();
	string getFirstProjectionName();
	string getSecondProjectionName();
	vector<int> getFirstProjectionAnswer();
	vector<int> getSecondProjectionAnswer();
	QueryEnums::QueryReladition getProjectionReladitionType();
	bool getBoolAnswer();
private:
	QueryEnums::QueryReladition reladitionType;
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> projectionHeader;
	vector<int> firstProjectionAnswer, secondProjectionAnswer;
	pair<vector<int>, vector<int>> projectionAnswer;
	bool boolAnswer;
};