#include "StdAfx.h"
#include "ContainerNode.h"

ContainerNode::ContainerNode(int StmtNumber, ASTNode ControlVar, NodeType Type)
{
	this->StmtNumber = StmtNumber;
	this->ControlVar = ControlVar;
	this->nType = Type;
}

ContainerNode::~ContainerNode()
{
}

ASTNode ContainerNode::getChild(int i)
{
	if (i < 0 || i > Children.size())
	{
		//throw exception
	}

	return Children[i];
}

void ContainerNode::AddStatements(ASTNode s1, int StmtLstIndex)
{
	Children.push_back(s1);
}
