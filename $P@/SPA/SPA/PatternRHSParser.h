#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "Helper.h"
#include "ASTExprNode.h"

class PatternRHSParser
{
typedef vector<string> MathExpression;

private:
	static int getOperatorWeight(string token);
	static bool isOperator(string token);
	static int compareOprPrecedence( string token1, string token2);
	static void popOperator(string operators, string operands);
	static int getConstantValue(string s);
	
	static bool isValidExpr(MathExpression expr);

public:
	static ASTExprNode* processAssignment(MathExpression);
	

};

