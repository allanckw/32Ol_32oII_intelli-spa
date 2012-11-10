#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "ASTNode.h"
#include "ASTStmtLstNode.h"
#include "ASTStmtNode.h"
#include "ASTExprNode.h"
#include "CallsTable.h"
#include "ModifiesTable.h"
#include "UsesTable.h"
#include "ParentTable.h"
#include "FollowsTable.h"

class DesignExtractor
{
private:
	static void buildFirstRound();
	static void buildOtherTables(PROC);
	static void CompleteExtraction();
	
public:
	static void extractDesign();
};