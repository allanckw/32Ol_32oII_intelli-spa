#pragma once
#include "StdAfx.h"
#include "ASTStmtLstNode.h"

/**
* This method will be used to create a new ASTStmtListNode
*/
ASTStmtLstNode::ASTStmtLstNode()
{
	this->nodeType = ASTNode::StmtLst;
}


ASTStmtLstNode::~ASTStmtLstNode(void)
{
}

/**
* This method will be used to get the number of children under it
*/
int ASTStmtLstNode::getSize()
{
	return children.size();
}

/**
* This method will be used to add child under the AST Stmt List Node
* @param node The Child of the AST Stmt List Node
*/
ASTNode* ASTStmtLstNode::addChild(ASTNode* node)
{
	if(node->getType() == Assign || node->getType() == If || node->getType() == While || node->getType() == Call)
		children.push_back(node);
	else
		throw SPAException("Invalid Node Type as children, Expected Type While/If/Assign/Call");
	return this;
}