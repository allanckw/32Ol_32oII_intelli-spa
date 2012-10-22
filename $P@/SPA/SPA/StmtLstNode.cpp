#include "StdAfx.h"
#include "StmtLstNode.h"

StmtLstNode::StmtLstNode()
{
	this->nodeType = ASTNode::NodeType::StmtLst;
}


StmtLstNode::~StmtLstNode(void)
{
}

int StmtLstNode::getSize()
{
	return children.size();
}

ASTNode* StmtLstNode::addChild(ASTNode* node)
{
	if(node->getType() == Assign || node->getType() == If || node->getType() == While || node->getType() == Call)
		children.push_back(node);
	else
		throw SPAException("StmtLstNode cannot accept weird nodes as children");
}