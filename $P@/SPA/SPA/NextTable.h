#pragma once
#include "StdAfx.h"

class NextTable
{
private:
	
public:
	NextTable(void);
	~NextTable(void);

	static bool contains(vector<STMT> list, STMT s1);
	static bool isNextStar(STMT s1, STMT s2); 
	static bool isNext(STMT s1, STMT s2); 
	static vector<STMT> getNextStar(STMT s1); 
	static vector<STMT> getPreviousStar(STMT s2); 
	static vector<STMT> getNext(STMT s1); 
	static vector<STMT> getPrevious(STMT s2); 
};

