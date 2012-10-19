#pragma once
#include "StdAfx.h"
#include "PROCTable.h"


class CallsTable
{

private:
	vector<pair <PROC, PROC>> callsTable;
	vector<vector<PROC>> optimizedCalledByTable;
	vector<vector<PROC>> optimizedCalledFromTable;
	vector<vector<PROC>> optimizedCalledByStarTable;
	vector<vector<PROC>> optimizedCalledFromStarTable;
	void optimizeCalledByStarTable(PROC, vector<PROC>, vector<PROC>);
	void optimizeCalledFromStarTable(PROC, vector<PROC>, vector<PROC>);

public:
	CallsTable();

	void insertCalls(PROC, PROC);
	void optimizeCallsTable();
	vector<PROC> getCalledBy(PROC);
	vector<PROC> getCalledFrom(PROC);
	vector<PROC> getCalledByStar(PROC);
	vector<PROC> getCalledFromStar(PROC);
	bool isCalled(PROC, PROC);
	bool isCalledStar(PROC, PROC);
};