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
private:
	static CFGNode* processProcedure(ASTNode* procedureNode);
	static CFGNode* processWhile(ASTNode* procedureNode,int* s, ASTStmtNode *stmtNode);
	static CFGNode* processIf(ASTNode* procedureNode,int* s, ASTStmtNode *stmtNode, CFGNode *prevNode);
	//Process While Node
	//Process If Node

};

