#pragma once
#include "StdAfx.h"

class ParentTable
{
private:
	unordered_map<STMT, unordered_map<STMT, void>> parentTo;
	unordered_map<STMT, STMT> parentFrom;

public:
	ParentTable();
	~ParentTable();

	void insertParent(STMT s1, STMT s2);
	bool isParent(STMT s1, STMT s2);
	vector<STMT> getChildren(STMT s1);
	STMT getParent(STMT s2);
	bool isParentStar(STMT s1, STMT s2);
	vector<STMT> getChildrenStar(STMT s1);
	vector<STMT> getParentStar(STMT s2);
};

