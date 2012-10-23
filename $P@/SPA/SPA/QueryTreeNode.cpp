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
	return parentNode;
}
	
QueryTreeNode* QueryTreeNode::getChild()
{
	return childNode;
}