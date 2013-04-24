#pragma once
#include "MultiQueryEval.h"
#include "Helper.h"
#include "PKB.h"
#include "RulesOfEngagement.h"
#include "AnswerTable.h"
#include "AssignmentParser.h"

/**
* Gets a token, defined as
* 1) LETTER (LETTER | DIGIT | #)*
* 1a) RelationStar
* 1b) prog_line
* 2) DIGIT*
* 3) Entire string enclosed on both sides by the quote character
* 4) A single character
* A token enclosed by the underscore character is obtained using getToken2()
* @return a token
*/
string MultiQueryEval::getToken()
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";
	pos = first + 1;
	if ((query.at(first) >= 'A' && query.at(first) <= 'Z') ||
		(query.at(first) >= 'a' && query.at(first) <= 'z')) { //IDENT
			while (pos < query.length() && (
				(query.at(pos) >= 'A' && query.at(pos) <= 'Z') ||
				(query.at(pos) >= 'a' && query.at(pos) <= 'z') || //letter or
				(query.at(pos) >= '0' && query.at(pos) <= '9') || query.at(pos) == '#' || //digit
				(query.at(pos) == '*') || //relation star or prog_line (below)
				(query.at(pos) == '_' && pos == first + 4 && query.substr(first, 4) == "prog")))
				pos++;
	} else if (query.at(first) >= '0' && query.at(first) <= '9') { //DIGIT
		while (pos < query.length() && (query.at(pos) >= '0' && query.at(pos) <= '9'))
			pos++;
	} else if (query.at(first) == '\"')
		pos = query.find_first_of('\"', pos) + 1;
	//else (pos == first + 1) => single character
	return query.substr(first, pos - first);
}


/**
* Gets a token specially for use in the second argument of an assign pattern.
* @return a token
*/
string MultiQueryEval::getToken2()
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";

	stack<char> charstack;
	int tempt = first;
	while (true)
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
			else {
				died = true;
				return "";
			}
		}
		else {
			died = true;
			return "";
		}
		tempt = pos+1;
	}
	if(charstack.size() != 0 ) {
		died = true;
		return "";
	}

	if (pos == first) {
		pos++;
		return query.substr(first, 1);
	}
	return query.substr(first, pos - first);
}

/**
* Checks that the next token is equal to the required string.
* @param match string for token to match with
* @return true if the token matched the required string, and false otherwise
*/
bool MultiQueryEval::matchToken(const string& match, bool error = true)
{
	if (error) {
		if (getToken() != match) {
			died = true;
			return false;
		}
		return true;
	} else {
		int tempPos = pos;
		string token = getToken();
		if (token != match) {
			pos = tempPos;
			return false;
		}
		return true;
	}
}

/**
* Public method for evaluating queries.
* Passes call to internal MultiQueryEval constructor. Only adds in the answer of FALSE
* if the evaluation was returned early and the select was for a BOOLEAN.
* @param query query string
* @return the vector of strings of the answers to the query
*/
void MultiQueryEval::evaluateQuery(const string& query, list<string>& results)
{
	MultiQueryEval queryEvaluator(Helper::reduce(query));
	queryEvaluator.validate();
	if (queryEvaluator.died)
		return;	
	if (queryEvaluator.selectBOOLEAN && queryEvaluator.earlyQuit) {
		results.push_back("false");
		return;
	}
	queryEvaluator.optimise();
	queryEvaluator.evaluate(results);
	if (queryEvaluator.selectBOOLEAN && queryEvaluator.earlyQuit)
		results.push_back("false");
}

/**
* Constructs a MultiQueryEval object.
* @param query the query string
* @return the MultiQueryEval object
*/
MultiQueryEval::MultiQueryEval(const string& query)
	: selectBOOLEAN(false), died(false), earlyQuit(false), pos(0), query(query)
{
}

