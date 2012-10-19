#pragma once
#include "StdAfx.h"

class FollowsTable
{
private:
	vector<pair <STMT, STMT>> follows;

public:
	FollowsTable();

	void insertFollows(STMT s1, STMT s2);
	bool isFollows(STMT s1, STMT s2);
	STMT getFollowsBy(STMT s1);
	STMT getFollowsFrom(STMT s2);
	bool isFollowsStar(STMT s1, STMT s2);
	vector<STMT> getFollowsStarBy(STMT s1);
	vector<STMT> getFollowsStarFrom(STMT s2);
	
	~FollowsTable();
};

