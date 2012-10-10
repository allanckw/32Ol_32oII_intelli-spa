#pragma once
#include "stmtnode.h"
class ContainerNode :
	public StmtNode
{
private:
	vector<StmtNode> Children;
	VarNode ControlVar;

public:
	ContainerNode(int, ASTNode, NodeType);
	~ContainerNode();
	ASTNode getChild(int);
	void AddStatements(ASTNode, int);
}