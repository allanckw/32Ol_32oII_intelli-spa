#pragma once
#include "stdAfx.h"
#include "RulesOfEngagement.h"
#include "SynonymTable.h"

class AnswerTable
{
private:
	AnswerTable();

	vector<string> header;
	vector<vector<int>> answers;

public:
	unordered_map<string, int> synonymPosition;

	AnswerTable(SynonymTable synonymTable, string synonym);
	void combine(string ownSynonym, AnswerTable otherTable, string otherSynonym,
		RulesOfEngagement::isRelation rel); //synonyms are all disjoint
	void prune(string firstSynonym, string secondSynonym, RulesOfEngagement::isRelation rel);
	
	AnswerTable project(vector<string> selection);
	void cartesian(AnswerTable otherTable);

	vector<string> getHeader();
	int getSize();
	vector<int> getRow(int index);
	//int getEntry(int index, string synonym);*/
};