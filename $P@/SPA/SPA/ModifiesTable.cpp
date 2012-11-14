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
	vector<PROC> answer;
	if (optimizedModifiedByStmt.count(s) > 0)
		answer = optimizedModifiedByStmt[s];
	return answer;
}

vector<VAR> ModifiesTable::getModifiedByProc(PROC p)
{
	vector<PROC> answer;
	if (optimizedModifiedByProc.count(p) > 0)
		answer = optimizedModifiedByProc[p];
	return answer;
}

vector<STMT> ModifiesTable::getModifiesStmt(VAR v)
{
	vector<PROC> answer;
	if (optimizedModifiesStmt.count(v) > 0)
		answer = optimizedModifiesStmt[v];
	return answer;
}

vector<PROC> ModifiesTable::getModifiesProc(VAR v)
{
	vector<PROC> answer;
	if (optimizedModifiesProc.count(v) > 0)
		answer = optimizedModifiesProc[v];
	return answer;
}

bool ModifiesTable::isModifiedStmt(STMT s, VAR v)
{
	return (originalModifiedByStmt.count(s) > 0 && originalModifiedByStmt[s].count(v) > 0);
}

bool ModifiesTable::isModifiedProc(PROC p, VAR v)
{
	return (originalModifiedByProc.count(p) > 0 && originalModifiedByProc[p].count(v) > 0);
}

void ModifiesTable::linkCallStmtToProcModifies(STMT s, PROC p) {
	callLinksModifies[s] = p;
}

//This function should be invoked once modifies has been fully populated by whoever is populating it
void ModifiesTable::optimizeModifiesTable()
{
	for (auto it = originalModifiedByStmt.begin(); it != originalModifiedByStmt.end(); it++) {
		STMT s = (*it).first;
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedModifiedByStmt[s].push_back(*it2);
	}
	for (auto it = originalModifiedByProc.begin(); it != originalModifiedByProc.end(); it++) {
		PROC p = (*it).first;
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedModifiedByProc[p].push_back(*it2);
	}
	for (auto it = callLinksModifies.begin(); it != callLinksModifies.end(); it++) {
		STMT s = (*it).first;
		PROC p = (*it).second;
		originalModifiedByStmt[s] = originalModifiedByProc[p];
		optimizedModifiedByStmt[s] = optimizedModifiedByProc[p];

		for (auto it2 = optimizedModifiedByStmt[s].begin(); it2 != optimizedModifiedByStmt[s].end(); it2++) {
			VAR v = *it2;
			originalModifiesStmt[v].insert(s);
			originalModifiesProc[v].insert(p);
		}
	}
	for (auto it = originalModifiesStmt.begin(); it != originalModifiesStmt.end(); it++) {
		VAR v = (*it).first;
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedModifiesStmt[v].push_back(*it2);
	}
	for (auto it = originalModifiesProc.begin(); it != originalModifiesProc.end(); it++) {
		VAR v = (*it).first;
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			optimizedModifiesProc[v].push_back(*it2);
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