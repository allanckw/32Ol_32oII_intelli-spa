#pragma once
#include "StdAfx.h"

class CallsTable
{

private:
	unordered_map<PROC, set<PROC>> originalCalledBy;
	unordered_map<PROC, set<PROC>> originalCalledFrom;
	unordered_map<PROC, set<PROC>> originalCalledByStar;
	unordered_map<PROC, set<PROC>> originalCalledFromStar;

	unordered_map<PROC, vector<PROC>> optimizedCalledBy;
	unordered_map<PROC, vector<PROC>> optimizedCalledFrom;
	unordered_map<PROC, vector<PROC>> optimizedCalledByStar;
	unordered_map<PROC, vector<PROC>> optimizedCalledFromStar;

	unordered_map<PROC, vector<STMT>> stmtCall;

	set<STMT> analyseCallByStar(PROC);
	set<STMT> analyseCallFromStar(PROC);

	int size;

public:
	CallsTable();

	void insertCalls(PROC, PROC);
	vector<PROC> getCalledBy(PROC);
	vector<PROC> getCalledFrom(PROC);
	vector<PROC> getCalledByStar(PROC);
	vector<PROC> getCalledFromStar(PROC);
	bool isCalled(PROC, PROC);
	bool isCalledStar(PROC, PROC);
	bool isEmpty();

	void insertStmtCall(STMT, PROC);
	vector<STMT> getStmtCall(PROC);

	void optimizeCallsTable();

	int getCallsSize();
	int getCallsStarSize();

	//Functions for testing purposes
	void displayCallsTables();
};