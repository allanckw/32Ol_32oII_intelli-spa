#pragma once
#include "stdAfx.h"

class QueryCluster
{
public:
	vector<string> synonyms;
	unordered_set<string> synonymsSet;
	//condition: v.varName='x' => (v, <varName, x>)
	unordered_map< string, unordered_map<QueryEnums::QueryReladition, int> > conditions;
	//relation:  Modifies(s,v) => (s, <Modifies, v>)
	unordered_map< string, set<pair<QueryEnums::QueryReladition, string>> > relations;
	bool marked;

};