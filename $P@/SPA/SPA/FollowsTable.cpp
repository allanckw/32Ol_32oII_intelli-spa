#pragma once
#include "StdAfx.h"
#include "FollowsTable.h"


FollowsTable::FollowsTable()
{
}


FollowsTable::~FollowsTable()
{
}

//Let r be the number of relationships in FollowsTable
//Armotised: O(1) (O(k) after k insertions)
//Worst: O(c) <= O(r), where c is the number of relationships in the table
void FollowsTable::insertFollows(STMT s1, STMT s2)
{
	followsTo[s1] = s2;
	followsFrom[s2] = s1;
}

//O(1)
bool FollowsTable::isFollows(STMT s1, STMT s2)
{
	return (followsTo.count(s1) && followsTo[s1] == 2);
}

//O(1)
STMT FollowsTable::getFollowsBy(STMT s1) 
{
	if (followsTo.count(s1))
		return followsTo[s1];
	else
		return -1;
}

//O(1)
STMT FollowsTable::getFollowsFrom(STMT s2) 
{
	if (followsFrom.count(s2))
		return followsFrom[s2];
	else
		return -1;
}

//O(n) <= O(r), where n is the number of siblings right of s1
bool FollowsTable::isFollowsStar(STMT s1, STMT s2)
{
	STMT follows = s1;
	do {
		if (followsTo.count(follows))
			follows = followsTo[follows];
		else
			return false;
	} while (follows != s2);
	return true;
}

//O(n) <= O(r), where n is the number of siblings right of s1
vector <STMT> FollowsTable::getFollowsStarBy(STMT s1) 
{
	vector<STMT> output;
	STMT follows = s1;
	do {
		if (followsTo.count(follows)) {
			follows = followsTo[follows];
			output.push_back(follows);
		} else
			return output;
	} while (true);
}

//O(n) <= O(r), where n is the number of siblings left of s2
vector <STMT> FollowsTable::getFollowsStarFrom(STMT s2) 
{
	vector<STMT> output;
	STMT follows = s2;
	do {
		if (followsFrom.count(follows)) {
			follows = followsFrom[follows];
			output.push_back(follows);
		} else
			return output;
	} while (true);
}
