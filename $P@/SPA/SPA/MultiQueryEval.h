#pragma once
#include "stdafx.h"
#include "RulesOfEngagement.h"
#include "SynonymTable.h"
#include "DisjointSet.h"

class MultiQueryEval
{
private:
	static string getToken(const string& query, int& pos);
	static void matchToken(const string& query, int& pos, const string& match);

	MultiQueryEval(const string& query);

	bool selectBOOLEAN;
	vector<string> finalanswer;

public:
	static vector<string> evaluateQuery(const string& query);
};