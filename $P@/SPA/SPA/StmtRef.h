#pragma once
#include "stdAfx.h"
#include "ASTStmtNode.h"
#include "CFGNode.h"

class StmtRef
{
private:
	ASTStmtNode* astRef;
	CFGNode* cfgRef;

	PROG_LINE p;
	STMT s;

public:
	StmtRef(PROG_LINE, STMT);

	StmtRef(PROG_LINE, STMT, ASTStmtNode*);

	StmtRef(PROG_LINE, STMT, ASTStmtNode*, CFGNode*);

	void setASTStmtNode(ASTStmtNode*);

	void setCFGNode(CFGNode*);

	ASTStmtNode* getASTStmtNode();

	CFGNode* getCFGNode();

	PROG_LINE getProgLine();

	STMT getStmtNo();

};

