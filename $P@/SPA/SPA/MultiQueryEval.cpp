#pragma once
#include "MultiQueryEval.h"
#include "Helper.h"
#include "PKB.h"
#include "RulesOfEngagement.h"
#include "AnswerTable.h"
#include "ASTStmtNode.h"
#include "ASTStmtLstNode.h"
#include "AssignmentParser.h"

string MultiQueryEval::getToken(const string& query, int& pos)
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";
	pos = query.find_first_of(" ;,<>().", first);
	if (pos == first) {
		pos++;
		return query.substr(first, 1);
	}
	return query.substr(first, pos - first);
}

string MultiQueryEval::getToken2(string query, int& pos)
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";

	stack<char> charstack;
	int tempt = first;
	while(true)
	{
		pos = query.find_first_of("\"()", tempt);
		if(query.at(pos) == '\"')
		{
			
			if(charstack.size() > 0 && charstack.top() == '\"')
				charstack.pop();
			else
				charstack.push('\"');
		}
		else if(query.at(pos) == '(')
		{
			charstack.push('(');
		}
		else if(query.at(pos) == ')')
		{
			if(charstack.size() == 0)
				break;
			else if(charstack.size() > 0 && charstack.top() == '(')
				charstack.pop();
			else
				throw new SPAException("Error in parsing query");
		}
		else
			throw new SPAException("Error in parsing query");
		tempt = pos+1;
	}
	if(charstack.size() != 0 )
		throw new SPAException("Error in parsing query");

	if (pos == first) {
		pos++;
		return query.substr(first, 1);
	}
	return query.substr(first, pos - first);
}

void MultiQueryEval::matchToken(const string& query, int& pos, const string& match)
{
	if (getToken(query, pos) != match)
		throw new SPAException("Error in parsing query");
}

vector<string> MultiQueryEval::evaluateQuery(const string& query)
{
	//read through the query and make sure it is valid and also looking for possible optimisations
	//then the optimisations
	//finally evaluate the relationships in the optimal order
	MultiQueryEval result(query);
	if (result.selectBOOLEAN && result.earlyQuit)
		result.finalanswer.push_back("FALSE");
	return result.finalanswer;
}

