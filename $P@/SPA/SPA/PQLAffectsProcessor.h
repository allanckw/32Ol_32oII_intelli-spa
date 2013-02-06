#pragma once
#include "PKB.h"
#include "AffectsTable.h"
#include "ASTStmtNode.h"
#include "ASTNode.h"

class PQLAffectsProcessor
{
public:
	static bool computeAffects(STMT, STMT);
	static bool computeAffectsStar(STMT, STMT);
	static bool computeAffectsBip(STMT, STMT);
};

