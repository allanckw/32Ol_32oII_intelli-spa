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
//	//vector<string> tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a such that s1.stmt# = 5");
//	vector<string> tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a such that modifies(a, b)");
//	QueryPreprocessor QE;
//	QE.preProcess(tokens);
//	vector<string> test;
//	vector<vector<string>> test2;
//	cout<<"user var: ";
//	for (auto it = QE.userVariables.begin(); it != QE.userVariables.end(); it++)
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
//	for (int i = 0; i < QE.selectVariables.size(); i++)
//		cout<<QE.selectVariables.at(i)<<" ";
//
//	cout<<endl;
//
//	cout<<"relationships: ";
//	for (auto it = QE.relationships.begin(); it != QE.relationships.end(); it++)
//	{
//		test2 = (*it).second;
//		for (int x = 0; x < test2.size(); x++)
//		{
//			cout<<"(";
//			for (int y = 0; y < test2.at(x).size(); y++)
//				cout<<test2.at(x).at(y)<<" ";
//			cout<<")";
//		}
//		cout<<", ";
//	}
//	cout<<endl;
//
//	cout<<"conditions: ";
//	for (auto it = QE.conditions.begin(); it != QE.conditions.end(); it++)
//	{
//		test2 = (*it).second;
//		for (int a = 0; a < test2.size(); a++)
//		{
//			cout<<"(";
//			for (int b = 0; b < test2.at(a).size(); b++)
//				cout<<test2.at(a).at(b)<<" ";
//			cout<<")";
//		}
//		cout<<", ";
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

	variableTypes.insert("assignment");
	variableTypes.insert("variable");
	variableTypes.insert("statement");
	variableTypes.insert("procedure");
	variableTypes.insert("call");
	variableTypes.insert("while");
	variableTypes.insert("ifstatement");
	variableTypes.insert("constant");
	
	relationshipTypes.insert("modifies");
	relationshipTypes.insert("uses");
	relationshipTypes.insert("calls");
	relationshipTypes.insert("calls*");
	relationshipTypes.insert("follows");
	relationshipTypes.insert("follows*");
	relationshipTypes.insert("parent");
	relationshipTypes.insert("parent*");
	relationshipTypes.insert("next");
	relationshipTypes.insert("next*");
	relationshipTypes.insert("affects");
	relationshipTypes.insert("affects*");

	conditionAttributeTypes.insert("procname");
	conditionAttributeTypes.insert("varname");
	conditionAttributeTypes.insert("value");
	conditionAttributeTypes.insert("stmt#");
}

