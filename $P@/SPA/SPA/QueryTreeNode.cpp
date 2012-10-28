#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "PKB.h"

QueryTreeNode::QueryTreeNode()
{
}

void QueryTreeNode::setParent(QueryTreeNode* p)
{
	this->parentNode = p;
}
	
void QueryTreeNode::setChild(QueryTreeNode* c)
{
	this->childNode = c;
}

QueryTreeNode* QueryTreeNode::getParent()
{
	return this->parentNode;
}
	
QueryTreeNode* QueryTreeNode::getChild()
{
	return this->childNode;
}

QueryTreeNode::QTNodeType QueryTreeNode::getNodeType()
{
	return this->nodeType;
}