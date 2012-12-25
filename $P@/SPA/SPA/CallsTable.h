#pragma once
#include "StdAfx.h"

class CallsTable
{

private:

public:
	CallsTable();

	void insertCalls(PROC, PROC);
	vector<PROC> getCalledBy(PROC);
	vector<PROC> getCalledFrom(PROC);
	vector<PROC> getCalledByStar(PROC);
	vector<PROC> getCalledFromStar(PROC);
	bool isCalled(PROC, PROC);
	bool isCalledStar(PROC, PROC);

	void insertStmtCall(STMT, PROC);
	vector<STMT> getStmtCall(PROC);

	void optimizeCallsTable();

	//Functions for testing purposes
	void displayCallsTables();
};