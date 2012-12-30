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

	string find(string);
	void makeComponents();

public:
	DisjointSet(void);
	~DisjointSet(void);

	void makeSet(string);
	void setUnion(string, string);

	vector<unordered_set<string>> getComponents();
};