#pragma once
#include "ASTNode.h"

/**
* Default Constructor for the ASTNode,
*/
ASTNode::ASTNode()
{
	root = false;
}

/**
* Constructor for the ASTNode, with type being initialized, type must be of type procedure or program
* otherwise an exception will be thrown as other Node Types uses inherited version of ASTNodes
* @param type The node type of the ASTNode
*/
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

/**
* Constructor for the ASTNode, with type and procedure being initialized, used to create ASTNodes for procedures
* @param type	The node type of the ASTNode - Procedure only, otherwise an exception will be thrown 
* @param p	the procedure of the ASTNode
*/
ASTNode::ASTNode(NodeType type, PROC p)
{
	if (type != Procedure)
	{
		throw SPAException("Constructor is used for procedure only!");
	}
	else
	{
		this->nodeType = type;
		this->value = p;
	}
	root = false;
}

/**
* Returns the value of the ASTNode
* @param type	The node type of the ASTNode
* @param p	the procedure of the ASTNode
*/
int ASTNode::getValue() const
{
	return this->value;
}

/**
* Set parent to the node and return its reference
* @param p	the parent node to set
* @return its reference
*/
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

void ASTNode::setAncestor(ASTNode* p)
{
	if (this->root){
		throw SPAException("Node is already the root, Unable to set ancestor");
		//Exception.. already the root set wat parent :@
	}
	else{
		this->ancestorNode = p;
	}
}

ASTNode* ASTNode::getAncestor() const
{
	return this->ancestorNode;
}

/**
* Add a child node to the ASTNode, 
* @param c the child node to add, Type of child must be procedure when adding to a ProgramNode and type of child must be of type stmtLst when adding to a procedure node
* @return its reference
*/
ASTNode* ASTNode::addChild(ASTNode* c)
{
	if(this->getType() == Program)
	{
		if (c->getType() != Procedure){
			throw SPAException("Invalid Operation: Program Node Children must be of type Procedures");
		}
		else{
			this->children.push_back(c);
			c->setAncestor(this);
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
				c->setAncestor(this);
			}
		}
	}
	else
	{
		throw SPAException("Invalid Operation: Please use ASTStmtNode to denote stmts, ASTStmtLstNode to denote stmtLst, and ASTExprNode to denote expressions");
	}
		
	return this;
}


/**
* Set the root of an ASTNode, the NodeType must be of type program or an exception will be thrown
* @param p the first procedure index that will denote the value of the the root node
*/
void ASTNode::setRoot(PROC p)
{
	if(getType() != Program){
		throw SPAException("Invalid Operation: Root must be a Program Node");
	}else{
		this->root = true;
		this->value = p;
	}
}

/**
* Check if the ASTNode is the root
* @return return true if it is the root, false otherwise
*/
bool ASTNode::isRoot()
{
	return this->root;
}

/**
* Return the children ASTNode of the current node 
* @return the list of ASTNodes
*/
vector<ASTNode*> ASTNode::getChildren() const
{
	return this->children;
}

/**
* Check if the ASTNode has children
* @return return true if it has children, false otherwise
*/
bool ASTNode::isHasChildren() 
{
	return (children.size() > 0);
}


/**
* Get the Node type of the ASTNode
* @return the node type
*/
ASTNode::NodeType ASTNode::getType() const
{
	return this->nodeType;
}

/**
* Get the child ASTNode at a specific index
* @param the index of the child node, starts from 0, 0 denotes left most child
* @return the child ASTNode  
*/
ASTNode* ASTNode::getChild(unsigned int i) const
{
	if (i > children.size() -1)
	{
		throw SPAException("Index Out of Bound Exception, No Child at Index " + i);
	}
	return this->children[i];
}

/**
* Get the parent ASTNode 
* @return the parent ASTNode  
*/
ASTNode* ASTNode::getParent() const
{
	return this->parent;
}
