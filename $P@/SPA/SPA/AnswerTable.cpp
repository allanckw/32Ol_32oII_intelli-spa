#pragma once
#include "AnswerTable.h"
#include "PKB.h"
#include "Helper.h"

AnswerTable::AnswerTable()
{
}

AnswerTable::AnswerTable(const SynonymTable& synonymTable, const string& synonym)
{
	header.push_back(synonym);
	synonymPosition.insert(pair<string, int>(synonym, 0));
	bool unrestricted = true;

	vector<int> table;
	const unordered_map<string, string>& attributes = synonymTable.getAllAttributes(synonym);
	const unordered_set<RulesOfEngagement::QueryRelations>& selfReferences =
		synonymTable.getAllSelfReferences(synonym);
	const unordered_set<RulesOfEngagement::QueryRelations>& firstGeneric =
		synonymTable.getAllFirstGeneric(synonym);
	const vector<pair<RulesOfEngagement::QueryRelations, string>>& firstSpecific =
		synonymTable.getAllFirstSpecific(synonym);
	const unordered_set<RulesOfEngagement::QueryRelations>& secondGeneric =
		synonymTable.getAllSecondGeneric(synonym);
	const vector<pair<RulesOfEngagement::QueryRelations, string>>& secondSpecific =
		synonymTable.getAllSecondSpecific(synonym);

	if (attributes.count("stmtNo") > 0) {
		const int stmtNo = Helper::stringToInt(attributes.at("stmtNo"));
		if (stmtNo > PKB::maxProgLines)
			return;
		table.push_back(stmtNo);
		unrestricted = false;
	}

	if (unrestricted && attributes.count("procName") > 0) {
		const string& procName = attributes.at("procName");
		const PROCIndex procIndex = PKB::procedures.getPROCIndex(
			procName.substr(1, procName.length() - 2));
		if (procIndex == -1)
			return;

		if (synonymTable.getType(synonym) == RulesOfEngagement::Procedure)
			table.push_back(procIndex);
		else if (synonymTable.getType(synonym) == RulesOfEngagement::Calls)
			table = PKB::calls.getStmtCall(procIndex);
		unrestricted = false;
	}

	if (attributes.count("varName") > 0) {
		const string& varName = attributes.at("varName");
		const VARIndex varIndex = PKB::variables.getVARIndex(
			varName.substr(1, varName.length() - 2));
		if (varIndex == -1)
			return;

		table.push_back(varIndex);
		unrestricted = false;
	}

	if (attributes.count("value") > 0) {
		const int value = Helper::stringToInt(attributes.at("value"));
		if (PKB::constantsTable.count(value) == 0)
			return;
		table.push_back(value);
		unrestricted = false;
	}

	if (unrestricted)
		table = RulesOfEngagement::getType(synonymTable.getType(synonym))();

	for (auto it = selfReferences.begin(); it != selfReferences.end(); it++) {
		const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
		vector<int> table2;
		for (auto it2 = table.begin(); it2 != table.end(); it2++)
			if (rel(*it2, *it2))
				table2.push_back(*it2);
		table = table2;
	}

	for (auto it = firstGeneric.begin(); it != firstGeneric.end(); it++) {
		//to optimise
		const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
		const vector<int>& table2 = RulesOfEngagement::getType(
			RulesOfEngagement::privilegedSecondArgument[*it])();
		vector<int> newTable;
		for (auto it2 = table.begin(); it2 != table.end(); it2++)
			for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
				if (rel(*it2, *it3)) {
					newTable.push_back(*it2);
					break;
				}
		table = newTable;
	}

	for (auto it = firstSpecific.begin(); it != firstSpecific.end(); it++) {
		//to optimise
		const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
		const int arg = RulesOfEngagement::convertArgumentToInteger((*it).first, false, (*it).second);
		vector<int> newTable;
		for (auto it2 = table.begin(); it2 != table.end(); it2++)
			if (rel(*it2, arg))
				newTable.push_back(*it2);
		table = newTable;
	}

	for (auto it = secondGeneric.begin(); it != secondGeneric.end(); it++) {
		//to optimise
		const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
		const vector<int>& table2 = RulesOfEngagement::getType(
			RulesOfEngagement::privilegedFirstArgument[*it])();
		vector<int> newTable;
		for (auto it2 = table.begin(); it2 != table.end(); it2++)
			for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
				if (rel(*it3, *it2)) {
					newTable.push_back(*it2);
					break;
				}
		table = newTable;
	}

	for (auto it = secondSpecific.begin(); it != secondSpecific.end(); it++) {
		//to optimise
		const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
		const int arg = RulesOfEngagement::convertArgumentToInteger((*it).first, true, (*it).second);
		vector<int> newTable;
		for (auto it2 = table.begin(); it2 != table.end(); it2++)
			if (rel(arg, *it2))
				newTable.push_back(*it2);
		table = newTable;
	}

	//convert vector<int> to vector<vector<int>>
	for (auto it = table.begin(); it != table.end(); it++) {
		vector<int> temp;
		temp.push_back(*it);
		answers.push_back(temp);
	}
}

