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
	variableQueryEnums.insert("Assignment");
	variableQueryEnums.insert("assign");
	variableQueryEnums.insert("Assign");
	variableQueryEnums.insert("variable");
	variableQueryEnums.insert("Variable");
	variableQueryEnums.insert("var");
	variableQueryEnums.insert("Var");
	variableQueryEnums.insert("statement");
	variableQueryEnums.insert("Statement");
	variableQueryEnums.insert("stmt");
	variableQueryEnums.insert("Stmt");
	variableQueryEnums.insert("procedure");
	variableQueryEnums.insert("Procedure");
	variableQueryEnums.insert("proc");
	variableQueryEnums.insert("Proc");
	variableQueryEnums.insert("call");
	variableQueryEnums.insert("Call");
	variableQueryEnums.insert("while");
	variableQueryEnums.insert("While");
	variableQueryEnums.insert("ifstatement");
	variableQueryEnums.insert("Ifstatement");
	variableQueryEnums.insert("constant");
	variableQueryEnums.insert("Constant");
	variableQueryEnums.insert("prog_line");
	variableQueryEnums.insert("Prog_line");

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

	delimiters = ";,() \"'";
	variableDeclaration = false;
	selectVariableDeclaration = false;
	suchThat = false; 
	relationship = false; 
	condition = false;
	complete = true;
	pattern = false;
	patternVariable = false;
	patternFirst = false;
	patternSecond = false;
	checkCapitalLetter = false;
	relationshipDef = false;
	openBracket = false;
	firstVariable = false;
	comma = false;
	secondVariable = false;
	conditionVariable = false;
	dot = false;
	attribute = false;
	equals = false; 
	existsVariable = false;
}

