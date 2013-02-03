#pragma once
#include "StdAfx.h"
#include "ParentTable.h"

ParentTable::ParentTable()
{
}

/**
* This method will be used to insert the Parent relation.
* Let r be the number of relationships in ParentTable.
* Armotised: O(1) (O(k) after k insertions)
* Worst: O(c) <= O(r), where c is the number of relationships in the table
* @param s1 statement followed by s2
* @param s2 statement following s1
*/
void ParentTable::insertParent(STMT s1, STMT s2)
{
	parentTo[s1].first.insert(s2);
	parentTo[s1].second.push_back(s2); //assume the s2's will be distinct

	parentFrom.insert(pair<STMT, STMT>(s2, s1));
}

/**
* This method will be used to check if the Parent table is empty.
* O(1)
* @return true if the Parent table is empty, and false otherwise
*/
bool ParentTable::isEmpty()
{
	return parentTo.empty();
}

/**
* This method will be used to check if the statement s1 is the parent of statement s2.
* O(1)
* @param s1 statement that is parent of s2
* @param s2 statement that is child of s1
* @return true if statement s1 is an parent of statement s2.
*/
bool ParentTable::isParent(STMT s1, STMT s2)
{
	return (parentTo.count(s1) > 0 && parentTo[s1].first.count(s2) > 0);
}

/**
* This method returns a list of all statements that s1 is the parent of.
* O(1)
* @param s2 target statement
* @return the vector
*/
vector<STMT> ParentTable::getChildren(STMT s1)
{
	if (parentTo.count(s1) > 0)
		return parentTo[s1].second;
	return vector<STMT>();
}

/**
* This method returns the statement that is the parent of s2.
* O(1)
* @param s2 target statement
* @return the statement
*/
STMT ParentTable:: getParent(STMT s2) 
{
	if (parentFrom.count(s2) > 0)
		return parentFrom[s2];
	return -1;
}

/**
* This method will be used to check if statement s1 is an ancestor of statement s2.
* O(n) <= O(r), where n is the depth of s2
* @param s1 ancestor statement
* @param s2 descendant statement
* @return true if statement s1 is an ancestor of statement s2.
*/
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

/**
* This method returns a list of statements that s2 is the ancestor of.
* O(n) <= O(r), where n is the number of children of s1
* @param s1 target statement
* @return the vector
*/
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

/**
* This method returns a list of statements that is the ancestor of s2.
* O(n) <= O(r), where n is the depth of s2
* @param s2 target statement
* @return the vector
*/
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

