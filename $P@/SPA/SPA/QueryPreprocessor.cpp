#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"
#include "QueryPreprocessor.h"
#include "Helper.h"
#include <set>

QueryPreprocessor::QueryPreprocessor()
{

	keywords.insert("assignment");
	keywords.insert("variable");
	keywords.insert("statement");
	keywords.insert("procedure");
	keywords.insert("call");
	keywords.insert("while");
	keywords.insert("ifstatement");
	keywords.insert("constant");
	keywords.insert("modifies");
	keywords.insert("uses");
	keywords.insert("calls");
	keywords.insert("calls*");
	keywords.insert("follows");
	keywords.insert("follows*");
	keywords.insert("parent");
	keywords.insert("parent*");
	keywords.insert("next");
	keywords.insert("next*");
	keywords.insert("affects");
	keywords.insert("affects*");
	keywords.insert("pattern");
	keywords.insert("select");
	keywords.insert("and");
	keywords.insert("with");
	keywords.insert("such");
	keywords.insert("that");
	keywords.insert("boolean");

	variableQueryEnums.insert("assignment");
	variableQueryEnums.insert("assign");
	variableQueryEnums.insert("variable");
	variableQueryEnums.insert("var");
	variableQueryEnums.insert("statement");
	variableQueryEnums.insert("stmt");
	variableQueryEnums.insert("procedure");
	variableQueryEnums.insert("proc");
	variableQueryEnums.insert("call");
	variableQueryEnums.insert("while");
	variableQueryEnums.insert("ifstatement");
	variableQueryEnums.insert("constant");
	
	relationshipQueryEnums.insert("modifies");
	relationshipQueryEnums.insert("uses");
	relationshipQueryEnums.insert("calls");
	relationshipQueryEnums.insert("calls*");
	relationshipQueryEnums.insert("follows");
	relationshipQueryEnums.insert("follows*");
	relationshipQueryEnums.insert("parent");
	relationshipQueryEnums.insert("parent*");
	relationshipQueryEnums.insert("next");
	relationshipQueryEnums.insert("next*");
	relationshipQueryEnums.insert("affects");
	relationshipQueryEnums.insert("affects*");

	conditionAttributeQueryEnums.insert("procname");
	conditionAttributeQueryEnums.insert("varname");
	conditionAttributeQueryEnums.insert("value");
	conditionAttributeQueryEnums.insert("stmt#");
}

