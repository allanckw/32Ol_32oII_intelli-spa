#include "StdAfx.h"
#include "ParentTable.h"


ParentTable::ParentTable()
{
}


ParentTable::~ParentTable()
{
}

//Armotised: O(1) (O(n) after n insertions)
//Worst: O(r) <= O(n), where r, n is the number of relationships already, total relationships resp.
void ParentTable::insertParent(STMT s1, STMT s2)
{
	parentTo[s1][s2];
	parentFrom[s2] = s1;
}

//O(1)
bool ParentTable::isParent(STMT s1, STMT s2)
{
	return (parentTo.count(s1) && parentTo[s1].count(s2)) > 0;
}

//O(c) <= O(n), where c, n is the number of children, total relationships respectively
vector<STMT> ParentTable::getChildren(STMT s1)
{
	vector<STMT> output;
	if (parentTo.count(s1)) {
		unordered_map<STMT, void> children = parentTo[s1];
		for (auto it = children.begin(); it != children.end(); it++)
			output.push_back(it->first);
	}
	return output;
}

//O(1)
STMT ParentTable:: getParent(STMT s2) 
{
	if (parentFrom.count(s2))
		return parentFrom[s2];
	else
		return -1;
}

//O(p) <= O(n), where p, n is the depth of s2, total relationships respectively
bool ParentTable::isParentStar(STMT s1, STMT s2)
{
	STMT parent = s2;
	do {
		if (parentFrom.count(parent))
			parent = parentFrom[parent];
		else
			return false;
	} while (parent != s1);
	return true;
}

//O(c) <= O(n), where c, n is the number of children of s1, total relationships respectively
vector<STMT> ParentTable::getChildrenStar(STMT s1) 
{
	vector<STMT> output;
	stack<STMT> stack;
	STMT current;
	//int numSTMT = PKB->statements.getSize();
	int numSTMT = 100; //temporarily
	
	vector<STMT> children = getChildren(s1);
	for (auto it = children.end(); it != children.begin(); it--)
		stack.push(*it);
	while (!stack.empty()) {
		current = stack.top();
		stack.pop();
		output.push_back(current);
		children = getChildren(current);
		for (auto it = children.end(); it != children.begin(); it--)
			stack.push(*it);
	}
	return output;
}

//O(p) <= O(n), where p, n is the depth of s2, total relationships respectively
vector <STMT> ParentTable::getParentStar(STMT s2) 
{
	vector <STMT> answer;
	STMT parent = s2;
	do {
		if (parentFrom.count(parent)) {
			parent = parentFrom[parent];
			answer.push_back(parent);
		} else
			return answer;
	} while (true);
}
