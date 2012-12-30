#pragma once
#include "AnswerTable.h"
#include "PKB.h"
#include "QueryEnums.h"
#include "Helper.h"

AnswerTable::AnswerTable()
{
}

AnswerTable::AnswerTable(SynonymTable synonymTable, string synonym)
{
	header.push_back(synonym);
	synonymPosition[synonym] = 0;
	bool unrestricted = true;

	vector<int> table;
	unordered_map<string, string> attributes = synonymTable.getAllAttributes(synonym);
	unordered_set<QueryEnums::QueryReladition> selfReferences = synonymTable.getAllSelfReferences(synonym);

	if (attributes.count("stmtNo") > 0) {
		table.push_back(Helper::stringToInt(attributes["stmtNo"]));
		unrestricted = false;
	}

	if (unrestricted && attributes.count("procName") > 0) {
		if (synonymTable.getType(synonym) == QueryEnums::Procedure)
			table.push_back(PKB::procedures.getPROCIndex(attributes["procName"]));
		else if (synonymTable.getType(synonym) == QueryEnums::Calls)
			table = PKB::calls.getStmtCall(
			PKB::procedures.getPROCIndex(attributes["procName"]));
		unrestricted = false;
	}

	if (attributes.count("varName") > 0) {
		table.push_back(PKB::variables.getVARIndex(attributes["varName"]));
		unrestricted = false;
	}

	if (attributes.count("value") > 0) {
		table.push_back(Helper::stringToInt(attributes["value"]));
		unrestricted = false;
	}

	if (selfReferences.size() > 0) {
		table.empty();
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

void AnswerTable::patternPrune(string synonym, bool modifiesIsSynonym,
	int modifies, string uses)
{
	int firstRelIndex = synonymPosition[synonym];

	vector<vector<int>> newTable;
	if (modifiesIsSynonym) {
		for (auto it = answers.begin(); it != answers.end(); it++)
			if (RulesOfEngagement::satisfyPattern((*it)[firstRelIndex], (*it)[modifies], uses))
				newTable.push_back(*it);
	} else {
		for (auto it = answers.begin(); it != answers.end(); it++)
			if (RulesOfEngagement::satisfyPattern((*it)[firstRelIndex], modifies, uses))
				newTable.push_back(*it);
	}
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