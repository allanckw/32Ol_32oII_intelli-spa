#pragma once
#include "StdAfx.h"

class UsesTable
{
private:
	unordered_map<STMT, set<VAR>> originalUsedByStmt;
	unordered_map<PROC, set<VAR>> originalUsedByProc;
	unordered_map<VAR, set<STMT>> originalUsesInStmt;
	unordered_map<VAR, set<PROC>> originalUsesInProc;
	unordered_map<STMT, PROC> callLinksUses;

	unordered_map<STMT, vector<VAR>> optimizedUsedByStmt;
	unordered_map<PROC, vector<VAR>> optimizedUsedByProc;
	unordered_map<VAR, vector<STMT>> optimizedUsesInStmt;
	unordered_map<VAR, vector<PROC>> optimizedUsesInProc;
public:
	UsesTable();

	void insertStmtUses(STMT, VAR);
	void insertProcUses(PROC, VAR);
	vector<VAR> getUsedByStmt(STMT);
	vector<VAR> getUsedByProc(PROC);
	vector<STMT> getUsedInStmt(VAR);
	vector<PROC> getUsedInProc(VAR);
	bool isUsedStmt(STMT, VAR);
	bool isUsedProc(PROC, VAR);
	bool isEmpty();

	void linkCallStmtToProcUses(STMT, PROC);
	void optimizeUsesTable();

	//Functions for testing purposes
	void displayUsesTables();
};