#pragma once
#include "StdAfx.h"
#include "ASTNode.h"

class AssignmentParser
{
private:
	//static stack<string> operators;
	//static stack<ASTNode*> operands;
	//stack<> partialTree;

public:
	AssignmentParser(void);
	static int getOperatorWeight(string token);
	static bool AssignmentParser::isOperator(string token);
	static int compareOprPrecedence( string token1, string token2);
	static ASTNode* ProcessAssignment(ASTNode* procNode, int &line, vector<string> progline);

	static void popOperator(string operators, string operands);
 
	~AssignmentParser(void);
};