/**
* Validates the query is in line with the PQL grammar. Throw an exception if it does not.
*/
void MultiQueryEval::validate()
{
	//		PARSE SYNONYM DECLARATION

	while (true) {
		string token = getToken();
		if (token == "Select")
			break;
		if (RulesOfEngagement::tokenToVar.count(token) == 0) {
			died = true;
			return;
		}
		RulesOfEngagement::QueryVar type = RulesOfEngagement::tokenToVar[token];
		
		string variable;
		do {
			variable = getToken();
			if (stringToQueryVar.count(variable) > 0) {
				died = true;
				return;
			}
			stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>(variable, type));
			stringCount.insert(pair<string, int>(variable, 0));
		} while ((variable = getToken()) == ",");
		if (variable != ";") {
			died = true;
			return;
		}
	}
	//		END PARSE SYNONYM DECLARATION

	//		PARSE SELECTED VARIABLES
	string selected = getToken();
	if (selected.at(0) == '<') { //tuple -> multiple selected variables
		do {
			string synonym = getToken();
			if (stringToQueryVar.count(synonym) == 0) {
				died = true;
				return;
			}
			selected = getToken();
			string condition;
			if (selected == ".") {
				condition = getToken();
				selected = getToken();
			} else if (selected == ",")
				condition = "";
			else if (selected != ">") {
				died = true;
				return;
			}

			selects.push_back(pair<string, string>(synonym, condition));
			if (selectsSet.count(synonym) > 0) {
				if (selectsSet[synonym].count(condition) == 0)
					selectsSet[synonym].insert(condition);
			} else {
				unordered_set<string> set;
				set.insert(condition);
				selectsSet.insert(pair<string, unordered_set<string>>(synonym, set));
			}

			if (selected == ">")
				break;
			else if (selected != ",") {
				died = true;
				return;
			}
		} while (true);
	} else if (selected == "BOOLEAN") {
		selectBOOLEAN = true;
	} else {
		if (stringToQueryVar.count(selected) == 0) {
			died = true;
			return;
		}
		bool dot = matchToken(".", false);
		string condition;
		if (dot) {
			condition = getToken();
		} else
			condition = "";
		selects.push_back(pair<string, string>(selected, condition));

		if (selectsSet.count(selected) > 0) {
			if (selectsSet[selected].count(condition) == 0)
				selectsSet[selected].insert(condition);
		} else {
			unordered_set<string> set;
			set.insert(condition);
			selectsSet.insert(pair<string, unordered_set<string>>(selected, set));
		}
	}
	//		END PARSE SELECTED VARIABLES

	//		PARSE RELATIONSHIPS, CONDITIONS AND PATTERN
	enum Clauses { Undefined, Such_That, With, Pattern };
	Clauses clauseType = Undefined;
	while (true) {
		string clause = getToken();
		if (clause == "") {
			break;
		} else if (clause == "such") {
			if (!matchToken("that"))
				return;
			clauseType = Such_That;
		} else if (clause == "with") {
			clauseType = With;
		} else if (clause == "pattern") {
			clauseType = Pattern;
		} else if (clause != "and" || clauseType == Undefined) {
			died = true;
			return;
		}

		switch (clauseType) {
		case Such_That:
			{
				string relation = getToken();
				if (!matchToken("("))
					return;
				string firstRel = getToken();
				if (!matchToken(","))
					return;
				string secondRel = getToken();
				if (!matchToken(")"))
					return;

				if (RulesOfEngagement::tokenToRel.count(relation) == 0) {
					died = true;
					return;
				}
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
				else {
					died = true;
					return;
				}

				RulesOfEngagement::QueryRelations type =
					RulesOfEngagement::PatternUses; //sentinel value
				for (auto it = types.begin(); it != types.end(); it++)
					if (RulesOfEngagement::allowableFirstArgument[*it].count(firstRelType) > 0) {
						type = *it;
						break;
					}
				if (type == RulesOfEngagement::PatternUses) {
					died = true;
					return;
				}

				RulesOfEngagement::QueryVar secondRelType;
				if (secondRel == "_")
					secondRelType = RulesOfEngagement::WildCard;
				else if (stringToQueryVar.count(secondRel) > 0)
					secondRelType = stringToQueryVar[secondRel];
				else if (secondRel.at(0) == '\"' && secondRel.at(secondRel.length() - 1) == '\"')
					secondRelType = RulesOfEngagement::String;
				else if (Helper::isNumber(secondRel))
					secondRelType = RulesOfEngagement::Integer;
				else {
					died = true;
					return;
				}

				if (RulesOfEngagement::allowableSecondArgument[type].count(secondRelType) == 0) {
					died = true;
					return;
				}
				
				relationStore[type][firstRel].insert(secondRel);
				if (stringToQueryVar.count(firstRel) > 0) {
					if (stringToQueryVar.count(secondRel) > 0) {
						++stringCount[firstRel];
						++stringCount[secondRel];
					} else
					mustHandle.insert(firstRel);
				} else if (stringToQueryVar.count(secondRel) > 0)
					mustHandle.insert(secondRel);
			}
			break;

		case With:
			{
				string synonym = getToken();
				if (stringToQueryVar.count(synonym) == 0) { //10 = c.value OR 10 = 5
					RulesOfEngagement::QueryVar LHSType;
					if (Helper::isNumber(synonym))
						LHSType = RulesOfEngagement::Integer;
					else if (synonym.at(0) == '"' && synonym.at(synonym.length() - 1) == '"')
						LHSType = RulesOfEngagement::String;
					else {
						died = true;
						return;
					}

					if (!matchToken("="))
						return;
					string token = getToken();
					if (stringToQueryVar.count(token) == 0) { //10 = 5
						earlyQuit |= (synonym != token);
					} else { //10 = c.value
						RulesOfEngagement::QueryVar type = stringToQueryVar[token];
						string condition;
						RulesOfEngagement::QueryVar RHSType;
						if (type == RulesOfEngagement::Prog_Line) {
							condition = "";
							RHSType = RulesOfEngagement::Integer;
						} else {
							if (!matchToken("."))
								return;
							condition = getToken();
							if (RulesOfEngagement::allowableConditions[type].count(
								condition) == 0) {
								died = true;
								return;
							}
							RHSType = RulesOfEngagement::conditionTypes[condition];
						}

						if (RHSType == RulesOfEngagement::Integer) {
							if (!Helper::isNumber(synonym)) {
								died = true;
								return;
							}
						} else if (RHSType == RulesOfEngagement::String) {
							if (synonym.at(0) != '"' || synonym.at(synonym.length() - 1) != '"') {
								died = true;
								return;
							}
						} else {
							died = true;
							return;
						}
						if (LHSType != RHSType) {
							died = true;
							return;
						}
						
						if (conditionStore[token].count(condition) == 0)
							conditionStore[token].insert(
							pair<string, string>(condition, synonym));
						else
							earlyQuit |= (conditionStore[token][condition] != synonym);
						mustHandle.insert(token);
					}
				} else {
					RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];
					string condition;
					RulesOfEngagement::QueryVar LHSType;
					if (type == RulesOfEngagement::Prog_Line) {
						condition = "";
						LHSType = RulesOfEngagement::Integer;
					} else {
						if (!matchToken("."))
							return;
						condition = getToken();
						if (RulesOfEngagement::allowableConditions[type].count(condition) == 0) {
							died = true;
							return;
						}
						LHSType = RulesOfEngagement::conditionTypes[condition];
					}

					if (!matchToken("="))
						return;
					//either c.value = 10 OR c.value = s.stmt#
					string token = getToken();
					if (stringToQueryVar.count(token) == 0) { //c.value = 10
						if (LHSType == RulesOfEngagement::Integer) {
							if (!Helper::isNumber(token)) {
								died = true;
								return;
							}
						} else if (LHSType == RulesOfEngagement::String) {
							if (token.at(0) != '"' || token.at(token.length() - 1) != '"') {
								died = true;
								return;
							}
						} else {
							died = true;
							return;
						}

						if (conditionStore[synonym].count(condition) == 0) {
							conditionStore[synonym].insert(
							pair<string, string>(condition, token));
							mustHandle.insert(synonym);
						} else
							earlyQuit |= (conditionStore[synonym][condition] != token);
					} else { //c.value = s.stmt# (OR c.value = n)
						RulesOfEngagement::QueryVar type2 = stringToQueryVar[token];
						string condition2;
						if (type2 == RulesOfEngagement::Prog_Line) {
							condition2 = "";
						} else {
							if (!matchToken("."))
								return;
							condition2 = getToken();
						}

						if (synonym == token && condition == condition2)
							break;

						if (RulesOfEngagement::allowableConditions[type2].count(condition2) == 0) {
							died = true;
							return;
						}
						RulesOfEngagement::QueryVar RHSType =
							RulesOfEngagement::conditionTypes[condition2];
						if (LHSType != RHSType) {
							died = true;
							return;
						}

						if (condition2Store.count(token) > 0) {
							if (condition2Store[token].count(condition2) > 0 && condition2Store[token][condition2].count(synonym) > 0)
								earlyQuit |= (condition != condition2Store[token][condition2][synonym]);
							else
								condition2Store[token][condition2].insert(pair<string, string>(synonym, condition));
						} else {
							if (condition2Store.count(synonym) > 0 && condition2Store[synonym].count(condition) > 0 && condition2Store[synonym][condition].count(token) > 0)
								earlyQuit |= (condition2 != condition2Store[synonym][condition][token]);
							else
								condition2Store[synonym][condition].insert(pair<string, string>(token, condition2));
						}
						mustHandle.insert(synonym);
						mustHandle.insert(token);
					}
				}
			}
			break;

		case Pattern:
			{
				const string synonym = getToken();
				if (stringToQueryVar.count(synonym) == 0) {
					died = true;
					return;
				}
				const RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];

				if (!matchToken("("))
					return;
				const string firstRel = getToken();

				switch (type) {
				case RulesOfEngagement::Assign: {
						if (firstRel != "_") {
							if (stringToQueryVar.count(firstRel) > 0) {
								if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable) {
									died = true;
									return;
								}
								stringCount[firstRel]++;
							} else if (firstRel.at(0) != '\"' ||
								firstRel.at(firstRel.length() - 1) != '\"') {
								died = true;
								return;
							}
							
							relationStore[RulesOfEngagement::ModifiesStmt][synonym].insert(firstRel);
							if (stringToQueryVar.count(firstRel) > 0) {
								stringCount[synonym]++;
								stringCount[firstRel]++;
							} else
								mustHandle.insert(synonym);
						}
						
						if (!matchToken(","))
							return;
						string secondRel = getToken2();
						if (!matchToken(")"))
							return;

						secondRel.erase(remove(secondRel.begin(), secondRel.end(), '\t'),
							secondRel.end());
						secondRel.erase(remove(secondRel.begin(), secondRel.end(), ' '),
							secondRel.end());
						if (secondRel != "_") {
							//remove white spaces
							const size_t length = secondRel.length();
							if (length <= 2) {
								died = true;
								return;
							}
							if (secondRel.at(0) == '_' && secondRel.at(length - 1) == '_') {
								if (length <= 4 || secondRel.at(1) != '\"' ||
									secondRel.at(length - 2) != '\"') {
									died = true;
									return;
								}
							} else if (secondRel.at(0) != '\"' ||
								secondRel.at(length - 1) != '\"') {
								died = true;
								return;
							}

							if (secondRel.at(0) == '_' && secondRel.at(length - 1) == '_' &&
								secondRel.at(1) == '\"' && secondRel.at(length - 2) == '\"' &&
								PKB::variables.getVARIndex(secondRel.substr(2, length - 4)) != -1) {
									const string& variable = secondRel.substr(1, length - 2);
									relationStore[RulesOfEngagement::UsesStmt][synonym].
										insert(variable);
									mustHandle.insert(synonym);
							} else {
								patternAssignUsesStore[synonym].insert(secondRel);
								if (stringToQueryVar.count(secondRel) > 0) {
									stringCount[synonym]++;
									stringCount[secondRel]++;
								} else
									mustHandle.insert(synonym);
							}
						}
												}
					break;

				case RulesOfEngagement::If: {
					if (!matchToken(","))
						return;
					const string secondRel = getToken();
					if (!matchToken(","))
						return;
					const string thirdRel = getToken();
					if (!matchToken(")"))
						return;
					bool nothing = true;

					if (firstRel != "_") {
						nothing = false;
						if (stringToQueryVar.count(firstRel) > 0) {
							if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable) {
								died = true;
								return;
							}
						} else if (firstRel.at(0) != '\"' ||
							firstRel.at(firstRel.length() - 1) != '\"') {
							died = true;
							return;
						}

						relationStore[RulesOfEngagement::PatternModifies][synonym].insert(firstRel);
						if (stringToQueryVar.count(firstRel) > 0) {
							stringCount[synonym]++;
							stringCount[firstRel]++;
						} else
							mustHandle.insert(synonym);
					}

					if (secondRel != "_") {
						nothing = false;
						if (stringToQueryVar.count(secondRel) == 0 ||
							stringToQueryVar[secondRel] != RulesOfEngagement::Statement_List) {
							died = true;
							return;
						}

						relationStore[RulesOfEngagement::PatternSecond][synonym].insert(secondRel);
						stringCount[synonym]++;
						stringCount[secondRel]++;
					}

					if (thirdRel != "_") {
						nothing = false;
						if (stringToQueryVar.count(thirdRel) == 0 ||
							stringToQueryVar[thirdRel] != RulesOfEngagement::Statement_List) {
							died = true;
							return;
						}

						relationStore[RulesOfEngagement::PatternThird][synonym].insert(thirdRel);
						stringCount[synonym]++;
						stringCount[thirdRel]++;
					}

					if (nothing)
						earlyQuit |= !RulesOfEngagement::isExistType(type);
											}
					break;

				case RulesOfEngagement::While: {
					if (!matchToken(","))
						return;
					const string secondRel = getToken();
					if (!matchToken(")"))
						return;
					bool nothing = true;

					if (firstRel != "_") {
						nothing = false;
						if (stringToQueryVar.count(firstRel) > 0) {
							if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable) {
								died = true;
								return;
							}
						} else if (firstRel.at(0) != '\"' ||
							firstRel.at(firstRel.length() - 1) != '\"') {
							died = true;
							return;
						}

						relationStore[RulesOfEngagement::PatternModifies][synonym].insert(firstRel);
						if (stringToQueryVar.count(firstRel) > 0) {
							stringCount[synonym]++;
							stringCount[firstRel]++;
						} else
							mustHandle.insert(synonym);
					}

					if (secondRel != "_") {
						nothing = false;
						if (stringToQueryVar.count(secondRel) == 0 ||
							stringToQueryVar[secondRel] != RulesOfEngagement::Statement_List) {
							died = true;
							return;
						}

						relationStore[RulesOfEngagement::PatternSecond][synonym].insert(secondRel);
						stringCount[synonym]++;
						stringCount[secondRel]++;
					}

					if (nothing)
						earlyQuit |= !RulesOfEngagement::isExistType(type);
											   }
					break;

				default:
					died = true;
					return;
				
				} //end switch of synonym type in pattern
			} //end case of pattern
		} //end switch of clauseType
	}
	//		END PARSE RELATIONSHIPS, CONDITIONS AND PATTERN
}

