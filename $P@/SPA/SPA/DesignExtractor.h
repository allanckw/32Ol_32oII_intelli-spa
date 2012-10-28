#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "ASTNode.h"
#include "StmtLstNode.h"
#include "StmtNode.h"
#include "ExprNode.h"
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
	
public:
	static void extractDesign();
};