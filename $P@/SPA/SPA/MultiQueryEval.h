#pragma once
#include "stdafx.h"
#include "RulesOfEngagement.h"
#include "SynonymTable.h"
#include "DisjointSet.h"
//#include "MultiQueryTree.h"
//#include "StagingArea.h"

class MultiQueryEval
{
private:
	static string getToken(string, int& pos);

	MultiQueryEval(string query);

	SynonymTable synonymTable;
	DisjointSet disjointSet;
	bool selectBOOLEAN;
	unordered_map<string, int> inWhichTable; //can incorporate in synonym table
	vector<string> finalanswer;

	vector<vector<int>> makeTable(string rel);
	vector<vector<int>> combine(vector<vector<int>> firstTable, string firstRel,
		vector<vector<int>> secondTable, string secondRel, RulesOfEngagement::isRelation rel);
	vector<vector<int>> prune(vector<vector<int>> table, string firstRel,
		string secondRel, RulesOfEngagement::isRelation rel);

public:
	static vector<string> evaluateQuery(string query);
};