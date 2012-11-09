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

void DisjointSet::makeComponent()
{
	components.clear();
	outputComponents.clear();
	for (auto it = parent.begin(); it != parent.end(); it++)
		components[(*it).second].insert((*it).first);
	for (auto it = components.begin(); it != components.end(); it++)
		outputComponents.push_back((*it).second);
}

vector<unordered_set<string>> DisjointSet::getComponents()
{
	if (dirty) {
		makeComponent();
		dirty = false;
	}
	return outputComponents;
}