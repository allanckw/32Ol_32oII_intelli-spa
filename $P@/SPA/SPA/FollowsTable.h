#pragma once
#include "StdAfx.h"

class FollowsTable
{
private:
	unordered_map<STMT, STMT> followsTo;
	unordered_map<STMT, STMT> followsFrom;

public:
	FollowsTable();

	void insertFollows(STMT s1, STMT s2); //constant time
	bool isEmpty(); //constant time

	bool isFollows(STMT s1, STMT s2); //constant time
	STMT getFollowsBy(STMT s1); //constant time
	STMT getFollowsFrom(STMT s2); //constant time

	bool isFollowsStar(STMT s1, STMT s2); //linear time
	vector<STMT> getFollowsStarBy(STMT s1); //time linear in output
	vector<STMT> getFollowsStarFrom(STMT s2); //time linear in output
	int FollowsTable::getSize();
};

