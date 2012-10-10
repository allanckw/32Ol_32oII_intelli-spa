#include "StdAfx.h"
#include "ASTNode.h"


ASTNode::ASTNode(NodeType type, string name)
{
	this->nType = type;
	this->name = name;
	isRoot = false;
}

ASTNode::ASTNode()
{

}


ASTNode::~ASTNode(void)
{
}

//Set parent to the node, return its reference
ASTNode* ASTNode::SetParent(ASTNode * p)
{
	if (this->isRoot){
		//Exception.. already the root set wat parent :@
	}
	else{
		this->parent = p;
	}
	return this;
}

ASTNode* ASTNode::AddChild(ASTNode* c)
{
	this->children.push_back(c);
	
	return this;
}

//Set right sibling to the node, return its reference
ASTNode* ASTNode::SetRightSibling(ASTNode * s)
{
	this->rightSibling = s;
	return this;
}

void ASTNode::SetRoot()
{
	this->isRoot = true;
}