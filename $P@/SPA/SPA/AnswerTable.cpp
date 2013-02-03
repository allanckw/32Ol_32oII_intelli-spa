#pragma once
#include "AnswerTable.h"
#include "PKB.h"
#include "Helper.h"

AnswerTable::AnswerTable()
{
}

/**
* Constructs the AnswerTable for the synonym. Values are those that satisfy all conditions.
* This excludes all other relations.
* @param synonymTable contains all the information about the synonym
* @param synonym the name of the synonym to construct
* @return the AnswerTable containing the one synonym
*/
AnswerTable::AnswerTable(const SynonymTable& synonymTable, const string& synonym)
{
	header.push_back(synonym);
	synonymPosition.insert(pair<string, int>(synonym, 0));
	bool unrestricted = true;

	vector<int> table;
	const unordered_map<string, string>& specificAttributes =
		synonymTable.getAllSpecificAttributes(synonym);
	const unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>&
		genericAttributes = synonymTable.getAllGenericAttributes(synonym);
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

	if (specificAttributes.count("stmt#") > 0) {
		const int stmtNo = Helper::stringToInt(specificAttributes.at("stmt#"));
		if (stmtNo > PKB::maxProgLines)
			return;
		table.push_back(stmtNo);
		unrestricted = false;
	}

	if (unrestricted && specificAttributes.count("procName") > 0) {
		const string& procName = specificAttributes.at("procName");
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

	if (specificAttributes.count("varName") > 0) {
		const string& varName = specificAttributes.at("varName");
		const VARIndex varIndex = PKB::variables.getVARIndex(
			varName.substr(1, varName.length() - 2));
		if (varIndex == -1)
			return;

		table.push_back(varIndex);
		unrestricted = false;
	}

	if (specificAttributes.count("value") > 0) {
		const int value = Helper::stringToInt(specificAttributes.at("value"));
		if (PKB::constantsTable.count(value) == 0)
			return;
		table.push_back(value);
		unrestricted = false;
	}

	if (unrestricted)
		table = RulesOfEngagement::getType(synonymTable.getType(synonym))();

	for (auto it = genericAttributes.begin(); it != genericAttributes.end(); it++) {
		const string& firstCondition = it->first;
		const unordered_map<RulesOfEngagement::QueryVar, string>& itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			const RulesOfEngagement::QueryVar secondVar = it2->first;
			const string& secondCondition = it2->second;

			const RulesOfEngagement::QueryVar attributeType =
				RulesOfEngagement::conditionTypes[firstCondition];
			const vector<int>& table2 = RulesOfEngagement::getType(secondVar)();

			vector<int> newTable;
			switch (attributeType) {
			case RulesOfEngagement::Integer:
				for (auto it3 = table.begin(); it3 != table.end(); it3++)
					for (auto it4 = table2.begin(); it4 != table2.end(); it4++)
						if (*it3 == *it4) {
							newTable.push_back(*it3);
							break;
						}
				break;
		
			case RulesOfEngagement::String:
			{
				vector<string> RHSequivs;
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++) {
					string RHSequiv;
					switch (secondVar) {
					case RulesOfEngagement::Procedure: //procName
						RHSequiv = PKB::procedures.getPROCName(*it3);
					case RulesOfEngagement::Variable: //varName
						RHSequiv = PKB::variables.getVARName(*it3);
					}
				}

				for (auto it3 = table.begin(); it3 != table.end(); it3++) {
					string LHSequiv;
					switch (synonymTable.getType(synonym)) {
					case RulesOfEngagement::Procedure: //procName
						LHSequiv = PKB::procedures.getPROCName(*it3);
					case RulesOfEngagement::Variable: //varName
						LHSequiv = PKB::variables.getVARName(*it3);
					}

					auto it4 = table2.begin();
					auto it5 = RHSequivs.begin();
					for (; it4 != table2.end(); it4++, it5++) {
						if (LHSequiv == *it5) {
							newTable.push_back(*it3);
							break;
						}
					} //end table2 iteration
				} //end table iteration
			} //end case if comparing strings
			} //end switch of comparison type
			table = newTable;
		}
	} //end generic attributes

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

