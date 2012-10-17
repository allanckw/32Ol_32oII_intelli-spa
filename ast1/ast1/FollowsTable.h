#pragma once
#include "StdAfx.h"

class FollowsTable
{
private:
	vector<pair <int, int>> follows;

public:
	FollowsTable();

	void insertFollows(int s1, int s2);
	bool isFollows(int s1, int s2);
	int getFollowsBy(int s1);
	int getFollowsFrom(int s2);
	bool isFollowsStar(int s1, int s2);
	vector<int> getFollowsStarBy(int s1);
	vector<int> getFollowsStarFrom(int s2);
	
	~FollowsTable();
}