void QueryPreprocessor::preProcess(vector<string> tokens)
{
	resetAll();

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

		if (variableQueryEnums.find(currentToken) != variableQueryEnums.end() || currentToken.compare("Select") == 0)
		{
			if (currentToken.compare("assignment") == 0 || currentToken.compare("assign") == 0 ||
				currentToken.compare("Assignment") == 0 || currentToken.compare("Assign") == 0)
				variableType = QueryEnums::Assign;
			else if (currentToken.compare("variable") == 0 || currentToken.compare("var") == 0 ||
					currentToken.compare("Variable") == 0 || currentToken.compare("Var") == 0)
				variableType =QueryEnums:: Variable;
			else if (currentToken.compare("statement") == 0 || currentToken.compare("stmt") == 0 ||
				currentToken.compare("Statement") == 0 || currentToken.compare("Stmt") == 0 	||
				currentToken.compare("prog_line") == 0 || currentToken.compare("Prog_line") == 0 )
				variableType = QueryEnums::Stmt;
			else if (currentToken.compare("procedure") == 0 || currentToken.compare("proc") == 0 ||
				currentToken.compare("Procedure") == 0 || currentToken.compare("Proc") == 0)
				variableType = QueryEnums::Procedure;
			else if (currentToken.compare("call") == 0 || currentToken.compare("Call") == 0)
				variableType = QueryEnums::Call;
			else if (currentToken.compare("while") == 0 || currentToken.compare("While") == 0)
				variableType = QueryEnums::While;
			else if (currentToken.compare("ifstatement") == 0 || currentToken.compare("Ifstatement") == 0)
				variableType = QueryEnums::If;
			else if (currentToken.compare("constant") == 0 || currentToken.compare("Constant") == 0)
				variableType = QueryEnums::Constant;
			else if (currentToken.compare("Select") == 0) //Select must be capital s, the rest dont matter
				selectVariableDeclaration = true;
			else
				throw SPAException("Invalid data type declaration");

			variableDeclaration = true;
			checkCapitalLetter = false;
			complete = false;
		}
		else if (variableDeclaration == true || selectVariableDeclaration == true)
		{
			if (currentToken.compare(";") == 0) //end of variable declaration
			{
				variableDeclaration = false;
				checkCapitalLetter = true;
			}
			else if (currentToken.compare("such") == 0)
			{
					suchThat = true;
					variableDeclaration = false;
					selectVariableDeclaration = false;
					complete = false;
			}
			else if (currentToken.compare("pattern") == 0)
			{
				pattern = true;
				variableDeclaration = false;
				selectVariableDeclaration = false;
				complete = false;
			}
			else if (currentToken.compare("with") == 0)
			{
				condition = true;
				variableDeclaration = false;
				selectVariableDeclaration = false;
				complete = false;
			}
			else if (currentToken.compare(",") == 0) //there are more variables of the same type to store
				complete = false; //commas means you expect something more
			else if (selectVariableDeclaration == true)
			{
				//ASSUMING 1 SELECT VARIABLE
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
					existsVariable = false;
					selectVariableDeclaration = false;
					complete = true;
				}
				else if (currentToken.compare("boolean") == 0)
				{
					selectVariables[QueryEnums::Boolean].push_back(currentToken);
					//More select variable declarations should not be allowed after boolean
					selectVariableDeclaration = false;
					complete = true;
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
						reladitionType = QueryEnums::Modifies;
					else if (currentToken.compare("uses") == 0)
						reladitionType = QueryEnums::Uses;
					else if (currentToken.compare("calls") == 0)
						reladitionType = QueryEnums::Calls;
					else if (currentToken.compare("calls*") == 0)
						reladitionType = QueryEnums::CallsStar;
					else if (currentToken.compare("follows") == 0)
						reladitionType = QueryEnums::Follows;
					else if (currentToken.compare("follows*") == 0)
						reladitionType = QueryEnums::FollowsStar;
					else if (currentToken.compare("parent") == 0)
						reladitionType = QueryEnums::Parent;
					else if (currentToken.compare("parent*") == 0)
						reladitionType = QueryEnums::ParentStar;
					else if (currentToken.compare("next") == 0)
						reladitionType = QueryEnums::Next;
					else if (currentToken.compare("next*") == 0)
						reladitionType = QueryEnums::NextStar;
					else if (currentToken.compare("affects") == 0)
						reladitionType = QueryEnums::Affects;
					else if (currentToken.compare("affect*") == 0)
						reladitionType = QueryEnums::AffectsStar;
					else
						throw SPAException("Invalid relationship type");

					relationshipDef = true;
				}
			}
			else if (relationshipDef = true && currentToken.compare("(") == 0)
				openBracket = true;
			else if (openBracket == true && firstVariable == false && comma == false)
			{
				if (currentToken.compare("_") == 0)
				{
					if (reladitionType == QueryEnums::Modifies || reladitionType == QueryEnums::Uses)
						throw SPAException("Modifies and Uses cannot have '_' as the first parameter");
					else
						relationshipDeclaration.first = (make_pair(QueryEnums::WildCard, currentToken));
				}
				else if ((reladitionType == QueryEnums::Modifies || reladitionType == QueryEnums::Uses ||
						reladitionType == QueryEnums::Calls || reladitionType == QueryEnums::CallsStar) &&
						currentToken.front() == '\"' && 
						currentToken.at(currentToken.size() - 1) == '\"')
				{
					if((currentToken.size() > 2 && delimiters.find(currentToken.substr(1, currentToken.size() - 2)) != 0) ||
						(currentToken.size() == 3 && isName(Helper::charToString(currentToken.at(1)))))
						relationshipDeclaration.first = (make_pair(QueryEnums::Procedure, currentToken));
				}
				else if(Helper::isNumber(currentToken) && reladitionType != QueryEnums::Calls && 
					reladitionType != QueryEnums::CallsStar)
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
						(reladitionType == QueryEnums::Follows || reladitionType == QueryEnums::FollowsStar ||
						reladitionType == QueryEnums::Parent || reladitionType == QueryEnums::ParentStar))
					{
						throw SPAException("Follows and Parent parameters cannot be procedures");
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
					relationshipDeclaration.second = make_pair(QueryEnums::WildCard, currentToken);
				else if ((reladitionType == QueryEnums::Calls || reladitionType == QueryEnums::CallsStar) &&
						currentToken.front() == '\"')
				{
					if((currentToken.size() > 2 && delimiters.find(currentToken.substr(1, currentToken.size() - 2)) != 0) ||
						(currentToken.size() == 3 && isName(Helper::charToString(currentToken.at(1)))))
						relationshipDeclaration.second = make_pair(QueryEnums::Procedure, currentToken);
				}
				else if ((reladitionType == QueryEnums::Modifies || reladitionType == QueryEnums::Uses) &&
						currentToken.front() == '\"' && 
						currentToken.at(currentToken.size() - 1) == '\"')
				{
					if((currentToken.size() > 2 && delimiters.find(currentToken.substr(1, currentToken.size() - 2)) != 0) ||
						(currentToken.size() == 3 && isName(Helper::charToString(currentToken.at(1)))))
						relationshipDeclaration.second = make_pair(QueryEnums::Variable, currentToken);
				}
				else if(Helper::isNumber(currentToken) && reladitionType != QueryEnums::Calls && 
						reladitionType != QueryEnums::CallsStar  && reladitionType != QueryEnums::Pattern)
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
						(reladitionType == QueryEnums::Follows || reladitionType == QueryEnums::FollowsStar ||
						reladitionType == QueryEnums::Parent || reladitionType == QueryEnums::ParentStar))
					{
						throw SPAException("Follows and Parent parameters cannot be procedures");
					}
					relationshipDeclaration.second = (make_pair(variableType, currentToken));
					existsVariable = false;
				}
				secondVariable = true;
			}
			else if (currentToken.compare(")") == 0) // reset all flags
			{
				relationships[reladitionType].push_back(relationshipDeclaration);
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
				else if (Helper::isNumber(currentToken))
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
		else if (pattern)
		{
			if (!patternVariable)
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
						throw SPAException("Variable in condition was not found in query");
					patternDeclaration.first = make_pair(variableType, currentToken);
					patternVariable = true;
					existsVariable = false;
				}
			}
			else if (patternVariable && currentToken.compare("(") == 0)
				openBracket = true;
			else if (openBracket && !patternFirst)
			{
				if (currentToken.compare("_") == 0)
				{
					patternDeclaration.second.first = make_pair(QueryEnums::WildCard, currentToken);
					patternFirst = true;
				}
				else if (currentToken.front() == '\"' && currentToken.at(currentToken.size() - 1) == '\"')
				{
					if ((currentToken.size() == 3 && isName(Helper::charToString(currentToken.at(1)))) ||
						(currentToken.size() > 3 && delimiters.find(currentToken.substr(1, currentToken.size() - 2)) != 0))
					{
						patternDeclaration.second.first = make_pair(QueryEnums::Variable, currentToken);
						patternFirst = true;
					}
				}
				else if (isName(currentToken) && !patternFirst)
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
						throw SPAException("Variable in pattern was not found in query");
					patternDeclaration.second.first = make_pair(variableType, currentToken);
					patternFirst = true;
					existsVariable = false;
				}
			}
			else if (patternFirst && currentToken.compare(",") == 0)
				comma = true;
			else if (comma && !patternSecond)
			{
				if (currentToken.compare("_") == 0)
				{
					patternDeclaration.second.second = make_pair(QueryEnums::WildCard, currentToken);
					patternSecond = true;
				}
				else if(currentToken.at(0) != '_' || currentToken.at(currentToken.size() - 1) != '_' ||
					currentToken.at(1) != '\"' || currentToken.at(currentToken.size() - 2) != '\"')
					throw SPAException("Pattern must be in the form _\"variable\"_"); //for sem 1 only
				else if ((currentToken.size() == 3 && !isName(Helper::charToString(currentToken.at(1)))) ||
						!isName(currentToken.substr(1, currentToken.size() - 2)))
					throw SPAException("Invalid parameter for pattern");
				else
				{
					patternDeclaration.second.second = make_pair(QueryEnums::Expression, currentToken);
					patternSecond = true;
				}
			}
			else if (patternSecond && currentToken.compare(")") == 0)
			{
				patterns.push_back(patternDeclaration);
				pattern = false;
				patternVariable = false;
				patternFirst = false;
				patternSecond = false;
				openBracket = false;
				comma = false;
				complete = true;
			}
			else
				throw SPAException("Invalid pattern syntax");
		}
		else if (relationship == false && condition == false)
		{
			if (currentToken.compare("and") == 0)
			{
				checkCapitalLetter = true;
				relationship = true;
				complete = false;
			}
			else if (currentToken.compare("pattern") == 0)
			{
				pattern = true;
				complete = false;
			}
			else if (currentToken.compare("with") == 0)
			{
				condition = true;
				complete = false;
			}
			else if (currentToken.compare("such") == 0)
			{
				suchThat = true;
				complete = false;
			}
		}
		else
			throw SPAException("Invalid query");
	}

	if (!complete)
		throw SPAException("Incomplete query");
}

bool QueryPreprocessor::isName(string s) //first char of name cannot be digit
{
	//Check if first char of name is digit or character
	string delimiters = " ,;:.()";
	int result = s.find_first_of("!@$%^&*()_+{}|\][~`", 0);

	return (!isdigit(s.at(0)) && delimiters.find(s) != 0 && (keywords.find(s) == keywords.end()) && result == string::npos);
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
vector<pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>>
	QueryPreprocessor::getPatterns()
{
	return patterns;
}

void QueryPreprocessor::resetAll(){
	userVariables.clear();
	selectVariables.clear();
	relationships.clear();
	conditions.clear();
}

