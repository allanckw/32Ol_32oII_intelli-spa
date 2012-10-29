#pragma once
#include "StdAfx.h"

class ModifiesTable
{
private:

public:
	ModifiesTable();

	void insertStmtModifies(STMT, VAR);
	void insertProcModifies(PROC, VAR);
	vector<VAR> getModifiedByStmt(STMT);
	vector<VAR> getModifiedByProc(PROC);
	vector<STMT> getModifiesStmt(VAR);
	vector<PROC> getModifiesProc(VAR);
	bool isModifiedStmt(STMT, VAR);
	bool isModifiedProc(PROC, VAR);
	
	void linkCallStmtToProcModifies(STMT, PROC);
	void optimizeModifiesTable();

	//Functions for testing purposes
	void displayModifiesTables();
};