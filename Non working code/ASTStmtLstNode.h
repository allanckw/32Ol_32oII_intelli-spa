#pragma once
#include "AstNode.h"
#include "ASTStmtNode.h"



class ASTStmtLstNode :
	public ASTNode
{
public:
	ASTStmtLstNode();
	~ASTStmtLstNode(void);
	int getSize();
	virtual ASTNode* addChild(ASTNode*);
};

