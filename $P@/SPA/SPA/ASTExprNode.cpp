#pragma once
#include "StdAfx.h"
#include "ASTExprNode.h"
#include "PKB.h"
#include "Helper.h"
#include <typeinfo.h>
#include <sstream>

/**
* Constructor for the ASTExprNode, initiate the node’s type and the value as integer, usually used for constants.
* @param nodeType	The nodeType of expression node (Constant, Operator and Variable)
* @param value	The value of the Expression Represented, 
* 0 for +, 1 for -, 2 for * for operator
* Actual value for constant
* VARIndex for variable
*/
ASTExprNode::ASTExprNode(NodeType nodeType, int value)
{
	if (nodeType != Constant && nodeType != Operator && nodeType != Variable){
		throw SPAException("Invalid Type for Expression, Expected Constant/Variable/Operator");
	
	} else {
	
		this->nodeType = nodeType;
		if (this->getType() == Variable){
			if (value < 0 || value > (PKB::variables.getSize() - 1))
			{
				throw SPAException("Invalid Variable Index, please check the VARTable");
			}else{
				this->value = value;
			}
		}else if(this->getType() == Constant){
			this->value = value;
		}else if(this->getType() == Operator){
			if (value < 0 || value > 2){
				throw SPAException("Invalid Operator Type, 0 for +, 1 for -, 2 for *");
			} else {
				this->value = value;
			}
		}
	}
}

/*Constructor for the ASTExprNode, initiate the node’s type and expression.
* Throws Exception When NodeType is a constant, but value is not of type integer (DIGIT+) or
* NodeType is Variable, but value is not cannot be found in VARTable or
* NodeType is Operator, but value is not of ‘+’, ‘-‘ or ‘*’
* @param nodeType	The nodeType of expression node (Constant, Operator and Variable)
* @param value	The value of the Expression Represented
* An expression can be a variable (e.g.: x), a constant (e.g.: 10), or a mathematical symbol “+”, “-“, “*”, etc
*/
ASTExprNode::ASTExprNode(NodeType nodeType, Expr value)
{
	this->nodeType = nodeType;
	if (this->getType() == Variable)
	{
		int idx = PKB::variables.getVARIndex(value);
		if (idx == -1)
		{
			throw SPAException("Invalid Variable, please check the VARTable");
		}
		else
		{
			this->value = idx;
		}
	}
	else if(this->getType() == Constant)
	{
		if (!Helper::isNumber(value))
		{
			throw SPAException("Invalid constant value!");
		}
		else
			this->value = atoi(value.c_str());
	}
	else if(this->getType() == Operator)
	{
		setOperator(value);
	}
}

/**
* This method will be used to add child under the AST Node
* @param c	The AST Child Node to add
* @param childLoc	The Child Location, must be 1 or 2, 1 denotes left, 2 denotes right, other exception will be thrown
* @return itself
*/
ASTNode* ASTExprNode::addChild(ASTNode* c, int childLoc){

	if (childLoc <= 0 )
	{
		throw SPAException("Invalid Parameter: Child Location must be at least 1. 1 denotes 1st child, 2 denotes 2nd child, etc...");
	}
	else if (childLoc > 2){
		throw SPAException("Invalid Parameter: There is only a maximum of 2 children in Expression Nodes!");
	}
	else if (this->getType() == Constant){
		throw SPAException("Invalid Operation: Cannot add a child to constant node");
	}
	else if (this->getType() == Variable){
		throw SPAException("Invalid Operation: Cannot add a child to variable node");
	}
	else if (this->getType() == Operator){

		if (c->getType() != Operator && c->getType() != Constant && c->getType() != Variable){
			throw SPAException("Invalid Operation: Child of Operators must be of type Expression (Operator/Constant/Variable)");
		}
		else
		{
			if (children.size() == 0 && childLoc == 1) {
				children.push_back(c);
			}
			else if (children.size() == 1 && childLoc == 2) {
				children.push_back(c);
			}
			else if (children.size() == 0 && childLoc == 2) {
				throw SPAException("Invalid Operation: First child must be added first before adding the second!");
			}
			else {//do replacement
				//http://www.java2s.com/Tutorial/Cpp/0320__vector/Insertelementbyindex.htm
				vector<ASTNode*>::iterator itr;
				itr = children.begin() + (childLoc - 1);
				*itr = c;
			
			}
		}
	}
	return this;
}

/**
* This method will be used to match the value of the expression node
* @param expr	the expression to test if it matches the value of the node
* @Return true if the node is of the correct value, false otherwise
*/
bool ASTExprNode::isMatched(Expr expr)
{
	if (this->getType() == Variable)
	{
		return isVARMatched(expr);
	}
	else if(this->getType() == Constant)
	{
		int output; 
		istringstream (expr) >> output;
		return (output == value);
	}
	else if(this->getType() == Operator)
	{
		return isOPRMatched(expr);
	}
	 
	return false;
}

//Check if variable passed is the same as in the varNode
bool ASTExprNode::isVARMatched(string varName)
{
	int index = PKB::variables.getVARIndex(varName);
	if (index == -1) {
		cout << "Variable Not found!" << endl;
		return false;
	} else 	{
		return (index == value);
	}
}

bool ASTExprNode::isOPRMatched(string opr)
{
	if (opr !=  "+" || opr != "-" || opr != "*") {
		cout << "Invalid Operator Type! Expected '+', '-' or '*' " << endl;
		return false;
	} else {
		return (opr == getOperator());
	}
}

string ASTExprNode::getOperator(){
	if (value == 0)
		return "+";
	else if (value == 1)
		return "-";
	else if (value == 2)
		return "*";
	else
		throw SPAException("Invalid Operator type");
	
}

void ASTExprNode::setOperator(string opr)
{
	if (opr == "+")
		this->value = 0;
	else if (opr == "-")
		this->value = 1;
	else if (opr == "*")
		this->value = 2;
	else
		throw SPAException("Invalid Operator Type, Expected '+', '-', '*'");
}