void AnswerTable::combine(const string& ownSynonym, const AnswerTable& otherTable,
	const string& otherSynonym, const RulesOfEngagement::isRelation rel)
{
	const int firstRelIndex = synonymPosition[ownSynonym];
	const int secondRelIndex = otherTable.synonymPosition.at(otherSynonym);

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
		synonymPosition.insert(pair<string, int>(*it, header.size()));
		header.push_back(*it);
	}
}

void AnswerTable::prune(const string& firstSynonym,
	const string& secondSynonym, const RulesOfEngagement::isRelation rel)
{
	const int firstRelIndex = synonymPosition[firstSynonym];
	const int secondRelIndex = synonymPosition[secondSynonym];

	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++)
		if (rel((*it)[firstRelIndex], (*it)[secondRelIndex]))
			newTable.push_back(*it);
	answers = newTable;
}

/*void AnswerTable::withPrune(const string& firstSynonym, const RulesOfEngagement::QueryVar firstVar,
	const string& firstCondition, const string& secondSynonym,
	const RulesOfEngagement::QueryVar secondVar, const string& secondCondition)
{
	const int firstRelIndex = synonymPosition[firstSynonym];
	
	
	const int secondRelIndex = synonymPosition[secondSynonym];


	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++)
		if (RulesOfEngagement::satisfyPattern((*it)[firstRelIndex], RHS, RHSVarName, RHSexprs))
			newTable.push_back(*it);
	answers = newTable;
}*/

void AnswerTable::patternPrune(const string& synonym,
	const RulesOfEngagement::PatternRHSType RHS, const string& RHSVarName, const ASTExprNode* RHSexprs)
{
	const int firstRelIndex = synonymPosition[synonym];

	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++)
		if (RulesOfEngagement::satisfyPattern((*it)[firstRelIndex], RHS, RHSVarName, RHSexprs))
			newTable.push_back(*it);
	answers = newTable;
}

/*void AnswerTable::patternPrune(string synonym, bool modifiesIsSynonym, int modifies,
	RulesOfEngagement::PatternRHSType RHS, string RHSVarName, ASTExprNode* RHSexprs)
{
	int firstRelIndex = synonymPosition[synonym];

	vector<vector<int>> newTable;
	if (modifiesIsSynonym) {
		for (auto it = answers.begin(); it != answers.end(); it++)
			if (RulesOfEngagement::satisfyPattern((*it)[firstRelIndex],
				(*it)[modifies], RHS, RHSVarName, RHSexprs))
				newTable.push_back(*it);
	} else {
		for (auto it = answers.begin(); it != answers.end(); it++)
			if (RulesOfEngagement::satisfyPattern((*it)[firstRelIndex],
				modifies, RHS, RHSVarName, RHSexprs))
				newTable.push_back(*it);
	}
	answers = newTable;
}*/

AnswerTable AnswerTable::project(const vector<string>& selection)
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

void AnswerTable::cartesian(const AnswerTable& otherTable)
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
		synonymPosition.insert(pair<string, int>(*it, header.size()));
		header.push_back(*it);
	}
}

vector<string> AnswerTable::getHeader() const
{
	return header;
}

unsigned int AnswerTable::getSize() const
{
	return answers.size();
}

vector<int> AnswerTable::getRow(const int index) const
{
	return answers[index];
}