/**
* Scans through two AnswerTables and combine those rows where the synonyms in each Answertable
* satisfy the relation.
* @param ownSynonym name of synonym in this AnswerTable
* @param otherTable pointer to other AnswerTable
* @param otherSynonym name of synonym in the other AnswerTable
* @param rel function pointer to method to evaluate satisfiability of relation
* @return the AnswerTable having combined with the other AnswerTable
*/
void AnswerTable::combine(const string& ownSynonym, const AnswerTable& otherTable,
	const string& otherSynonym, const RulesOfEngagement::isRelation rel)
{
	const int firstRelIndex = synonymPosition[ownSynonym];
	const int secondRelIndex = otherTable.synonymPosition.at(otherSynonym);

	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++)
		for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++)
			if (rel((*it)[firstRelIndex], (*it2)[secondRelIndex])) {
				vector<int> newRow(*it);
				newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
				newTable.push_back(newRow);
			}
	answers = newTable;

	for (auto it = otherTable.header.begin(); it != otherTable.header.end(); it++) {
		synonymPosition.insert(pair<string, int>(*it, header.size()));
		header.push_back(*it);
	}
}

/**
* Scans through the AnswerTable and keep those rows where the synonyms satisfy the relation.
* @param firstSynonym name of first synonym
* @param secondSynonym name of second synonym
* @param rel function pointer to method to evaluate satisfiability of relation
* @return the AnswerTable
*/
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

/**
* Scans through two AnswerTables and combine those rows where the synonyms in each Answertable
* satisfy the relation as defined with the 'with' keyword.
* @param ownSynonym name of synonym in this AnswerTable
* @param otherTable pointer to other AnswerTable
* @param otherSynonym name of synonym in the other AnswerTable
* @param rel function pointer to method to evaluate satisfiability of relation
* @return the AnswerTable having combined with the other AnswerTable
*/
void AnswerTable::withCombine(const SynonymTable& synonymTable, const string& firstSynonym,
	const string& firstCondition, const AnswerTable& otherTable,
	const string& secondSynonym, const string& secondCondition)
{
	const int firstRelIndex = synonymPosition[firstSynonym];
	const RulesOfEngagement::QueryVar firstVar = synonymTable.getType(firstSynonym);
	
	const int secondRelIndex = synonymPosition[secondSynonym];
	const RulesOfEngagement::QueryVar secondVar = synonymTable.getType(secondSynonym);

	const RulesOfEngagement::QueryVar attributeType =
		RulesOfEngagement::conditionTypes[firstCondition];

	vector<vector<int>> newTable;
	switch (attributeType) {
	case RulesOfEngagement::Integer:
		for (auto it = answers.begin(); it != answers.end(); it++)
			for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++)
				if ((*it)[firstRelIndex] == (*it2)[secondRelIndex]) {
					vector<int> newRow(*it);
					newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
					newTable.push_back(newRow);
				}
		break;
		
	case RulesOfEngagement::String:
	{
		vector<string> RHSequivs;
		for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
			int RHS = (*it2)[secondRelIndex];
			string RHSequiv;
			switch (secondVar) {
			case RulesOfEngagement::Procedure: //procName
				RHSequiv = PKB::procedures.getPROCName(RHS);
			case RulesOfEngagement::Variable: //varName
				RHSequiv = PKB::variables.getVARName(RHS);
			}
		}

		for (auto it = answers.begin(); it != answers.end(); it++) {
			int LHS = (*it)[firstRelIndex];
			string LHSequiv;
			switch (firstVar) {
			case RulesOfEngagement::Procedure: //procName
				LHSequiv = PKB::procedures.getPROCName(LHS);
			case RulesOfEngagement::Variable: //varName
				LHSequiv = PKB::variables.getVARName(LHS);
			}

			auto it2 = otherTable.answers.begin();
			auto it3 = RHSequivs.begin();
			for (; it2 != otherTable.answers.end(); it2++, it3++) {
				
				if (LHSequiv == *it3) {
					vector<int> newRow(*it);
					newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
					newTable.push_back(newRow);
				}
			}
		}
	}
	break;
	}
	answers = newTable;

	for (auto it = otherTable.header.begin(); it != otherTable.header.end(); it++) {
		synonymPosition.insert(pair<string, int>(*it, header.size()));
		header.push_back(*it);
	}
}

