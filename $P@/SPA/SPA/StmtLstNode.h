#pragma once
#include "AstNode.h"
#include "StmtNode.h"



class StmtLstNode :
	public ASTNode
{
public:
	StmtLstNode(NodeType type);
	~StmtLstNode(void);
	int getSize();
	void addChild(ASTNode*);
};

