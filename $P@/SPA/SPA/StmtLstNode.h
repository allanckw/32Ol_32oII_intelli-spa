#pragma once
#include "AstNode.h"
#include "StmtNode.h"



class StmtLstNode :
	public ASTNode
{
public:
	StmtLstNode();
	~StmtLstNode(void);
	int getSize();
	virtual ASTNode* addChild(ASTNode*);
};

