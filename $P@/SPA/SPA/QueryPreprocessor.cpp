#pragma once
#include "QueryPreprocessor.h"
#include "Helper.h"


/**
* Validates the query is in line with the PQL grammar. Throw an exception if it does not.
* @param query query string
*/
void QueryPreprocessor::validate(const string& query)
{
	//parse the query statement
	int pos = 0;

	//parse synonym declaration
	unordered_map<string, RulesOfEngagement::QueryVar> stringToQueryVar;
	while (true) {
		string token = QueryPreprocessor::getToken(query, pos);
		if (token == "Select")
			break;
		if (RulesOfEngagement::tokenToVar.count(token) == 0)
			throw new SPAException("Error in parsing query - Unrecognised synonym declaration");
		RulesOfEngagement::QueryVar type = RulesOfEngagement::tokenToVar[token];
		
		string variable;
		do {
			variable = QueryPreprocessor::getToken(query, pos);
			if (stringToQueryVar.count(variable) > 0)
				throw new SPAException("Double declaration of synonym - " + variable);
			stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>(variable, type));
		} while ((variable = getToken(query, pos)) == ",");
		if (variable != ";")
			throw new SPAException("Error in parsing query");
	}

	//parse selected variables
	string selected = QueryPreprocessor::getToken(query, pos);
	if (selected.at(0) == '<') { //tuple -> multiple selected variables
		do {
			selected = QueryPreprocessor::getToken(query, pos);
			if (stringToQueryVar.count(selected) == 0)
				throw new SPAException("Selected variable is not recognised");
			selected = QueryPreprocessor::getToken(query, pos);
			if (selected == ">")
				break;
			else if (selected != ",")
				throw new SPAException("Error in parsing query");
		} while (true);
	} else if (selected != "BOOLEAN") {
		if (stringToQueryVar.count(selected) == 0)
			throw new SPAException("Selected variable is not recognised");
	}

	//parse relationships, conditions and pattern

	enum Clauses { Undefined, Such_That, With, Pattern };
	Clauses clauseType = Undefined;
	while (true) {
		string clause = QueryPreprocessor::getToken(query, pos);
		if (clause == "") {
			break;
		} else if (clause == "such") {
			QueryPreprocessor::matchToken(query, pos, "that");
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
				string relation = QueryPreprocessor::getToken(query, pos);
				QueryPreprocessor::matchToken(query, pos, "(");
				string firstRel = QueryPreprocessor::getToken(query, pos);
				QueryPreprocessor::matchToken(query, pos, ",");			
				string secondRel = QueryPreprocessor::getToken(query, pos);
				QueryPreprocessor::matchToken(query, pos, ")");

				if (RulesOfEngagement::tokenToRel.count(relation) == 0)
					throw new SPAException("Unrecognised relationship " + relation);
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
			}
			break;

		case With:
			{
				//REF = REF Change this With Case to accomodate 10 = 10, "abc" = "abc"
				//etc
				string synonym = QueryPreprocessor::getToken(query, pos);
				if (stringToQueryVar.count(synonym) == 0) {
				QueryPreprocessor::matchToken(query, pos, "=");
				string token = QueryPreprocessor::getToken(query, pos);
				if (stringToQueryVar.count(token) == 0) {
					if (token == synonym)
						break;
				} else {
				RulesOfEngagement::QueryVar type = stringToQueryVar[token];
				string condition;
				RulesOfEngagement::QueryVar RHSType;
				if (type == RulesOfEngagement::Prog_Line) {
					condition = "";
					RHSType = RulesOfEngagement::Integer;
				} else {
					QueryPreprocessor::matchToken(query, pos, ".");
					condition = QueryPreprocessor::getToken(query, pos);
					if (RulesOfEngagement::allowableConditions[type].count(condition) == 0)
						throw new SPAException(token +
						" does not have the " + condition + " condition");
					RHSType = RulesOfEngagement::conditionTypes[condition];
				}
					if (RHSType == RulesOfEngagement::Integer) {
						if (!Helper::isNumber(synonym))
							throw new SPAException("Unable to parse with");
					} else if (RHSType == RulesOfEngagement::String) {
						if (synonym.at(0) != '"' || synonym.at(synonym.length() - 1) != '"')
							throw new SPAException("Unable to parse with");
					} else
						throw new SPAException("Unable to parse with");
				}
				} else {
				RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];
				string condition;
				RulesOfEngagement::QueryVar LHSType;
				if (type == RulesOfEngagement::Prog_Line) {
					condition = "";
					LHSType = RulesOfEngagement::Integer;
				} else {
					QueryPreprocessor::matchToken(query, pos, ".");
					condition = QueryPreprocessor::getToken(query, pos);
					if (RulesOfEngagement::allowableConditions[type].count(condition) == 0)
						throw new SPAException(synonym +
						" does not have the " + condition + " condition");
					LHSType = RulesOfEngagement::conditionTypes[condition];
				}

				QueryPreprocessor::matchToken(query, pos, "=");
				//either c.value = 10 OR c.value = s.stmt#
				string token = QueryPreprocessor::getToken(query, pos);
				if (stringToQueryVar.count(token) == 0) { //c.value = 10
					if (LHSType == RulesOfEngagement::Integer) {
						if (!Helper::isNumber(token))
							throw new SPAException("Unable to parse with");
					} else if (LHSType == RulesOfEngagement::String) {
						if (token.at(0) != '"' || token.at(token.length() - 1) != '"')
							throw new SPAException("Unable to parse with");
					} else
						throw new SPAException("Unable to parse with");
				} else { //c.value = s.stmt# //c.value = s.stmt# OR c.value = n
					RulesOfEngagement::QueryVar type2 = stringToQueryVar[token];
					string condition2;
					if (type2 == RulesOfEngagement::Prog_Line) {
						condition2 = "";
					} else {
						QueryPreprocessor::matchToken(query, pos, ".");
						condition2 = QueryPreprocessor::getToken(query, pos);
					}

					if (synonym == token && condition == condition2)
						break;

					if (RulesOfEngagement::allowableConditions[type2].count(condition2) == 0)
						throw new SPAException(token +
						" does not have the " + condition2 + " condition");
					RulesOfEngagement::QueryVar RHSType =
						RulesOfEngagement::conditionTypes[condition2];
					if (LHSType != RHSType)
						throw new SPAException("Left and right hand side of with are not of same type");
				}
				}
			}
			break;

		case Pattern:
			{
				string synonym = QueryPreprocessor::getToken(query, pos);
				if (stringToQueryVar.count(synonym) == 0)
					throw new SPAException("The variable " + synonym + " is not recognised");
				RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];

				QueryPreprocessor::matchToken(query, pos, "(");
				string firstRel = QueryPreprocessor::getToken(query, pos);

				switch (type) {
				case RulesOfEngagement::Assign:
					{
						if (firstRel != "_") {
							if (stringToQueryVar.count(firstRel) > 0) {
								if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable)
									throw new SPAException("The first argument of pattern must be a variable");
							}
							else if (firstRel.at(0) != '\"' || firstRel.at(firstRel.length() - 1) != '\"')
								throw new SPAException("Could not parse the first argument");
						}
						
						QueryPreprocessor::matchToken(query, pos, ",");
						string secondRel = QueryPreprocessor::getToken2(query, pos);
						QueryPreprocessor::matchToken(query, pos, ")");

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
						}
					}
					break;

				case RulesOfEngagement::If:
					QueryPreprocessor::matchToken(query, pos, ",");
					QueryPreprocessor::matchToken(query, pos, "_");
					QueryPreprocessor::matchToken(query, pos, ",");
					QueryPreprocessor::matchToken(query, pos, "_");
					QueryPreprocessor::matchToken(query, pos, ")");

					if (firstRel != "_") {
						if (stringToQueryVar.count(firstRel) > 0) {
							if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable)
								throw new SPAException("The first argument of pattern must be a variable");
						} else if (firstRel.at(0) != '\"' || firstRel.at(firstRel.length() - 1) != '\"')
							throw new SPAException("Could not parse the first argument");
					}
					break;

				case RulesOfEngagement::While:
					QueryPreprocessor::matchToken(query, pos, ",");
					QueryPreprocessor::matchToken(query, pos, "_");
					QueryPreprocessor::matchToken(query, pos, ")");

					if (firstRel != "_") {
						if (stringToQueryVar.count(firstRel) > 0) {
							if (stringToQueryVar[firstRel] != RulesOfEngagement::Variable)
								throw new SPAException("The first argument of pattern must be a variable");
						} else if (firstRel.at(0) != '\"' || firstRel.at(firstRel.length() - 1) != '\"')
							throw new SPAException("Could not parse the first argument");
					}
					break;

				default:
					throw new SPAException("Pattern only valid on assign, if and while synonyms");
				} //end switch of synonym type in pattern
			} //end case of pattern
		} //end switch of clauseType
	}
	//end of query validation
}


