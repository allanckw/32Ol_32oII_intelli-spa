#pragma once
#include "PKB.h"
#include "AffectsTable.h"
#include "ASTStmtNode.h"
#include "ASTNode.h"

class PQLAffectsProcessor
{
private:
	static bool isSatifyAffects(STMT a1, STMT a2);

public:
	static bool isAffects(STMT, STMT);
	static bool isAffectsStar(STMT, STMT);
	static bool isAffectsBip(STMT, STMT);

	static vector<STMT> AffectsBy(STMT);
	static vector<STMT> AffectsFrom(STMT);
};

