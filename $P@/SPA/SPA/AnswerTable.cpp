#pragma once
#include "AnswerTable.h"
#include "PKB.h"
#include "Helper.h"
#include "AssignmentParser.h"

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
	const RulesOfEngagement::QueryVar synType = synonymTable.getType(synonym);
	type.push_back(synType);
	synonymPosition.insert(pair<string, int>(synonym, 0));

	if (RulesOfEngagement::formOfASTNode[synType]) {
		const vector<ASTNode*>& table = RulesOfEngagement::getType2(synType)();
		const unordered_set<ASTNode*> table2(table.begin(), table.end());
		vector<pair<int, unordered_set<ASTNode*>>> temp;
		temp.push_back(pair<int, unordered_set<ASTNode*>>(-1, table2));
		answers.push_back(temp);
		return;
	}

	bool unrestricted = true;

	vector<pair<int, unordered_set<ASTNode*>>> table;
	
	{
	const unordered_map<string, string>& specificAttributes =
		synonymTable.getAllSpecificAttributes(synonym);

	if (specificAttributes.count("procName") > 0) {
		const string& procName = specificAttributes.at("procName");
		const PROCIndex procIndex = PKB::procedures.getPROCIndex(
			procName.substr(1, procName.length() - 2));
		if (procIndex == -1)
			return;
		
		switch (synType) {
		case RulesOfEngagement::Procedure:
			table.push_back(
				pair<int, unordered_set<ASTNode*>>(procIndex, unordered_set<ASTNode*>()));
			break;
		case RulesOfEngagement::Calls: {
			const vector<int>& stmts = PKB::calls.getStmtCall(procIndex);
			for (auto it = stmts.begin(); it != stmts.end(); it++)
				table.push_back(
				pair<int, unordered_set<ASTNode*>>(*it, unordered_set<ASTNode*>()));
									   }
			break;
		}
		unrestricted = false;
	}

	if (specificAttributes.count("stmt#") > 0) {
		const int stmtNo = Helper::stringToInt(specificAttributes.at("stmt#"));
		if (stmtNo > PKB::maxProgLines)
			return;

		if (unrestricted) {
			switch (synType) {
			case RulesOfEngagement::Statement:
				table.push_back(
					pair<int, unordered_set<ASTNode*>>(stmtNo, unordered_set<ASTNode*>()));
				break;
			case RulesOfEngagement::Assign:
				if (PKB::assignTable.count(stmtNo) > 0)
					table.push_back(
					pair<int, unordered_set<ASTNode*>>(stmtNo, unordered_set<ASTNode*>()));
				else
					return;
				break;
			case RulesOfEngagement::While:
				if (PKB::whileTable.count(stmtNo) > 0)
					table.push_back(
					pair<int, unordered_set<ASTNode*>>(stmtNo, unordered_set<ASTNode*>()));
				else
					return;
				break;
			case RulesOfEngagement::If:
				if (PKB::ifTable.count(stmtNo) > 0)
					table.push_back(
					pair<int, unordered_set<ASTNode*>>(stmtNo, unordered_set<ASTNode*>()));
				else
					return;
				break;
			case RulesOfEngagement::Call:
				if (PKB::callTable.count(stmtNo) > 0)
					table.push_back(
					pair<int, unordered_set<ASTNode*>>(stmtNo, unordered_set<ASTNode*>()));
				else
					return;
				break;
			}
			unrestricted = false;
		} else { //already a procName in there (must be call)
			bool found = false;
			for (auto it = table.begin(); it != table.end(); it++)
				if (it->first == stmtNo) {
					found = true;
					break;
				}
			if (found)
				table = vector<pair<int, unordered_set<ASTNode*>>>(1,
				pair<int, unordered_set<ASTNode*>>(stmtNo, unordered_set<ASTNode*>()));
			else {
				table = vector<pair<int, unordered_set<ASTNode*>>>();
				return;
			}
		}
	}

	else if (specificAttributes.count("") > 0) {
		const int stmtNo = Helper::stringToInt(specificAttributes.at(""));
		if (stmtNo > PKB::maxProgLines)
			return;

		table.push_back(pair<int, unordered_set<ASTNode*>>(stmtNo, unordered_set<ASTNode*>()));
		unrestricted = false;
	}

	if (specificAttributes.count("varName") > 0) {
		const string& varName = specificAttributes.at("varName");
		const VARIndex varIndex = PKB::variables.getVARIndex(
			varName.substr(1, varName.length() - 2));
		if (varIndex == -1)
			return;

		table.push_back(pair<int, unordered_set<ASTNode*>>(varIndex, unordered_set<ASTNode*>()));
		unrestricted = false;
	}

	if (specificAttributes.count("value") > 0) {
		const int value = Helper::stringToInt(specificAttributes.at("value"));
		if (PKB::constantsTable.count(value) == 0)
			return;
		table.push_back(pair<int, unordered_set<ASTNode*>>(value, unordered_set<ASTNode*>()));
		unrestricted = false;
	}
	}

	{
	const vector<pair<RulesOfEngagement::QueryRelations, string>>& firstSpecific =
		synonymTable.getAllFirstSpecific(synonym);
	vector<vector<pair<RulesOfEngagement::QueryRelations, string>>::const_iterator>
		skippedFirstSpecific;
	for (auto it = firstSpecific.begin(); it != firstSpecific.end(); it++) {
		if (RulesOfEngagement::takesInASTNode[it->first]) {
			const ASTNode * const arg = PKB::statementNodes[
				RulesOfEngagement::convertArgumentToInteger(it->first, false, it->second)];
			const RulesOfEngagement::relation2Family fn =
				RulesOfEngagement::getRelation2FromFamily(it->first);
			if (fn == 0) {
				if (unrestricted) {
					skippedFirstSpecific.push_back(it);
					continue;
				}
				const RulesOfEngagement::isRelation2 rel =
					RulesOfEngagement::getRelation2((*it).first);
				vector<pair<int, unordered_set<ASTNode*>>> newTable;
				for (auto it2 = table.begin(); it2 != table.end(); it2++) {
					pair<int, unordered_set<ASTNode*>>& info = *it2;
					if (info.second.empty())
						info.second =
						RulesOfEngagement::convertIntegerToASTNode(synType, info.first);

					bool satisfy = false;
					for (auto it3 = info.second.begin(); it3 != info.second.end();) {
						if (rel(*it3, arg)) {
							satisfy = true;
							++it3;
						} else {
							info.second.erase(*it3++);
						}
					}
					if (satisfy)
						newTable.push_back(*it2);
				}
				table = newTable;
			} else {
				const vector<ASTNode*>& answers = fn(arg);
				if (unrestricted) {
					switch (synType) {
					case RulesOfEngagement::Assign:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::assignNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::assignNodesBack[*it2], uset));
							}
						break;
					case RulesOfEngagement::While:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::whileNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::whileNodesBack[*it2], uset));
							}
						break;
					case RulesOfEngagement::If:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::ifNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::ifNodesBack[*it2], uset));
							}
						break;
					case RulesOfEngagement::Call:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::callNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::callNodesBack[*it2], uset));
							}
						break;
					default:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++) {
							unordered_set<ASTNode*> uset;
							uset.insert(*it2);
							table.push_back(pair<int, unordered_set<ASTNode*>>
								(((ASTStmtNode*) *it2)->getStmtNumber(), uset));
						}
					}
					unrestricted = false;
				} else {
					vector<pair<int, unordered_set<ASTNode*>>> newTable;
					const unordered_set<ASTNode*> memo(answers.begin(), answers.end());
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						bool satisfy = false;
						for (auto it3 = it2->second.begin(); it3 != it2->second.begin();) {
							if (memo.count(*it3) > 0) {
								satisfy = true;
								++it3;
							} else {
								it2->second.erase(*it3++);
							}
						}
						if (satisfy)
							newTable.push_back(*it2);
					}
					table = newTable;
				}
			}
		} else { //the normal relations
			const int arg =
				RulesOfEngagement::convertArgumentToInteger(it->first, false, it->second);
			const RulesOfEngagement::relationFamily fn =
				RulesOfEngagement::getRelationFromFamily(it->first);
			if (fn == 0) {
				if (unrestricted) {
					skippedFirstSpecific.push_back(it);
					continue;
				}
				const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
				vector<pair<int, unordered_set<ASTNode*>>> newTable;
				for (auto it2 = table.begin(); it2 != table.end(); it2++)
					if (rel(it2->first, arg))
						newTable.push_back(*it2);
				table = newTable;
			} else {
				const vector<int>& answers = fn(arg);
				if (unrestricted) {
					switch (synType) {
					case RulesOfEngagement::Assign:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::assignTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					case RulesOfEngagement::While:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::whileTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					case RulesOfEngagement::If:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::ifTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					case RulesOfEngagement::Call:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::callTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					default:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
					}
					unrestricted = false;
				} else {
					vector<pair<int, unordered_set<ASTNode*>>> newTable;
					const unordered_set<int> memo(answers.begin(), answers.end());
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (memo.count(it2->first) > 0)
							newTable.push_back(*it2);
					table = newTable;
				}
			}
		}
	}
	
	const vector<pair<RulesOfEngagement::QueryRelations, string>>& secondSpecific =
		synonymTable.getAllSecondSpecific(synonym);
	vector<vector<pair<RulesOfEngagement::QueryRelations, string>>::const_iterator>
		skippedSecondSpecific;
	for (auto it = secondSpecific.begin(); it != secondSpecific.end(); it++) {
		if (RulesOfEngagement::takesInASTNode[it->first]) {
			const ASTNode * const arg = PKB::statementNodes[
				RulesOfEngagement::convertArgumentToInteger(it->first, true, it->second)];
			const RulesOfEngagement::relation2Family fn =
				RulesOfEngagement::getRelation2ByFamily(it->first);
			if (fn == 0) {
				if (unrestricted) {
					skippedSecondSpecific.push_back(it);
					continue;
				}
				const RulesOfEngagement::isRelation2 rel =
					RulesOfEngagement::getRelation2((*it).first);
				vector<pair<int, unordered_set<ASTNode*>>> newTable;
				for (auto it2 = table.begin(); it2 != table.end(); it2++) {
					pair<int, unordered_set<ASTNode*>>& info = *it2;
					if (info.second.empty())
						info.second =
						RulesOfEngagement::convertIntegerToASTNode(synType, info.first);

					bool satisfy = false;
					for (auto it3 = info.second.begin(); it3 != info.second.end();) {
						if (rel(arg, *it3)) {
							satisfy = true;
							++it3;
						} else {
							info.second.erase(*it3++);
						}
					}
					if (satisfy)
						newTable.push_back(*it2);
				}
				table = newTable;
			} else {
				const vector<ASTNode*>& answers = fn(arg);
				if (unrestricted) {
					switch (synType) {
					case RulesOfEngagement::Assign:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::assignNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::assignNodesBack[*it2], uset));
							}
						break;
					case RulesOfEngagement::While:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::whileNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::whileNodesBack[*it2], uset));
							}
						break;
					case RulesOfEngagement::If:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::ifNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::ifNodesBack[*it2], uset));
							}
						break;
					case RulesOfEngagement::Call:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::callNodesBack.count(*it2) > 0) {
								unordered_set<ASTNode*> uset;
								uset.insert(*it2);
								table.push_back(pair<int, unordered_set<ASTNode*>>
									(PKB::callNodesBack[*it2], uset));
							}
						break;
					default:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++) {
							unordered_set<ASTNode*> uset;
							uset.insert(*it2);
							table.push_back(pair<int, unordered_set<ASTNode*>>
								(((ASTStmtNode*) *it2)->getStmtNumber(), uset));
						}
					}
					unrestricted = false;
				} else {
					vector<pair<int, unordered_set<ASTNode*>>> newTable;
					const unordered_set<ASTNode*> memo(answers.begin(), answers.end());
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						bool satisfy = false;
						for (auto it3 = it2->second.begin(); it3 != it2->second.begin();) {
							if (memo.count(*it3) > 0) {
								satisfy = true;
								++it3;
							} else {
								it2->second.erase(*it3++);
							}
						}
						if (satisfy)
							newTable.push_back(*it2);
					}
					table = newTable;
				}
			}
		} else { //the normal relations
			const int arg =
				RulesOfEngagement::convertArgumentToInteger(it->first, true, it->second);
			const RulesOfEngagement::relationFamily fn =
				RulesOfEngagement::getRelationByFamily(it->first);
			if (fn == 0) {
				if (unrestricted) {
					skippedSecondSpecific.push_back(it);
					continue;
				}
				const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
				vector<pair<int, unordered_set<ASTNode*>>> newTable;
				for (auto it2 = table.begin(); it2 != table.end(); it2++)
					if (rel(arg, it2->first))
						newTable.push_back(*it2);
				table = newTable;
			} else {
				const vector<int>& answers = fn(arg);
				if (unrestricted) {
					switch (synType) {
					case RulesOfEngagement::Assign:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::assignTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					case RulesOfEngagement::While:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::whileTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					case RulesOfEngagement::If:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::ifTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					case RulesOfEngagement::Call:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							if (PKB::callTable.count(*it2) > 0)
								table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
						break;
					default:
						for (auto it2 = answers.begin(); it2 != answers.end(); it2++)
							table.push_back(pair<int, unordered_set<ASTNode*>>
								(*it2, unordered_set<ASTNode*>()));
					}
					unrestricted = false;
				} else {
					vector<pair<int, unordered_set<ASTNode*>>> newTable;
					const unordered_set<int> memo(answers.begin(), answers.end());
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (memo.count(it2->first) > 0)
							newTable.push_back(*it2);
					table = newTable;
				}
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
		vector<pair<int, unordered_set<ASTNode*>>> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedSecondArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(it2->first, *it3)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedSecondArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(it2->first).size() > 0)
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
		vector<pair<int, unordered_set<ASTNode*>>> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedFirstArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(*it3, it2->first)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedFirstArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(it2->first).size() > 0)
							newTable.push_back(*it2);
				}
		}
		table = newTable;
	}

	if (unrestricted) {
		const vector<int>& answers = RulesOfEngagement::getType(synType)();
		for (auto it = answers.begin(); it != answers.end(); ++it)
			table.push_back(pair<int, unordered_set<ASTNode*>>(*it, unordered_set<ASTNode*>()));
	}

	for (auto it3 = skippedFirstSpecific.begin(); it3 != skippedFirstSpecific.end(); it3++) {
		const auto it = *it3;
		vector<pair<int, unordered_set<ASTNode*>>> newTable;

		if (RulesOfEngagement::takesInASTNode[it->first]) {
			const ASTNode * const arg = PKB::statementNodes[
				RulesOfEngagement::convertArgumentToInteger(it->first, false, it->second)];
			const RulesOfEngagement::isRelation2 rel =
				RulesOfEngagement::getRelation2((*it).first);
			for (auto it2 = table.begin(); it2 != table.end(); it2++) {
				pair<int, unordered_set<ASTNode*>>& info = *it2;
				if (info.second.empty())
					info.second = RulesOfEngagement::convertIntegerToASTNode(synType, info.first);

				bool satisfy = false;
				for (auto it3 = info.second.begin(); it3 != info.second.end();) {
					if (rel(*it3, arg)) {
						satisfy = true;
						++it3;
					} else {
						info.second.erase(*it3++);
					}
				}
				if (satisfy)
					newTable.push_back(*it2);
			}
		} else { //the normal relations
			const int arg =
				RulesOfEngagement::convertArgumentToInteger(it->first, false, it->second);

			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				if (rel(it2->first, arg))
					newTable.push_back(*it2);
		}
		table = newTable;
	}

	for (auto it3 = skippedSecondSpecific.begin(); it3 != skippedSecondSpecific.end(); it3++) {
		const auto it = *it3;
		vector<pair<int, unordered_set<ASTNode*>>> newTable;

		if (RulesOfEngagement::takesInASTNode[it->first]) {
			const ASTNode * const arg = PKB::statementNodes[
				RulesOfEngagement::convertArgumentToInteger(it->first, true, it->second)];
			const RulesOfEngagement::isRelation2 rel =
				RulesOfEngagement::getRelation2((*it).first);
			for (auto it2 = table.begin(); it2 != table.end(); it2++) {
				pair<int, unordered_set<ASTNode*>>& info = *it2;
				if (info.second.empty())
					info.second = RulesOfEngagement::convertIntegerToASTNode(synType, info.first);

				bool satisfy = false;
				for (auto it3 = info.second.begin(); it3 != info.second.end();) {
					if (rel(arg, *it3)) {
						satisfy = true;
						++it3;
					} else {
						info.second.erase(*it3++);
					}
				}
				if (satisfy)
					newTable.push_back(*it2);
			}
		} else { //the normal relations
			const int arg =
				RulesOfEngagement::convertArgumentToInteger(it->first, true, it->second);

			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation((*it).first);
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				if (rel(arg, it2->first))
					newTable.push_back(*it2);
		}
		table = newTable;
	}

	for (auto it4 = skippedFirstGeneric.begin(); it4 != skippedFirstGeneric.end(); it4++) {
		const auto it = *it4;

		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationByFamily(*it);
		vector<pair<int, unordered_set<ASTNode*>>> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedSecondArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(it2->first, *it3)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedSecondArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(it2->first).size() > 0)
							newTable.push_back(*it2);
				}
		}
		table = newTable;
	}

	for (auto it4 = skippedSecondGeneric.begin(); it4 != skippedSecondGeneric.end(); it4++) {
		const auto it = *it4;

		RulesOfEngagement::relationFamily fn = RulesOfEngagement::getRelationFromFamily(*it);
		vector<pair<int, unordered_set<ASTNode*>>> newTable;
		if (fn == 0) {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			const vector<int>& table2 = RulesOfEngagement::getType(
				RulesOfEngagement::privilegedFirstArgument[*it])();
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				for (auto it3 = table2.begin(); it3 != table2.end(); it3++)
					if (rel(*it3, it2->first)) {
						newTable.push_back(*it2);
						break;
					}
		} else {
			switch (RulesOfEngagement::privilegedFirstArgument[*it]) {
				case RulesOfEngagement::Assign:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::assignTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::While:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::whileTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::If:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::ifTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				case RulesOfEngagement::Call:
					for (auto it2 = table.begin(); it2 != table.end(); it2++) {
						vector<int>& tentative = fn(it2->first);
						for (auto it3 = tentative.begin(); it3 != tentative.end(); it3++)
							if (PKB::callTable.count(*it3) > 0) {
								table.push_back(*it2);
								break;
							}
					}
					break;
				default:
					for (auto it2 = table.begin(); it2 != table.end(); it2++)
						if (fn(it2->first).size() > 0)
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
					switch (synType) {
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
		vector<pair<int, unordered_set<ASTNode*>>> table2;
		if (*it == RulesOfEngagement::NextStar) {	//alternative call for more efficient method
			const vector<int>& tentative = PKB::next.getNextStar(0);	//of evaluating next*(n, n)
			const unordered_set<int> memo(tentative.begin(), tentative.end());
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				if (memo.count(it2->first) > 0)
					table2.push_back(*it2);
		} else {
			const RulesOfEngagement::isRelation rel = RulesOfEngagement::getRelation(*it);
			for (auto it2 = table.begin(); it2 != table.end(); it2++)
				if (rel(it2->first, it2->first))
					table2.push_back(*it2);
		}
		table = table2;
	}
	}

	{
	const vector<string>& patterns = synonymTable.getAllPattern(synonym);
	for (auto it = patterns.begin(); it != patterns.end(); it++) {
		const string& expression = *it;

		//actual setting up of patterns of assign for the right hand side
		RulesOfEngagement::PatternRHSType RHS;
		string RHSVarName;
		if (expression.at(0) == '_' && expression.at(expression.size() - 1) == '_') {
			RHS = RulesOfEngagement::PRSub;
			RHSVarName = expression.substr(1, expression.length() - 2);
		} else {
			RHS = RulesOfEngagement::PRNoSub;
			RHSVarName = expression;
		}
		RHSVarName = RHSVarName.substr(1, RHSVarName.length() - 2);
		ASTExprNode* RHSexprs;
		try {
			RHSexprs = AssignmentParser::processAssignment(MiniTokenizer(RHSVarName));
		} catch (SPAException& e) {	//exception indicates that the right hand side
			table.clear();		//is not correct, probably due to it containing
			return;					//a variable that is not actually present.
		}

		vector<pair<int, unordered_set<ASTNode*>>> table2;
		for (auto it = table.begin(); it != table.end(); it++)
			if (RulesOfEngagement::satisfyPattern(it->first, RHS, RHSVarName, RHSexprs))
				table2.push_back(*it);
		table = table2;
	}
	}

	//convert vector<int> to vector<vector<int>>
	for (auto it = table.begin(); it != table.end(); it++) {
		vector<pair<int, unordered_set<ASTNode*>>> temp;
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
void AnswerTable::combine(const string& ownSynonym, AnswerTable& otherTable,
	const string& otherSynonym, const RulesOfEngagement::QueryRelations rel)
{
	const int firstRelIndex = synonymPosition[ownSynonym];
	const int secondRelIndex = otherTable.synonymPosition.at(otherSynonym);
	vector<vector<pair<int, unordered_set<ASTNode*>>>> newTable;

	if (RulesOfEngagement::takesInASTNode[rel]) { //the ASTnode relations
		RulesOfEngagement::isRelation2 fn = RulesOfEngagement::getRelation2(rel);
		unordered_map<ASTNode*, unordered_map<ASTNode*, bool>> memo;
		for (auto it = answers.begin(); it != answers.end(); it++) {
			pair<int, unordered_set<ASTNode*>>& info = (*it)[firstRelIndex];
			if (info.second.empty())
				info.second =
				RulesOfEngagement::convertIntegerToASTNode(type[firstRelIndex], info.first);
			const unordered_set<ASTNode*>& first = info.second;

			for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
				pair<int, unordered_set<ASTNode*>>& info2 = (*it2)[secondRelIndex];
				if (info2.second.empty())
					info2.second = RulesOfEngagement::convertIntegerToASTNode(
					otherTable.type[secondRelIndex], info2.first);
				const unordered_set<ASTNode*>& second = info2.second;

				bool satisfy = false;
				for (auto it3 = first.begin(); it3 != first.end(); it3++) {
					unordered_map<ASTNode*, bool> memofirst = memo[*it3];
					for (auto it4 = second.begin(); it4 != second.end(); it4++) {
						if (memofirst.count(*it4) == 0)
							memofirst.insert(pair<ASTNode*, bool>(*it4, fn(*it3, *it4)));

						if (memofirst[*it4]) {
							satisfy = true;
							break;
						}
					}
					if (satisfy)
						break;
				}
				if (satisfy) {
					vector<pair<int, unordered_set<ASTNode*>>> newRow(*it);
					newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
					newTable.push_back(newRow);
				}
			}
		}
	} else { //the normal relations
		RulesOfEngagement::relationFamily fn1 = RulesOfEngagement::getRelationByFamily(rel);
		RulesOfEngagement::relationFamily fn2 = RulesOfEngagement::getRelationFromFamily(rel);
		if (fn1 == 0 && fn2 == 0) {
			RulesOfEngagement::isRelation fn3 = RulesOfEngagement::getRelation(rel);
			unordered_map<int, unordered_map<int, bool>> memo;
			for (auto it = answers.begin(); it != answers.end(); it++) {
				int first = (*it)[firstRelIndex].first;
				unordered_map<int, bool> memofirst = memo[first];

				for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
					int second = (*it2)[secondRelIndex].first;
					if (memofirst.count(second) == 0)
						memofirst.insert(pair<int, bool>(second, fn3(first, second)));

					if (memofirst[second]) {
						vector<pair<int, unordered_set<ASTNode*>>> newRow(*it);
						newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
						newTable.push_back(newRow);
					}
				}
			}
		} else {
			unordered_map<int, unordered_set<int>> memo;
			if (fn1 != 0 && (answers.size() <= otherTable.answers.size() || fn2 == 0)) {
				for (auto it = answers.begin(); it != answers.end(); it++) {
					int value1 = (*it)[firstRelIndex].first;
					if (memo.count(value1) == 0) {
						vector<int>& temp1 = fn1(value1);
						unordered_set<int> temp2(temp1.begin(), temp1.end());
						memo.insert(pair<int, unordered_set<int>>(value1, temp2));
					}
					const unordered_set<int>& otherAnswers = memo[value1];

					for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
						int value2 = (*it2)[secondRelIndex].first;
						if (otherAnswers.count(value2) > 0) {
							vector<pair<int, unordered_set<ASTNode*>>> newRow(*it);
							newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
							newTable.push_back(newRow);
						}
					}
				}
			} else {
				for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
					int value2 = (*it2)[secondRelIndex].first;
					if (memo.count(value2) == 0) {
						vector<int>& temp1 = fn2(value2);
						unordered_set<int> temp2(temp1.begin(), temp1.end());
						memo.insert(pair<int, unordered_set<int>>(value2, temp2));
					}
					const unordered_set<int>& otherAnswers = memo[value2];

					for (auto it = answers.begin(); it != answers.end(); it++) {
						int value1 = (*it)[firstRelIndex].first;
						if (otherAnswers.count(value1) > 0) {
							vector<pair<int, unordered_set<ASTNode*>>> newRow(*it);
							newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
							newTable.push_back(newRow);
						}
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
	for (auto it = otherTable.type.begin(); it != otherTable.type.end(); it++) {
		type.push_back(*it);
	}
}

/**
* Scans through the AnswerTable and keep those rows where the synonyms satisfy the relation.
* @param firstSynonym name of first synonym
* @param secondSynonym name of second synonym
* @param rel relation type
* @return the AnswerTable
*/
void AnswerTable::prune(const string& firstSynonym,
	const string& secondSynonym, const RulesOfEngagement::QueryRelations rel)
{
	const int firstRelIndex = synonymPosition[firstSynonym];
	const int secondRelIndex = synonymPosition[secondSynonym];
	vector<vector<pair<int, unordered_set<ASTNode*>>>> newTable;

	if (RulesOfEngagement::takesInASTNode[rel]) { //the ASTnode relations
		RulesOfEngagement::isRelation2 fn = RulesOfEngagement::getRelation2(rel);
		unordered_map<ASTNode*, unordered_map<ASTNode*, bool>> memo;
		for (auto it = answers.begin(); it != answers.end(); it++) {
			pair<int, unordered_set<ASTNode*>>& info = (*it)[firstRelIndex];
			if (info.second.empty())
				info.second =
				RulesOfEngagement::convertIntegerToASTNode(type[firstRelIndex], info.first);
			unordered_set<ASTNode*> first = info.second;
			
			pair<int, unordered_set<ASTNode*>>& info2 = (*it)[secondRelIndex];
			if (info2.second.empty())
				info2.second =
				RulesOfEngagement::convertIntegerToASTNode(type[secondRelIndex], info2.first);
			unordered_set<ASTNode*> second = info2.second;

			bool satisfy = false;
			for (auto it2 = first.begin(); it2 != first.end(); it2++) {
				unordered_map<ASTNode*, bool> memofirst = memo[*it2];
				for (auto it3 = second.begin(); it3 != second.end(); it3++) {
					if (memofirst.count(*it3) == 0)
						memofirst.insert(pair<ASTNode*, bool>(*it3, fn(*it2, *it3)));

					if (memofirst[*it3]) {
						satisfy = true;
						break;
					}
				}
				if (satisfy)
					break;
			}
			if (satisfy)
				newTable.push_back(*it);
		}
	} else { //the normal relations
		RulesOfEngagement::isRelation fn = RulesOfEngagement::getRelation(rel);
		unordered_map<int, unordered_map<int, bool>> memo;
		for (auto it = answers.begin(); it != answers.end(); it++) {
			int first = (*it)[firstRelIndex].first;
			int second = (*it)[secondRelIndex].first;
			unordered_map<int, bool> memofirst = memo[first];
			if (memofirst.count(second) == 0)
				memofirst.insert(pair<int, bool>(second, fn(first, second)));

			if (memofirst[second])
				newTable.push_back(*it);
		}
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

	vector<vector<pair<int, unordered_set<ASTNode*>>>> newTable;
	switch (attributeType) {
	case RulesOfEngagement::Integer:
		for (auto it = answers.begin(); it != answers.end(); it++)
			for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++)
				if ((*it)[firstRelIndex] == (*it2)[secondRelIndex]) {
					vector<pair<int, unordered_set<ASTNode*>>> newRow(*it);
					newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
					newTable.push_back(newRow);
				}
		break;
		
	case RulesOfEngagement::String:
	{
		vector<string> RHSequivs;
		for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
			int RHS = (*it2)[secondRelIndex].first;
			string RHSequiv;
			switch (secondVar) {
			case RulesOfEngagement::Procedure: //procName
				RHSequiv = PKB::procedures.getPROCName(RHS);
			case RulesOfEngagement::Variable: //varName
				RHSequiv = PKB::variables.getVARName(RHS);
			}
		}

		for (auto it = answers.begin(); it != answers.end(); it++) {
			int LHS = (*it)[firstRelIndex].first;
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
					vector<pair<int, unordered_set<ASTNode*>>> newRow(*it);
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
	for (auto it = otherTable.type.begin(); it != otherTable.type.end(); it++) {
		type.push_back(*it);
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

	vector<vector<pair<int, unordered_set<ASTNode*>>>> newTable;
	switch (attributeType) {
	case RulesOfEngagement::Integer:
		for (auto it = answers.begin(); it != answers.end(); it++)
			if ((*it)[firstRelIndex] == (*it)[secondRelIndex])
				newTable.push_back(*it);
		break;
		
	case RulesOfEngagement::String:
		for (auto it = answers.begin(); it != answers.end(); it++) {
			int LHS = (*it)[firstRelIndex].first;
			string LHSequiv;
			switch (firstVar) {
			case RulesOfEngagement::Procedure: //procName
				LHSequiv = PKB::procedures.getPROCName(LHS);
			case RulesOfEngagement::Variable: //varName
				LHSequiv = PKB::variables.getVARName(LHS);
			}

			int RHS = (*it)[secondRelIndex].first;
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
	const RulesOfEngagement::PatternRHSType RHS,
	const string& RHSVarName, const ASTExprNode* RHSexprs)
{
	const int firstRelIndex = synonymPosition[synonym];

	vector<vector<pair<int, unordered_set<ASTNode*>>>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++)
		if (RulesOfEngagement::satisfyPattern(
			(*it)[firstRelIndex].first, RHS, RHSVarName, RHSexprs))
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
		newTable.type.push_back(type[*it]);
	}
	
	unordered_set<string> seen;
	for (auto it = answers.begin(); it != answers.end(); it++) {
		vector<pair<int, unordered_set<ASTNode*>>> newRow;
		string equiv = "";
		for (auto it2 = indices.begin(); it2 != indices.end(); it2++) {
			newRow.push_back((*it)[*it2]);
			equiv += Helper::intToString((*it)[*it2].first) + " ";
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
	vector<vector<pair<int, unordered_set<ASTNode*>>>> newTable;
	for (auto it = answers.begin(); it != answers.end(); it++) {
		for (auto it2 = otherTable.answers.begin(); it2 != otherTable.answers.end(); it2++) {
			vector<pair<int, unordered_set<ASTNode*>>> newRow(*it);
			newRow.insert(newRow.end(), (*it2).begin(), (*it2).end());
			newTable.push_back(newRow);
		}
	}
	answers = newTable;

	for (auto it = otherTable.header.begin(); it != otherTable.header.end(); it++) {
		synonymPosition.insert(pair<string, int>(*it, header.size()));
		header.push_back(*it);
		//type doesn't matter by now
	}
}

/**
* Returns true if this AnswerTable is empty
* @return whether this AnswerTable is empty
*/
bool AnswerTable::isEmpty() const
{
	return header.empty();
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
size_t AnswerTable::getHeaderSize() const
{
	return header.size();
}

/**
* Returns the number of results (tuples) in this AnswerTable.
* @return the number of results
*/
size_t AnswerTable::getSize() const
{
	return answers.size();
}

/**
* Returns a particular row of this AnswerTable. The validity of the value given is not checked.
* @param index index to the row
* @return the vector corresponding to the required row
*/
vector<pair<int, unordered_set<ASTNode*>>> AnswerTable::getRow(const int index) const
{
	return answers[index];
}

/**
* Reads a string of an expression, and converts it to a vector of tokens.
* @param query expression string
*/
vector<string> AnswerTable::MiniTokenizer(const string& line)
{
	/*string tempstr = " ";
	line = tempstr.append(line);*/
	vector<string> list;
	string delimiter = " -+*;(){}=";//delimiters
	int position = 0;//starting position
	int startindex = -1;
	int endindex = -1;

	
	do//loop thru the string
	{
		startindex = line.find_first_not_of(delimiter,position);

		if(endindex != -1 && endindex<line.size())
			{
				string tempstr1; //temp str to store subset of currently working substring
				if(startindex == -1)
				{
					tempstr1 = line.substr(endindex,line.size() - endindex);
				}
				else
				{
					tempstr1 = line.substr(endindex,startindex - endindex);
				}
				for(size_t i=0;i<tempstr1.size();)
				{
					string tempstr2 = tempstr1.substr(0,1);
					tempstr1 = tempstr1.substr(1,tempstr1.size()-1);
					
					if(tempstr2 != " " && tempstr2 != "")
					list.push_back(tempstr2);
					//	AddToList(list,tempstr2);
						//AddTables(list,tempstr2);
						//list.push_back(tempstr2);
					
				}
			} else if(endindex == -1)
			{
				if(line.substr(0,startindex) != "")
				list.push_back(line.substr(0,startindex));
			}

			endindex = line.find_first_of(delimiter,startindex);

			position = endindex;
		
			if(startindex != -1 || line.size() == 1)
			{
				string tempstr;
				if(line.size() == 1)
					tempstr= line;
				else
					tempstr= line.substr(startindex,endindex-startindex);

				if(tempstr != " " && tempstr != "")
				list.push_back(tempstr);
			}

	}while(startindex != -1 && position < line.size() && endindex != -1);
	

	//house keeping
	/*if(Parser::tokenized_codes.size() > 1)
	{
		vector<string> temp_vec = Parser::tokenized_codes.at(0);
		temp_vec.insert(temp_vec.end(), Parser::tokenized_codes.at(1).begin(),Parser::tokenized_codes.at(1).end());
		Parser::tokenized_codes.erase(Parser::tokenized_codes.begin());
		Parser::tokenized_codes.at(0) = temp_vec;
	}*/

	//if(list.size() > 0)
	//Parser::tokenized_codes.push_back(list);

	//vector<string> tokens;
	return list;
}