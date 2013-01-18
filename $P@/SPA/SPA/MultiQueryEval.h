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
	typedef enum enumPatternLHSType { 
		PLWildcard, PLStringVariable
	} PattenLHSType;
	typedef enum enumPatternRHSType { 
		PRWildcard, PRSub, PRNoSub
	} PattenRHSType;
	static string getToken(string query, int& pos);
	static string getToken2(string query, int& pos);
	
	static void matchToken(string query, int& pos, const string& match);

	string getToken(string query, int& pos, vector<string> matcher, int cutamt);//nick


	MultiQueryEval(string query);

	bool MatcherTree(ASTNode* Original,ASTNode* Pattern);//, bool isSub);


	bool selectBOOLEAN;
	vector<string> finalanswer;

	void EvaluatePattern(ASTNode::NodeType type,string PatternLHS, string PatternRHS);
	bool TryMatch(ASTNode* testedNode,PattenLHSType LHS, PattenRHSType RHS,  int LHSVarNum, ASTExprNode* RHSexpr);

	vector<string> MiniTokenizer(string line);

public:
	static vector<string> evaluateQuery(const string& query);
};