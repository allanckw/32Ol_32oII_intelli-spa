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
	
	{
	const unordered_map<string, string>& specificAttributes =
		synonymTable.getAllSpecificAttributes(synonym);

	if (specificAttributes.count("procName") > 0) {
		const string& procName = specificAttributes.at("procName");
		const PROCIndex procIndex = PKB::procedures.getPROCIndex(
			procName.substr(1, procName.length() - 2));
		if (procIndex == -1)
			return;
		
		switch (synonymTable.getType(synonym)) {
		case RulesOfEngagement::Procedure:
			table.push_back(procIndex);
			break;
		case RulesOfEngagement::Calls:
			table = PKB::calls.getStmtCall(procIndex);
			break;
		}
		unrestricted = false;
	}

	if (specificAttributes.count("stmt#") > 0) {
		const int stmtNo = Helper::stringToInt(specificAttributes.at("stmt#"));
		if (stmtNo > PKB::maxProgLines)
			return;

		if (unrestricted) {
			table.push_back(stmtNo);
			unrestricted = false;
		} else { //already a procName in there (must be call)
			bool found = false;
			for (auto it = table.begin(); it != table.end(); it++)
				if (*it == stmtNo) {
					found = true;
					break;
				}
			if (found)
				table = vector<int>(1, stmtNo);
			else
				table = vector<int>();
		}
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
	}

	{
	const vector<pair<RulesOfEngagement::QueryRelations, string>>& firstSpecific =
		synonymTable.getAllFirstSpecific(synonym);
	vector<vector<pair<RulesOfEngagement::QueryRelations, string>>::const_iterator>
		skippedFirstSpecific;
	for (auto it = firstSpecific.begin(); it != firstSpecific.end(); it++) {
		const int arg = RulesOfEngagement::convertArgumentToInteger((*it).first, false, (*it).second);
		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationFromFamily(it->first);
		if (fn == 0) {
			if (unrestricted) {
				skippedFirstSpecific.push_back(it);
				continue;
			}
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
			vector<int> newTable;
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				if (rel(*it2, arg))
					newTable.push_back(*it2);
			table = newTable;
		} else {
			if (unrestricted) {
				switch (synonymTable.getType(synonym)) {
				case RulesOfEngagement::Assign: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::assignTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				case RulesOfEngagement::While: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::whileTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				case RulesOfEngagement::If: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::ifTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				case RulesOfEngagement::Call: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::callTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				default:
					table = fn(arg);
				}
				unrestricted = false;
			} else {
				vector<int>& answers = fn(arg);
				vector<int> newTable;
				if (table.size() <= answers.size()) {
					unordered_set<int> memo(answers.begin(), answers.end());
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (memo.count(*it2) > 0)
							newTable.push_back(*it2);
				} else {
					unordered_set<int> memo(table.begin(), table.end());
					for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
						if (memo.count(*it2) > 0)
							newTable.push_back(*it2);
				}
				table = newTable;
			}
		}
	}
	
	const vector<pair<RulesOfEngagement::QueryRelations, string>>& secondSpecific =
		synonymTable.getAllSecondSpecific(synonym);
	vector<vector<pair<RulesOfEngagement::QueryRelations, string>>::const_iterator>
		skippedSecondSpecific;
	for (auto it = secondSpecific.begin(); it != secondSpecific.end(); it++) {
		const int arg = RulesOfEngagement::convertArgumentToInteger(it->first, true, it->second);
		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationByFamily(it->first);
		if (fn == 0) {
			if (unrestricted) {
				skippedSecondSpecific.push_back(it);
				continue;
			}
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
			vector<int> newTable;
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				if (rel(arg, *it2))
					newTable.push_back(*it2);
			table = newTable;
		} else {
			if (unrestricted) {
				switch (synonymTable.getType(synonym)) {
				case RulesOfEngagement::Assign: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::assignTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				case RulesOfEngagement::While: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::whileTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				case RulesOfEngagement::If: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::ifTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				case RulesOfEngagement::Call: {
					vector<int>& tentative = fn(arg);
					for (auto it = tentative.begin(); it != tentative.end(); it++)
						if (PKB::callTable.count(*it) > 0)
							table.push_back(*it); }
					break;
				default:
					table = fn(arg);
				}
				unrestricted = false;
			} else {
				vector<int>& answers = fn(arg);
				vector<int> newTable;
				if (table.size() <= answers.size()) {
					unordered_set<int> memo(answers.begin(), answers.end());
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (memo.count(*it2) > 0)
							newTable.push_back(*it2);
				} else {
					unordered_set<int> memo(table.begin(), table.end());
					for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
						if (memo.count(*it2) > 0)
							newTable.push_back(*it2);
				}
				table = newTable;
			}
		}
	}

	const unordered_set<RulesOfEngagement::QueryRelations>& firstGeneric =
		synonymTable.getAllFirstGeneric(synonym);
	vector<unordered_set<RulesOfEngagement::QueryRelations>::const_iterator>
		skippedFirstGeneric;
	for (auto it = firstGeneric.begin(); it != firstGeneric.end(); it++) {
		if (unrestricted) {
			skippedFirstGeneric.push_back(it);
			continue;
		}

		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationByFamily(*it);
		vector<int> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedSecondArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(*it2, *it3)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedSecondArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(*it2).size() > 0)
							newTable.push_back(*it2);
				}
		}
		table = newTable;
	} //end first generic
	
	const unordered_set<RulesOfEngagement::QueryRelations>& secondGeneric =
		synonymTable.getAllSecondGeneric(synonym);
	vector<unordered_set<RulesOfEngagement::QueryRelations>::const_iterator>
		skippedSecondGeneric;
	for (auto it = secondGeneric.begin(); it != secondGeneric.end(); it++) {
		if (unrestricted) {
			skippedSecondGeneric.push_back(it);
			continue;
		}

		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationFromFamily(*it);
		vector<int> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedFirstArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(*it3, *it2)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedFirstArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(*it2).size() > 0)
							newTable.push_back(*it2);
				}
		}
		table = newTable;
	}

	if (unrestricted)
		table = RulesOfEngagement::getType(synonymTable.getType(synonym))();

	for (auto it3 = skippedFirstSpecific.begin(); it3 != skippedFirstSpecific.end(); it3++) {
		const auto it = *it3;
		const int arg = RulesOfEngagement::convertArgumentToInteger((*it).first, false, (*it).second);
		
		const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
		vector<int> newTable;
		for (auto it2 = table.begin(); it2 != table.end(); it2++)
			if (rel(*it2, arg))
				newTable.push_back(*it2);
		table = newTable;
	}

	for (auto it3 = skippedSecondSpecific.begin(); it3 != skippedSecondSpecific.end(); it3++) {
		const auto it = *it3;
		const int arg = RulesOfEngagement::convertArgumentToInteger((*it).first, false, (*it).second);
		
		const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
		vector<int> newTable;
		for (auto it2 = table.begin(); it2 != table.end(); it2++)
			if (rel(arg, *it2))
				newTable.push_back(*it2);
		table = newTable;
	}

	for (auto it4 = skippedFirstGeneric.begin(); it4 != skippedFirstGeneric.end(); it4++) {
		const auto it = *it4;

		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationByFamily(*it);
		vector<int> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedSecondArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(*it2, *it3)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedSecondArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(*it2).size() > 0)
							newTable.push_back(*it2);
				}
		}
		table = newTable;
	}

	for (auto it4 = skippedSecondGeneric.begin(); it4 != skippedSecondGeneric.end(); it4++) {
		const auto it = *it4;

		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationFromFamily(*it);
		vector<int> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedFirstArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(*it3, *it2)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedFirstArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(*it2);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(*it2).size() > 0)
							newTable.push_back(*it2);
				}
		}
		table = newTable;
	}
	}
	
	/*{ //optimisation of folding
	const unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>&
		genericAttributes = synonymTable.getAllGenericAttributes(synonym);
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
	}*/
	
	{
	const unordered_set<RulesOfEngagement::QueryRelations>& selfReferences =
		synonymTable.getAllSelfReferences(synonym);
	for (auto it = selfReferences.begin(); it != selfReferences.end(); it++) {
		vector<int> table2;
		if (*it == RulesOfEngagement::NextStar) { //alternative call for more efficient method
			vector<int>& tentative = PKB::next.getNextStar(0); //for evaluating next*(n, n)
			if (table.size() <= tentative.size()) {
				unordered_set<int> memo(tentative.begin(), tentative.end());
				for (auto it2 = table.begin(); it2 != table.end(); it2++)
					if (memo.count(*it2) > 0)
						table2.push_back(*it2);
			} else {
				unordered_set<int> memo(table.begin(), table.end());
				for (auto it2 = tentative.begin(); it2 != tentative.end(); it2++)
					if (memo.count(*it2) > 0)
						table2.push_back(*it2);
			}
		} else {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				if (rel(*it2, *it2))
					table2.push_back(*it2);
		}
		table = table2;
	}
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
* @param rel relation type
* @return the AnswerTable having combined with the other AnswerTable
*/
void AnswerTable::combine(const string& ownSynonym, const AnswerTable& otherTable,
	const string& otherSynonym, const RulesOfEngagement::QueryRelations rel)
{
	const int firstRelIndex = synonymPosition[ownSynonym];
	const int secondRelIndex = otherTable.synonymPosition.at(otherSynonym);
	vector<vector<int>> newTable;

	RulesOfEngagement::relationFamily fn1 = RulesOfEngagement::getRelationByFamily(rel);
	RulesOfEngagement::relationFamily fn2 = RulesOfEngagement::getRelationFromFamily(rel);
	if (fn1 == 0 && fn2 == 0) {
		RulesOfEngagement::isRelation fn3 = RulesOfEngagement::getRelation(rel);
		unordered_map<int, unordered_map<int, bool>> memo;
		for (auto it = answers.begin(); it != answers.end(); it++) {
			int first = (*it)[firstRelIndex];
			unordered_map<int, bool> memofirst = memo[first];

			for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
				int second = (*it2)[secondRelIndex];
				if (memofirst.count(second) == 0)
					memofirst.insert(pair<int, bool>(second, fn3(first, second)));

				if (memofirst[second]) {
					vector<int> newRow(*it);
					newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
					newTable.push_back(newRow);
				}
			}
		}
	} else {
		unordered_map<int, unordered_set<int>> memo;
		if (answers.size() <= otherTable.answers.size() || fn2 == 0) {
			for (auto it = answers.begin(); it != answers.end(); it++) {
				int value1 = (*it)[firstRelIndex];
				if (memo.count(value1) == 0) {
					vector<int>& temp1 = fn1(value1);
					unordered_set<int> temp2(temp1.begin(), temp1.end());
					memo.insert(pair<int, unordered_set<int>>(value1, temp2));
				}
				const unordered_set<int>& otherAnswers = memo[value1];

				for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
					int value2 = (*it2)[secondRelIndex];
					if (otherAnswers.count(value2) > 0) {
						vector<int> newRow(*it);
						newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
						newTable.push_back(newRow);
					}
				}
			}
		} else {
			for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
				int value2 = (*it2)[secondRelIndex];
				if (memo.count(value2) == 0) {
					vector<int>& temp1 = fn2(value2);
					unordered_set<int> temp2(temp1.begin(), temp1.end());
					memo.insert(pair<int, unordered_set<int>>(value2, temp2));
				}
				const unordered_set<int>& otherAnswers = memo[value2];

				for (auto it = answers.begin(); it != answers.end(); it++) {
					int value1 = (*it)[firstRelIndex];
					if (otherAnswers.count(value1) > 0) {
						vector<int> newRow(*it);
						newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
						newTable.push_back(newRow);
					}
				}
			}
		}
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
	unordered_map<int, unordered_map<int, bool>> memo;

	vector<vector<int>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++) {
		int first = (*it)[firstRelIndex];
		int second = (*it)[secondRelIndex];
		unordered_map<int, bool> memofirst = memo[first];
		if (memofirst.count(second) == 0)
			memofirst.insert(pair<int, bool>(second, rel(first, second)));

		if (memofirst[second])
			newTable.push_back(*it);
	}
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
	
	unordered_set<string> seen;
	for (auto it = answers.begin(); it != answers.end(); it++) {
		vector<int> newRow;
		string equiv = "";
		for (auto it2 = indices.begin(); it2 != indices.end(); it2++) {
			newRow.push_back((*it)[*it2]);
			equiv += Helper::intToString((*it)[*it2]) + " ";
		}
		if (seen.count(equiv) == 0) {
			seen.insert(equiv);
			newTable.answers.push_back(newRow);
		}
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
unsigned int AnswerTable::getHeaderSize() const
{
	return header.size();
}

/**
* Returns the number of results (tuples) in this AnswerTable.
* @return the number of results
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