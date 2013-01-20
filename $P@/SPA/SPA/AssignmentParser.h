#pragma once
#include "StdAfx.h"
#include "ASTExprNode.h"

typedef vector<string> MathExpression;

class AssignmentParser
{
private:
	static int getOperatorWeight(string token);
	static bool AssignmentParser::isOperator(string token);
	static int compareOprPrecedence( string token1, string token2);
	static void popOperator(string operators, string operands);
	static int getConstantValue(string s);
	
	static bool isValidExpr(MathExpression expr);

public:
	static ASTExprNode* processAssignment(MathExpression);
	
};
