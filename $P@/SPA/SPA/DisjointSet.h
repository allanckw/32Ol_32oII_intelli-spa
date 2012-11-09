#pragma once
#include "stdAfx.h"

class DisjointSet
{
private:
	bool dirty;
	vector<string> list;
	unordered_map<string, int> index;
	unordered_map<string, string> parent;

	unordered_map<string, unordered_set<string>> components;
	vector<unordered_set<string>> outputComponents;

	void makeSet(string);
	string find(string);
	void makeComponent();

public:
	DisjointSet(void);
	~DisjointSet(void);
	void setUnion(string, string);

	vector<unordered_set<string>> getComponents();
}

/*
--------------------------
for example
Select <s1,s2> such that modifies(s1,v1) and uses(s2,v2)
When we read  modifies(s1,v1)
do: setUnion(s1,v1);
When we read  uses(s2,v2)
do: setUnion(s2,v2);

star(s1)
star(s2)
Example OVER
--------------------------
*/