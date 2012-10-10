#include "StdAfx.h"
#include "AssignmentParser.h"



AssignmentParser::AssignmentParser(void)
{
}


AssignmentParser::~AssignmentParser(void)
{
}

int AssignmentParser::getOperatorWeight(string token)
{
	if (token == "+" || token == "-") //+ 0 has weight 1
		return 1; 
	else if (token == "*") // * has weight 2
		return 2;
	else
		return 0; //else weightless
}

// Test if token is an operator    
bool AssignmentParser::isOperator(string token)    
{    
    return token == "+" || token == "-" ||  
           token == "*";  
}    
  
int AssignmentParser::compareOprPrecedence( string token1, string token2)    
{    
	 return getOperatorWeight(token1) - getOperatorWeight(token2);
}

ASTNode* AssignmentParser::ProcessAssignment(ASTNode* procNode, int &line, vector<string> progline)
{
	stack<string> operators;
	stack<ASTNode*> operands;
	int index = 0;

	// For each token   
	for ( int i = 0; i < progline.size(); i++ )   
	{
		//http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
		//http://www.technical-recipes.com/2011/a-mathematical-expression-parser-in-java/
		string token = progline[i]; 

		if (AssignmentParser::isOperator(token))
		{
			
		}
		else
		{

		}
	}
	
	return procNode;
}