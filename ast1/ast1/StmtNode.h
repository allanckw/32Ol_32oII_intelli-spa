#pragma once
#include "astnode.h"

class StmtNode :
	public ASTNode
{
public:
	StmtNode(void);
	virtual ~StmtNode(void);
	virtual int GetStmtNo() = 0;
};

