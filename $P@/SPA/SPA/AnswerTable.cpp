#pragma once
#include "AnswerTable.h"
#include "PKB.h"
#include "QueryEnums.h"

AnswerTable::AnswerTable()
{
}

AnswerTable::AnswerTable(SynonymTable synonymTable, string synonym)
{
	header.push_back(synonym);
	synonymPosition[synonym] = 0;
	bool unrestricted = true;

	vector<int> table;
	if (synonymTable.getStmtNo(synonym) != -1) {
		table.push_back(synonymTable.getStmtNo(synonym));
		unrestricted = false;
	}

	if (unrestricted && synonymTable.getProcName(synonym) != "") {
		if (synonymTable.getType(synonym) == QueryEnums::Procedure)
			table.push_back(PKB::procedures.getPROCIndex(synonymTable.getProcName(synonym)));
		else if (synonymTable.getType(synonym) == QueryEnums::Calls)
			table = PKB::calls.getStmtCall(
			PKB::procedures.getPROCIndex(synonymTable.getProcName(synonym)));
		unrestricted = false;
	}

	if (synonymTable.getVarName(synonym) != "") {
		table.push_back(PKB::variables.getVARIndex(synonymTable.getVarName(synonym)));
		unrestricted = false;
	}

	if (synonymTable.getValue(synonym) != -1) {
		table.push_back(synonymTable.getValue(synonym));
		unrestricted = false;
	}

	if (unrestricted)
		table = RulesOfEngagement::getType(synonymTable.getType(synonym))();

	//convert vector<int> to vector<vector<int>>
	for (auto it = table.begin(); it != table.end(); it++) {
		vector<int> temp;
		temp.push_back(*it);
		answers.push_back(temp);
	}
}

void AnswerTable::combine(string ownSynonym, AnswerTable otherTable,
	string otherSynonym, RulesOfEngagement::isRelation rel)
{
	int firstRelIndex = synonymPosition[ownSynonym];
	int secondRelIndex = otherTable.synonymPosition[otherSynonym];

	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++) {
		for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++)
			if (rel((*it)[firstRelIndex], (*it2)[secondRelIndex])) {
				vector<int> newRow(*it);
				newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
				newTable.push_back(newRow);
			}
	}
	answers = newTable;

	for (auto it = otherTable.header.begin(); it != otherTable.header.end(); it++) {
		synonymPosition[*it] = header.size();
		header.push_back(*it);
	}
}

void AnswerTable::prune(string firstSynonym,
	string secondSynonym, RulesOfEngagement::isRelation rel)
{
	int firstRelIndex = synonymPosition[firstSynonym];
	int secondRelIndex = synonymPosition[secondSynonym];

	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++)
		if (rel((*it)[firstRelIndex], (*it)[secondRelIndex]))
			newTable.push_back(*it);
	answers = newTable;
}

AnswerTable AnswerTable::project(vector<string> selection)
{
	AnswerTable newTable;

	vector<int> indices;
	for (auto it = selection.begin(); it != selection.end(); it++)
		indices.push_back(synonymPosition[*it]);

	for (auto it = indices.begin(); it != indices.end(); it++) {
		newTable.synonymPosition[header[*it]] = newTable.header.size();
		newTable.header.push_back(header[*it]);
	}
	
	for (auto it = answers.begin(); it != answers.end(); it++) {
		vector<int> newRow;
		for (auto it2 = indices.begin(); it2 != indices.end(); it2++)
			newRow.push_back((*it)[*it2]);
		newTable.answers.push_back(newRow);
	}

	return newTable;
}

void AnswerTable::cartesian(AnswerTable otherTable)
{
	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++) {
		for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
			vector<int> newRow(*it);
			newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
			newTable.push_back(newRow);
		}
	}
	answers = newTable;

	for (auto it = otherTable.header.begin(); it != otherTable.header.end(); it++) {
		synonymPosition[*it] = header.size();
		header.push_back(*it);
	}
}

vector<string> AnswerTable::getHeader()
{
	return header;
}

int AnswerTable::getSize()
{
	return answers.size();
}

vector<int> AnswerTable::getRow(int index)
{
	return answers[index];
}