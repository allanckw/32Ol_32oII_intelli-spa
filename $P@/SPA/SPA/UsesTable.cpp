#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "UsesTable.h"

UsesTable::UsesTable()
{
}

/**
* This method will be used to insert the Uses relation, where the first argument of the relation is a statement
* @param s statement that uses v
* @param v variable that is used by s
*/
void UsesTable::insertStmtUses(STMT s, VAR v)
{
	if (s <= 0)
		throw SPAException("Invalid Reference: statement no. cannot be less than or equal to zero");

	originalUsedByStmt[s].insert(v);
	originalUsesInStmt[v].insert(s);
}

/**
* This method will be used to insert the Uses relation, where the first argument of the relation is a procedure
* @param p procedure that uses v
* @param v variable that is used by p
*/
void UsesTable::insertProcUses(PROC p, VAR v)
{
	originalUsedByProc[p].insert(v);
	originalUsesInProc[v].insert(p);
}

/**
* This method returns a list of variables that are used by the target statement.
* @param s target statement
* @return a vector of all variables
*/
vector<VAR> UsesTable::getUsedByStmt(STMT s)
{
	if (optimizedUsedByStmt.count(s) > 0)
		return optimizedUsedByStmt[s];
	return vector<VAR>();
}

/**
* This method returns a list of variables that are used by the target procedure.
* @param p target procedure
* @return a vector of all variables
*/
vector<VAR> UsesTable::getUsedByProc(PROC p)
{
	if (optimizedUsedByProc.count(p) > 0)
		return optimizedUsedByProc[p];
	return vector<VAR>();
}

/**
* This method returns a list of statements that are used by the target variable.
* @param v target variable
* @return a vector of all statements
*/
vector<STMT> UsesTable::getUsedInStmt(VAR v)
{
	if (optimizedUsesInStmt.count(v) > 0)
		return optimizedUsesInStmt[v];
	return vector<STMT>();
}

/**
* This method returns a list of procedures that are used by the target variable.
* @param v target variable
* @return a vector of all procedures
*/
vector<PROC> UsesTable::getUsedInProc(VAR v)
{
	if (optimizedUsesInProc.count(v) > 0)
		return optimizedUsesInProc[v];
	return vector<PROC>();
}

/**
* This method will be used to check if the statement s uses variable v.
* @param s statement that uses v
* @param v variable that is used by s
* @return true if the statement s uses variable v.
*/
bool UsesTable::isUsedStmt(STMT s, VAR v)
{
	return (originalUsedByStmt.count(s) > 0 && originalUsedByStmt[s].count(v) > 0);
}

/**
* This method will be used to check if the procedure p uses variable v.
* @param p procedure that uses v
* @param v variable that is used by s
* @return true if the procedure p uses variable v.
*/
bool UsesTable::isUsedProc(PROC p, VAR v)
{
	return (originalUsedByProc.count(p) > 0 && originalUsedByProc[p].count(v) > 0);
}

/**
* This method will be used to check if the Uses table is empty.
* @return true if the Uses table is empty, and false otherwise
*/
bool UsesTable::isEmpty()
{
	return originalUsedByProc.empty();
}

/**
* This method will be used to link a call statement to the procedure it calls.
* @param s statement calling procedure p
* @param p procedure called by statement s
*/
void UsesTable::linkCallStmtToProcUses(STMT s, PROC p) {
	callLinksUses.insert(pair<STMT, PROC>(s, p));
}


/**
* This method will be used to optimise the populated Uses table for fast access
*/
void UsesTable::optimizeUsesTable()
{
	for (auto it = originalUsedByStmt.begin(); it != originalUsedByStmt.end(); it++) {
		STMT s = (*it).first;
		vector<VAR>& optimizedUsedByStmtTable = optimizedUsedByStmt[s];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedUsedByStmtTable.push_back(*it2);
	}
	for (auto it = originalUsedByProc.begin(); it != originalUsedByProc.end(); it++) {
		PROC p = (*it).first;
		vector<VAR>& optimizedUsedByProcTable = optimizedUsedByProc[p];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedUsedByProcTable.push_back(*it2);
	}
	for (auto it = callLinksUses.begin(); it != callLinksUses.end(); it++) {
		STMT s = (*it).first;
		PROC p = (*it).second;

		set<VAR>& originalUsedByStmtTable = originalUsedByStmt[s];
		set<VAR>& originalUsedByProcTable = originalUsedByProc[p];
		originalUsedByStmtTable.insert(originalUsedByProcTable.begin(), originalUsedByProcTable.end());
		
		vector<VAR>& optimizedUsedByStmtTable = optimizedUsedByStmt[s];
		optimizedUsedByStmtTable.insert(optimizedUsedByStmtTable.begin(), originalUsedByStmtTable.begin(), originalUsedByStmtTable.end());
		
		for (auto it2 = optimizedUsedByStmtTable.begin(); it2 != optimizedUsedByStmtTable.end(); it2++) {
			VAR v = *it2;
			originalUsesInStmt[v].insert(s);
			originalUsesInProc[v].insert(p);
		}
	}
	for (auto it = originalUsesInStmt.begin(); it != originalUsesInStmt.end(); it++) {
		VAR v = (*it).first;
		vector<STMT>& optimizedUsesInStmtTable = optimizedUsesInStmt[v];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedUsesInStmtTable.push_back(*it2);
	}
	for (auto it = originalUsesInProc.begin(); it != originalUsesInProc.end(); it++) {
		VAR v = (*it).first;
		vector<PROC>& optimizedUsesInProcTable = optimizedUsesInProc[v];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedUsesInProcTable.push_back(*it2);
	}
}

/**
* This method will be used for testing purposes for viewing the content of the Uses table
*/
void UsesTable::displayUsesTables()
{
	cout << "OPTIMISED USES BY (STMT):" << endl;
	cout << "s: v such that Uses(s, v)" << endl;
	for (auto it = optimizedUsedByStmt.begin(); it != optimizedUsedByStmt.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
	
	cout << "OPTIMISED USES BY (PROC):" << endl;
	cout << "p: v such that Uses(p, v)" << endl;
	for (auto it = optimizedUsedByProc.begin(); it != optimizedUsedByProc.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}

	cout << "OPTIMISED USES (STMT):" << endl;
	cout << "v: s such that Uses(s, v)" << endl;
	for (auto it = optimizedUsesInStmt.begin(); it != optimizedUsesInStmt.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
	
	cout << "OPTIMISED USES (PROC):" << endl;
	cout << "v: p such that Uses(p, v)" << endl;
	for (auto it = optimizedUsesInProc.begin(); it != optimizedUsesInProc.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
}