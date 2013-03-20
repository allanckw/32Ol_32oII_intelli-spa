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

	void makeComponents();

public:
	DisjointSet();

	void makeSet(string);
	void setUnion(string, string);
	
	string find(string);
	vector<unordered_set<string>> getComponents();
};