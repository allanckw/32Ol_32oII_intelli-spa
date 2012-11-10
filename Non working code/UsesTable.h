#pragma once
#include "StdAfx.h"

class UsesTable
{
private:

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

	void linkCallStmtToProcUses(STMT, PROC);
	void optimizeUsesTable();

	//Functions for testing purposes
	void displayUsesTables();
};