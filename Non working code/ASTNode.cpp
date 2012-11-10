#pragma once
#include "ASTNode.h"


ASTNode::ASTNode()
{
	root = false;
}

ASTNode::ASTNode(NodeType type)
{
	if (type != Procedure && type != Program)
	{
		throw SPAException("Invalid NodeType: Please use ASTStmtNode to denote stmts, ASTStmtLstNode to denote stmtLst, and ASTExprNode to denote expressions");
	}
	else
	{
		this->nodeType = type;
	}
	root = false;
}

ASTNode::ASTNode(NodeType type, PROC p)
{
	if (type != Procedure && type != Program)
	{
		throw SPAException("Invalid NodeType: Please use ASTStmtNode to denote stmts, ASTStmtLstNode to denote stmtLst, and ASTExprNode to denote expressions");
	}
	else
	{
		this->nodeType = type;
		this->value = p;
	}
	root = false;
}

int ASTNode::getValue()
{
	return this->value;
}

//Set parent to the node, return its reference
ASTNode* ASTNode::setParent(ASTNode * p)
{
	if (this->root){
		throw SPAException("Node is already the root, Unable to set parent");
		//Exception.. already the root set wat parent :@
	}
	else{
		this->parent = p;
	}
	return this;
}

ASTNode* ASTNode::addChild(ASTNode* c)
{
	if(this->getType() == Program)
	{
		if (c->getType() != Procedure){
			throw SPAException("Invalid Operation: Program Node Children must be of type Procedures");
		}
		else{
			this->children.push_back(c);
		}
	}
	else if (this->getType() == Procedure)
	{
		if (this->children.size() == 1){
			throw SPAException("Invalid Operation: Procedure Node Can only have one children of type StmtLst");
		}
		else if (this->children.size() == 0)
		{
			if (c->getType() != StmtLst){
				throw SPAException("Invalid Operation: Procedure Node Children must be of type StmtLst");
			}
			else{
				this->children.push_back(c);
			}
		}
	}
	else
	{
		throw SPAException("Invalid Operation: Please use ASTStmtNode to denote stmts, ASTStmtLstNode to denote stmtLst, and ASTExprNode to denote expressions");
	}
		
	return this;
}



void ASTNode::setRoot(int PROCIndex)
{
	if(getType() != Program){
		throw SPAException("Invalid Operation: Root must be a Program Node");
	}else{
		this->root = true;
		this->value = PROCIndex;
	}
}

bool ASTNode::isRoot()
{
	return this->root;
}

bool ASTNode::isHasChildren()
{
	return (children.size() > 0);
}


ASTNode::NodeType ASTNode::getType()
{
	return this->nodeType;
}

ASTNode* ASTNode::getChild(int i)
{
	if (i < 0 || i > children.size() -1)
	{
		throw SPAException("Index Out of Bound Exception, No Child at Index " + i);
	}
	return this->children[i];
}

ASTNode* ASTNode::getParent()
{
	return this->parent;
}

ASTNode::~ASTNode(void)
{
}

//ASTNode* ASTNode::SetRightSibling(ASTNode * s)
//{
//	this->rightSibling = s;
//	return this;
//}

//bool ASTNode::isHasRightSibling()
//{
//	return (this->rightSibling == NULL);
//}
