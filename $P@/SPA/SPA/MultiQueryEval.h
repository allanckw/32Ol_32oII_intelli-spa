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
	string getToken(const string& query, int& pos, vector<string> matcher, int cutamt);//nick	
	static void matchToken(const string& query, int& pos, const string& match);
	
	MultiQueryEval(const string& query);
	
	vector<string> MiniTokenizer(string line);

	bool selectBOOLEAN;
	bool earlyQuit;
	vector<string> finalanswer;

public:
	static vector<string> evaluateQuery(const string& query);
};