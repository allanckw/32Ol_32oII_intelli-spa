#pragma once
#include "stdAfx.h"
#include "PKB.h"
#include "CFGNode.h"

class PQLNextProcessor
{
private:
	

public:
	
	static bool isNextStar(PROG_LINE p1, PROG_LINE p2);
	//static methods only!!
	
	static vector<PROG_LINE> getPreviousStar(PROG_LINE p2);

	static vector<PROG_LINE> getNextStar(PROG_LINE p1);
};
