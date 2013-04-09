#pragma once
#include "AstNode.h"
#include "ASTStmtNode.h"



class ASTStmtLstNode :
	public ASTNode
{
public:
	ASTStmtLstNode();
	int getSize() const;
	virtual ASTNode* addChild(ASTNode*);
};