/**
* Optimises the query so as to do less computation.
*/
void MultiQueryEval::optimise()
{
	//apply special rules
	//1) stmt s1, s2, ... s_n; 
	//   Follows(s1,s2) and Follows(s2,s3) and ... and Follows(sk,1)
	//   (Each Follows can refer to either normal Follows or Follows*)
	//   will be converted to
	//   earlyQuit = true
	unordered_map<string, unordered_set<string>> lessThanRelation;
	unordered_map<string, unordered_set<string>> moreThanRelation;

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
				const string& secondRel = *it2;
				if (stringToQueryVar.count(secondRel) == 0)
					continue;

				if (lessThanRelation.count(secondRel) > 0) {
					unordered_set<string> lessSecond = lessThanRelation[secondRel];
					if (lessSecond.count(firstRel) > 0) {
						earlyQuit = true;
						return;
					}
					for (auto it = lessSecond.begin(); it != lessSecond.end(); it++) {
						lessThanRelation[firstRel].insert(*it);
						moreThanRelation[*it].insert(firstRel);
					}
				}

				lessThanRelation[firstRel].insert(secondRel);
				moreThanRelation[secondRel].insert(firstRel);

				if (moreThanRelation.count(firstRel) > 0) {
					unordered_set<string> moreFirst = moreThanRelation[firstRel];
					for (auto it = moreFirst.begin(); it != moreFirst.end(); it++) {
						lessThanRelation[*it].insert(secondRel);
						moreThanRelation[secondRel].insert(*it);
					}
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
				const string& secondRel = *it2;
				if (stringToQueryVar.count(secondRel) == 0)
					continue;

				if (lessThanRelation.count(secondRel) > 0) {
					unordered_set<string> lessSecond = lessThanRelation[secondRel];
					if (lessSecond.count(firstRel) > 0) {
						earlyQuit = true;
						return;
					}
					for (auto it = lessSecond.begin(); it != lessSecond.end(); it++) {
						lessThanRelation[firstRel].insert(*it);
						moreThanRelation[*it].insert(firstRel);
					}
				}

				lessThanRelation[firstRel].insert(secondRel);
				moreThanRelation[secondRel].insert(firstRel);

				if (moreThanRelation.count(firstRel) > 0) {
					unordered_set<string> moreFirst = moreThanRelation[firstRel];
					for (auto it = moreFirst.begin(); it != moreFirst.end(); it++) {
						lessThanRelation[*it].insert(secondRel);
						moreThanRelation[secondRel].insert(*it);
					}
				}
			}
		}
	//end special rules

	//second stage
	//analyse synonyms part 1. look for those with count of 1 to be folded in
	/*unordered_set<string> toFold; //todo:: alias synonyms
	for (auto it = stringCount.begin(); it != stringCount.end(); it++)
		if (selects.count(it->first) == 0 && it->second == 1)
			toFold.insert(it->first);*/

	//parse equality of attributes, alias those that can be aliased

	for (auto it = condition2Store.begin(); it != condition2Store.end(); it++) {
		const string& firstRel = it->first;
		const auto itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			const string& firstCondition = it2->first;
			const auto it2second = it2->second;
			for (auto it3 = it2second.begin(); it3 != it2second.end(); it3++) {
				const string& secondRel = it3->first;
				const string& secondCondition = it3->second;

				if (firstCondition == secondCondition) { // a.cond = b.cond
					if (firstRel == secondRel) // s.cond = s.cond -> skip
						continue;
					if (firstCondition == "stmt#") { // a.stmt# = b.stmt#
						if (stringToQueryVar[firstRel] == RulesOfEngagement::Statement) { // s.stmt# = b.stmt#
							if (stringToQueryVar[secondRel] != RulesOfEngagement::Statement) // s.stmt# = b.stmt#, b.type != statement
								stringToQueryVar[firstRel] = stringToQueryVar[secondRel]; //-> a1.stmt# = a2.stmt#
						} else { // b.stmt# = s.stmt#
							if (stringToQueryVar[secondRel] == RulesOfEngagement::Statement) // b.stmt# = s.stmt#, b.type != statement
								stringToQueryVar[secondRel] = stringToQueryVar[firstRel]; //-> a1.stmt# = a2.stmt#, a1.type = a2.type
							else if (stringToQueryVar[firstRel] != stringToQueryVar[secondRel]) { // a.stmt# = b.stmt#, a.type != b.type
								earlyQuit = true;
								return;
							}
						}
						//at this point, either quit already, else firstRel.type = secondRel.type
					}
					if (RulesOfEngagement::cannotAlias.count(firstCondition) == 0 &&
						stringToQueryVar[firstRel] == stringToQueryVar[secondRel]) { // s1.cond = s2.cond
						dsAlias.setUnion(firstRel, secondRel);
						//string& firstRelParent = dsAlias.find(firstRel);
						//int firstRelCount = stringCount[firstRelParent];
						//string& secondRelParent = dsAlias.find(secondRel);
						//int secondRelCount = stringCount[secondRelParent];
						//stringCount[firstRelParent] = stringCount[secondRelParent] =
							//firstRelCount + secondRelCount - 2;
						continue;
					}
				}
		
				//Fold in conditions if possible (optimisation)
				//stmt s1, s2; constant c; Select s1 with s2.stmt# = c.value -> no idea how to fold
				//stmt s; constant c; Select s with s.stmt# = c.value -> fold in (constant, value) into s
				/*if (selects.count(firstRel) == 0 && stringCount[firstRel] == 1) {
					if (selects.count(secondRel) == 0 && stringCount[secondRel] == 1) { //don't fold
						stringCount[firstRel] = stringCount[secondRel] = 1 << 16;
					} else { //stmt s; constant c; Select s with c.value = s.stmt#
						synonymTable.insert(secondRel, stringToQueryVar[secondRel]);
						synonymTable.setGenericAttribute(secondRel,
							secondCondition, stringToQueryVar[firstRel], firstCondition);
						stringCount[firstRel]--;
						stringCount[secondRel]--;
					}
				} else {
					if (selects.count(secondRel) == 0 && stringCount[secondRel] == 1) { //stmt s; constant c; Select s with s.stmt# = c.value
						synonymTable.insert(firstRel, stringToQueryVar[firstRel]);
						synonymTable.setGenericAttribute(firstRel,
							firstCondition, stringToQueryVar[secondRel], secondCondition);
						stringCount[firstRel]--;
						stringCount[secondRel]--;
					} else { //stmt s; constant c; Select <s,c> with s.stmt# = c.value*/
						/*condFirstRel.push_back(firstRel);
						condFirstCondition.push_back(firstCondition);
						condSecondRel.push_back(secondRel);
						condSecondCondition.push_back(secondCondition);*/
						conditionsList.push_back(
							RulesOfEngagement::Condition(firstRel, firstCondition, secondRel, secondCondition));
						dsSynonym.setUnion(firstRel, secondRel);
					/*}
				}*/
			}
		}
	}

	//		SET UP ALIASING
	const vector<unordered_set<string>>& aliasSets = dsAlias.getComponents();
	for (auto it = aliasSets.begin(); it != aliasSets.end(); it++) {
		const unordered_set<string>& set = *it;
		const string& target = *set.begin();
		for (auto it2 = set.begin(); it2 != set.end(); it2++)
			aliasMap.insert(pair<string, string>(*it2, target));
	}
	//		END SET UP ALIASING

	//		ALIAS SYNONYMS IN CONDITIONLIST
	for (auto it = conditionsList.begin(); it != conditionsList.end(); it++) {
		RulesOfEngagement::Condition& cond = *it;
		if (aliasMap.count(cond.firstRel) > 0)
			cond.firstRel = aliasMap[cond.firstRel];
		if (aliasMap.count(cond.secondRel) > 0)
			cond.secondRel = aliasMap[cond.secondRel];
	}
	//		END ALIAS SYNONYMS IN CONDITIONLIST
}

