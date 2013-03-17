#pragma once
#include "stdafx.h"
#include "RulesOfEngagement.h"
#include "SynonymTable.h"
#include "DisjointSet.h"
#include "ASTNode.h"
#include "ASTExprNode.h"

class MultiQueryEval
{
private:
	static string getToken(const string& query, int& pos);
	static string getToken2(const string& query, int& pos);
	static void matchToken(const string& query, int& pos, const string& match);

	static void validate(const string& query);
	MultiQueryEval(const string& query, list<string>& results);

	bool selectBOOLEAN;
	bool earlyQuit;

public:
	static void evaluateQuery(const string& query, list<string>& results);
};