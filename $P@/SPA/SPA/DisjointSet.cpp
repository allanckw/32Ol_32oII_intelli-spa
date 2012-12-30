#pragma once
#include "DisjointSet.h"

DisjointSet::DisjointSet()
{
	dirty = false;
}

DisjointSet::~DisjointSet()
{
}

void DisjointSet::makeSet(string var)
{
	if (parent.count(var) == 0) {
		index[var] = list.size();
		parent[var] = var;
		list.push_back(var);
		dirty = true;
	}
}

string DisjointSet::find(string var)
{
	if (parent[var] == var)
		return var;
	else {
		parent[var] = find(parent[var]);
		return parent[var];
	}
}

void DisjointSet::setUnion(string var1, string var2)
{
	makeSet(var1);
	makeSet(var2);
	parent[find(var1)] = find(var2);
	dirty = true;
}

void DisjointSet::makeComponents()
{
	components.clear();
	outputComponents.clear();
	for (auto it = list.begin(); it != list.end(); it++)
		components[find(*it)].insert(*it);
	for (auto it = components.begin(); it != components.end(); it++)
		outputComponents.push_back((*it).second);
	dirty = false;
}

vector<unordered_set<string>> DisjointSet::getComponents()
{
	if (dirty)
		makeComponents();
	return outputComponents;
}