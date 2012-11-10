#pragma once
#include "StdAfx.h"
#include "DisjointSet.h"
#include "QueryEnums.h"
#include "QueryCluster.h"

class MultiQueryTree
{
private:
	//creation
	unordered_map<string, QueryEnums::QueryVar> synonymToType;
	unordered_map<QueryEnums::QueryVar, string> typeToSynonym;
	DisjointSet synonyms;
	
	//condition: v.varName='x' => (v, <varName, x>)
	unordered_map< string, unordered_map<QueryEnums::QueryCond, int> > unsortedConditions;
	//relation:  Modifies(s,v) => (s, <Modifies, v>)
	unordered_map< string, set<pair<QueryEnums::QueryRel, string>> > unsortedRelations;
	
	vector<string> selectedSynonyms;
	unordered_set<string> selectedSynonymsSet;

	//evaluation
	vector<QueryCluster> tree;

public:
	//creation
	void insertSynonym(string synonym, QueryEnums::QueryVar);
	bool insertCondition(string synonym, QueryEnums::QueryCond attribute, int condition);
	void insertRelationship(QueryEnums::QueryRel, string synonym1, string synonym2);
	void insertSelectSynonym(string synonym);
	void buildTree();

	//evaluation
	unordered_map<string, QueryEnums::QueryVar> getSelectedSynonymToType();
	unordered_map<QueryEnums::QueryVar, string> getSelectedTypeToSynonym();
	vector<QueryCluster> getTree();
};