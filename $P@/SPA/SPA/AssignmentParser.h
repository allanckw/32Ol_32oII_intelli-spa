#pragma once
#include "StdAfx.h"
#include "ASTNode.h"

//TODO: For Allan, Implement Shunting Yard Algorithm, to combine later with nick's parser class

class AssignmentParser
{
private:

public:
	AssignmentParser(void);
	static int getOperatorWeight(string token);
	static bool AssignmentParser::isOperator(string token);
	static int compareOprPrecedence( string token1, string token2);
	static ASTNode* ProcessAssignment(ASTNode* procNode, int &line, vector<string> progline);

	static void popOperator(string operators, string operands);
 
	~AssignmentParser(void);
};
