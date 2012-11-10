#pragma once
#include "stdAfx.h"

class QueryCluster
{
public:
	vector<string> synonyms;
	unordered_set<string> synonymsSet;
	//condition: v.varName='x' => (v, <varName, x>)
	unordered_map< string, unordered_map<QueryEnums::QueryCond, int> > conditions;
	//relation:  Modifies(s,v) => (s, <Modifies, v>)
	unordered_map< string, set<pair<QueryEnums::QueryRel, string>> > relations;
	bool marked;

}