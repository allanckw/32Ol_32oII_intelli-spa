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

	//
	static vector<STMT> AffectsBy2(STMT);
	static vector<STMT> AffectsStarBy(STMT);
	//

	//Affects*
	
	static bool isAffectsStar(STMT, STMT);
	
	//AffectsBip
	static bool isAffectsBip(STMT, STMT);


};

