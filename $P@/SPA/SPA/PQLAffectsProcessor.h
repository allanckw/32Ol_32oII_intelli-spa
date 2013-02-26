#pragma once
#include "PKB.h"
#include "AffectsTable.h"
#include "ASTStmtNode.h"
#include "ASTNode.h"

class PQLAffectsProcessor
{
private:
	static bool isSatifyAffects(STMT a1, STMT a2);
	static bool isSatifyAffectsStar(STMT a1, STMT a2);

public:
	//Affects
	static bool isAffects(STMT, STMT);
	static vector<STMT> getAffectsBy(STMT);
	static vector<STMT> getAffectsFrom(STMT);

	//Affects*
	static bool isAffectsStar(STMT, STMT);
	static vector<STMT> getAffectsByStar(STMT a1);
	static vector<STMT> getAffectsFromStar(STMT a2);

	//AffectsBip
	static bool isAffectsBip(STMT, STMT);


};

