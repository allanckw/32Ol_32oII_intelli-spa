#pragma once
#include "stdafx.h"
#include <set>
#include "QueryEnums.h"

class QueryPreprocessor
{
private:
	set<string> keywords;
	set<string> relationshipQueryEnums;
	set<string> variableQueryEnums;
	set<string> conditionAttributeQueryEnums;
	unordered_map<int, vector<string>> userVariables;
	unordered_map<int, vector<string>> selectVariables;
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> relationships;
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, string>>> conditions;
	
public:
	QueryPreprocessor();
	void preProcess(vector<string>);
	bool isName(string);
	unordered_map<int, vector<string>> getUserVariables();
	unordered_map<int, vector<string>> getSelectVariables();
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> 
		getRelationships();
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, string>>> getConditions();
};