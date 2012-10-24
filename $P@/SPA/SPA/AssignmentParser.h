#pragma once
#include "StdAfx.h"
#include "ExprNode.h"

class AssignmentParser
{
private:
	static int getOperatorWeight(string token);
	static bool AssignmentParser::isOperator(string token);
	static int compareOprPrecedence( string token1, string token2);
	static void popOperator(string operators, string operands);
	static int getConstantValue(string s);


public:
	static ExprNode* processAssignment(vector<string> progline);
	static bool isValidExpr(vector<string> expr);
};
