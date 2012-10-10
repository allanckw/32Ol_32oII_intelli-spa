#pragma once
#include "astnode.h"

class StmtNode : public ASTNode
{
protected:
	int StmtNumber;

public:
	StmtNode(void);
	virtual ~StmtNode(void);
	virtual int GetStmtNumber() = 0;
};

