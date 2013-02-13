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
	static string getToken2(string query, int& pos);
	static void matchToken(const string& query, int& pos, const string& match);
	static vector<string> MiniTokenizer(const string& line);
	static void validate(const string& query);

	MultiQueryEval(const string& query);
	
	bool selectBOOLEAN;
	bool earlyQuit;
	vector<string> finalanswer;

public:
	static vector<string> evaluateQuery(const string& query);
};