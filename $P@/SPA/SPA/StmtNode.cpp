#include "StdAfx.h"
#include "StmtNode.h"
#include "ASTNode.h"
#include "PKB.h"
#include "ExprNode.h"
#include "StmtLstNode.h"

StmtNode::StmtNode(int stmtNo, NodeType nodeType, Index value)
{
	if (stmtNo < 0){
		throw SPAException("Invalid Statement Number, must be more than 0");
	}
	else if (getType() != Assign && getType() != Call && getType() != While && getType() != If){
		throw SPAException("Invalid Type for Statements, Expected Assign/Call/While/If");
	}
	else{
		this->nodeType = nodeType;
		stmtNumber = stmtNo;
		this->setValue(value);
	}
}

ASTNode* StmtNode::AddChild(ASTNode* c)
{
	int childLoc=this->children.size();

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
			addChildToWhile(c, childLoc);
		else if (getType() == If)
			addChildToIF(c, childLoc);
		else if (getType() == Assign)
			addChildToAssign(c, childLoc);
		else
			throw SPAException("Invalid Type for Statements, Expected Assign/Call/While/If");
	}
	return this;
}

ASTNode* StmtNode::AddChild(ASTNode* c, int childLoc)
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
			addChildToWhile(c, childLoc);
		else if (getType() == If)
			addChildToIF(c, childLoc);
		else if (getType() == Assign)
			addChildToAssign(c, childLoc);
		else
			throw SPAException("Invalid Type for Statements, Expected Assign/Call/While/If");
	}
	return this;
}

void StmtNode::addChildToWhile(ASTNode* c, int childLoc)
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

void StmtNode::addChildToIF(ASTNode* c, int childLoc)
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

void StmtNode::addChildToAssign(ASTNode* c, int childLoc)
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

StmtNode::~StmtNode()
{
}

int StmtNode::getStmtNumber()
{
	return stmtNumber;
}


void StmtNode::setValue(Index i)
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
			ExprNode* varNode = new ExprNode(ASTNode::NodeType::Variable, i);
			this->AddChild(varNode, 1);
		}
	}
	
}