#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "ModifiesTable.h"



ModifiesTable::ModifiesTable()
{
}

void ModifiesTable::insertStmtModifies(STMT s, VAR v)
{
	if (s <= 0)
		throw SPAException("Invalid Reference: statement no. cannot be less than or equal to zero");

	originalModifiedByStmt[s].insert(v);
	originalModifiesStmt[v].insert(s);
}

void ModifiesTable::insertProcModifies(PROC p, VAR v)
{
	originalModifiedByProc[p].insert(v);
	originalModifiesProc[v].insert(p);
}

vector<VAR> ModifiesTable::getModifiedByStmt(STMT s)
{
	if (optimizedModifiedByStmt.count(s) > 0)
		return optimizedModifiedByStmt[s];
	return vector<VAR>();
}

vector<VAR> ModifiesTable::getModifiedByProc(PROC p)
{
	if (optimizedModifiedByProc.count(p) > 0)
		return optimizedModifiedByProc[p];
	return vector<VAR>();
}

vector<STMT> ModifiesTable::getModifiesStmt(VAR v)
{
	if (optimizedModifiesStmt.count(v) > 0)
		return optimizedModifiesStmt[v];
	return vector<STMT>();
}

vector<PROC> ModifiesTable::getModifiesProc(VAR v)
{
	if (optimizedModifiesProc.count(v) > 0)
		return optimizedModifiesProc[v];
	return vector<PROC>();
}

bool ModifiesTable::isModifiedStmt(STMT s, VAR v)
{
	return (originalModifiedByStmt.count(s) > 0 && originalModifiedByStmt[s].count(v) > 0);
}

bool ModifiesTable::isModifiedProc(PROC p, VAR v)
{
	return (originalModifiedByProc.count(p) > 0 && originalModifiedByProc[p].count(v) > 0);
}

bool ModifiesTable::isEmpty()
{
	return originalModifiedByProc.empty();
}

void ModifiesTable::linkCallStmtToProcModifies(STMT s, PROC p) {
	callLinksModifies.insert(pair<STMT, PROC>(s, p));
}

//This function should be invoked once modifies has been fully populated by whoever is populating it
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

//////////////////////////////////
//Functions for testing purposes//
//////////////////////////////////
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