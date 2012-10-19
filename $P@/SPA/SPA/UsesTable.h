#pragma once
#include "StdAfx.h"
typedef int PROC;
typedef int STMT;
typedef int VAR;

class UsesTable
{
private:
	vector<pair <STMT, VAR>> usesStmtTable; //Table of (stmt, var used by stmt)
	vector<pair <PROC, VAR>> usesProcTable; //Table of (proc, var used by proc)
	//Hash Tables
	vector<vector<VAR>> optimizedUsedByStmtTable;
	vector<vector<VAR>> optimizedUsedByProcTable;
	vector<vector<STMT>> optimizedUsedInStmtTable;
	vector<vector<PROC>> optimizedUsedInProcTable;

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
};