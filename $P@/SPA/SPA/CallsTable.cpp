#pragma once
#include "StdAfx.h"
#include "CallsTable.h"
#include "PKB.h"

CallsTable::CallsTable()
{
}

void CallsTable::insertCalls(PROC p1, PROC p2)
{
	if (p1 == p2)
		throw SPAException("Procedure cannot perform direct call on itself.");

	originalCalledBy[p1].insert(p2);
	originalCalledFrom[p2].insert(p1);
}

//This function should be called after CallsTable is fully populated
//Creates hash tables callsTable and callsTable star for fast access during queries
void CallsTable::optimizeCallsTable() {
	for (auto it = originalCalledBy.begin(); it != originalCalledBy.end(); it++) {
		PROC proc = (*it).first;
		vector<PROC>& children = optimizedCalledBy[proc];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			children.push_back(*it2);
		
		set<STMT> childrenStarSet = analyseCallByStar(proc);
		vector<PROC>& childrenStarVector = optimizedCalledByStar[proc];
		for (auto it2 = childrenStarSet.begin(); it2 != childrenStarSet.end(); it2++)
			childrenStarVector.push_back(*it2);
	}
	
	for (auto it = originalCalledFrom.begin(); it != originalCalledFrom.end(); it++) {
		PROC proc = (*it).first;
		vector<PROC>& parents = optimizedCalledFrom[proc];
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			parents.push_back(*it2);
		
		set<STMT> parentsStarSet = analyseCallFromStar(proc);
		vector<PROC>& parentsStarVector = optimizedCalledFromStar[proc];
		for (auto it2 = parentsStarSet.begin(); it2 != parentsStarSet.end(); it2++)
			parentsStarVector.push_back(*it2);
	}
}

set<STMT> CallsTable::analyseCallByStar(PROC proc) {
	if (originalCalledByStar.count(proc) == 0) {
		set<PROC>& childrenStar = originalCalledByStar[proc];
		set<PROC>& children = originalCalledBy[proc];
		for (auto it = children.begin(); it != children.end(); it++) {
			childrenStar.insert(*it);
			set<PROC>& childchildren = analyseCallByStar(*it);
			for (auto it2 = childchildren.begin(); it2 != childchildren.end(); it2++)
				childrenStar.insert(*it2);
		}
		return childrenStar;
	}
	return originalCalledByStar[proc];
}

set<STMT> CallsTable::analyseCallFromStar(PROC proc) {
	if (originalCalledFromStar.count(proc) == 0) {
		set<PROC>& parentsStar = originalCalledFromStar[proc];
		set<PROC>& parents = originalCalledFrom[proc];
		for (auto it = parents.begin(); it != parents.end(); it++) {
			parentsStar.insert(*it);
			set<PROC>& parentparents = analyseCallFromStar(*it);
			for (auto it2 = parentparents.begin(); it2 != parentparents.end(); it2++)
				parentsStar.insert(*it2);
		}
		return parentsStar;
	}
	return originalCalledFromStar[proc];
}

vector<PROC> CallsTable::getCalledBy(PROC p)
{
	if (optimizedCalledBy.count(p) > 0)
		return optimizedCalledBy[p];
	return vector<PROC>();
}

vector<PROC> CallsTable::getCalledFrom(PROC p)
{
	if (optimizedCalledFrom.count(p) > 0)
		return optimizedCalledFrom[p];
	return vector<PROC>();
}

vector<PROC> CallsTable::getCalledByStar(PROC p)
{
	if (optimizedCalledByStar.count(p) > 0)
		return optimizedCalledByStar[p];
	return vector<PROC>();
}

vector<PROC> CallsTable::getCalledFromStar(PROC p)
{
	if (optimizedCalledFromStar.count(p) > 0)
		return optimizedCalledFromStar[p];
	return vector<PROC>();
}

bool CallsTable::isCalled(PROC p1, PROC p2)
{
	return (originalCalledBy.count(p1) > 0 && originalCalledBy[p1].count(p2) > 0);
}

bool CallsTable::isCalledStar(PROC p1, PROC p2)
{
	return (originalCalledByStar.count(p1) > 0 && originalCalledByStar[p1].count(p2) > 0);
}

bool CallsTable::empty()
{
	return originalCalledBy.empty();
}

void CallsTable::insertStmtCall(STMT s, PROC p)
{
	if (stmtCall.count(p) > 0)
		stmtCall[p].push_back(s);
	else {
		vector<STMT> stmt;
		stmt.push_back(s);
		stmtCall.insert(pair<PROC, vector<STMT>>(p, stmt));
	}
}

vector<STMT> CallsTable::getStmtCall(PROC p)
{
	if (stmtCall.count(p) > 0)
		return stmtCall[p];
	return vector<STMT>();
}

//////////////////////////////////
//Functions for Testing purposes//
//////////////////////////////////
void CallsTable::displayCallsTables()
{
	cout << "OPTIMISED CALLSTABLE BY:" << endl;
	cout << "p1: p2 such that Calls(p1, p2)" << endl;
	for (auto it = optimizedCalledBy.begin(); it != optimizedCalledBy.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
	
	cout << "OPTIMISED CALLSTABLE FROM:" << endl;
	cout << "p2: p1 such that Calls(p1, p2)" << endl;
	for (auto it = optimizedCalledFrom.begin(); it != optimizedCalledFrom.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}

	cout << "OPTIMISED CALLSTABLE BY STAR:" << endl;
	cout << "p1: p2 such that Calls*(p1, p2)" << endl;
	for (auto it = optimizedCalledByStar.begin(); it != optimizedCalledByStar.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
	
	cout << "OPTIMISED CALLSTABLE FROM STAR:" << endl;
	cout << "p2: p1 such that Calls*(p1, p2)" << endl;
	for (auto it = optimizedCalledFromStar.begin(); it != optimizedCalledFromStar.end(); it++) {
		cout << (*it).first << ": ";
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			cout<< (*it2) << " ";
		cout << endl;
	}
}