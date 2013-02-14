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

	static vector<string> MiniTokenizer(const string& line);
	MultiQueryEval(const string& query);
	bool selectBOOLEAN;
	bool earlyQuit;
	vector<string> finalanswer;

public:
	static vector<string> evaluateQuery(const string& query);
};