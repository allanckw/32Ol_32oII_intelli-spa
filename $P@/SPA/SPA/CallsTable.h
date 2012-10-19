#pragma once
#include "StdAfx.h"

class CallsTable
{

private:
	int noProcs;
	vector<pair <PROC, PROC>> callsTable;
	vector<PROC> * optimizedCalledByTable;
	vector<PROC> * optimizedCalledFromTable;
	vector<PROC> * optimizedCalledByStarTable;
	vector<PROC> * optimizedCalledFromStarTable;
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

	//Functions for testing purposes
	void displayCallsTable();
	void displayCalledByTable();
	void displayCalledByStarTable();
	void displayCalledFromTable();
	void displayCalledFromStarTable();
};