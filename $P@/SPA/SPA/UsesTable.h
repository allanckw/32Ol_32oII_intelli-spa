#pragma once
#include "StdAfx.h"
typedef int PROC;
typedef int STMT;
typedef int VAR;

class UsesTable
{
private:
	int noStmts;
	int noProcs;
	int noVars;
	vector<pair <STMT, VAR>> usesStmtTable; //Table of (stmt, var used by stmt)
	vector<pair <PROC, VAR>> usesProcTable; //Table of (proc, var used by proc)
	//Hash Tables
	vector<VAR> * optimizedUsedByStmtTable;
	vector<VAR> * optimizedUsedByProcTable;
	vector<STMT> * optimizedUsedInStmtTable;
	vector<PROC> * optimizedUsedInProcTable;

public:
	UsesTable();

	void insertStmtUses(STMT, VAR);
	void insertProcUses(PROC, VAR);
	void optimizeUsesTables();
	vector<VAR> getUsedByStmt(STMT);
	vector<VAR> getUsedByProc(PROC);
	vector<STMT> getUsedInStmt(VAR);
	vector<PROC> getUsedInProc(VAR);
	bool isUsedStmt(STMT, VAR);
	bool isUsedProc(PROC, VAR);

	//Functions for testing purposes
	void displayUsedByProcTable();
	void displayUsedByStmtTable();
	void displayUsedInProcTable();
	void displayUsedInStmtTable();
};