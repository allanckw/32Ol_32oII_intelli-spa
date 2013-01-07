#pragma once
#include "StdAfx.h"
#include "ParentTable.h"

ParentTable::ParentTable()
{
}

ParentTable::~ParentTable()
{
}

//Let r be the number of relationships in ParentTable
//Armotised: O(1) (O(k) after k insertions)
//Worst: O(c) <= O(r), where c is the number of relationships in the table
void ParentTable::insertParent(STMT s1, STMT s2)
{
	parentTo[s1].first.insert(s2);
	parentTo[s1].second.push_back(s2); //assume the s2's will be distinct

	parentFrom.insert(pair<STMT, STMT>(s2, s1));
}

//O(1)
bool ParentTable::isParent(STMT s1, STMT s2)
{
	return (parentTo.count(s1) > 0 && parentTo[s1].first.count(s2) > 0);
}

//O(1)
vector<STMT> ParentTable::getChildren(STMT s1)
{
	if (parentTo.count(s1) > 0)
		return parentTo[s1].second;
	return vector<STMT>();
}

//O(1)
STMT ParentTable:: getParent(STMT s2) 
{
	if (parentFrom.count(s2) > 0)
		return parentFrom[s2];
	return -1;
}

//O(n) <= O(r), where n is the depth of s2
bool ParentTable::isParentStar(STMT s1, STMT s2)
{
	STMT parent = s2;
	do {
		if (parentFrom.count(parent) > 0)
			parent = parentFrom[parent];
		else
			return false;
	} while (parent != s1);
	return true;
}

//O(n) <= O(r), where n is the number of children of s1
vector<STMT> ParentTable::getChildrenStar(STMT s1) 
{
	vector<STMT> output;
	stack<STMT> stack;
	STMT current;
	
	vector<STMT> children = getChildren(s1);
	for (auto it = children.rbegin(); it < children.rend(); it++)
		stack.push(*it);
	while (!stack.empty()) {
		current = stack.top();
		stack.pop();
		output.push_back(current);
		children = getChildren(current);
		for (auto it = children.rbegin(); it < children.rend(); it++)
			stack.push(*it);
	}
	return output;
}

//O(n) <= O(r), where n is the depth of s2
vector <STMT> ParentTable::getParentStar(STMT s2) 
{
	vector <STMT> answer;
	STMT parent = s2;
	while (parentFrom.count(parent) > 0) {
		parent = parentFrom[parent];
		answer.push_back(parent);
	}
	return answer;
}

