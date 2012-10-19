#include "StdAfx.h"
#include "FollowsTable.h"


FollowsTable::FollowsTable()
{
}


FollowsTable::~FollowsTable()
{
}

void FollowsTable::insertFollows(STMT s1, STMT s2)
{
	follows.push_back(make_pair(s1, s2));
}

bool FollowsTable::isFollows(STMT s1, STMT s2)
{
	bool found = false;
	STMT index = 0;
	while (!found && index != follows.size()) {
		if (follows.at(index).first == s1) {
			if (follows.at(index).second == s2) {
				return true;
			} else {
				return false;
			}
			found = true;
		} else
		index++;
	}

}

STMT FollowsTable::getFollowsBy(STMT s1) 
{
	STMT answer = -1;
	STMT index = 0;
	bool found = false;
		while (!found && index != follows.size()) {
		if (follows.at(index).first == s1) {
			found = true;
			answer = follows.at(index).second;
		} else
		index++;
	}
	return answer;
}

STMT FollowsTable::getFollowsFrom(STMT s2) 
{
	STMT answer = -1;
	STMT index = 0;
	bool found = false;
		while (!found && index != follows.size()) {
		if (follows.at(index).second == s2) {
			found = true;
			answer = follows.at(index).first;
		} else
		index++;
	}
	return answer;
}

bool FollowsTable::isFollowsStar(STMT s1, STMT s2)
{
	if (getFollowsBy(s1) == -1) {
		return false;
	} else if (getFollowsBy(s1) == s2) {
		return true;
	} else {
		STMT next = getFollowsBy(s1);
		return isFollowsStar(next, s2);
	}
}

vector <STMT> FollowsTable::getFollowsStarBy(STMT s1) 
{
	vector <STMT> answer;
	STMT current = s1;
	while (getFollowsBy(current) != -1) {
		STMT num = getFollowsBy(current);
		answer.push_back(num);
		current = num;
	}
	return answer;
}

vector <STMT> FollowsTable::getFollowsStarFrom(STMT s2) 
{
	vector <STMT> answer;
	STMT current = s2;
	while (getFollowsFrom(current) != -1) {
		STMT num = getFollowsFrom(current);
		answer.push_back(num);
		current = num;
	}
	return answer;
}
