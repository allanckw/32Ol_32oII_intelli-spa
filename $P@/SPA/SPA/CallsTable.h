#pragma once
#include "StdAfx.h"
#include "PROCTable.h"
//typedef int PROCIndex;

class CallsTable
{

private:
	vector<pair <PROCIndex, PROCIndex>> callsTable;
	vector<vector<PROCIndex>> optimizedCalledByTable;
	vector<vector<PROCIndex>> optimizedCalledFromTable;
	vector<vector<PROCIndex>> optimizedCalledByStarTable;
	vector<vector<PROCIndex>> optimizedCalledFromStarTable;
	void optimizeCalledByStarTable(PROCIndex, vector<PROCIndex>, vector<PROCIndex>);
	void optimizeCalledFromStarTable(PROCIndex, vector<PROCIndex>, vector<PROCIndex>);

public:
	CallsTable();

	void insertCalls(PROCIndex, PROCIndex);
	void optimizeCallsTable();
	vector<PROCIndex> getCalledBy(PROCIndex);
	vector<PROCIndex> getCalledFrom(PROCIndex);
	vector<PROCIndex> getCalledByStar(PROCIndex);
	vector<PROCIndex> getCalledFromStar(PROCIndex);
	bool isCalled(PROCIndex, PROCIndex);
	bool isCalledStar(PROCIndex, PROCIndex);
}