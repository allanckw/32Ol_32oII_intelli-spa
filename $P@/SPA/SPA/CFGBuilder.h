#pragma once
#include "StdAfx.h"
#include "SPAException.h"
#include "ASTNode.h"
#include "CFGNode.h"
#include "PKB.h"
#include "ASTStmtLstNode.h"
#include "ASTStmtNode.h"

class CFGBuilder
{

public:

	static void buildCFG();
	static void traverseCFGToPopulateNext();

private:
	static set<CFGNode*> visited;
	static CFGNode* processProcedure(ASTNode* procedureNode);
	static CFGNode* processWhile(ASTNode* procedureNode,int* s, ASTStmtNode *stmtNode);
	static CFGNode* processIf(ASTNode* procedureNode,int* s, ASTStmtNode *stmtNode, CFGNode *ifNode);

	static void traverseCFGWhile(CFGNode* whileNode);
	static vector<PROG_LINE> getPrevFromWhile(CFGNode* whileNode);
	static CFGNode* traverseCFGIf(CFGNode* ifNode);
};

