#include "StdAfx.h"
#include "StmtLstNode.h"

//TODO: For HW, Full implementation of StmtLstNode

StmtLstNode::StmtLstNode(void)
{
}


StmtLstNode::~StmtLstNode(void)
{
}

int StmtLstNode::getSize()
{
	return children.size();
}

void StmtLstNode::addChild(ASTNode* node)
{
	if(node->getType() == Assign || node->getType() == If || node->getType() == While)
		children.push_back(node);
	else
		throw SPAException("StmtLstNode cannot accept weird nodes as children");
}