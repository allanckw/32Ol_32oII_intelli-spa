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

	string delimiters;
	string currentToken;
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> relationshipDeclaration, conditionDeclaration;
	pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>> patternDeclaration;
	vector<string> variableNames;
	QueryEnums::QueryVar variableType;
	QueryEnums::QueryReladition reladitionType, conditionAttributeType;
	bool variableDeclaration, selectVariableDeclaration, suchThat; 
	bool relationship, condition;
	bool complete; //determines whether the query is complete
	bool pattern, patternVariable, patternFirst, patternSecond;
	//relationship validation
	bool checkCapitalLetter;
	bool relationshipDef, openBracket, firstVariable, comma, secondVariable;
	//condition declaration
	bool conditionVariable, dot, attribute, equals; 
	bool existsVariable; //check for existing variable name in relationship/condition in user variables 

	unordered_map<int, vector<string>> userVariables;
	unordered_map<int, vector<string>> selectVariables;
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> relationships;
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> conditions;
	vector<pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> patterns;
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
	vector<pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> getPatterns();
};