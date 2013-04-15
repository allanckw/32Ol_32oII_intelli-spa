#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "ModifiesTable.h"


/**
* This method will be used as a constructor to create ModifiesTable
*/
ModifiesTable::ModifiesTable()
{
}

/**
* This method will be used to insert the Modifies relation, where the first argument of the relation is a statement
* @param s statement that modifies v
* @param v variable that is modified by s
*/
void ModifiesTable::insertStmtModifies(STMT s, VAR v)
{
	if (s <= 0)
		throw SPAException("Invalid Reference: statement no. cannot be less than or equal to zero");

	originalModifiedByStmt[s].insert(v);
	originalModifiesStmt[v].insert(s);
}

/**
* This method will be used to insert the Modifies relation, where the first argument of the relation is a procedure
* @param p procedure that modifies v
* @param v variable that is modified by p
*/
void ModifiesTable::insertProcModifies(PROC p, VAR v)
{
	originalModifiedByProc[p].insert(v);
	originalModifiesProc[v].insert(p);
}

/**
* This method returns a list of variables that are modified by the target statement.
* @param s target statement
* @return a vector of all variables
*/
vector<VAR> ModifiesTable::getModifiedByStmt(STMT s)
{
	if (optimizedModifiedByStmt.count(s) > 0)
		return optimizedModifiedByStmt[s];
	return vector<VAR>();
}

/**
* This method returns a list of variables that are modified by the target procedure.
* @param p target procedure
* @return a vector of all variables
*/
vector<VAR> ModifiesTable::getModifiedByProc(PROC p)
{
	if (optimizedModifiedByProc.count(p) > 0)
		return optimizedModifiedByProc[p];
	return vector<VAR>();
}

/**
* This method returns a list of statements that are modified by the target variable.
* @param v target variable
* @return a vector of all statements
*/
vector<STMT> ModifiesTable::getModifiesStmt(VAR v)
{
	if (optimizedModifiesStmt.count(v) > 0)
		return optimizedModifiesStmt[v];
	return vector<STMT>();
}

/**
* This method returns a list of procedures that are modified by the target variable.
* @param v target variable
* @return a vector of all procedures
*/
vector<PROC> ModifiesTable::getModifiesProc(VAR v)
{
	if (optimizedModifiesProc.count(v) > 0)
		return optimizedModifiesProc[v];
	return vector<PROC>();
}

/**
* This method will be used to check if the statement s modifies variable v.
* @param s statement that modifies v
* @param v variable that is modified by s
* @return true if the statement s modifies variable v.
*/
bool ModifiesTable::isModifiedStmt(STMT s, VAR v)
{
	return (originalModifiedByStmt.count(s) > 0 && originalModifiedByStmt[s].count(v) > 0);
}

/**
* This method will be used to check if the procedure p modifies variable v.
* @param p procedure that modifies v
* @param v variable that is modified by s
* @return true if the procedure p modifies variable v.
*/
bool ModifiesTable::isModifiedProc(PROC p, VAR v)
{
	return (originalModifiedByProc.count(p) > 0 && originalModifiedByProc[p].count(v) > 0);
}

/**
* This method will be used to check if the Modifies table is empty.
* @return true if the Modifies table is empty, and false otherwise
*/
bool ModifiesTable::isEmpty()
{
	return originalModifiedByProc.empty();
}

/*
* This method will be used to link a call statement to the procedure it calls.
* @param s statement calling procedure p
* @param p procedure called by statement s
*/
void ModifiesTable::linkCallStmtToProcModifies(STMT s, PROC p) {
	callLinksModifies.insert(pair<STMT, PROC>(s, p));
}

//This function should be invoked once modifies has been fully populated by whoever is populating it
/**
* This method will be used to optimise the populated Modifies table for fast access
*/
void ModifiesTable::optimizeModifiesTable()
{
	for (auto it = originalModifiedByStmt.begin(); it != originalModifiedByStmt.end(); it++) {
		STMT s = (*it).first;
		vector<VAR>& table = optimizedModifiedByStmt[s];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			table.push_back(*it2);
	}
	for (auto it = originalModifiedByProc.begin(); it != originalModifiedByProc.end(); it++) {
		PROC p = (*it).first;
		vector<VAR>& table = optimizedModifiedByProc[p];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			table.push_back(*it2);
	}
	for (auto it = callLinksModifies.begin(); it != callLinksModifies.end(); it++) {
		STMT s = (*it).first;
		PROC p = (*it).second;

		set<VAR>& originalModifiedByStmtTable = originalModifiedByStmt[s];
		set<VAR>& originalModifiedByProcTable = originalModifiedByProc[p];
		originalModifiedByStmtTable.insert(originalModifiedByProcTable.begin(), originalModifiedByProcTable.end());

		vector<VAR>& optimizedModifiedByStmtTable = optimizedModifiedByStmt[s];
		optimizedModifiedByStmtTable.insert(optimizedModifiedByStmtTable.begin(), originalModifiedByStmtTable.begin(), originalModifiedByStmtTable.end());

		for (auto it2 = optimizedModifiedByStmtTable.begin(); it2 != optimizedModifiedByStmtTable.end(); it2++) {
			VAR v = *it2;
			originalModifiesStmt[v].insert(s);
			originalModifiesProc[v].insert(p);
		}
	}
	for (auto it = originalModifiesStmt.begin(); it != originalModifiesStmt.end(); it++) {
		VAR v = (*it).first;
		vector<STMT>& optimizedModifiesStmtTable = optimizedModifiesStmt[v];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedModifiesStmtTable.push_back(*it2);
	}
	for (auto it = originalModifiesProc.begin(); it != originalModifiesProc.end(); it++) {
		VAR v = (*it).first;
		vector<PROC>& optimizedModifiesProcTable = optimizedModifiesProc[v];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedModifiesProcTable.push_back(*it2);
	}
}

/**
* This method will be used to get the Size of ModTable by PROC
*/
int ModifiesTable::getModProcSize()
{
	return this->optimizedModifiedByProc.size();
}

/**
* This method will be used to get the Size of ModTable by STMT
*/
int ModifiesTable::getModStmtSize()
{
	return this->optimizedModifiedByStmt.size();
}

//////////////////////////////////
//Functions for testing purposes//
//////////////////////////////////
/**
* This method will be used for testing purposes for viewing the content of the Modifies table
*/
void ModifiesTable::displayModifiesTables()
{
	cout << "OPTIMISED MODIFIED BY (STMT):" << endl;
	cout << "s: v such that Modifies(s, v)" << endl;
	for (auto it = optimizedModifiedByStmt.begin(); it != optimizedModifiedByStmt.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
	
	cout << "OPTIMISED MODIFIED BY (PROC):" << endl;
	cout << "p: v such that Modifies(p, v)" << endl;
	for (auto it = optimizedModifiedByProc.begin(); it != optimizedModifiedByProc.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}

	cout << "OPTIMISED MODIFIES (STMT):" << endl;
	cout << "v: s such that Modifies(s, v)" << endl;
	for (auto it = optimizedModifiesStmt.begin(); it != optimizedModifiesStmt.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
	
	cout << "OPTIMISED MODIFIES (PROC):" << endl;
	cout << "v: p such that Modifies(p, v)" << endl;
	for (auto it = optimizedModifiesProc.begin(); it != optimizedModifiesProc.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
}