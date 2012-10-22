#include "StdAfx.h"
#include "AssignmentParser.h"
#include "PKB.h"

int AssignmentParser::getOperatorWeight(string token)
{
	if (token == "+" || token == "-") //+ 0 has weight 1
		return 1; 
	else if (token == "*") // * has weight 2
		return 2;
	else
		return 0; //else weightless
}

int AssignmentParser::getConstantValue(string s) //first char of name cannot be digit
{
	//Check if first char of name is digit or character
	int output; 
	istringstream (s) >> output;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
	return output;
	//return (output != -858993460);
}

// Test if token is an operator    
bool AssignmentParser::isOperator(string token)    
{    
    return token == "+" || token == "-" ||  
           token == "*";
}    
  
int AssignmentParser::compareOprPrecedence( string opr1, string opr2)    
{    
	 if (opr1 == ";" || opr2 == ";")
	 {
		 return -1;
	 }
	 return getOperatorWeight(opr1) - getOperatorWeight(opr2);
}

//http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
//http://www.technical-recipes.com/2011/a-mathematical-expression-parser-in-java/
ExprNode* AssignmentParser::processAssignment(vector<string> expr)
{
	stack<string> operators; 
	stack<ExprNode*> operands;
	int index = 0;
 
	if (expr.at(expr.size() -1 ) != ";")
			throw SPAException("Invalid Token Found: Expected ;");


	for ( int i = 0; i < expr.size(); i++ ) {
		string token = expr[i]; 
		if (token == "/" || token == "^" || token == "%")
			throw SPAException("Operator not supported, use '+', '-' or '*' only");
		
		if (token == ";")
			break; 

		if (token == "("){
			//Create the expression, in vector form until ")", ")" not found then Throw exception
		}
		else if (AssignmentParser::isOperator(token) || token == ";") {
			//if the operator stack is empty simply push
			if (operators.empty()) {
				operators.push(token);
			} else {
				//Compare the precedence of + with the top of the stack (+)
				if (AssignmentParser::compareOprPrecedence(token, operators.top()) > 0)	{
					operators.push(token); //if it is greater, push
				} else {
					ExprNode* oprNode = new ExprNode(ASTNode::NodeType::Operator, token);
					
					ExprNode* rightChild = operands.top();
					operands.pop();
					
					ExprNode* leftChild = operands.top();
					operands.pop();

					oprNode->addChild(leftChild, 1);
					oprNode->addChild(rightChild, 2);

					operands.push(oprNode);
				}
			}
		}
		else { //it is an operands
			int value = AssignmentParser::getConstantValue(token);
			if (value != -858993460){
				ExprNode* constNode = new ExprNode(ASTNode::NodeType::Constant, value);
				operands.push(constNode);
			}
			else {
				VAR i = PKB::variables.getVARIndex(token);
				if (i == -1){
					throw SPAException("Variable cannot be found in assignment statement!");
				}
				else{
					ExprNode* varNode = new ExprNode(ASTNode::NodeType::Variable, i);
					operands.push(varNode);
				}
			} 
		}
	}
	
	while (operators.size() > 0)
	{
		ExprNode* oprNode = new ExprNode(ASTNode::NodeType::Operator, operators.top());
					
		ExprNode* rightChild = operands.top();
		operands.pop();
					
		ExprNode* leftChild = operands.top();
		operands.pop();

		oprNode->addChild(leftChild, 1);
		oprNode->addChild(rightChild, 2);

		operands.push(oprNode);
		operators.pop();
	}

	return operands.top();
}


AssignmentParser::~AssignmentParser(void)
{
}