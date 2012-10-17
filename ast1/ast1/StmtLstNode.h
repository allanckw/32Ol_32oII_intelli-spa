#pragma once
#include "AstNode.h"
#include "StmtNode.h"

//TODO: For HW, Full implementation of StmtLstNode

class StmtLstNode :
	public StmtNode
{
public:
	StmtLstNode(void);
	~StmtLstNode(void);
	int getSize();
	void addChild(ASTNode*);
};

