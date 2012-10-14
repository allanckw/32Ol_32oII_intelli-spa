#pragma once
#include "AstNode.h"
#include "StmtNode.h"

//TODO: For HW, Full implementation of StmtLstNode

class StmtLstNode :
	public ASTNode
{
private:
	vector<StmtNode*> stmts;

public:
	StmtLstNode(void);
	~StmtLstNode(void);
	int getSize();
};

