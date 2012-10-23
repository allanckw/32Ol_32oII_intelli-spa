#pragma once
#include "stdafx.h"
#include <set>

class QueryPreprocessor
{
private:
	set<string> keywords;
	set<string> relationshipTypes;
	set<string> variableTypes;
	set<string> conditionAttributeTypes;
	static unordered_map<int, vector<string>> userVariables;
	static vector<string> selectVariables;
	static unordered_map<int, vector<vector<string>>> conditions;
	static unordered_map<int, vector<vector<string>>> relationships;

	typedef enum ashketchum { //variable types
		Procedure,
		Stmt,
		Assign,
		Call,
		While,
		If,
		Variable,
		Constant
	};

public:
	typedef enum garyOak { //relationship types
		Modifies,
		Uses,
		Calls,
		CallsStar,
		Follows,
		FollowsStar,
		Parent,
		ParentStar,
		Next,
		NextStar,
		Affects,
		AffectsStar
	};

	typedef enum rorona { //condition attribute types
		ProcName,
		VarName,
		Value,
		StmtNo
	};
	QueryPreprocessor();
	void preProcess(vector<string>);
	bool isName(string);
	static unordered_map<int, vector<string>> getUserVariables();
	static vector<string> getSelectVariables();
	static unordered_map<int, vector<vector<string>>> getConditions();
	static unordered_map<int, vector<vector<string>>> getRelationships();
};