void QueryPreprocessor::preProcess(vector<string> tokens)
{
	string delimiters = ";,()"; //all spaces are already taken care of
	string currentToken;
	vector<string> relationshipDeclaration, conditionDeclaration;
	ashketchum variableType;
	garyOak relationshipType;
	rorona conditionAttributeType;
	bool variableDeclaration = false, selectVariableDeclaration = false, suchThat = false, logicSets = false, relationship = false, condition = false;
	bool complete = false;
	bool openBracket = false, firstVariable = false, comma = false, secondVariable = false; //relationship validation
	bool dot = false, attribute = false, equals = false; //condition declaration

	for (int currentIndex = 0; currentIndex < tokens.size(); currentIndex++)
	{
		currentToken = tokens.at(currentIndex);
		if (variableTypes.find(currentToken) != variableTypes.end() || currentToken.compare("select") == 0)
		{
			if (currentToken.compare("assignment") == 0)
				variableType = Assign;
			else if (currentToken.compare("variable") == 0)
				variableType = Variable;
			else if (currentToken.compare("statement") == 0)
				variableType = Stmt;
			else if (currentToken.compare("procedure") == 0)
				variableType = Procedure;
			else if (currentToken.compare("call") == 0)
				variableType = Call;
			else if (currentToken.compare("while") == 0)
				variableType = While;
			else if (currentToken.compare("ifstatement") == 0)
				variableType = If;
			else if (currentToken.compare("constant") == 0)
				variableType = Constant;
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
					selectVariableDeclaration = false;
					variableDeclaration = false;
			}
			else if (currentToken.compare("with") == 0)
			{
				logicSets = true;
				selectVariableDeclaration = false;
			}
			else if (currentToken.compare(",") == 0) //there are more variables of the same type to store
				continue;
			else if (isName(currentToken) && selectVariableDeclaration)
				selectVariables.push_back(currentToken);
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
				logicSets = true;
			}
		}
		else if (logicSets == true && relationship == false && condition == false)
		{
			if (relationshipTypes.find(currentToken) != relationshipTypes.end())
			{
				if (currentToken.compare("modifies") == 0)
					relationshipType = Modifies;
				else if (currentToken.compare("uses") == 0)
					relationshipType = Uses;
				else if (currentToken.compare("calls") == 0)
					relationshipType = Calls;
				else if (currentToken.compare("calls*") == 0)
					relationshipType = CallsStar;
				else if (currentToken.compare("follows") == 0)
					relationshipType = Follows;
				else if (currentToken.compare("follows*") == 0)
					relationshipType = FollowsStar;
				else if (currentToken.compare("parent") == 0)
					relationshipType = Parent;
				else if (currentToken.compare("parent*") == 0)
					relationshipType = ParentStar;
				else if (currentToken.compare("next") == 0)
					relationshipType = Next;
				else if (currentToken.compare("next*") == 0)
					relationshipType = NextStar;
				else if (currentToken.compare("affects") == 0)
					relationshipType = Affects;
				else if (currentToken.compare("affect*") == 0)
					relationshipType = AffectsStar;

				relationship = true;
			}
			else if (isName(currentToken))
			{
				condition = true;
				conditionDeclaration.push_back(currentToken);
			}
			else
				throw SPAException("Valid relationship or condition expected");
		}
		else if (relationship == true)
		{
			if (currentToken.compare("(") == 0)
				openBracket = true;
			else if (openBracket == true && firstVariable == false && comma == false)
			{
				if (isName(currentToken))
				{
					relationshipDeclaration.push_back(currentToken);
					firstVariable = true;
				}
				else
					throw SPAException("Invalid relationship attribute name");
			}
			else if (openBracket == true && firstVariable == true && currentToken.compare(",") == 0)
				comma = true;
			else if (openBracket == true && comma == true && secondVariable == false)
			{
				if (isName(currentToken))
				{
					relationshipDeclaration.push_back(currentToken);
					secondVariable = true;
				}
				else
					throw SPAException("Invalid relationship attribute name");
			}
			else if (currentToken.compare(")") == 0) // reset all flags
			{
				relationships[relationshipType].push_back(relationshipDeclaration);
				relationshipDeclaration.clear();
				openBracket = false;
				firstVariable = false;
				comma = false;
				secondVariable = false;
				relationship = false;
				complete = true;
			}
			else
				throw SPAException("Invalid relationship syntax");
		}
		else if (condition == true)
		{
			if (currentToken.compare(".") == 0)
				dot = true;
			else if (dot == true && conditionAttributeTypes.find(currentToken) != conditionAttributeTypes.end())
			{
				if (currentToken.compare("procname") == 0)
					conditionAttributeType = ProcName;
				else if (currentToken.compare("varname") == 0)
					conditionAttributeType = VarName;
				else if (currentToken.compare("value") == 0)
					conditionAttributeType = Value;
				else if (currentToken.compare("stmt#") == 0)
					conditionAttributeType = StmtNo;

				attribute = true;
			}
			else if (currentToken.compare("=") == 0)
				equals = true;
			else if (attribute == true && equals == true)
			{
				conditionDeclaration.push_back(currentToken);
				conditions[conditionAttributeType].push_back(conditionDeclaration);
				conditionDeclaration.clear();
				dot = false;
				attribute = false;
				equals = false;
				condition = false;
				complete = true;
			}
			else
				throw SPAException("Invalid condition syntax");
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
	
vector<string> QueryPreprocessor::getSelectVariables()
{
	return selectVariables;
}
	
unordered_map<int, vector<vector<string>>> QueryPreprocessor::getConditions()
{
	return conditions;
}

unordered_map<int, vector<vector<string>>> QueryPreprocessor::getRelationships()
{
	return relationships;
}