#pragma once
#include "StdAfx.h"


class ModifiesTable
{
private:
	int noProgLines;
	int noProcs;
	int noVars;
	vector<pair <STMT, VAR>> modifiesStmtTable; //Table of (stmt, var modified by stmt)
	vector<pair <PROC, VAR>> modifiesProcTable; //Table of (proc, var modified by proc)
	//Hash Tables
	vector<VAR> * optimizedModifiesStmtTable;
	vector<VAR> * optimizedModifiesProcTable;
	vector<STMT> * optimizedModifiedByStmtTable;
	vector<PROC> * optimizedModifiedByProcTable;

public:
	ModifiesTable();

	void insertStmtModifies(STMT, VAR);
	void insertProcModifies(PROC, VAR);
	void optimizeModifiesTable();
	vector<VAR> getModifiedByStmt(STMT);
	vector<VAR> getModifiedByProc(PROC);
	vector<STMT> getModifiesStmt(VAR);
	vector<PROC> getModifiesProc(VAR);
	bool isModifiedStmt(STMT, VAR);
	bool isModifiedProc(PROC, VAR);

	//Functions for testing purposes
	void displayModifiedByProcTable();
	void displayModifiedByStmtTable();
	void displayModifiesProcTable();
	void displayModifiesStmtTable();
};