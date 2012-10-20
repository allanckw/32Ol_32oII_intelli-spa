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
	static CallsTable c;
	static ModifiesTable m;
	static UsesTable u;
	static ParentTable p;
	static FollowsTable f;
	
public:
	DesignExtractor(CallsTable& ca, ModifiesTable& mo,
	UsesTable& us, ParentTable& pa, FollowsTable& fo);
	~DesignExtractor(void);
};

