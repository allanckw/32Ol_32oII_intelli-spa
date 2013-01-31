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
* This method will be used to get the number of statements in the statement list
* @return the number of statements in the statement list
*/
int ASTStmtLstNode::getSize()
{
	return children.size();
}

/**
* This method will be used to add a child node in the the ASTStmtList Node
* @param node The Child node to add to the ASTStmtListNode, must be of type Assign / If / While / Call or an exception will be thrown
* @return its reference
*/
ASTNode* ASTStmtLstNode::addChild(ASTNode* node)
{
	if(node->getType() == Assign || node->getType() == If || node->getType() == While || node->getType() == Call)
		children.push_back(node);
	else
		throw SPAException("Invalid Node Type as children, Expected Type While/If/Assign/Call");
	return this;
}