/**
* Scans through the AnswerTable and keep those rows where the synonyms satisfy the relation
* as defined with the 'with' keyword.
* @param firstSynonym name of first synonym
* @param secondSynonym name of second synonym
* @param rel function pointer to method to evaluate satisfiability of relation
* @return the AnswerTable
*/
void AnswerTable::withPrune(const SynonymTable& synonymTable, const string& firstSynonym,
	const string& firstCondition, const string& secondSynonym, const string& secondCondition)
{
	const int firstRelIndex = synonymPosition[firstSynonym];
	const RulesOfEngagement::QueryVar firstVar = synonymTable.getType(firstSynonym);
	
	const int secondRelIndex = synonymPosition[secondSynonym];
	const RulesOfEngagement::QueryVar secondVar = synonymTable.getType(secondSynonym);

	const RulesOfEngagement::QueryVar attributeType =
		RulesOfEngagement::conditionTypes[firstCondition];

	vector<vector<int>> newTable;
	switch (attributeType) {
	case RulesOfEngagement::Integer:
		for (auto it = answers.begin(); it != answers.end(); it++)
			if ((*it)[firstRelIndex] == (*it)[secondRelIndex])
				newTable.push_back(*it);
		break;
		
	case RulesOfEngagement::String:
		for (auto it = answers.begin(); it != answers.end(); it++) {
			int LHS = (*it)[firstRelIndex];
			string LHSequiv;
			switch (firstVar) {
			case RulesOfEngagement::Procedure: //procName
				LHSequiv = PKB::procedures.getPROCName(LHS);
			case RulesOfEngagement::Variable: //varName
				LHSequiv = PKB::variables.getVARName(LHS);
			}

			int RHS = (*it)[secondRelIndex];
			string RHSequiv;
			switch (secondVar) {
			case RulesOfEngagement::Procedure: //procName
				RHSequiv = PKB::procedures.getPROCName(RHS);
			case RulesOfEngagement::Variable: //varName
				RHSequiv = PKB::variables.getVARName(RHS);
			}
			if (LHSequiv == RHSequiv)
				newTable.push_back(*it);
		}
	}
	answers = newTable;
}
/**
* This method will be used to evaluate the pattern relation and return
the assignment,if,while that are valid
* @param synonym the pattern's LHS var
* @param RHS the right hand side's type
* @param RHSVarName right hand side's variable name
* @param RHSexprs a right hand side expression tree
* @return the AnswerTable
*/
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


/**
* Creates a new AnswerTable with only the columns corresponding to the synonyms that are
* required. The required synonyms are given in a vector given in the argument.
* @param selection list of synonyms to be projected out
* @return a new AnswerTable with only the selected columns
*/
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

/**
* Modifies this AnswerTable by taking the Cartesian Product of this AnswerTable
* with the AnswerTable given in the argument.
* @param otherTable pointer to the other AnswerTable
* @return the AnswerTable having been cartesianed with the other AnswerTable
*/
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

/**
* Returns a vector of the synonyms included in this AnswerTable.
* @return the vector of the synonyms
*/
vector<string> AnswerTable::getHeader() const
{
	return header;
}

/**
* Returns the number of synonyms in this AnswerTable.
* @return the number of synonyms
*/
unsigned int AnswerTable::getSize() const
{
	return answers.size();
}

/**
* Returns a particular row of this AnswerTable. The validity of the value given is not checked.
* @param index index to the row
* @return the vector corresponding to the required row
*/
vector<int> AnswerTable::getRow(const int index) const
{
	return answers[index];
}