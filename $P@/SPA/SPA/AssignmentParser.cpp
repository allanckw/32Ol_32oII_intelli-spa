#include "StdAfx.h"
#include "AssignmentParser.h"
#include "PKB.h"

int AssignmentParser::getOperatorWeight(string token)
{
	if (token == "+" || token == "-") //+ and - has weight 1
		return 1; 
	// * has weight 2, expandable for divide too, 
	// for expotential just make it to has weight 3 but that is out of scope for simple
	// It will become COMPLICATED =(
	else if (token == "*") 
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
    return (token == "+" || token == "-" ||  token == "*");
}    
  
int AssignmentParser::compareOprPrecedence(string opr1, string opr2)    
{    
	 if (opr1 == ";" || opr2 == ";")
		return -1;

	 return getOperatorWeight(opr1) - getOperatorWeight(opr2);
}

//Validating incorrect expression, half completed.. 
bool AssignmentParser::isValidExpr(vector<string> expr)
{
	stack<string> brackets;
	stack<int> exprIndexStack;
	int exprIndex;
	if (expr.at(expr.size() - 1) != ";")
			throw SPAException("Invalid Token Found: Expected ; at the end of the expression");

	for ( int i = 0; i < expr.size() - 1; i++ ) {
		string token = expr[i]; 
		if (i == 0 && AssignmentParser::isOperator(token)){ //the first token can never be an operator
			return false;
		}else if (i == 0 && !AssignmentParser::isOperator(token)) {
			exprIndex = 0;
		} else if (token == ";"){
			break;
		} else {
			if (token == "("){
				brackets.push(token);
				exprIndexStack.push(exprIndex);
				exprIndex = 0;
			} else if (token == ")" && brackets.size() == 0){
				return false;
			} else if (token == ")" && brackets.size() > 0){
				brackets.pop();
				exprIndex = exprIndexStack.top();
				exprIndexStack.pop();
			} else {
				exprIndex += 1;
			}

		}
	}

	if (brackets.size() > 0 || exprIndex % 2 != 0)
		return false;
	else
		return true;
}

//http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
//http://www.technical-recipes.com/2011/a-mathematical-expression-parser-in-java/
//Shunting Yard Algorithm Tested to Work With No Brackets, Brackets May Contain Bugs as it is not Tested
//But The General Idea is there - Allan
ExprNode* AssignmentParser::processAssignment(vector<string> expr)
{
	stack<string> operators; 
	stack<ExprNode*> operands;

	stack<string> subExprBrackets;
	vector<string> subExpr;

	for ( int i = 0; i < expr.size(); i++ ) {
		string token = expr[i]; 
		if (token == "/" || token == "^" || token == "%")
			throw SPAException("Operator not supported, use '+', '-' or '*' only");
		
		if (token == ";")
			break; 
		//Bracket For Shunting Yard, May Contain Bugs.. Not Tested
		if (token == "("){
			//Create the expression, in vector form until ")", ")" not found then Throw exception
			subExprBrackets.push(token);
		}
		else if (subExprBrackets.size() > 0){
			if (token == "("){
				subExprBrackets.push(token);
				subExpr.push_back(token);
			} else if (token == ")" && subExprBrackets.size() == 1) {
				subExprBrackets.pop();
				subExpr.push_back(";");
				operands.push(AssignmentParser::processAssignment(subExpr));
			} else if (token == ")" && subExprBrackets.size() > 1) {
				subExprBrackets.pop();
				subExpr.push_back(token);
			} 
		}
		else if (AssignmentParser::isOperator(token)) {
			if (operators.empty()) {//if the operator stack is empty simply push
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
					cout << token << " cannot be found " << endl;
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
