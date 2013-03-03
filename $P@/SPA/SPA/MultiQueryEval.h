#pragma once
#include "stdafx.h"
#include "RulesOfEngagement.h"
#include "SynonymTable.h"
#include "DisjointSet.h"
#include "ASTNode.h"
#include "ASTExprNode.h"
#include "PKB.h"
#include "RulesOfEngagement.h"
#include "AnswerTable.h"
#include "QueryPreprocessor.h"
#include "AssignmentParser.h"

class MultiQueryEval
{
private:

	static vector<string> MiniTokenizer(const string& line);
	
	bool selectBOOLEAN;
	bool earlyQuit;
	vector<string> finalanswer;

public:
	MultiQueryEval(const string& query);

	static vector<string> evaluateQuery(const string& query);

	bool isSelectBoolean();

	bool isEarlyQuit();

	vector<string> getFinalAnswer();
};