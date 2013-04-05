#pragma once
#include "StdAfx.h"
#include "ASTStmtNode.h"
#include "ASTNode.h"
#include "PKB.h"
#include "ASTExprNode.h"
#include "ASTStmtLstNode.h"

/**
* Constructor: Used to create a new ASTStmtNode
* @param stmtNo The statement number
* @param nodeType The AST NodeType of statement
* @param value The Index could be PROC or VAR
*/
ASTStmtNode::ASTStmtNode(int stmtNo, NodeType nodeType, Index value)
{
	stmtNo = stmtNo + 1;
	if (stmtNo <= 0){
		throw SPAException("Invalid Statement Number, must be more than 0");
	}
	else if (nodeType != Assign && nodeType != Call && nodeType != While && nodeType != If){
		throw SPAException("Invalid Type for Statements, Expected Assign/Call/While/If");
	}
	else{
		this->nodeType = nodeType;
		stmtNumber = stmtNo;
		this->setValue(value);
	}
}

/**
* This method will be used to add child under the AST Node
* @param c The Child of the ASTStmtNode
* @return its reference
*/
ASTNode* ASTStmtNode::addChild(ASTNode* c)
{
	int childLoc=this->children.size()+1;

	if(getType() == Call){
		throw SPAException("Invalid Operation: No Children can be added to Call Nodes");
	}
	else if (childLoc <= 0 ){
		throw SPAException("Invalid Parameter: Child Location must be at least 1. 1 denotes 1st child, 2 denotes 2nd child, etc...");
	}
	else if (getType() == If && childLoc > 3){
		throw SPAException("Invalid Parameter: There is only a maximum of 3 children in If Statements Nodes!");
	}
	else if (childLoc > 2 && (getType() == While || getType() == Assign)){
		throw SPAException("Invalid Parameter: There is only a maximum of 2 children in While/Assignment Nodes!");
	}
	else{
		if (getType() == While)
		{
			addChildToWhile(c, childLoc);
			c->setAncestor(this);
		}
		else if (getType() == If)
		{
			addChildToIF(c, childLoc);
			c->setAncestor(this);
		}
		else if (getType() == Assign)
		{
			addChildToAssign(c, childLoc);
			c->setAncestor(this);
		}
		else
			throw SPAException("Invalid Type for Statements, Expected Assign/Call/While/If");
	}
	return this;
}

/**
* This method will be used to add child under the AST Node
* @param c The Child of the ASTStatNode
* @param childLoc The Index of the Child of the ASTStmtNode
* @return its reference
*/
ASTNode* ASTStmtNode::addChild(ASTNode* c, int childLoc)
{
	if(getType() == Call){
		throw SPAException("Invalid Operation: No Children can be added to Call Nodes");
	}
	else if (childLoc <= 0 ){
		throw SPAException("Invalid Parameter: Child Location must be at least 1. 1 denotes 1st child, 2 denotes 2nd child, etc...");
	}
	else if (getType() == If && childLoc > 3){
		throw SPAException("Invalid Parameter: There is only a maximum of 3 children in If Statements Nodes!");
	}
	else if (childLoc > 2 && (getType() == While || getType() == Assign)){
		throw SPAException("Invalid Parameter: There is only a maximum of 2 children in While/Assignment Nodes!");
	}
	else{
		if (getType() == While)
		{
			addChildToWhile(c, childLoc);
			c->setAncestor(this);
		}
		else if (getType() == If)
		{
			addChildToIF(c, childLoc);
			c->setAncestor(this);
		}
		else if (getType() == Assign)
		{
			addChildToAssign(c, childLoc);
			c->setAncestor(this);
		}
		else
			throw SPAException("Invalid Type for Statements, Expected Assign/Call/While/If");
	}
	return this;
}

