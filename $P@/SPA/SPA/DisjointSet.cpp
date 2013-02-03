#pragma once
#include "DisjointSet.h"

DisjointSet::DisjointSet()
{
	dirty = false;
}


/**
* Initalises the element in the disjoint set.
* @param var the name of the element
*/
void DisjointSet::makeSet(string var)
{
	if (parent.count(var) == 0) {
		index[var] = list.size();
		parent[var] = var;
		list.push_back(var);
		dirty = true;
	}
}

/**
* Returns the name of the representative of the partition in which the element lies.
* @param var the name of the element
* @return the name of the representative
*/
string DisjointSet::find(string var)
{
	if (parent[var] == var)
		return var;
	else {
		parent[var] = find(parent[var]);
		return parent[var];
	}
}

/**
* Puts two elements in the same partition.
* @param var1 the name of the first element
* @param var2 the name of the second element
*/
void DisjointSet::setUnion(string var1, string var2)
{
	makeSet(var1);
	makeSet(var2);
	parent[find(var1)] = find(var2);
	dirty = true;
}

/**
* Identifies the elements belonging to each partition. Caches the result.
*/
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

/**
* Returns a list of a set of all elements belonging to each partition.
* @return the list in the form of a vector
*/
vector<unordered_set<string>> DisjointSet::getComponents()
{
	if (dirty)
		makeComponents();
	return outputComponents;
}