#pragma once
#include "StdAfx.h"
#include "FollowsTable.h"


FollowsTable::FollowsTable()
{
}


/**
* This method will be used to insert the Follows relation.
* Let r be the number of relationships in FollowsTable.
* Armotised: O(1) (O(k) after k insertions)
* Worst: O(c) <= O(r), where c is the number of relationships in the table
* @param s1 statement followed by s2
* @param s2 statement following s1
*/
void FollowsTable::insertFollows(STMT s1, STMT s2)
{
	followsTo.insert(pair<STMT, STMT>(s1, s2));
	followsFrom.insert(pair<STMT, STMT>(s2, s1));
}

/**
* This method will be used to check if the Follows table is empty.
* O(1)
* @return true if the Follows table is empty, and false otherwise
*/
bool FollowsTable::isEmpty()
{
	return followsTo.empty();
}

/**
* This method will be used to check if statement s1 is followed by statement s2.
* O(1)
* @param s1 statement followed by s2
* @param s2 statement following s1
* @return true if statement s1 is followed by statement s2.
*/
bool FollowsTable::isFollows(STMT s1, STMT s2)
{
	return (followsTo.count(s1) > 0 && followsTo[s1] == s2);
}

/**
* This method returns the statement that follows from s1, or -1 if no such statement exists.
* O(1)
* @param s1 target statement
* @return the statement
*/
STMT FollowsTable::getFollowsBy(STMT s1) 
{
	if (followsTo.count(s1) > 0)
		return followsTo[s1];
	else
		return -1;
}

/**
* This method returns the statement that s2 follows from, or -1 if no such statement exists.
* O(1)
* @param s2 target statement
* @return the statement
*/
STMT FollowsTable::getFollowsFrom(STMT s2) 
{
	if (followsFrom.count(s2) > 0)
		return followsFrom[s2];
	else
		return -1;
}

/**
* This method will be used to check if the statement s1 is followed by statement s2 directly or indirectly.
* O(n) <= O(r), where n is the number of siblings right of s1
* @param s1 earlier statement
* @param s2 later statement
* @return true if statement s1 is followed by statement s2 directly or indirectly.
*/
bool FollowsTable::isFollowsStar(STMT s1, STMT s2)
{
	STMT follows = s1;
	do {
		if (followsTo.count(follows) > 0)
			follows = followsTo[follows];
		else
			return false;
	} while (follows != s2);
	return true;
}

/**
* This method returns a list of statements that follows from s1, directly or indirectly.
* O(n) <= O(r), where n is the number of siblings right of s1
* @param s1 target statement
* @return the vector
*/
vector <STMT> FollowsTable::getFollowsStarBy(STMT s1) 
{
	vector<STMT> output;
	STMT follows = s1;
	do {
		if (followsTo.count(follows) > 0) {
			follows = followsTo[follows];
			output.push_back(follows);
		} else
			return output;
	} while (true);
}

/**
* This method returns a list of statements that s2 follows from, directly or indirectly.
* O(n) <= O(r), where n is the number of siblings left of s2
* @param s2 target statement
* @return the vector
*/
vector <STMT> FollowsTable::getFollowsStarFrom(STMT s2) 
{
	vector<STMT> output;
	STMT follows = s2;
	do {
		if (followsFrom.count(follows) > 0) {
			follows = followsFrom[follows];
			output.push_back(follows);
		} else
			return output;
	} while (true);
}


int FollowsTable::getSize()
{
	return this->followsFrom.size();
}
