#pragma once
#include "StdAfx.h"
#include "ASTStmtLstNode.h"

ASTStmtLstNode::ASTStmtLstNode()
{
	this->nodeType = ASTNode::NodeType::StmtLst;
}


ASTStmtLstNode::~ASTStmtLstNode(void)
{
}

int ASTStmtLstNode::getSize()
{
	return children.size();
}

ASTNode* ASTStmtLstNode::addChild(ASTNode* node)
{
	if(node->getType() == Assign || node->getType() == If || node->getType() == While || node->getType() == Call)
		children.push_back(node);
	else
		throw SPAException("Invalid Node Type as children, Expected Type While/If/Assign/Call");
	return this;
}