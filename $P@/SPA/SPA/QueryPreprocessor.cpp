#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"
#include "QueryPreprocessor.h"
#include <set>

//FOR TESTING PURPOSES COS I SEE UNITTESTER NO UP
//int main(int argc, char* arg[])
//{
//	QueryParser QP;
//	vector<string> tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a1 with s1.stmt# = 5 and modifies(a1, _) with s2.procname = 'hi' and uses(a2, a1)");
//	//vector<string> tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a1 such that modifies(a1, a2)");
//	QueryPreprocessor QE;
//	QE.preProcess(tokens);
//	vector<string> test;
//	vector<unordered_map<int, string>> test2;
//	unordered_map<int, vector<string>> UT = QE.getUserVariables();
//	unordered_map<int, vector<string>> SV = QE.getSelectVariables();
//	unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>>>> r = 
//		QE.getRelationships();
//	unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, string>>> c = QE.getConditions();
//	cout<<"user var: ";
//	for (auto it = UT.begin(); it != UT.end(); it++)
//	{
//		test = (*it).second;
//		for (int i = 0; i < test.size(); i++)
//			cout<<test.at(i)<<" ";
//		cout<<", ";
//	}
//
//	cout<<endl;
//
//	cout<<"select var: ";
//	for (auto it = SV.begin(); it != SV.end(); it++)
//	{
//		test = (*it).second;
//		for (int i = 0; i < test.size(); i++)
//			cout<<test.at(i)<<" ";
//		cout<<", ";
//	}
//
//	cout<<endl;
//
//	cout<<"relationships: ";
//	for (auto it = r.begin(); it != r.end(); it++)
//	{
//		for (int x = 0; x < (*it).second.size(); x++)
//		{
//			cout<<"(";
//			cout<<(*it).second.at(x).first.first<<","<<(*it).second.at(x).first.second<<" ";
//			cout<<(*it).second.at(x).second.first<<","<<(*it).second.at(x).second.second;
//			cout<<")";
//		}
//	}
//	cout<<endl;
//	cout<<"conditions: ";
//	for (auto it = c.begin(); it != c.end(); it++)
//	{
//		for (int y = 0; y < (*it).second.size(); y++)
//		{
//			cout<<"(";
//			cout<<(*it).second.at(y).first.first<<","<<(*it).second.at(y).first.second<<" ";
//			cout<<(*it).second.at(y).second;
//			cout<<")";
//		}
//	}
//	cout<<endl;
//
//	system("PAUSE");
//
//	return 0;
//}

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

	variablePalletTown.insert("assignment");
	variablePalletTown.insert("variable");
	variablePalletTown.insert("statement");
	variablePalletTown.insert("procedure");
	variablePalletTown.insert("call");
	variablePalletTown.insert("while");
	variablePalletTown.insert("ifstatement");
	variablePalletTown.insert("constant");
	
	relationshipPalletTown.insert("modifies");
	relationshipPalletTown.insert("uses");
	relationshipPalletTown.insert("calls");
	relationshipPalletTown.insert("calls*");
	relationshipPalletTown.insert("follows");
	relationshipPalletTown.insert("follows*");
	relationshipPalletTown.insert("parent");
	relationshipPalletTown.insert("parent*");
	relationshipPalletTown.insert("next");
	relationshipPalletTown.insert("next*");
	relationshipPalletTown.insert("affects");
	relationshipPalletTown.insert("affects*");

	conditionAttributePalletTown.insert("procname");
	conditionAttributePalletTown.insert("varname");
	conditionAttributePalletTown.insert("value");
	conditionAttributePalletTown.insert("stmt#");
}