/**
* Gets a token, defined as
* 1) LETTER (LETTER | DIGIT | #)*
* 1a) RelationStar
* 1b) prog_line
* 2) DIGIT*
* 3) Entire string enclosed on both sides by the quote character
* 4) A single character
* A token enclosed by the underscore character is obtained using getToken2()
* @param query query string
* @param pos position read until
* @return a token
*/
string QueryPreprocessor::getToken(const string& query, int& pos)
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";
	pos = first + 1;
	if ((query.at(first) >= 'A' && query.at(first) <= 'Z') ||
		(query.at(first) >= 'a' && query.at(first) <= 'z')) { //IDENT
			while (pos < query.length() && ((query.at(pos) >= 'A' && query.at(pos) <= 'Z') ||
				(query.at(pos) >= 'a' && query.at(pos) <= 'z') || //letter or
				(query.at(pos) >= '0' && query.at(pos) <= '9') || query.at(pos) == '#' || //digit
				(query.at(pos) == '*') || //relation star
				(query.at(pos) == '_' && pos == first + 4 && query.substr(first, 4) == "prog")))
				pos++;
	} else if (query.at(first) >= '0' && query.at(first) <= '9') { //DIGIT
		while (pos < query.length() && (query.at(pos) >= '0' && query.at(pos) <= '9'))
			pos++;
	} else if (query.at(first) == '\"')
		pos = query.find_first_of('\"', pos) + 1;
	//else return single character
	return query.substr(first, pos - first);
}


/**
* Gets a token specially for use in the second argument of an assign pattern.
* @param query query string
* @param pos position read until
* @return a token
*/
string QueryPreprocessor::getToken2(string query, int& pos)
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

/**
* Checks that the next token is equal to the required string.
* @param query query string
* @param pos position read until
* @param match string for token to match with
* @return true if the token matched the required string, and false otherwise
* @throws SPAException when token does not match
*/
void QueryPreprocessor::matchToken(const string& query, int& pos, const string& match)
{
	if (QueryPreprocessor::getToken(query, pos) != match)
		throw new SPAException("Error in parsing query");
}