MultiQueryEval::MultiQueryEval(const string& query)
{
	//parse the query statement
	int pos = 0;
	earlyQuit = false;

	//parse synonym declaration
	unordered_map<string, RulesOfEngagement::QueryVar> stringToQueryVar;
	unordered_map<string, int> stringCount;
	while (true) {
		string token = getToken(query, pos);
		if (token == "Select")
			break;
		if (RulesOfEngagement::tokenToVar.count(token) == 0)
			throw new SPAException("Error in parsing query - Unrecognised synonym declaration");
		RulesOfEngagement::QueryVar type = RulesOfEngagement::tokenToVar[token];
		
		string variable;
		do {
			variable = getToken(query, pos);
			if (stringToQueryVar.count(variable) > 0)
				throw new SPAException("Double declaration of synonym - " + variable);
			stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>(variable, type));
			stringCount.insert(pair<string, int>(variable, 0));
		} while ((variable = getToken(query, pos)) == ",");
		if (variable != ";")
			throw new SPAException("Error in parsing query");
	}

	//parse selected variables
	string selected = getToken(query, pos);
	unordered_set<string> selects;
	if (selected.at(0) == '<') { //tuple -> multiple selected variables
		do {
			selected = getToken(query, pos);
			if (stringToQueryVar.count(selected) == 0)
				throw new SPAException("Selected variable is not recognised");
			selects.insert(selected);
			selected = getToken(query, pos);
			if (selected == ">")
				break;
			else if (selected != ",")
				throw new SPAException("Error in parsing query");
		} while (true);
		selectBOOLEAN = false;
	} else if (selected == "BOOLEAN") {
		selectBOOLEAN = true;
	} else {
		if (stringToQueryVar.count(selected) == 0)
			throw new SPAException("Selected variable is not recognised");
		selects.insert(selected);
		selectBOOLEAN = false;
	}

	//parse relationships, conditions and pattern
	//look for possible optimisations
	unordered_map<RulesOfEngagement::QueryRelations, unordered_map<string,
		unordered_set<string>>> relationStore;
	unordered_map<string, unordered_map<string, string>> conditionStore;
	vector<tuple<string, string, string, string>> condition2Store;
	unordered_map<string, unordered_set<string>> patternAssignUsesStore;
	
	//for new synonyms created due to synthetic sugaring
	int tempVars = 0;

	enum Clauses { Undefined, Such_That, With, Pattern };
	Clauses clauseType = Undefined;
	while (true) {
		string clause = getToken(query, pos);
		if (clause == "") {
			break;
		} else if (clause == "such") {
			matchToken(query, pos, "that");
			clauseType = Such_That;
		} else if (clause == "with") {
			clauseType = With;
		} else if (clause == "pattern") {
			clauseType = Pattern;
		} else if (clause != "and" || clauseType == Undefined)
			throw new SPAException("Error in parsing query");

		switch (clauseType) {
		case Such_That:
			{
				string relation = getToken(query, pos);
				matchToken(query, pos, "(");
				string firstRel = getToken(query, pos);
				matchToken(query, pos, ",");			
				string secondRel = getToken(query, pos);
				matchToken(query, pos, ")");

				unordered_set<RulesOfEngagement::QueryRelations>& types =
					RulesOfEngagement::tokenToRel[relation];

				RulesOfEngagement::QueryVar firstRelType;
				if (firstRel == "_")
					firstRelType = RulesOfEngagement::WildCard;
				else if (stringToQueryVar.count(firstRel) > 0)
					firstRelType = stringToQueryVar[firstRel];
				else if (firstRel.at(0) == '\"' && firstRel.at(firstRel.length() - 1) == '\"')
					firstRelType = RulesOfEngagement::String;
				else if (Helper::isNumber(firstRel))
					firstRelType = RulesOfEngagement::Integer;
				else
					throw new SPAException("Could not parse the first argument");

				RulesOfEngagement::QueryRelations type = RulesOfEngagement::PatternUses; //sentinel value
				for (auto it = types.begin(); it != types.end(); it++)
					if (RulesOfEngagement::allowableFirstArgument[*it].count(firstRelType) > 0) {
						type = *it;
						break;
					}
				if (type == RulesOfEngagement::PatternUses)
					throw new SPAException("The argument " + firstRel +
					" was not valid for the first argument of " + relation);

				RulesOfEngagement::QueryVar secondRelType;
				if (secondRel == "_")
					secondRelType = RulesOfEngagement::WildCard;
				else if (stringToQueryVar.count(secondRel) > 0)
					secondRelType = stringToQueryVar[secondRel];
				else if (secondRel.at(0) == '\"' && secondRel.at(secondRel.length() - 1) == '\"')
					secondRelType = RulesOfEngagement::String;
				else if (Helper::isNumber(secondRel))
					secondRelType = RulesOfEngagement::Integer;
				else
					throw new SPAException("Could not parse the second argument");

				if (RulesOfEngagement::allowableSecondArgument[type].count(secondRelType) == 0)
					throw new SPAException("The argument " + secondRel +
					" was not valid for the second argument of " + relation);

				if (stringToQueryVar.count(firstRel) > 0) {
					if (stringToQueryVar.count(secondRel) > 0) {
						if (firstRel != secondRel) {
							stringCount[firstRel]++;
							stringCount[secondRel]++;
						}
					} else
						stringCount[firstRel]++;
				} else if (stringToQueryVar.count(secondRel) > 0)
					stringCount[secondRel]++;
				relationStore[type][firstRel].insert(secondRel);
			}
			break;

		case With:
			{
				string synonym = getToken(query, pos);
				if (stringToQueryVar.count(synonym) == 0)
					throw new SPAException("The variable " + synonym + " is not recognised");

				RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];
				string condition;
				RulesOfEngagement::QueryVar LHSType;
				if (stringToQueryVar[synonym] == RulesOfEngagement::Prog_Line) {
					condition = "";
					LHSType = RulesOfEngagement::Integer;
				} else {
					matchToken(query, pos, ".");
					condition = getToken(query, pos);
					if (RulesOfEngagement::allowableConditions[type].count(condition) == 0)
						throw new SPAException(synonym +
						" does not have the " + condition + " condition");
					LHSType = RulesOfEngagement::conditionTypes[condition];
				}

				matchToken(query, pos, "=");
				//either c.value = 10 OR c.value = s.stmt#
				string token = getToken(query, pos);
				if (stringToQueryVar.count(token) == 0) { //c.value = 10
					if (LHSType == RulesOfEngagement::Integer) {
						if (!Helper::isNumber(token))
							throw new SPAException("Unable to parse with part");
					} else if (LHSType == RulesOfEngagement::String) {
						if (token.at(0) != '"' || token.at(token.length() - 1) != '"')
							throw new SPAException("Unable to parse with part");
					} else
						throw new SPAException("Unable to parse with part");

					if (conditionStore[synonym].count(condition) > 0)
						earlyQuit |= (conditionStore[synonym][condition] == token);
					stringCount[synonym]++;
					conditionStore[synonym].insert(pair<string, string>(condition, token));
				} else { //c.value = s.stmt#
					matchToken(query, pos, ".");
					string condition2 = getToken(query, pos);

					if (synonym == token && condition == condition2)
						break;

					if (RulesOfEngagement::allowableConditions[type].count(condition) == 0)
						throw new SPAException(synonym +
						" does not have the " + condition + " condition");
					RulesOfEngagement::QueryVar RHSType =
						RulesOfEngagement::conditionTypes[condition2];
					if (LHSType != RHSType)
						throw new SPAException("Left and right hand side of with are not of same type");
					condition2Store.push_back(tuple<string, string, string, string>
						(synonym, condition, token, condition2));
				}
			}
			break;

		case Pattern:
			{
				string synonym = getToken(query, pos);
				if (stringToQueryVar.count(synonym) == 0)
					throw new SPAException("The variable " + synonym + " is not recognised");
				RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];

				matchToken(query, pos, "(");
				string firstRel = getToken(query, pos);

				switch (type) {
				case RulesOfEngagement::Assign:
					{
						if (firstRel != "_") {
							stringCount[synonym]++;
							if (stringToQueryVar.count(firstRel) > 0) {
								if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable)
									throw new SPAException("The first argument of pattern must be a variable");
								stringCount[firstRel]++;
							}
							else if (firstRel.at(0) != '\"' || firstRel.at(firstRel.length() - 1) != '\"')
								throw new SPAException("Could not parse the first argument");
							relationStore[RulesOfEngagement::ModifiesStmt][synonym].insert(firstRel);
						}
						
						matchToken(query, pos, ",");
						string secondRel = getToken2(query, pos);
						matchToken(query, pos, ")");

						if (secondRel != "_") {
							//remove white spaces
							secondRel.erase(remove(secondRel.begin(), secondRel.end(), '\t'), secondRel.end());
							secondRel.erase(remove(secondRel.begin(), secondRel.end(), ' '), secondRel.end());

							unsigned int length = secondRel.length();
							if (length <= 2)
								throw SPAException("Error, Pattern Right Hand Side Invalid");
							if (secondRel.at(0) == '_' && secondRel.at(length - 1) == '_') {
								if (length <= 4 || secondRel.at(1) != '\"' || secondRel.at(length - 2) != '\"')
									throw SPAException("Error, Pattern Right Hand Side Invalid");
							} else if (secondRel.at(0) != '\"' || secondRel.at(length - 1) != '\"')
								throw SPAException("Error, Pattern Right Hand Side Invalid");
						
							stringCount[synonym]++;
							patternAssignUsesStore[synonym].insert(secondRel);
						}
					}
					break;

				case RulesOfEngagement::If:
				case RulesOfEngagement::While:
					matchToken(query, pos, ",");
					matchToken(query, pos, "_");
					matchToken(query, pos, ",");
					matchToken(query, pos, "_");
					matchToken(query, pos, ")");

					if (firstRel != "_") {
						stringCount[synonym]++;
						if (stringToQueryVar.count(firstRel) > 0) {
							if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable)
								throw new SPAException("The first argument of pattern must be a variable");
							stringCount[firstRel]++;
						}
						else if (firstRel.at(0) != '\"' || firstRel.at(firstRel.length() - 1) != '\"')
							throw new SPAException("Could not parse the first argument");
						relationStore[RulesOfEngagement::PatternModifies][synonym].insert(firstRel);
					}
					break;

				default:
					throw new SPAException("Pattern only valid on assign, if and while synonyms");
				} //end switch of synonym type in pattern
			} //end case of pattern
		} //end switch of clauseType
	}
	//end of query validation

	if (earlyQuit)
		return;

	//apply special rules
	//1) stmt s, t; Follows(s,t) and Follows(t,s) -> earlyQuit
	//TODO: Generalise to Follows(s1,s2) and Follows(s2,s3) and ... and Follows(sk,1) -> earlyQuit
	//2) stmt s, t; Next*(s,t) and Next*(t,s)
	//-> (Remove the two original relationships at the end) + while w; Parent*(w,s) and Parent*(w,t)
	unordered_map<string, unordered_set<string>>* followsRel;
	if (relationStore.count(RulesOfEngagement::Follows) > 0)
		followsRel = &relationStore[RulesOfEngagement::Follows];
	else
		followsRel = 0;
	unordered_map<string, unordered_set<string>>* followsStarRel;
	if (relationStore.count(RulesOfEngagement::FollowsStar) > 0)
		followsStarRel = &relationStore[RulesOfEngagement::FollowsStar];
	else
		followsStarRel = 0;
	if (followsRel != 0)
		for (auto it = followsRel->begin(); it != followsRel->end(); it++) {
			const string& firstRel = it->first;
			if (stringToQueryVar.count(firstRel) == 0)
				continue;
			const unordered_set<string>& secondRels = it->second;
			for (auto it2 = secondRels.begin(); it2 != secondRels.end(); it2++) {
				if (stringToQueryVar.count(*it2) == 0)
					continue;
				if (firstRel == *it2 || 
					(followsRel->count(*it2) > 0 && followsRel->at(*it2).count(firstRel) > 0)
					|| (followsStarRel != 0 && followsStarRel->count(*it2) > 0
					&& followsStarRel->at(*it2).count(firstRel) > 0)) {
					earlyQuit = true;
					return;
				}
			}
		}
	if (followsStarRel != 0)
		for (auto it = followsStarRel->begin(); it != followsStarRel->end(); it++) {
			const string& firstRel = it->first;
			if (stringToQueryVar.count(firstRel) == 0)
				continue;
			const unordered_set<string>& secondRels = it->second;
			for (auto it2 = secondRels.begin(); it2 != secondRels.end(); it2++) {
				if (stringToQueryVar.count(*it2) == 0)
					continue;
				if (firstRel == *it2 ||
					(followsRel != 0 && followsRel->count(*it2) > 0
					&& followsRel->at(*it2).count(firstRel) > 0) ||
					(followsStarRel->count(*it2) > 0 &&
					followsStarRel->at(*it2).count(firstRel) > 0)) {
					earlyQuit = true;
					return;
				}
			}
		}

	/*const unordered_map<string, unordered_set<string>>& nextRel =
		relationStore[RulesOfEngagement::Next];
	const unordered_map<string, unordered_set<string>>& nextStarRel =
		relationStore[RulesOfEngagement::NextStar];
	bool specialForNext = false;
	string specialSynonym;
	vector<string> removeFirst;
	vector<string> removeSecond;
	vector<bool> removeFrom; //false = next, true = nextStar
	for (auto it = nextRel.begin(); it != nextRel.end(); it++) {
		const string& firstRel = it->first;
		if (stringToQueryVar.count(firstRel) == 0)
			continue;
		const unordered_set<string>& secondRels = it->second;
		for (auto it2 = secondRels.begin(); it2 != secondRels.end(); it2++) {
			if (stringToQueryVar.count(*it2) == 0)
				continue;
			if (firstRel == *it2) {
				if (!specialForNext) {
					specialSynonym = "t" + Helper::intToString(++tempVars);
					while (stringToQueryVar.count(specialSynonym))
						specialSynonym = "t" + specialSynonym;
					stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>
						(specialSynonym, RulesOfEngagement::While));
					specialForNext = true;
				}
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(firstRel);
				removeFirst.push_back(firstRel);
				removeSecond.push_back(firstRel);
				removeFrom.push_back(false);
				continue;
			}
			if (nextRel.at(*it2).count(firstRel) > 0) {
				if (!specialForNext) {
					specialSynonym = "t" + Helper::intToString(++tempVars);
					while (stringToQueryVar.count(specialSynonym))
						specialSynonym = "t" + specialSynonym;
					stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>
						(specialSynonym, RulesOfEngagement::While));
					specialForNext = true;
				}
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(firstRel);
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(*it2);
				removeFirst.push_back(firstRel);
				removeSecond.push_back(*it2);
				removeFrom.push_back(false);
				removeFirst.push_back(*it2);
				removeSecond.push_back(firstRel);
				removeFrom.push_back(false);
				continue;
			}
			if (nextStarRel.at(*it2).count(firstRel) > 0) {
				if (!specialForNext) {
					specialSynonym = "t" + Helper::intToString(++tempVars);
					while (stringToQueryVar.count(specialSynonym))
						specialSynonym = "t" + specialSynonym;
					stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>
						(specialSynonym, RulesOfEngagement::While));
					specialForNext = true;
				}
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(firstRel);
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(*it2);
				removeFirst.push_back(firstRel);
				removeSecond.push_back(*it2);
				removeFrom.push_back(false);
				removeFirst.push_back(*it2);
				removeSecond.push_back(firstRel);
				removeFrom.push_back(true);
				continue;
			}
		}
	}
	for (auto it = nextStarRel.begin(); it != nextStarRel.end(); it++) {
		const string& firstRel = it->first;
		if (stringToQueryVar.count(firstRel) == 0)
			continue;
		const unordered_set<string>& secondRels = it->second;
		for (auto it2 = secondRels.begin(); it2 != secondRels.end(); it2++) {
			if (stringToQueryVar.count(*it2) == 0)
				continue;
			if (firstRel == *it2) {
				if (!specialForNext) {
					specialSynonym = "t" + Helper::intToString(++tempVars);
					while (stringToQueryVar.count(specialSynonym))
						specialSynonym = "t" + specialSynonym;
					stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>
						(specialSynonym, RulesOfEngagement::While));
					specialForNext = true;
				}
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(firstRel);
				removeFirst.push_back(firstRel);
				removeSecond.push_back(firstRel);
				removeFrom.push_back(true);
				continue;
			}
			if (nextRel.at(*it2).count(firstRel) > 0) {
				if (!specialForNext) {
					specialSynonym = "t" + Helper::intToString(++tempVars);
					while (stringToQueryVar.count(specialSynonym))
						specialSynonym = "t" + specialSynonym;
					stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>
						(specialSynonym, RulesOfEngagement::While));
					specialForNext = true;
				}
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(firstRel);
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(*it2);
				removeFirst.push_back(firstRel);
				removeSecond.push_back(*it2);
				removeFrom.push_back(true);
				removeFirst.push_back(*it2);
				removeSecond.push_back(firstRel);
				removeFrom.push_back(false);
				continue;
			}
			if (nextStarRel.at(*it2).count(firstRel) > 0) {
				if (!specialForNext) {
					specialSynonym = "t" + Helper::intToString(++tempVars);
					while (stringToQueryVar.count(specialSynonym))
						specialSynonym = "t" + specialSynonym;
					stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>
						(specialSynonym, RulesOfEngagement::While));
					specialForNext = true;
				}
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(firstRel);
				relationStore[RulesOfEngagement::ParentStar][specialSynonym].insert(*it2);
				removeFirst.push_back(firstRel);
				removeSecond.push_back(*it2);
				removeFrom.push_back(true);
				removeFirst.push_back(*it2);
				removeSecond.push_back(firstRel);
				removeFrom.push_back(true);
				continue;
			}
		}
	}
	for (unsigned int i = 0; i < removeFirst.size(); i++) {
		unordered_map<string, unordered_set<string>>* tablePtr;
		if (removeFrom[i]) //remove from nextStar
			tablePtr = &relationStore[RulesOfEngagement::NextStar];
		else //remove from next
			tablePtr = &relationStore[RulesOfEngagement::Next];
		string& firstRel = removeFirst[i];
		string& secondRel = removeSecond[i];
		(*tablePtr)[firstRel].erase(secondRel);
		if ((*tablePtr)[firstRel].size() == 0)
			(*tablePtr).erase(firstRel);
		if (firstRel != secondRel) {
			stringCount[firstRel]--;
			stringCount[secondRel]--;
		}
	}*/
	//end special rules

	//second stage
	SynonymTable synonymTable;
	DisjointSet disjointSet;

	//analyse synonyms. those with count of 1 and are not selected can be folded in
	unordered_set<string> toFold; //todo:: alias synonyms
	for (auto it = stringCount.begin(); it != stringCount.end(); it++) {
		const string& synonym = it->first;
		const int count = it->second;
		if (selects.count(synonym) > 0) {
			synonymTable.insert(synonym, stringToQueryVar[synonym]);
			disjointSet.makeSet(synonym);
			synonymTable.setSelected(synonym);
		} else if (count > 1) {
			synonymTable.insert(synonym, stringToQueryVar[synonym]);
			disjointSet.makeSet(synonym);
		} else if (count == 1)
			toFold.insert(synonym);
	}

	//parse equality of attributes
	for (auto it = condition2Store.begin(); it != condition2Store.end(); it++) {
		string firstRel = get<0>(*it);
		string firstCondition = get<1>(*it);
		string secondRel = get<2>(*it);
		string secondCondition = get<3>(*it);

		disjointSet.setUnion(firstRel, secondRel);
	}
	
	//relationship table
	vector<RulesOfEngagement::QueryRelations> relType;
	vector<string> relFirst;
	vector<string> relSecond;
	vector<int> relClass;
	unordered_map<string, vector<int>> relFirstToIndices;

	/*//pattern table
	vector<string> patternSynonym;
	vector<RulesOfEngagement::QueryVar> patternType;
	vector<string> patternLHS;
	vector<string> patternRHS;*/

	//parse relations
	for (auto it = relationStore.begin(); it != relationStore.end(); it++) {
		const RulesOfEngagement::QueryRelations& type = it->first;
		const unordered_map<string, unordered_set<string>>& itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			string firstRel;
			if (toFold.count(it2->first) > 0) {
				if (stringToQueryVar[it2->first] == RulesOfEngagement::privilegedFirstArgument[type])
					firstRel = "_";
				else {
					firstRel = it2->first;
					synonymTable.insert(firstRel, stringToQueryVar[firstRel]);
					disjointSet.makeSet(firstRel);
				}
			} else
				firstRel = it2->first;
			const unordered_set<string>& it2second = it2->second;
			for (auto it3 = it2second.begin(); it3 != it2second.end(); it3++) {
				string secondRel;
				if (toFold.count(*it3) > 0) {
					if (stringToQueryVar[*it3] == RulesOfEngagement::privilegedSecondArgument[type])
						secondRel = "_";
					else {
						secondRel = *it3;
						synonymTable.insert(secondRel, stringToQueryVar[secondRel]);
						disjointSet.makeSet(secondRel);
					}
				} else
					secondRel = *it3;

				//actual setting up of relations
				//count number of synonyms
				int numOfSynonyms = 0;
				bool firstIsSynonym = false;
				if (synonymTable.isInTable(firstRel)) {
					numOfSynonyms++;
					firstIsSynonym = true;
				}
				if (synonymTable.isInTable(secondRel))
					numOfSynonyms++;

				switch (numOfSynonyms) {
				case 0: //handle 0 synonym, 3 cases
					//Case 1: rel( _ , _ ) -> On the spot test if the relationship is ever true
					//Case 2: rel( _ ,"1") -> rel(_,s) with s.stmtNo = 1 -> treat as a condition on s
					//Case 3: rel("1","2") -> On the spot test
					if (firstRel == "_") {
						if (secondRel == "_") { //Case 1: rel(_,_)
							if (RulesOfEngagement::emptyRel[type]) {
								earlyQuit = true;
								return;
							}
						} else { //Case 2: rel(_,"1")
							string sugar = "t" + Helper::intToString(++tempVars);
							while (synonymTable.isInTable(sugar))
								sugar = "t" + sugar;
							
							RulesOfEngagement::QueryVar secondArg = RulesOfEngagement::privilegedSecondArgument[type];
							synonymTable.insert(sugar, secondArg);
							synonymTable.setAttribute(sugar, *(RulesOfEngagement::allowableConditions[secondArg].begin()),
								secondRel);
							synonymTable.setSecondGeneric(sugar, type);
						}
					} else {
						if (secondRel == "_") { //Case 2: rel("1",_)
							string sugar = "t" + Helper::intToString(++tempVars);
							while (synonymTable.isInTable(sugar))
								sugar = "t" + sugar;

							RulesOfEngagement::QueryVar firstArg = RulesOfEngagement::privilegedFirstArgument[type];
							synonymTable.insert(sugar, firstArg);
							synonymTable.setAttribute(sugar, *(RulesOfEngagement::allowableConditions[firstArg].begin()),
								firstRel);
							synonymTable.setFirstGeneric(sugar, type);
						} else { //Case 3: rel("1","2") 
							int first = RulesOfEngagement::convertArgumentToInteger(type, true, firstRel);
							int second = RulesOfEngagement::convertArgumentToInteger(type, false, secondRel);
							if (!RulesOfEngagement::getRelation(type)(first, second)) {
								earlyQuit = true;
								return;
							}
						}
					}
					break;

				case 1: //handle 1 synonym: put as a condition
					//Case 1: rel(s, _ ) -> put in generic(first)
					//Case 2: rel(s,"1") -> put in specific(first)
					if (firstIsSynonym) {
						if (secondRel == "_")
							synonymTable.setFirstGeneric(firstRel, type);
						else
							synonymTable.setFirstSpecific(firstRel, type, secondRel);
					} else {
						if (firstRel == "_")
							synonymTable.setSecondGeneric(secondRel, type);
						else
							synonymTable.setSecondSpecific(secondRel, type, firstRel);
					}
					break;

				case 2: //handle both synonyms
					
					//handle self-reference
					if (firstRel == secondRel) {
						if (RulesOfEngagement::allowableSelfReference.count(type) > 0) {
							synonymTable.setSelfReference(firstRel, type);
							break;
						} else {
							earlyQuit = true;
							return;
						}
					}

					if (relFirstToIndices.count(firstRel) > 0)
						relFirstToIndices[firstRel].push_back(relType.size());
					else {
						vector<int> temp;
						temp.push_back(relType.size());
						relFirstToIndices.insert(pair<string, vector<int>>(firstRel, temp));
					}
					relType.push_back(type);
					relFirst.push_back(firstRel);
					relSecond.push_back(secondRel);
					relClass.push_back(-1);

					disjointSet.setUnion(firstRel, secondRel);
					break;
				}
			}
		}
	}

	//parse conditions
	//unordered_map<string, unordered_map<string, string>> conditionStore;
	for (auto it = conditionStore.begin(); it != conditionStore.end(); it++) {
		const string& synonym = it->first;
		const unordered_map<string, string>& itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			const string& condition = it2->first;
			const string& attribute = it2->second;
			
			//actual setting up of conditions
			bool passed = synonymTable.setAttribute(synonym, condition, attribute);
			if (!passed) {
				earlyQuit = true;
				return;
			}
		}
	}

	vector<unordered_set<string>> components = disjointSet.getComponents();
	for (unsigned int classIndex = 0; classIndex < components.size(); classIndex++)
		for (auto it = components[classIndex].begin(); it != components[classIndex].end(); it++)
			synonymTable.putIntoClass(*it, classIndex + 1);
	
	for (auto it = relFirstToIndices.begin(); it != relFirstToIndices.end(); it++)
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			relClass[*it2] = synonymTable.inClass((*it).first);
	
	vector<AnswerTable> tables;
	//could have incorporated in synonym table, but was not because it is implentation dependent
	unordered_map<string, int> inWhichTable;

	/*//evaluate equality of attributes
	for (auto it = condition2Store.begin(); it != condition2Store.end(); it++) {
		string firstRel = get<0>(*it);
		string firstCondition = get<1>(*it);
		string secondRel = get<2>(*it);
		string secondCondition = get<3>(*it);

		int matchNumberOfTables = 0;
		if (inWhichTable.count(firstRel) > 0)
			matchNumberOfTables++;
		if (inWhichTable.count(secondRel) > 0)
			matchNumberOfTables++;

		switch (matchNumberOfTables) {
		case 0:
			{
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				firstRelTable.withPrune

				firstRelTable.combine(firstRel, secondRelTable,
					secondRel, RulesOfEngagement::getRelation(type));
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[firstRel] = tables.size();
				inWhichTable[secondRel] = tables.size();
				tables.push_back(firstRelTable);
			}
			break;
		case 1:
			if (inWhichTable.count(firstRel) > 0) {
				int firstRelIndex = inWhichTable[firstRel];

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				tables[firstRelIndex].combine(firstRel, secondRelTable,
					secondRel, RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[secondRel] = firstRelIndex;
			} else {
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				int secondRelIndex = inWhichTable[secondRel];
				firstRelTable.combine(firstRel, tables[secondRelIndex],
					secondRel, RulesOfEngagement::getRelation(type));
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				tables[secondRelIndex] = firstRelTable;
				inWhichTable[firstRel] = secondRelIndex;
			}
			break;
		case 2:
			int firstRelIndex = inWhichTable[firstRel];
			int secondRelIndex = inWhichTable[secondRel];
			if (firstRelIndex == secondRelIndex) {
				tables[firstRelIndex].prune(firstRel, secondRel,
					RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}
			} else {
				tables[firstRelIndex].combine(firstRel, tables[secondRelIndex],
					secondRel, RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
					if ((*it).second == secondRelIndex)
						(*it).second = firstRelIndex;
			}
		}
	}*/
	
	for (unsigned int rel = 0; rel < relType.size(); rel++) {
		RulesOfEngagement::QueryRelations type = relType[rel];
		string firstRel = relFirst[rel];
		string secondRel = relSecond[rel];

		int matchNumberOfTables = 0;
		if (inWhichTable.count(firstRel) > 0)
			matchNumberOfTables++;
		if (inWhichTable.count(secondRel) > 0)
			matchNumberOfTables++;

		switch (matchNumberOfTables) {
		case 0:
			{
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				firstRelTable.combine(firstRel, secondRelTable,
					secondRel, RulesOfEngagement::getRelation(type));
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[firstRel] = tables.size();
				inWhichTable[secondRel] = tables.size();
				tables.push_back(firstRelTable);
			}
			break;
		case 1:
			if (inWhichTable.count(firstRel) > 0) {
				int firstRelIndex = inWhichTable[firstRel];

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				tables[firstRelIndex].combine(firstRel, secondRelTable,
					secondRel, RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[secondRel] = firstRelIndex;
			} else {
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				int secondRelIndex = inWhichTable[secondRel];
				firstRelTable.combine(firstRel, tables[secondRelIndex],
					secondRel, RulesOfEngagement::getRelation(type));
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				tables[secondRelIndex] = firstRelTable;
				inWhichTable[firstRel] = secondRelIndex;
			}
			break;
		case 2:
			int firstRelIndex = inWhichTable[firstRel];
			int secondRelIndex = inWhichTable[secondRel];
			if (firstRelIndex == secondRelIndex) {
				tables[firstRelIndex].prune(firstRel, secondRel,
					RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}
			} else {
				tables[firstRelIndex].combine(firstRel, tables[secondRelIndex],
					secondRel, RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
					if ((*it).second == secondRelIndex)
						(*it).second = firstRelIndex;
			}
		}
	}

	//evaluate patterns
	for (auto it = patternAssignUsesStore.begin(); it != patternAssignUsesStore.end(); it++) {
		const string& synonym = it->first;
		const unordered_set<string>& itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			const string& usesVar = *it2;

			//actual setting up of patterns of assign for the right hand side
			RulesOfEngagement::PatternRHSType RHS;
			string RHSVarName;
			if (usesVar.at(0) == '_' && usesVar.at(usesVar.size() - 1) == '_') {
				RHS = RulesOfEngagement::PRSub;
				RHSVarName = usesVar.substr(1, usesVar.length() - 2);
			} else {
				RHS = RulesOfEngagement::PRNoSub;
				RHSVarName = usesVar;
			}
			RHSVarName = RHSVarName.substr(1, RHSVarName.length() - 2);
			ASTExprNode* RHSexprs;
			try {
				RHSexprs = AssignmentParser::processAssignment(MiniTokenizer(RHSVarName));
			} catch (SPAException& e) {	//exception indicates that the right hand side
				earlyQuit = true;		//is not correct, probably due to it containing
				return;					//a variable that is not actually present.
			}

			if (inWhichTable.count(synonym) == 0) {
				AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[synonym] = tables.size();
				tables.push_back(firstRelTable);
			}
			int synonymIndex = inWhichTable[synonym];

			tables[synonymIndex].patternPrune(synonym, RHS, RHSVarName, RHSexprs);
			if (tables[synonymIndex].getSize() == 0) {
				earlyQuit = true;
				return;
			}
		}
	}
	/*for (unsigned int i = 0; i < patternType.size(); i++) {
		string synonym = patternSynonym[i];
		RulesOfEngagement::QueryVar type = patternType[i];
		string modifiesVar = patternLHS[i];
		string usesVar = patternRHS[i];
		
		RulesOfEngagement::PatternRHSType RHS;
		string RHSVarName;
		if (usesVar.at(0) == '_' && usesVar.at(usesVar.size() - 1) == '_') {
			RHS = RulesOfEngagement::PRSub;
			RHSVarName = usesVar.substr(1, usesVar.length() - 2);
		} else {
			RHS = RulesOfEngagement::PRNoSub;
			RHSVarName = usesVar;
		}
		if (RHSVarName.at(0) != '\"' || RHSVarName.at(RHSVarName.length() - 1) != '\"')
			throw SPAException("Error, Pattern Right Hand Side Invalid");
		RHSVarName = RHSVarName.substr(1, RHSVarName.length() - 2);
		ASTExprNode* RHSexprs = AssignmentParser::processAssignment(MiniTokenizer(RHSVarName));

		if (synonymTable.isInTable(modifiesVar)) {
			int matchNumberOfTables = 0;
			if (inWhichTable.count(synonym) > 0)
				matchNumberOfTables++;
			if (inWhichTable.count(modifiesVar) > 0)
				matchNumberOfTables++;

			switch (matchNumberOfTables) {
			case 0:
				{
					AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
					if (firstRelTable.getSize() == 0)
						return;

					AnswerTable secondRelTable = AnswerTable(synonymTable, modifiesVar);
					if (secondRelTable.getSize() == 0)
						return;

					firstRelTable.cartesian(secondRelTable);
					firstRelTable.patternPrune(synonym, true, 1, RHS, RHSVarName, RHSexprs);
					if (firstRelTable.getSize() == 0)
						return;

					inWhichTable[synonym] = tables.size();
					inWhichTable[modifiesVar] = tables.size();
					tables.push_back(firstRelTable);
				}
				break;
			case 1:
				if (inWhichTable.count(synonym) > 0) {
					int firstRelIndex = inWhichTable[synonym];

					AnswerTable secondRelTable = AnswerTable(synonymTable, modifiesVar);
					if (secondRelTable.getSize() == 0)
						return;

					tables[firstRelIndex].cartesian(secondRelTable);
					tables[firstRelIndex].patternPrune(synonym, true,
						tables[firstRelIndex].synonymPosition[modifiesVar], RHS, RHSVarName, RHSexprs);
					if (tables[firstRelIndex].getSize() == 0)
						return;

					inWhichTable[modifiesVar] = firstRelIndex;
				} else {
					AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
					if (firstRelTable.getSize() == 0)
						return;

					int secondRelIndex = inWhichTable[modifiesVar];
					tables[secondRelIndex].cartesian(firstRelTable);
					tables[secondRelIndex].patternPrune(synonym, true,
						tables[secondRelIndex].synonymPosition[modifiesVar], RHS, RHSVarName, RHSexprs);
					if (tables[secondRelIndex].getSize() == 0)
						return;

					inWhichTable[synonym] = secondRelIndex;
				}
				break;
			case 2:
				int firstRelIndex = inWhichTable[synonym];
				int secondRelIndex = inWhichTable[modifiesVar];
				if (firstRelIndex != secondRelIndex) {
					tables[firstRelIndex].cartesian(tables[secondRelIndex]);

					for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
						if ((*it).second == secondRelIndex)
							(*it).second = firstRelIndex;
				}
				tables[firstRelIndex].patternPrune(synonym, true,
						tables[firstRelIndex].synonymPosition[modifiesVar], RHS, RHSVarName, RHSexprs);
				if (tables[firstRelIndex].getSize() == 0)
					return;
			}
		} else { //modifiesVar is not a synonym
			if (inWhichTable.count(synonym) == 0) {
				AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
				if (firstRelTable.getSize() == 0)
					return;

				inWhichTable[synonym] = tables.size();
				tables.push_back(firstRelTable);
			}
			int synonymIndex = inWhichTable[synonym];

			int modifiesIndex;
			if (modifiesVar.at(0) == '\"' && modifiesVar.at(modifiesVar.size() - 1) == '\"') {
				modifiesIndex = PKB::variables.getVARIndex(modifiesVar.substr(1, modifiesVar.size() - 2));
			} else if (modifiesVar == "_") {
				modifiesIndex = -1;
			} else
				throw SPAException("Error, Pattern Left Hand Side Invalid");

			tables[synonymIndex].patternPrune(synonym, false, modifiesIndex, RHS, RHSVarName, RHSexprs);
			if (tables[synonymIndex].getSize() == 0)
				return;
		}
	}*/

	//examine table sizes
	vector<string>& synonyms = synonymTable.getAllNames();
	for (auto it = synonyms.begin(); it != synonyms.end(); it++)
		if (inWhichTable.count(*it) == 0) {
			AnswerTable table = AnswerTable(synonymTable, *it);
			if (table.getSize() == 0) {
				earlyQuit = true;
				return;
			}
			if (synonymTable.isSelected(*it)) {
				inWhichTable[*it] = tables.size();
				tables.push_back(table);
			}
		}

	if (selectBOOLEAN) {
		finalanswer.push_back("TRUE");
		return;
	}

	//project out selected variables
	vector<string> selecteds = synonymTable.getAllSelected();
	vector<vector<string>> projections(components.size());
	int firstNonEmpty = components.size();
	for (auto it = selecteds.begin(); it != selecteds.end(); it++) {
		if (inWhichTable.count(*it) == 0) {
			AnswerTable table = AnswerTable(synonymTable, *it);
			inWhichTable[*it] = tables.size();
			tables.push_back(table);
		}
		if (inWhichTable[*it] < firstNonEmpty)
			firstNonEmpty = inWhichTable[*it];
		projections[inWhichTable[*it]].push_back(*it);
	}

	AnswerTable concatenated = tables[firstNonEmpty].project(projections[firstNonEmpty]);
	for (unsigned int i = firstNonEmpty + 1; i < components.size(); i++)
		if (!projections[i].empty())
			concatenated.cartesian(tables[i].project(projections[i]));

	vector<int> orderOfSelection;
	for (auto it = selects.begin(); it != selects.end(); it++)
		orderOfSelection.push_back(concatenated.synonymPosition[*it]);
	
	//convert vector of vector of int to vector of string
	vector<string> header = concatenated.getHeader();
	for (unsigned int i = 0; i < concatenated.getSize(); i++) {
		vector<int> row = concatenated.getRow(i);
		string answer;
		for (auto it = orderOfSelection.begin(); it != orderOfSelection.end(); it++) {
			RulesOfEngagement::QueryVar type = synonymTable.getType(header[*it]);
			if (type == RulesOfEngagement::Procedure)
				answer += PKB::procedures.getPROCName(row[*it]);
			else if (type == RulesOfEngagement::Variable)
				answer += PKB::variables.getVARName(row[*it]);
			else
				answer += Helper::intToString(row[*it]);
			if (it + 1 != orderOfSelection.end())
				answer += " ";
		}
		finalanswer.push_back(answer);
	}
}

////mini tokenizer
vector<string> MultiQueryEval::MiniTokenizer(const string& line)
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
				for(unsigned int i=0;i<tempstr1.size();)
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
					//AddToList(list,tempstr);
					//AddTables(list,tempstr);
					//list.push_back(tempstr);			
				
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