/**
* This method will be used to add child under the AST While Node
* @param c The Child of the AST While Node
* @param childLoc The Index of the Child of the AST While Node
*/
void ASTStmtNode::addChildToWhile(ASTNode* c, int childLoc)
{
	if (childLoc == 1 && c->getType() != Variable) {
		throw SPAException("Invalid Operation: First Child of While must be a variable node (Control)");
	}
	else if (childLoc == 2 && c->getType() != StmtLst) {
		throw SPAException("Invalid Operation: Second Child of Assignment must be an expression (Operator/Variable/Constant)");
	}
	else if (children.size() == 0 && childLoc == 1) {
		children.push_back(c);
	}
	else if (children.size() == 1 && childLoc == 2) {
		children.push_back(c);	
	}
	else {
		//do replacement
		vector<ASTNode*>::iterator itr;
		itr = children.begin() + (childLoc - 1);
		*itr = c;
	}
}

/**
* This method will be used to add child under the AST If Node
* @param c The Child of the AST If Node
* @param childLoc The Index of the Child of the AST If Node
*/
void ASTStmtNode::addChildToIF(ASTNode* c, int childLoc)
{
	if (childLoc == 1 && c->getType() != Variable) {
		throw SPAException("Invalid Operation: First Child of If must be a variable node (Control)");
	}
	else if (c->getType() != StmtLst && (childLoc == 2 || childLoc == 3)) {
		throw SPAException("Invalid Operation: Second / Third Child of If must be a statementLst Node");
	}
	else if (children.size() == 0 && childLoc == 1) {
		children.push_back(c);
	}
	else if (children.size() == 1 && childLoc == 2) {
		children.push_back(c);
	}
	else if (children.size() == 2 && childLoc == 3) {
		children.push_back(c);
	}
	else {
		//do replacement
		vector<ASTNode*>::iterator itr;
		itr = children.begin() + (childLoc - 1);
		*itr = c;
	}
}

/**
* This method will be used to add child under the AST Assignment Node
* @param c The Child of the AST Assignment Node
* @param childLoc The Index of the Child of the AST Assignment Node
*/
void ASTStmtNode::addChildToAssign(ASTNode* c, int childLoc)
{
	if (childLoc == 1 && c->getType() != Variable) {
		throw SPAException("Invalid Operation: First Child of Assignment must be a variable");
	}
	else if (childLoc == 2 && c->getType() != Variable && c->getType() == Constant && c->getType() == Operator)  {
		throw SPAException("Invalid Operation: Second Child of Assignment must be an expression (Operator/Variable/Constant)");
	}
	else if (children.size() == 0 && childLoc == 1)  {
		children.push_back(c);
	}
	else if (children.size() == 1 && childLoc == 2)  {
		children.push_back(c);
	}
	else {
		//do replacement
		vector<ASTNode*>::iterator itr;
		itr = children.begin() + (childLoc - 1);
		*itr = c;
	}
}

/**
* This method will be used to get statement number of the AST Stmt Node
* @return the statement number of the node
*/
int ASTStmtNode::getStmtNumber()
{
	return stmtNumber;
}

/**
* This method will be used to set value for the AST Stmt Node
* @param i The Index of the VARIABLE
*/
void ASTStmtNode::setValue(Index i)
{
	if (i < 0)
		throw SPAException("Invalid Parameter, Index cannot be less than 0");

	if (this->getType() == Call)
	{
		if (i > (PKB::procedures.getSize() - 1))
		{
			throw SPAException("Invalid Parameter, Procedure Index cannot be found!");
		}
		else
		{
			this->value = i; //Sets the value of the CallNode to point to the Procedure Idx
		}
	}
	else if (this->getType() == Assign || this->getType() == While || this->getType() == If)
	{
		if (i > (PKB::variables.getSize() - 1))
		{
			throw SPAException("Invalid Parameter, Variable Index cannot be found!");
		}
		else
		{
			this->value = i;
			//Set first child after setting value
			ASTExprNode* varNode = new ASTExprNode(ASTNode::Variable, i);
			this->addChild(varNode, 1);
		}
	}
	
}