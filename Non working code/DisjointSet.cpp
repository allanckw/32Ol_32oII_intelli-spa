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
		list.push_back(var);
		index[var] = starred.size();
		parent[var] = var;
		starred.push_back(false);
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

void DisjointSet::bondage(string var1, string var2)
{
	makeSet(var1);
	makeSet(var2);
	parent[find(var1)] = find(var2);
	dirty = true;
}

void DisjointSet::star(string var)
{
	starred[parent[var]] = true;
	dirty = true;
}

void DisjointSet::makeComponent()
{
	components.clear();
	for (auto it = parent.begin(); it != parent.end(); it++) {
		components[(*it).second].insert((*it).first);
	}
	for (auto it = components.begin(); it != components.end(); it++) {
		if (starred[index[(*it).first]])
			starredComponents.push_back((*it).second);
		else
			unstarredComponents.push_back((*it).second);
	}
}

vector<unordered_set<string>> DisjointSet::getUnstarred()
{
	if (dirty) {
		makeComponent();
		dirty = false;
	}
	return unstarredComponents;
}

vector<unordered_set<string>> DisjointSet::getStarred()
{
	if (dirty) {
		makeComponent();
		dirty = false;
	}
	return starredComponents;
}