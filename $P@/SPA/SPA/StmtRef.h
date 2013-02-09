#pragma once
#include "stdAfx.h"
#include "ASTNode.h"
#include "CFGNode.h"

class StmtRef
{
private:
	ASTNode* astRef;
	CFGNode* cfgRef;

	PROG_LINE p;
	STMT s;

public:
	StmtRef(PROG_LINE, STMT);

	StmtRef(PROG_LINE, STMT, ASTNode*);

	StmtRef(PROG_LINE, STMT, ASTNode*, CFGNode*);

	void setASTNode(ASTNode* n);

	void setCFGNode(CFGNode* n);

	ASTNode* getASTNode();

	CFGNode* getCFGNode();

	PROG_LINE getProgLine();

	STMT getStmtNo();

};

