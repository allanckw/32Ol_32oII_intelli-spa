#pragma once
#include "StdAfx.h"

class ModifiesTable
{
private:
	unordered_map<STMT, set<VAR>> originalModifiedByStmt;
	unordered_map<PROC, set<VAR>> originalModifiedByProc;
	unordered_map<VAR, set<STMT>> originalModifiesStmt;
	unordered_map<VAR, set<PROC>> originalModifiesProc;
	unordered_map<STMT, PROC> callLinksModifies;

	unordered_map<STMT, vector<VAR>> optimizedModifiedByStmt;
	unordered_map<PROC, vector<VAR>> optimizedModifiedByProc;
	unordered_map<VAR, vector<STMT>> optimizedModifiesStmt;
	unordered_map<VAR, vector<PROC>> optimizedModifiesProc;
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
	bool isEmpty();
	
	void linkCallStmtToProcModifies(STMT, PROC);
	void optimizeModifiesTable();

	//Functions for testing purposes
	void displayModifiesTables();
};