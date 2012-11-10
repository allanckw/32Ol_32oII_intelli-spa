#pragma once
#include "StdAfx.h"
#include "CallsTable.h"
#include "PKB.h"

unordered_map<PROC, set<PROC>> originalCalledBy;
unordered_map<PROC, set<PROC>> originalCalledFrom;
unordered_map<PROC, set<PROC>> originalCalledByStar;
unordered_map<PROC, set<PROC>> originalCalledFromStar;

unordered_map<PROC, vector<PROC>> optimizedCalledBy;
unordered_map<PROC, vector<PROC>> optimizedCalledFrom;
unordered_map<PROC, vector<PROC>> optimizedCalledByStar;
unordered_map<PROC, vector<PROC>> optimizedCalledFromStar;

set<STMT> analyseCallByStar(PROC);
set<STMT> analyseCallFromStar(PROC);

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

//This function should be called by whoever after CallsTable is fully populated
//Creates hash tables callsTable and callsTable star for fast access during queries
void CallsTable::optimizeCallsTable() {
	for (auto it = originalCalledBy.begin(); it != originalCalledBy.end(); it++) {
		PROC proc = (*it).first;
		vector<PROC> children;
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			children.push_back(*it2);
		optimizedCalledBy[proc] = children;
		
		set<STMT> childrenStarSet = analyseCallByStar(proc);
		vector<PROC> childrenStarVector;
		for (auto it2 = childrenStarSet.begin(); it2 != childrenStarSet.end(); it2++)
			childrenStarVector.push_back(*it2);
		optimizedCalledByStar[proc] = childrenStarVector;
	}
	
	for (auto it = originalCalledFrom.begin(); it != originalCalledFrom.end(); it++) {
		PROC proc = (*it).first;
		vector<PROC> parents;
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			parents.push_back(*it2);
		optimizedCalledFrom[proc] = parents;
		
		set<STMT> parentsStarSet = analyseCallFromStar(proc);
		vector<PROC> parentsStarVector;
		for (auto it2 = parentsStarSet.begin(); it2 != parentsStarSet.end(); it2++)
			parentsStarVector.push_back(*it2);
		optimizedCalledFromStar[proc] = parentsStarVector;
	}
}

set<STMT> analyseCallByStar(PROC proc) {
	if (originalCalledByStar.count(proc) == 0) {
		set<PROC> childrenStar;
		set<PROC> children = originalCalledBy[proc];
		for (auto it = children.begin(); it != children.end(); it++) {
			childrenStar.insert(*it);
			set<PROC> childchildren = analyseCallByStar(*it);
			for (auto it2 = childchildren.begin(); it2 != childchildren.end(); it2++)
				childrenStar.insert(*it2);
		}
		originalCalledByStar[proc] = childrenStar;
	}
	return originalCalledByStar[proc];
}

set<STMT> analyseCallFromStar(PROC proc) {
	if (originalCalledFromStar.count(proc) == 0) {
		set<PROC> parentsStar;
		set<PROC> parents = originalCalledFrom[proc];
		for (auto it = parents.begin(); it != parents.end(); it++) {
			parentsStar.insert(*it);
			set<PROC> parentparents = analyseCallFromStar(*it);
			for (auto it2 = parentparents.begin(); it2 != parentparents.end(); it2++)
				parentsStar.insert(*it2);
		}
		originalCalledFromStar[proc] = parentsStar;
	}
	return originalCalledFromStar[proc];
}

vector<PROC> CallsTable::getCalledBy(PROC p)
{
	vector<PROC> answer;
	if (optimizedCalledBy.count(p) > 0)
		answer = optimizedCalledBy[p];
	return answer;
}

vector<PROC> CallsTable::getCalledFrom(PROC p)
{
	vector<PROC> answer;
	if (optimizedCalledFrom.count(p) > 0)
		answer = optimizedCalledFrom[p];
	return answer;
}

vector<PROC> CallsTable::getCalledByStar(PROC p)
{
	vector<PROC> answer;
	if (optimizedCalledByStar.count(p) > 0)
		answer = optimizedCalledByStar[p];
	return answer;
}

vector<PROC> CallsTable::getCalledFromStar(PROC p)
{
	vector<PROC> answer;
	if (optimizedCalledFromStar.count(p) > 0)
		answer = optimizedCalledFromStar[p];
	return answer;
}

bool CallsTable::isCalled(PROC p1, PROC p2)
{
	return (originalCalledBy.count(p1) > 0 && originalCalledBy[p1].count(p2) > 0);
}

bool CallsTable::isCalledStar(PROC p1, PROC p2)
{
	return (originalCalledByStar.count(p1) > 0 && originalCalledByStar[p1].count(p2) > 0);
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