#pragma once
#include "StdAfx.h"


class ModifiesTable
{
private:
	vector<pair <STMT, VAR>> modifiesStmtTable; //Table of (stmt, var modified by stmt)
	vector<pair <PROC, VAR>> modifiesProcTable; //Table of (proc, var modified by proc)
	//Hash Tables
	vector<vector<VAR>> optimizedModifiesStmtTable;
	vector<vector<VAR>> optimizedModifiesProcTable;
	vector<vector<STMT>> optimizedModifiedByStmtTable;
	vector<vector<PROC>> optimizedModifiedByProcTable;

public:
	ModifiesTable();

	void insertStmtModifies(STMT, VAR);
	void insertProcModifies(PROC, VAR);
	void optimizeModifiesTables();
	vector<VAR> ModifiedByStmt(STMT);
	vector<VAR> ModifiedByProc(PROC);
	vector<STMT> getModifiesStmt(VAR);
	vector<PROC> getModifiesProc(VAR);
	bool isModifiedStmt(STMT, VAR);
	bool isModifiedProc(PROC, VAR);
};