/**
* Does all the query evaluation.
* @param results the list provided by Autotester to store the results
* @return the MultiQueryEval object with answers if any
*/
void MultiQueryEval::evaluate(list<string>& results)
{
	if (earlyQuit)
		return;
	
	//analyse synonyms part 2. those selected and with count of more than 1 are put into the synonym table
	for (auto it = stringToQueryVar.begin(); it != stringToQueryVar.end(); it++) {
		const string& synonym = it->first;
		if (aliasMap.count(synonym) > 0) {
			if (aliasMap[synonym] == synonym) {
				synonymTable.insert(synonym, it->second);
				for (auto it2 = aliasMap.begin(); it2 != aliasMap.end(); it2++)
					if (selectsSet.count(it2->first)) {
						synonymTable.setSelected(synonym);
						break;
					}
			}
		} else if (selectsSet.count(synonym) > 0) {
			synonymTable.insert(synonym, it->second);
			//dsSynonym.makeSet(synonym);
			synonymTable.setSelected(synonym);
		} else if (mustHandle.count(synonym) > 0 ||
			(stringCount.count(synonym) > 0 && stringCount[synonym] > 0)) {
			synonymTable.insert(synonym, it->second);
			//dsSynonym.makeSet(synonym);
		}
	}
	
	//relationship table
	vector<RulesOfEngagement::Relation> relList;

	vector<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> zerosyno;
	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> onesyno;
	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> twosyno;

	//		SEPARATE RELATIONS
	for (auto it = relationStore.begin(); it != relationStore.end(); it++) {
		const RulesOfEngagement::QueryRelations& type = it->first;
		const unordered_map<string, unordered_set<string>>& itsecond = it->second;
		
		for (auto it2 = itsecond.begin(); it2 != itsecond.end();it2++) {
			const string& first =
				(aliasMap.count(it2->first) > 0) ? aliasMap[it2->first] : it2->first;
			int amount_of_synoname;
			if (synonymTable.isInTable(first))
				amount_of_synoname = 1;
			else
				amount_of_synoname = 0;

			for (auto it3 = it2->second.begin(); it3 != it2->second.end();it3++) {
				//if not a var dont add into pool
				const string& second = (aliasMap.count(*it3) > 0) ? aliasMap[*it3] : *it3;

				int amount_of_synoname2 = amount_of_synoname;
				if (synonymTable.isInTable(second))
					amount_of_synoname2++;

				switch (amount_of_synoname2) {
				case 0:
					zerosyno.push_back(pair<RulesOfEngagement::QueryRelations,
						pair<string, string>>(type,pair<string, string>(first,second)));
					break;
				case 1:
					onesyno.push_back(pair<RulesOfEngagement::QueryRelations,
						pair<string, string>>(type,pair<string, string>(first,second)));
					break;
				case 2:
					twosyno.push_back(pair<RulesOfEngagement::QueryRelations,
						pair<string, string>>(type,pair<string, string>(first,second)));
					break;
				}
			}
			
		}
	}

	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> temponesyno;
	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> temptwosyno;

	//sort
	for(size_t i=0;i<PKB::sortorder.size();i++)
	{
		list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>>::iterator
			j = onesyno.begin();
		while (j != onesyno.end())
		{
			if(j->first == PKB::sortorder.at(i).first)
			{
				temponesyno.push_back(*j);
				onesyno.erase(j++);
			}
			else
			{
				++j;
			}
			
			
		}
		

		j = twosyno.begin();
		while (j != twosyno.end())
		{
			if(j->first == PKB::sortorder.at(i).first)
			{
				temptwosyno.push_back(*j);
				twosyno.erase(j++);
			}
			else
			{
				++j;
			}		
		}
	}
	//add remaining back
	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>>::iterator
		j = onesyno.begin();
		while (j != onesyno.end())
		{
				temponesyno.push_back(*j);
				++j;
		}

		j = twosyno.begin();
		while (j != twosyno.end())
		{
				temptwosyno.push_back(*j);
				++j;
		}

		onesyno = temponesyno;
		twosyno = temptwosyno;
	
	/*
	*/
	//================================

	bool newpool = true;
	unordered_set<string> pool;

	vector<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> newcontainer;

	while (onesyno.size() > 0 || twosyno.size() > 0)
	{
		if (newpool) {
			if (onesyno.size() > 0) {
				pool = unordered_set<string>();
				pair<RulesOfEngagement::QueryRelations,
					pair<string, string>>& ones = onesyno.front();

				if (synonymTable.isInTable(ones.second.first))
					pool.insert(ones.second.first);
				else //if (synonymTable.isInTable(ones.second))
					pool.insert(ones.second.second);

				newcontainer.push_back(ones);
				onesyno.erase(onesyno.begin());
				//newpool = false;
			} else { //if (twosyno.size() > 0) {
				pool = unordered_set<string>();
				pair<RulesOfEngagement::QueryRelations,
					pair<string, string>>& two = twosyno.front();

				pool.insert(two.second.first);
				pool.insert(two.second.second);

				newcontainer.push_back(two);
				twosyno.erase(twosyno.begin());
				//newpool = false;
			}
			newpool = false;
		}

		bool addedtwosyno = false;

		//loop two syno for once
		for (auto it = twosyno.begin(); it != twosyno.end(); it++)
		{
			unordered_set<string>::const_iterator got1 = pool.find(it->second.first);
			unordered_set<string>::const_iterator got2;
			if (got1 == pool.end())
				got2 = pool.find(it->second.second);

			if (got1 != pool.end() || got2 != pool.end()) { //exist
				newcontainer.push_back(*it);
				pool.insert(it->second.first);
				pool.insert(it->second.second);
				//remove
				twosyno.erase(it);

				addedtwosyno = true;
				break;
			}
		}

		//loop one syno for all
		for (auto it = onesyno.begin(); it != onesyno.end();)
		{
			bool toadd = false;
			if (synonymTable.isInTable(it->second.first))
			{
				if (pool.find(it->second.first) != pool.end())
				{
					//exist
					toadd = true;
					pool.insert(it->second.first);
				}
			}
			else //if (synonymTable.isInTable(it->second))
			{
				if (pool.find(it->second.second) != pool.end())
				{
					//exist
					toadd = true;
					pool.insert(it->second.second);
				}
			}

			if (toadd) {
				//at this pt added to pool

				//add to new container
				newcontainer.push_back(*it);

				//now need to remove from the twosyno
				auto it2 = it;
				it2++;
				onesyno.erase(it);
				it = it2;
			} else
				it++;
		}

		if (!addedtwosyno)
			newpool = true;
	}

	//now the best order is stored in newcontainer but with vector<string,string> instead
	//		END SEPARATE RELATIONS

	//put the patterns into the synonym table
	for (auto it = patternAssignUsesStore.begin(); it != patternAssignUsesStore.end(); it++) {
		const unordered_set<string> itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++)
			synonymTable.setPattern(
			(aliasMap.count(it->first) > 0) ? aliasMap[it->first] : it->first, *it2);
	}


	//		PARSE RELATIONS
	//	PARSE TRIVIAL RELATIONS
	unsigned int tempVars = 0;
	for (auto it = zerosyno.begin(); it != zerosyno.end(); it++) {
		const RulesOfEngagement::QueryRelations& type = it->first;
		const string& firstRel= it->second.first;
		const string& secondRel= it->second.second;

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
				synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[secondArg].begin()),
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
				synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[firstArg].begin()),
					firstRel);
				synonymTable.setFirstGeneric(sugar, type);
			} else { //Case 3: rel("1","2") 
				int first = RulesOfEngagement::convertArgumentToInteger(type, true, firstRel);
				int second = RulesOfEngagement::convertArgumentToInteger(type, false, secondRel);
				if (RulesOfEngagement::takesInASTNode[type]) {
					const ASTNode * const firstAST = *RulesOfEngagement::convertIntegerToASTNode(
						RulesOfEngagement::privilegedFirstArgument[type], first).begin();
					const ASTNode * const secondAST = *RulesOfEngagement::convertIntegerToASTNode(
						RulesOfEngagement::privilegedSecondArgument[type], second).begin();
					if (!RulesOfEngagement::getRelation2(type)(firstAST, secondAST)) {
						earlyQuit = true;
						return;
					}
				} else {
					if (!RulesOfEngagement::getRelation(type)(first, second)) {
						earlyQuit = true;
						return;
					}
				}
			}
		}
	}
	//	END PARSE TRIVIAL RELATIONS

	//	PARSE NORMAL RELATIONS
	for (auto it = newcontainer.begin(); it != newcontainer.end(); it++) {
	//for (auto it = relationStore.begin(); it != relationStore.end(); it++) {
		const RulesOfEngagement::QueryRelations& type = it->first;
		const string& firstRel = it->second.first;
		const string& secondRel = it->second.second;
		//const unordered_map<string, unordered_set<string>>& itsecond = it->second;
		//for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			/*string firstRel;
			if (toFold.count(it2->first) > 0) {
				if (stringToQueryVar[it2->first] == RulesOfEngagement::privilegedFirstArgument[type])
					firstRel = "_";
				else {
					firstRel = it2->first;
					synonymTable.insert(firstRel, stringToQueryVar[firstRel]);
					dsSynonym.makeSet(firstRel);
				}
			} else
				firstRel = it2->first;*/
			//const string& firstRel= it2->first;
			//const unordered_set<string>& it2second = it2->second;
			//for (auto it3 = it2second.begin(); it3 != it2second.end(); it3++) {
				/*string secondRel;
				if (toFold.count(*it3) > 0) {
					if (stringToQueryVar[*it3] == RulesOfEngagement::privilegedSecondArgument[type])
						secondRel = "_";
					else {
						secondRel = *it3;
						synonymTable.insert(secondRel, stringToQueryVar[secondRel]);
						dsSynonym.makeSet(secondRel);
					}
				} else
					secondRel = *it3;*/
				//const string& secondRel= *it3;
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
				/*case 0: //handle 0 synonym, 3 cases
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
							synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[secondArg].begin()),
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
							synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[firstArg].begin()),
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
					break;*/

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

					/*if (relFirstToIndices.count(firstRel) > 0)
						relFirstToIndices[firstRel].push_back(relType.size());
					else {
						vector<int> temp;
						temp.push_back(relType.size());
						relFirstToIndices.insert(pair<string, vector<int>>(firstRel, temp));
					}*/

					/*relType.push_back(type);
					relFirst.push_back(firstRel);
					relSecond.push_back(secondRel);
					relClass.push_back(-1);*/
					const RulesOfEngagement::Relation rell(type, firstRel, secondRel);
					relList.push_back(rell);
					synonymTable.setRelation(rell);

					dsSynonym.setUnion(firstRel, secondRel);
					break;
				}
			//}
		//}
	}
	//	END PARSE NORMAL RELATIONS
	//		END PARSE RELATIONS
	
	//	PARSE CONDITIONS
	for (auto it = conditionStore.begin(); it != conditionStore.end(); it++) {
		const string& synonym = (aliasMap.count(it->first) > 0) ? aliasMap[it->first] : it->first;
		const unordered_map<string, string>& itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			const string& condition = it2->first;
			const string& attribute = it2->second;
			
			//actual setting up of conditions
			bool passed = synonymTable.setSpecificAttribute(synonym, condition, attribute);
			if (!passed) {
				earlyQuit = true;
				return;
			}
		}
	}

	for (auto it = conditionsList.begin(); it != conditionsList.end(); it++) {
		synonymTable.setCondition(*it);
	}
	//	END PARSE CONDITIONS

	//	ONE LAST OPTIMISATION
	//synonymTable.alias();
	//	END ONE LAST OPTIMISATION

	const vector<unordered_set<string>>& components = dsSynonym.getComponents();
	for (size_t classIndex = 0; classIndex < components.size(); classIndex++)
		for (auto it = components[classIndex].begin(); it != components[classIndex].end(); it++)
			synonymTable.putIntoClass(aliasMap.count(*it) > 0 ? aliasMap[*it] : *it, classIndex);
	
	/*for (auto it = relFirstToIndices.begin(); it != relFirstToIndices.end(); it++)
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			relClass[*it2] = synonymTable.getClass((*it).first);*/

	vector<vector<RulesOfEngagement::Relation>>
		relPartitioned(components.size(), vector<RulesOfEngagement::Relation>());
	vector<vector<RulesOfEngagement::Condition>>
		condPartitioned(components.size(), vector<RulesOfEngagement::Condition>());

	for (auto it = relList.begin(); it != relList.end(); it++)
		relPartitioned[synonymTable.getClass(it->firstSynonym)].push_back(*it);
	for (auto it = conditionsList.begin(); it != conditionsList.end(); it++)
		condPartitioned[synonymTable.getClass(it->firstRel)].push_back(*it);

	const vector<string>& synonyms = synonymTable.getAllNames();
	vector<pair<int, string>> selecteds;
	for (auto it = synonyms.begin(); it != synonyms.end(); it++) {
		if (synonymTable.getClass(*it) < 0) {
			if (synonymTable.isSelected(*it)) {
				selecteds.push_back(pair<int, string>(selecteds.size(), *it));
			} else {
				AnswerTable table = AnswerTable(synonymTable, *it);
				if (table.getSize() == 0) {
					earlyQuit = true;
					return;
				}
				table.finishHimOff();
				if (table.getSize() == 0) {
					earlyQuit = true;
					return;
				}
			}
		}
	}
	
	vector<AnswerTable> projections(components.size() + selecteds.size());
	vector<vector<string>> toProjectIndex(components.size());
	const vector<string>& selectedsVector = synonymTable.getAllSelected();

	//project out selected variables
	for (auto it = selectedsVector.begin(); it != selectedsVector.end(); ++it)
		if (synonymTable.getClass(*it) >= 0)
			toProjectIndex[synonymTable.getClass(*it)].push_back(*it);

	using namespace Concurrency;
	structured_task_group tasks;

	auto for_each_partition = [&](const int classIndex) {
	//for (size_t classIndex = 0; classIndex < components.size(); classIndex++) {
		vector<AnswerTable> tables;
		const vector<RulesOfEngagement::Relation>& rels = relPartitioned[classIndex];
		const vector<RulesOfEngagement::Condition>& conds = condPartitioned[classIndex];
		unordered_map<string, int> inWhichTable;

		//evaluate equality of attributes
		for (unsigned i = 0; i < conds.size(); i++) {
			const RulesOfEngagement::Condition& condition = conds[i];
			const string& firstRel = condition.firstRel;
			const string& firstCondition = condition.firstCondition;
			const string& secondRel = condition.secondRel;
			const string& secondCondition = condition.secondCondition;

			int matchNumberOfTables = 0;
			if (inWhichTable.count(firstRel) > 0)
				matchNumberOfTables++;
			if (inWhichTable.count(secondRel) > 0)
				matchNumberOfTables++;

			switch (matchNumberOfTables) {
			case 0:
				{
					AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
					if (secondRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					firstRelTable.withCombine(firstRel, firstCondition,
						secondRelTable, secondRel, secondCondition);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneCondition(condition);
					if (!synonymTable.stillActive(firstRel))
						firstRelTable.projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						firstRelTable.projectAway(secondRel);

					inWhichTable[firstRel] = tables.size();
					inWhichTable[secondRel] = tables.size();
					tables.push_back(firstRelTable);
				}
				break;
			case 1:
				if (inWhichTable.count(firstRel) > 0) {
					int firstRelIndex = inWhichTable[firstRel];

					AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
					if (secondRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					tables[firstRelIndex].withCombine(firstRel, firstCondition,
						secondRelTable, secondRel, secondCondition);
					if (tables[firstRelIndex].getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneCondition(condition);
					if (!synonymTable.stillActive(firstRel))
						tables[firstRelIndex].projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						tables[firstRelIndex].projectAway(secondRel);

					inWhichTable[secondRel] = firstRelIndex;
				} else {
					AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					int secondRelIndex = inWhichTable[secondRel];
					firstRelTable.withCombine(firstRel, firstCondition,
						tables[secondRelIndex], secondRel, secondCondition);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneCondition(condition);
					if (!synonymTable.stillActive(firstRel))
						firstRelTable.projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						firstRelTable.projectAway(secondRel);

					tables[secondRelIndex] = firstRelTable;
					inWhichTable[firstRel] = secondRelIndex;
				}
				break;
			case 2:
				int firstRelIndex = inWhichTable[firstRel];
				int secondRelIndex = inWhichTable[secondRel];
				if (firstRelIndex == secondRelIndex) {
					tables[firstRelIndex].withPrune(firstRel,
						firstCondition, secondRel, secondCondition);
					if (tables[firstRelIndex].getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneCondition(condition);
					if (!synonymTable.stillActive(firstRel))
						tables[firstRelIndex].projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						tables[firstRelIndex].projectAway(secondRel);
				} else {
					tables[firstRelIndex].withCombine(firstRel, firstCondition,
						tables[secondRelIndex], secondRel, secondCondition);
					if (tables[firstRelIndex].getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneCondition(condition);
					if (!synonymTable.stillActive(firstRel))
						tables[firstRelIndex].projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						tables[firstRelIndex].projectAway(secondRel);

					tables.erase(tables.begin() + secondRelIndex);

					for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
						if ((*it).second == secondRelIndex)
							(*it).second = firstRelIndex;
				}
			}
		}
	
		for (size_t rel = 0; rel < rels.size(); rel++) {
			const RulesOfEngagement::Relation& relation = rels[rel];
			const RulesOfEngagement::QueryRelations& type = relation.type;
			const string& firstRel = relation.firstSynonym;
			const string& secondRel = relation.secondSynonym;

			int matchNumberOfTables = 0;
			if (inWhichTable.count(firstRel) > 0)
				matchNumberOfTables++;
			if (inWhichTable.count(secondRel) > 0)
				matchNumberOfTables++;

			switch (matchNumberOfTables) {
			case 0:
				{
					AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
					if (secondRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					firstRelTable.combine(firstRel, secondRelTable, secondRel, type);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneRelation(relation);
					if (!synonymTable.stillActive(firstRel))
						firstRelTable.projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						firstRelTable.projectAway(secondRel);

					inWhichTable[firstRel] = tables.size();
					inWhichTable[secondRel] = tables.size();
					tables.push_back(firstRelTable);
				}
				break;
			case 1:
				if (inWhichTable.count(firstRel) > 0) {
					int firstRelIndex = inWhichTable[firstRel];

					AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
					if (secondRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					tables[firstRelIndex].combine(firstRel, secondRelTable, secondRel, type);
					if (tables[firstRelIndex].getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneRelation(relation);
					if (!synonymTable.stillActive(firstRel))
						tables[firstRelIndex].projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						tables[firstRelIndex].projectAway(secondRel);

					inWhichTable[secondRel] = firstRelIndex;
				} else {
					AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					int secondRelIndex = inWhichTable[secondRel];
					firstRelTable.combine(firstRel, tables[secondRelIndex], secondRel, type);
					if (firstRelTable.getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneRelation(relation);
					if (!synonymTable.stillActive(firstRel))
						firstRelTable.projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						firstRelTable.projectAway(secondRel);

					tables[secondRelIndex] = firstRelTable;
					inWhichTable[firstRel] = secondRelIndex;
				}
				break;
			case 2:
				int firstRelIndex = inWhichTable[firstRel];
				int secondRelIndex = inWhichTable[secondRel];
				if (firstRelIndex == secondRelIndex) {
					tables[firstRelIndex].prune(firstRel, secondRel, type);
					if (tables[firstRelIndex].getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneRelation(relation);
					if (!synonymTable.stillActive(firstRel))
						tables[firstRelIndex].projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						tables[firstRelIndex].projectAway(secondRel);
				} else {
					tables[firstRelIndex].combine(firstRel, tables[secondRelIndex], secondRel, type);
					if (tables[firstRelIndex].getSize() == 0)
						tasks.cancel();//{ earlyQuit = true; return; }

					synonymTable.doneRelation(relation);
					if (!synonymTable.stillActive(firstRel))
						tables[firstRelIndex].projectAway(firstRel);
					if (!synonymTable.stillActive(secondRel))
						tables[firstRelIndex].projectAway(secondRel);

					for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
						if ((*it).second == secondRelIndex)
							(*it).second = firstRelIndex;
					
					tables.erase(tables.begin() + secondRelIndex);
					for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
						if ((*it).second > secondRelIndex)
							(*it).second--;
				}
			}
		}

		size_t tablesSize = tables.size();

		if (tablesSize > 1)
			throw SPAException("Wrong number of tables");

		if (tablesSize == 1) {
			AnswerTable& table = tables[0];
			//finish off all the patterns now
			table.finishHimOff();
			if (table.getSize() == 0)
				tasks.cancel();

			if (!toProjectIndex[classIndex].empty()) {
				if (toProjectIndex[classIndex].size() == table.getHeaderSize())
					projections[classIndex] = table;
				else
					projections[classIndex] = table.project(toProjectIndex[classIndex]);
			}
		}
	};

	auto task = make_task([&]() {
		parallel_for(::size_t(0), components.size(), for_each_partition);
	});

	if (tasks.run_and_wait(task) == canceled) {
		earlyQuit = true;
		return;
	}
	
	parallel_for_each(selecteds.begin(), selecteds.end(), [&](const pair<int, string> synonym) {
		AnswerTable table = AnswerTable(synonymTable, synonym.second);
		if (table.getSize() == 0) {
			earlyQuit = true;
			return;
		}
		table.finishHimOff();
		if (table.getSize() == 0) {
			earlyQuit = true;
			return;
		}
		projections[components.size() + synonym.first] = table;
	});

	if (earlyQuit)
		return;

	if (selectBOOLEAN) {
		results.push_back("true");
		return;
	}

	vector<size_t> toConsider;
	vector<string> header;
	for (size_t classIndex = 0; classIndex < projections.size(); classIndex++)
		if (!projections[classIndex].isEmpty()) {
			toConsider.push_back(classIndex);
			const vector<string>& header2 = projections[classIndex].getHeader();
			header.insert(header.end(), header2.begin(), header2.end()); 
		}
	vector<pair<int, string>> orderOfSelection;
	for (auto it = selects.begin(); it != selects.end(); it++) {
		const string& target = (aliasMap.count(it->first) > 0) ? aliasMap[it->first] : it->first;
		for (size_t i = 0; i < header.size(); i++)
			if (target == header[i]) {
				orderOfSelection.push_back(pair<int, string>(i, it->second));
				break;
			}
	}
	
	stack<vector<int>> partialAnswers;
	stack<size_t> partialLocation;
	partialAnswers.push(vector<int>());
	partialLocation.push(0);
	size_t position = 0;
	while (!partialAnswers.empty()) {
		const vector<int>& ans = partialAnswers.top();
		const size_t& pos = partialLocation.top();
		partialLocation.pop();
		const AnswerTable& table = projections[toConsider[position]];
		if (position == toConsider.size() - 1) {
			for (size_t i = 0, maxSize = table.getSize(); i < maxSize; ++i) {
				vector<int> newans(ans.begin(), ans.end());
				const vector<pair<int, unordered_set<ASTNode*>>>& row = table.getRow(i);
				for (auto it = row.begin(); it != row.end(); it++)
					newans.push_back(it->first);
				string output = "";
				auto it = orderOfSelection.begin();
				while (true) {
					output += RulesOfEngagement::convertIntegerToArgument(
						synonymTable.getType(header[it->first]), it->second, newans[it->first]);
					it++;
					if (it != orderOfSelection.end())
						output += " ";
					else
						break;
				}
				results.push_back(output);
			}
			partialAnswers.pop();
			--position;
		} else {
			if (pos >= table.getSize()) {
				partialAnswers.pop();
				--position;
			} else {
				vector<int> newans(ans.begin(), ans.end());
				const vector<pair<int, unordered_set<ASTNode*>>>& row = table.getRow(pos);
				for (auto it = row.begin(); it != row.end(); it++)
					newans.push_back(it->first);
				partialAnswers.push(newans);
				partialLocation.push(pos + 1);
				partialLocation.push(0);
				++position;
			}
		}
	}
}