void QueryPreprocessor::preProcess(vector<string> tokens)
{
	resetAll();

	string delimiters = ";,() \"'"; //all spaces are already taken care of
	string currentToken;
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> relationshipDeclaration, conditionDeclaration;
	vector<string> variableNames;
	QueryEnums::QueryVar variableType;
	QueryEnums::QueryRel relationshipType;
	QueryEnums::QueryCond conditionAttributeType;
	bool variableDeclaration = false, selectVariableDeclaration = false, suchThat = false; 
	bool relationship = false, condition = false;
	bool complete = false; //determines whether the query is complete
	//relationship validation
	bool checkCapitalLetter = false;
	bool relationshipDef = false, openBracket = false, firstVariable = false, comma = false, secondVariable = false;
	//condition declaration
	bool conditionVariable = false, dot = false, attribute = false, equals = false; 
	bool existsVariable = false; //check for existing variable name in relationship/condition in user variables 

	for (int currentIndex = 0; currentIndex < tokens.size(); currentIndex++)
	{
		currentToken = tokens.at(currentIndex);
		//string comparisons are all done in lower case
		if (checkCapitalLetter == false)
		{
			for (int i = 0; i < currentToken.size(); i++)
				currentToken.at(i) = tolower(currentToken.at(i));
		}
		else
		{
			//leave the first letter unchanged because its the only letter needed to be validated 
			for (int i = 1; i < currentToken.size(); i++)
				currentToken.at(i) = tolower(currentToken.at(i));
		}

		if (variableQueryEnums.find(currentToken) != variableQueryEnums.end() || currentToken.compare("select") == 0)
		{
			if (currentToken.compare("assignment") == 0 || currentToken.compare("assign") == 0)
				variableType = QueryEnums::Assign;
			else if (currentToken.compare("variable") == 0 || currentToken.compare("var") == 0)
				variableType =QueryEnums:: Variable;
			else if (currentToken.compare("statement") == 0 || currentToken.compare("stmt") == 0)
				variableType = QueryEnums::Stmt;
			else if (currentToken.compare("procedure") == 0 || currentToken.compare("proc") == 0)
				variableType = QueryEnums::Procedure;
			else if (currentToken.compare("call") == 0)
				variableType = QueryEnums::Call;
			else if (currentToken.compare("while") == 0)
				variableType = QueryEnums::While;
			else if (currentToken.compare("ifstatement") == 0)
				variableType = QueryEnums::If;
			else if (currentToken.compare("constant") == 0)
				variableType = QueryEnums::Constant;
			else if (currentToken.compare("select") == 0)
			{
				selectVariableDeclaration = true;
				continue;
			}
			else
				throw SPAException("Unknown data type declaration");

			variableDeclaration = true;
		}
		else if (variableDeclaration == true || selectVariableDeclaration == true)
		{
			if (currentToken.compare(";") == 0) //end of variable declaration
				variableDeclaration = false;
			else if (currentToken.compare("such") == 0)
			{
					suchThat = true;
					variableDeclaration = false;
					selectVariableDeclaration = false;
			}
			else if (currentToken.compare("with") == 0)
			{
				condition = true;
				variableDeclaration = false;
				selectVariableDeclaration = false;
			}
			else if (currentToken.compare(",") == 0) //there are more variables of the same type to store
			{
				complete = false; //commas means you expect something more
				continue;
			}
			else if (selectVariableDeclaration == true)
			{
				if (isName(currentToken))
				{
					for (auto it = userVariables.begin(); it != userVariables.end(); it++)
					{
						variableNames = (*it).second;
						for (int i = 0; i < variableNames.size(); i++)
						{
							if (variableNames.at(i).compare(currentToken) == 0)
							{
								variableType = QueryEnums::QueryVar((*it).first);
								existsVariable = true;
							}
						}
					}
					if (existsVariable == false)
						throw SPAException("Variable in select was not found in query");
					selectVariables[variableType].push_back(currentToken);
				}
				else if (currentToken.compare("boolean") == 0)
				{
					selectVariables[QueryEnums::Boolean].push_back(currentToken);
					selectVariableDeclaration = false;
				}
			}
			else if (isName(currentToken))
				userVariables[variableType].push_back(currentToken);
			else
				throw SPAException("Invalid variable name or boolean");
		}
		else if (suchThat == true)
		{
			if (currentToken.compare("that") != 0)
				throw SPAException("that should follow after such");
			else
			{
				suchThat = false;
				relationship = true;
				checkCapitalLetter = true;
			}
		}
		else if (relationship == true)
		{
			//else case for non capital letter is handled collectively with the rest of the invalid relationship cases
			if (isupper(currentToken.at(0))) 
			{
				currentToken.at(0) = tolower(currentToken.at(0));
				checkCapitalLetter = false;
				if (relationshipQueryEnums.find(currentToken) != relationshipQueryEnums.end())
				{
					if (currentToken.compare("modifies") == 0)
						relationshipType = QueryEnums::Modifies;
					else if (currentToken.compare("uses") == 0)
						relationshipType = QueryEnums::Uses;
					else if (currentToken.compare("calls") == 0)
						relationshipType = QueryEnums::Calls;
					else if (currentToken.compare("calls*") == 0)
						relationshipType = QueryEnums::CallsStar;
					else if (currentToken.compare("follows") == 0)
						relationshipType = QueryEnums::Follows;
					else if (currentToken.compare("follows*") == 0)
						relationshipType = QueryEnums::FollowsStar;
					else if (currentToken.compare("parent") == 0)
						relationshipType = QueryEnums::Parent;
					else if (currentToken.compare("parent*") == 0)
						relationshipType = QueryEnums::ParentStar;
					else if (currentToken.compare("next") == 0)
						relationshipType = QueryEnums::Next;
					else if (currentToken.compare("next*") == 0)
						relationshipType = QueryEnums::NextStar;
					else if (currentToken.compare("affects") == 0)
						relationshipType = QueryEnums::Affects;
					else if (currentToken.compare("affect*") == 0)
						relationshipType = QueryEnums::AffectsStar;
					else if (currentToken.compare("pattern") == 0)
						relationshipType = QueryEnums::Pattern;
					else
						throw SPAException("Invalid relationship type");

					relationshipDef = true;
					continue;
				}
			}
			else if (relationshipDef = true && currentToken.compare("(") == 0)
				openBracket = true;
			else if (openBracket == true && firstVariable == false && comma == false)
			{
				if (currentToken.compare("_") == 0)
				{
					if (relationshipType == QueryEnums::Modifies || relationshipType == QueryEnums::Uses)
						throw SPAException("Modifies and Uses cannot have '_' as the first parameter");
					else
						relationshipDeclaration.first = (make_pair(QueryEnums::WildCard, currentToken));
				}
				else if ((relationshipType == QueryEnums::Modifies || relationshipType == QueryEnums::Uses ||
						relationshipType == QueryEnums::Calls || relationshipType == QueryEnums::CallsStar) &&
						currentToken.front() == '\"' && 
						currentToken.at(currentToken.size() - 1) == '\"' &&
						currentToken.size() > 2 && 
						delimiters.find(currentToken.substr(1, currentToken.size() - 2)) != 0)
				{
						relationshipDeclaration.first = (make_pair(QueryEnums::Procedure, currentToken));
				}
				else if (relationshipType == QueryEnums::Pattern && currentToken.front() == '\"' && 
						currentToken.at(currentToken.size() - 1) == '\"' &&
						!(currentToken.size() == 3 && !isName(Helper::charToString(currentToken.at(1))) &&
						isName(currentToken.substr(1, currentToken.size() - 2))))
				{
					relationshipDeclaration.first = (make_pair(QueryEnums::Variable, currentToken));
				}
				else if(isNumber(currentToken) && relationshipType != QueryEnums::Calls && 
					relationshipType != QueryEnums::CallsStar && relationshipType != QueryEnums::Pattern)
					relationshipDeclaration.first = (make_pair(QueryEnums::Stmt, currentToken));
				else
				{
					for (auto it = userVariables.begin(); it != userVariables.end(); it++)
					{
						variableNames = (*it).second;
						for (int i = 0; i < variableNames.size(); i++)
						{
							if (variableNames.at(i).compare(currentToken) == 0)
							{
								variableType = QueryEnums::QueryVar((*it).first);
								existsVariable = true;
							}
						}
					}
					if (existsVariable == false)
						throw SPAException("Variable in relationship was not found in query");
					if (variableType == QueryEnums::Procedure && 
						(relationshipType == QueryEnums::Follows || relationshipType == QueryEnums::FollowsStar ||
						relationshipType == QueryEnums::Parent || relationshipType == QueryEnums::ParentStar))
					{
						throw SPAException("Follows and Parent parameters cannot be procedures");
					}
					if (relationshipType == QueryEnums::Pattern && variableType != QueryEnums::Variable)
					{
						throw SPAException("First parameter of pattern must be a variable type");
					}
					relationshipDeclaration.first = (make_pair(variableType, currentToken));
					existsVariable = false;
				}

				firstVariable = true;
			}
			else if (openBracket == true && firstVariable == true && currentToken.compare(",") == 0)
				comma = true;
			else if (openBracket == true && comma == true && secondVariable == false)
			{
				if (currentToken.compare("_") == 0)
					relationshipDeclaration.second = (make_pair(QueryEnums::WildCard, currentToken));
				else if ((relationshipType == QueryEnums::Calls || relationshipType == QueryEnums::CallsStar) &&
						currentToken.front() == '\"' && 
						currentToken.at(currentToken.size() - 1) == '\"' &&
						currentToken.size() > 2 && 
						delimiters.find(currentToken.substr(1, currentToken.size() - 2)) != 0)
				{
						relationshipDeclaration.second = (make_pair(QueryEnums::Procedure, currentToken));
				}
				else if ((relationshipType == QueryEnums::Modifies || relationshipType == QueryEnums::Uses) &&
						currentToken.front() == '\"' && 
						currentToken.at(currentToken.size() - 1) == '\"' &&
						!(currentToken.size() == 3 && !isName(Helper::charToString(currentToken.at(1))) &&
						isName(currentToken.substr(1, currentToken.size() - 2))))
				{
						relationshipDeclaration.second = (make_pair(QueryEnums::Variable, currentToken));
				}
				else if(isNumber(currentToken) && relationshipType != QueryEnums::Calls && 
						relationshipType != QueryEnums::CallsStar  && relationshipType != QueryEnums::Pattern)
					relationshipDeclaration.second = (make_pair(QueryEnums::Stmt, currentToken));
				else
				{
					for (auto it = userVariables.begin(); it != userVariables.end(); it++)
					{
						variableNames = (*it).second;
						for (int i = 0; i < variableNames.size(); i++)
						{
							if (variableNames.at(i).compare(currentToken) == 0)
							{
								variableType = QueryEnums::QueryVar((*it).first);
								existsVariable = true;
							}
						}
					}
					if (existsVariable == false)
						throw SPAException("Variable in relationship was not found in query");
					if (variableType == QueryEnums::Procedure && 
						(relationshipType == QueryEnums::Follows || relationshipType == QueryEnums::FollowsStar ||
						relationshipType == QueryEnums::Parent || relationshipType == QueryEnums::ParentStar))
					{
						throw SPAException("Follows and Parent parameters cannot be procedures");
					}
					if (relationshipType == QueryEnums::Pattern)
					{
						if(currentToken.at(0) != '_' || currentToken.at(currentToken.size() - 1) != '_' ||
							currentToken.at(1) != '\"' || currentToken.at(currentToken.size() - 2) != '\"')
							throw SPAException("Pattern must be in the form _\"variable\"_");
						else if ((currentToken.size() == 3 && !isName(Helper::charToString(currentToken.at(1)))) ||
								!isName(currentToken.substr(1, currentToken.size() - 2)))
							throw SPAException("Invalid parameter for pattern");
					}
					relationshipDeclaration.second = (make_pair(variableType, currentToken));
					existsVariable = false;
				}
				secondVariable = true;
			}
			else if (currentToken.compare(")") == 0) // reset all flags
			{
				relationships[relationshipType].push_back(relationshipDeclaration);
				openBracket = false;
				firstVariable = false;
				comma = false;
				secondVariable = false;
				relationship = false;
				complete = true;
				relationship = false;
			}
			else
				throw SPAException("Invalid relationship syntax");
		}
		else if (condition == true)
		{
			if (isName(currentToken) && conditionVariable == false)
			{
				for (auto it = userVariables.begin(); it != userVariables.end(); it++)
				{
					variableNames = (*it).second;
					for (int i = 0; i < variableNames.size(); i++)
					{
						if (variableNames.at(i).compare(currentToken) == 0)
						{
							variableType = QueryEnums::QueryVar((*it).first);
							existsVariable = true;
						}
					}
				}
				if (existsVariable == false)
					throw SPAException("Variable in condition was not found in query");
				conditionDeclaration.first = (make_pair(variableType, currentToken));
				conditionVariable = true;
				existsVariable = false;
			}
			else if (currentToken.compare(".") == 0 && conditionVariable == true)
				dot = true;
			else if (dot == true && conditionAttributeQueryEnums.find(currentToken) != conditionAttributeQueryEnums.end())
			{
				if (currentToken.compare("procname") == 0)
					conditionAttributeType = QueryEnums::ProcName;
				else if (currentToken.compare("varname") == 0)
					conditionAttributeType = QueryEnums::VarName;
				else if (currentToken.compare("value") == 0)
					conditionAttributeType = QueryEnums::Value;
				else if (currentToken.compare("stmt#") == 0)
					conditionAttributeType = QueryEnums::StmtNo;
				else
					throw SPAException("Invalid condition attribute");

				attribute = true;
			}
			else if (attribute == true && currentToken.compare("=") == 0)
				equals = true;
			else if (equals == true)
			{
				if (currentToken.front() == '\"' && 
					currentToken.at(currentToken.size() - 1) == '\"' &&
					currentToken.size() > 2 && 
					delimiters.find(currentToken.substr(1, currentToken.size() - 2)) != 0)
				{
					if (conditionAttributeType == QueryEnums::ProcName)
						conditionDeclaration.second = make_pair(QueryEnums::Procedure, currentToken);
					if (conditionAttributeType == QueryEnums::VarName)
						conditionDeclaration.second = make_pair(QueryEnums::Variable, currentToken);
				}
				else if (isNumber(currentToken))
				{
					if (conditionAttributeType == QueryEnums::Value)
						conditionDeclaration.second = make_pair(QueryEnums::Constant, currentToken);
					if (conditionAttributeType == QueryEnums::StmtNo)
						conditionDeclaration.second = make_pair(QueryEnums::Stmt, currentToken);
				}
				else
					throw SPAException("Condition equates to invalid type");
				conditions[conditionAttributeType].push_back(conditionDeclaration);
				conditionVariable = false;
				dot = false;
				attribute = false;
				equals = false;
				condition = false;
				complete = true;
				condition = false;
			}
			else
				throw SPAException("Invalid condition syntax");
		}
		else if (relationship == false && condition == false)
		{
			if (currentToken.compare("and") == 0)
				relationship = true;
			else if (currentToken.compare("with") == 0)
				condition = true;
			else if (currentToken.compare("such") == 0)
				suchThat = true;
		}
		else if (complete == false)
			throw SPAException("Incomplete query");
		else
			throw SPAException("Invalid query");
	}
}

bool QueryPreprocessor::isName(string s) //first char of name cannot be digit
{
	//Check if first char of name is digit or character
	int output;
	string delimiters = " ,;:.()";
	istringstream (s.at(0)) >> output;
	return (output == -858993460 && delimiters.find(s) != 0 && (keywords.find(s) == keywords.end()));
}

bool QueryPreprocessor::isNumber(string& s)
{
    auto it = s.begin();
    while (it != s.end() && isdigit(*it)) 
		it++;
    return !s.empty() && it == s.end();
}

unordered_map<int, vector<string>> QueryPreprocessor::getUserVariables()
{
	return userVariables;
}
	
unordered_map<int, vector<string>> QueryPreprocessor::getSelectVariables()
{
	return selectVariables;
}

unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> 
	QueryPreprocessor::getRelationships()
{
	return relationships;
}
	
unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> 
	QueryPreprocessor::getConditions()
{
	return conditions;
}

void QueryPreprocessor::resetAll(){
	userVariables.clear();
	selectVariables.clear();
	relationships.clear();
	conditions.clear();
}