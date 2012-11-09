#pragma once
#include "stdafx.h"
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
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> conditions;
	void resetAll();

	bool isName(string);

public:
	QueryPreprocessor();
	void preProcess(vector<string>);

	unordered_map<int, vector<string>> getUserVariables();
	unordered_map<int, vector<string>> getSelectVariables();
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> 
		getRelationships();
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>,pair<QueryEnums::QueryVar, string>>>> getConditions();
};