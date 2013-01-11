#pragma once
#include "StdAfx.h"

class ParentTable
{
private:
	unordered_map<STMT,
		pair<unordered_set<STMT>, vector<STMT>>> parentTo;

	unordered_map<STMT, STMT> parentFrom;

public:
	ParentTable();
	~ParentTable();
	void insertParent(STMT s1, STMT s2); //constant time
	bool empty(); //constant time

	bool isParent(STMT s1, STMT s2); //constant time
	vector<STMT> getChildren(STMT s1); //constant time
	STMT getParent(STMT s2); //constant time

	bool isParentStar(STMT s1, STMT s2); //linear time
	vector<STMT> getChildrenStar(STMT s1); //time linear in output
	vector<STMT> getParentStar(STMT s2); //time linear in output
};

