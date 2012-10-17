#include "StdAfx.h"
#include "FollowsTable.h"


FollowsTable::FollowsTable(void)
{
}


FollowsTable::~FollowsTable(void)
{
}

void FollowsTable::insertFollows(int s1, int s2)
{
	follows.push_back(make_pair(s1, s2));
}

bool FollowsTable::isFollows(int s1, int s2)
{
	bool found = false;
	int index = 0;
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

int FollowsTable::getFollowsBy(int s1) 
{
	int answer = -1;
	int index = 0;
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

int FollowsTable::getFollowsFrom(int s2) 
{
	int answer = -1;
	int index = 0;
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

bool FollowsTable::isFollowsStar(int s1, int s2)
{
	if (getFollowsBy(s1) == -1) {
		return false;
	} else if (getFollowsBy(s1) == s2) {
		return true;
	} else {
		int next = getFollowsBy(s1);
		return isFollowsStar(next, s2);
	}
}

vector <int> FollowsTable::getFollowsStarBy(int s1) 
{
	vector <int> answer;
	int current = s1;
	while (getFollowsBy(current) != -1) {
		int num = getFollowsBy(current);
		answer.push_back(num);
		current = num;
	}
	return answer;
}

vector <int> FollowsTable::getFollowsStarFrom(int s2) 
{
	vector <int> answer;
	int current = s2;
	while (getFollowsFrom(current) != -1) {
		int num = getFollowsFrom(current);
		answer.push_back(num);
		current = num;
	}
	return answer;
}