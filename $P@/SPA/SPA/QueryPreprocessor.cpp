#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"
#include "QueryPreprocessor.h"
#include <set>

//FOR TESTING PURPOSES COS I SEE UNITTESTER NO UP
int main(int argc, char* arg[])
{
	QueryParser QP;
	//vector<string> tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a1 such that uses(10, \"x\")");
	vector<string> tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a1 such that calls(\"second\", 20)");
	//vector<string> tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a1 such that modifies(a1, a2)");
	QueryPreprocessor QE;
	QE.preProcess(tokens);
	vector<string> test;
	vector<unordered_map<int, string>> test2;
	unordered_map<int, vector<string>> UT = QE.getUserVariables();
	unordered_map<int, vector<string>> SV = QE.getSelectVariables();
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> r = 
		QE.getRelationships();
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> c = 
		QE.getConditions();
	cout<<"user var: ";
	for (auto it = UT.begin(); it != UT.end(); it++)
	{
		test = (*it).second;
		for (int i = 0; i < test.size(); i++)
			cout<<test.at(i)<<" ";
		cout<<", ";
	}

	cout<<endl;

	cout<<"select var: ";
	for (auto it = SV.begin(); it != SV.end(); it++)
	{
		test = (*it).second;
		for (int i = 0; i < test.size(); i++)
			cout<<test.at(i)<<" ";
		cout<<", ";
	}

	cout<<endl;

	cout<<"relationships: "<<endl;;
	for (auto it = r.begin(); it != r.end(); it++)
	{
		cout<<(*it).first<<": ";
		for (int x = 0; x < (*it).second.size(); x++)
		{
			cout<<"(";
			cout<<(*it).second.at(x).first.first<<","<<(*it).second.at(x).first.second<<" ";
			cout<<(*it).second.at(x).second.first<<","<<(*it).second.at(x).second.second;
			cout<<")";
		}
	}
	cout<<endl;
	cout<<"conditions: ";
	for (auto it = c.begin(); it != c.end(); it++)
	{
		cout<<(*it).first<<": ";
		for (int y = 0; y < (*it).second.size(); y++)
		{
			cout<<"(";
			cout<<(*it).second.at(y).first.first<<","<<(*it).second.at(y).first.second<<" ";
			cout<<(*it).second.at(y).second.first<<","<<(*it).second.at(y).second.second;
			cout<<")";
		}
	}
	cout<<endl;

	system("PAUSE");

	return 0;
}

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
	keywords.insert("select");
	keywords.insert("and");
	keywords.insert("with");
	keywords.insert("such");
	keywords.insert("that");

	variableQueryEnums.insert("assignment");
	variableQueryEnums.insert("variable");
	variableQueryEnums.insert("statement");
	variableQueryEnums.insert("procedure");
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
	bool openBracket = false, firstVariable = false, comma = false, secondVariable = false; //relationship validation
	bool conditionVariable = false, dot = false, attribute = false, equals = false; //condition declaration
	bool existsVariable = false; //check for existing variable name in relationship/condition in user variables 

	for (int currentIndex = 0; currentIndex < tokens.size(); currentIndex++)
	{
		currentToken = tokens.at(currentIndex);
		if (variableQueryEnums.find(currentToken) != variableQueryEnums.end() || currentToken.compare("select") == 0)
		{
			if (currentToken.compare("assignment") == 0)
				variableType = QueryEnums::Assign;
			else if (currentToken.compare("variable") == 0)
				variableType =QueryEnums:: Variable;
			else if (currentToken.compare("statement") == 0)
				variableType = QueryEnums::Stmt;
			else if (currentToken.compare("procedure") == 0)
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
			else if (isName(currentToken) && selectVariableDeclaration)
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
				selectVariables[variableType].push_back(currentToken);
			}
			else if (isName(currentToken))
				userVariables[variableType].push_back(currentToken);
			else
				throw SPAException("Invalid variable name");
		}
		else if (suchThat == true)
		{
			if (currentToken.compare("that") != 0)
				throw SPAException("that should follow after such");
			else
			{
				suchThat = false;
				relationship = true;
			}
		}
		else if (relationship == true)
		{
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
				else
					throw SPAException("Invalid relationship type");
			}
			else if (currentToken.compare("(") == 0)
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
				else if(isNumber(currentToken))
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
				else if(isNumber(currentToken))
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