void QueryPreprocessor::preProcess(vector<string> tokens)
{
	string delimiters = ";,()"; //all spaces are already taken care of
	string currentToken;
	pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>> relationshipDeclaration;
	pair<pair<PalletTown::ashKetchum, string>, string> conditionDeclaration;
	vector<string> variableNames;
	PalletTown::ashKetchum variableType;
	PalletTown::garyOak relationshipType;
	PalletTown::mistyWaterflower conditionAttributeType;
	bool variableDeclaration = false, selectVariableDeclaration = false, suchThat = false; 
	bool relationship = false, condition = false;
	bool complete = false;
	bool openBracket = false, firstVariable = false, comma = false, secondVariable = false; //relationship validation
	bool conditionVariable = false, dot = false, attribute = false, equals = false; //condition declaration
	bool existsVariable = false; //check for existing variable name in relationship/condition in user variables 

	for (int currentIndex = 0; currentIndex < tokens.size(); currentIndex++)
	{
		currentToken = tokens.at(currentIndex);
		if (variablePalletTown.find(currentToken) != variablePalletTown.end() || currentToken.compare("select") == 0)
		{
			if (currentToken.compare("assignment") == 0)
				variableType = PalletTown::Assign;
			else if (currentToken.compare("variable") == 0)
				variableType =PalletTown:: Variable;
			else if (currentToken.compare("statement") == 0)
				variableType = PalletTown::Stmt;
			else if (currentToken.compare("procedure") == 0)
				variableType = PalletTown::Procedure;
			else if (currentToken.compare("call") == 0)
				variableType = PalletTown::Call;
			else if (currentToken.compare("while") == 0)
				variableType = PalletTown::While;
			else if (currentToken.compare("ifstatement") == 0)
				variableType = PalletTown::If;
			else if (currentToken.compare("constant") == 0)
				variableType = PalletTown::Constant;
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
				continue;
			else if (isName(currentToken) && selectVariableDeclaration)
			{
				for (auto it = userVariables.begin(); it != userVariables.end(); it++)
				{
					variableNames = (*it).second;
					for (int i = 0; i < variableNames.size(); i++)
					{
						if (variableNames.at(i).compare(currentToken) == 0)
						{
							variableType = PalletTown::ashKetchum((*it).first);
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
			if (relationshipPalletTown.find(currentToken) != relationshipPalletTown.end())
			{
				if (currentToken.compare("modifies") == 0)
					relationshipType = PalletTown::Modifies;
				else if (currentToken.compare("uses") == 0)
					relationshipType = PalletTown::Uses;
				else if (currentToken.compare("calls") == 0)
					relationshipType = PalletTown::Calls;
				else if (currentToken.compare("calls*") == 0)
					relationshipType = PalletTown::CallsStar;
				else if (currentToken.compare("follows") == 0)
					relationshipType = PalletTown::Follows;
				else if (currentToken.compare("follows*") == 0)
					relationshipType = PalletTown::FollowsStar;
				else if (currentToken.compare("parent") == 0)
					relationshipType = PalletTown::Parent;
				else if (currentToken.compare("parent*") == 0)
					relationshipType = PalletTown::ParentStar;
				else if (currentToken.compare("next") == 0)
					relationshipType = PalletTown::Next;
				else if (currentToken.compare("next*") == 0)
					relationshipType = PalletTown::NextStar;
				else if (currentToken.compare("affects") == 0)
					relationshipType = PalletTown::Affects;
				else if (currentToken.compare("affect*") == 0)
					relationshipType = PalletTown::AffectsStar;
				else
					throw SPAException("Invalid relationship type");
			}
			else if (currentToken.compare("(") == 0)
				openBracket = true;
			else if (openBracket == true && firstVariable == false && comma == false)
			{
				if (currentToken.compare("_") == 0)
				{
					if (relationshipType == PalletTown::Modifies || relationshipType == PalletTown::Uses)
						throw SPAException("Modifies and Uses cannot have '_' as the first parameter");
					else
						relationshipDeclaration.first = (make_pair(PalletTown::WildCard, currentToken));
				}
				else
				{
					for (auto it = userVariables.begin(); it != userVariables.end(); it++)
					{
						variableNames = (*it).second;
						for (int i = 0; i < variableNames.size(); i++)
						{
							if (variableNames.at(i).compare(currentToken) == 0)
							{
								variableType = PalletTown::ashKetchum((*it).first);
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
					relationshipDeclaration.second = (make_pair(PalletTown::WildCard, currentToken));
				else
				{
					for (auto it = userVariables.begin(); it != userVariables.end(); it++)
					{
						variableNames = (*it).second;
						for (int i = 0; i < variableNames.size(); i++)
						{
							if (variableNames.at(i).compare(currentToken) == 0)
							{
								variableType = PalletTown::ashKetchum((*it).first);
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
							variableType = PalletTown::ashKetchum((*it).first);
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
			else if (dot == true && conditionAttributePalletTown.find(currentToken) != conditionAttributePalletTown.end())
			{
				if (currentToken.compare("procname") == 0)
					conditionAttributeType = PalletTown::ProcName;
				else if (currentToken.compare("varname") == 0)
					conditionAttributeType = PalletTown::VarName;
				else if (currentToken.compare("value") == 0)
					conditionAttributeType = PalletTown::Value;
				else if (currentToken.compare("stmt#") == 0)
					conditionAttributeType = PalletTown::StmtNo;
				else
					throw SPAException("Invalid condition attribute");

				attribute = true;
			}
			else if (currentToken.compare("=") == 0)
				equals = true;
			else if (attribute == true && equals == true)
			{
				conditionDeclaration.second = currentToken; // value can be anything
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

unordered_map<int, vector<string>> QueryPreprocessor::getUserVariables()
{
	return userVariables;
}
	
unordered_map<int, vector<string>> QueryPreprocessor::getSelectVariables()
{
	return selectVariables;
}

unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>>>> 
	QueryPreprocessor::getRelationships()
{
	return relationships;
}
	
unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, string>>> QueryPreprocessor::getConditions()
{
	return conditions;
}
