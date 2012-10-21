#pragma once
#include "StdAfx.h"
#include <set>
#include "PKB.h"

vector<pair <string, string>> userVariables;
vector<string> selectVariables;
vector<vector<string>> conditions;
vector<vector<string>> relationships;

void queryParser(string);

int main(int argc, char* arg[])
{
	string query;

	//cout<<"Enter query plz:"<<endl;
	//cin>>query;

	queryParser("assignment a1, a2; statement s1, s2; select a such that a1.value = 5");
	cout<<"user var: ";
	for (int i = 0; i < userVariables.size(); i++)
		cout<<"("<<userVariables.at(i).first<<", "<<userVariables.at(i).second<<") ";

	cout<<endl;

	cout<<"select var: ";
	for (int i = 0; i < selectVariables.size(); i++)
		cout<<selectVariables.at(i)<<" ";

	cout<<endl;

	cout<<"conditions: ";
	for (int i = 0; i < conditions.size(); i++)
	{
		cout<<"(";
		for (int j = 0; j < conditions.at(i).size(); j++)
			cout<<conditions.at(i).at(j)<<",";
		cout<<") ,";
	}

	cout<<endl;

	cout<<"relationships: ";
	for (int i = 0; i < relationships.size(); i++)
	{
		cout<<"(";
		for (int j = 0; j < relationships.at(i).size(); j++)
			cout<<relationships.at(i).at(j)<<",";
		cout<<") ,";
	}

	cout<<endl;

	system("PAUSE");

	return 0;
}

void queryParser(string query)
{
	string delimiters = " ;,";
	string currentStr, nextStr, tempStr;
	int lastPos = query.find_first_not_of(delimiters, 0); //where you first start off
	int nextPos = query.find_first_of(delimiters, lastPos);
	int tempPos1, tempPos2;
	vector<string> tempCondition;
	string variableType; 
	bool conditionsAndRelationShips = false;

	while (!(query.size() < nextPos) && !(query.size() < lastPos))
	{
		currentStr = query.substr(lastPos, nextPos - lastPos);
		cout<<"current str: "<<currentStr<<endl;
		if (currentStr.compare("select") == 0)
		{
			while (true)
			{
				lastPos = query.find_first_not_of(' ', lastPos + (nextPos - lastPos)); //getting variable name
				tempPos1 = query.find_first_of(',', lastPos); //TODO:REMEMBER TO CHECK FOR ANY OTHER DELIMITER IN VARIABLE NAME
				tempPos2 = query.find_first_of(" ", lastPos);
				if (tempPos2 < tempPos1 || tempPos1 < 0) //if "," comes before ";", there are more variables of same type to declare
				{
					selectVariables.push_back(query.substr(lastPos, tempPos2 - lastPos));
					break;
				}
				selectVariables.push_back(query.substr(lastPos, tempPos1 - lastPos));
			}

			//check for "with" or "such that" after select (todo: pattern)
			tempPos2 = query.find_first_not_of(' ', tempPos2);
			tempPos1 = query.find_first_of(' ', tempPos2);
			currentStr = query.substr(tempPos2, tempPos1 - tempPos2);
			if (currentStr.compare("such") == 0)
			{
				tempPos2 = query.find_first_not_of(' ', tempPos1);
				tempPos1 = query.find_first_of(' ', tempPos2);
				nextStr = query.substr(tempPos2, tempPos1 - tempPos2);
				if (nextStr.compare("that") != 0)
					throw SPAException("'that' expected after 'such'");
			}
			else if (currentStr.compare("with") != 0)
				throw SPAException("such that or with expected after select");

			//start parsing conditions and relationships
			conditionsAndRelationShips = true;
			lastPos = query.find_first_not_of(' ', tempPos1);
			nextPos = query.find_first_of(' ', lastPos);
		}
		else if (conditionsAndRelationShips == true)
		{
			int index = 0;
			while (index < currentStr.size())
			{
				if (currentStr.at(index) == '(' || currentStr.at(index) == '.')
				{
					currentStr = currentStr.substr(0, index);
					break;
				}
				index++;
			}

			nextPos = lastPos + index;
			if (query.at(nextPos) == '(' || query.at(nextPos) == ' ')
				{
					if (currentStr.compare("follows") == 0 || currentStr.compare("follows*") == 0 || currentStr.compare("parent") == 0 ||
						currentStr.compare("parent*") == 0 || currentStr.compare("modifies") == 0 || currentStr.compare("uses") == 0 ||
						currentStr.compare("calls") == 0 || currentStr.compare("calls*") == 0 || currentStr.compare("next") == 0 ||
						currentStr.compare("next*") == 0 || currentStr.compare("affects") == 0 || currentStr.compare("affects*") == 0)
					{
						tempCondition.push_back(currentStr);
						tempPos1 = query.find_first_of('(', nextPos) + 1; //skip over the ( bracket token
						tempPos1 = query.find_first_not_of(' ', tempPos1); //skip over blank spaces
						tempPos2 = query.find_first_of(',', tempPos1);
						tempCondition.push_back(query.substr(tempPos1, tempPos2 - tempPos1));
						tempPos2++; //skip over the , token
						tempPos2 = query.find_first_not_of(' ', tempPos2);
						tempPos1 = query.find_first_of(')', nextPos);
						tempCondition.push_back(query.substr(tempPos2, tempPos1 - tempPos2));
						relationships.push_back(tempCondition);

						lastPos = query.find_first_not_of(' ', tempPos1 + 1); //skip over the ) bracket token
						nextPos = query.find_first_of(' ', lastPos);
						//entire relationship should be parsed at this point.
					}
					else
						throw SPAException("unidentified relationship");
				}
				else if (query.at(nextPos) == '.')
				{
					tempCondition.push_back(currentStr);
					int index = nextPos + 1; //skipping . symbol
					while (index < query.size())
					{
						if (query.at(index) == ' ' || query.at(index) == '=')
							break;
						else
							tempStr.push_back(query.at(index));

						index++;
					}
					tempCondition.push_back(tempStr);
					tempPos1 = query.find_first_not_of(' ', index);
					if (query.at(tempPos1) == '=')
					{
						tempPos1++;
						tempPos1 = query.find_first_not_of(' ', tempPos1); //skip over = symbol
					}
					if (query.at(tempPos1) == '\"')
					{
						tempPos1++;
						tempPos1 = query.find_first_not_of(' ', tempPos1); //skip over opening " symbol
						tempPos2 = query.find_first_of("\"", tempPos1);
						tempCondition.push_back(query.substr(tempPos1, tempPos2 - tempPos1));
						tempPos2++; //skip over closing " symbol
					}
					else
					{
						tempPos2 = query.find_first_of(' ', tempPos1);
						tempCondition.push_back(query.substr(tempPos1, tempPos2 - tempPos1));
					}
					conditions.push_back(tempCondition);
					lastPos = query.find_first_not_of(' ', tempPos2);
					nextPos = query.find_first_of(' ', lastPos);
				}
		}
		else
		{
			if (currentStr.compare("assignment") == 0)
				variableType = "assignment";
			else if (currentStr.compare("variable") == 0)
				variableType = "variable";
			else if (currentStr.compare("statement") == 0)
				variableType = "statement";
			else if (currentStr.compare("call") == 0)
				variableType = "call";
			else if (currentStr.compare("while") == 0)
				variableType = "while";
			else if (currentStr.compare("ifstatement") == 0)
				variableType = "ifstatement";
			else
				throw SPAException("unknown data type! dont try to be smart ok");		
			
			tempPos1 = nextPos;
			while (true)
			{
				lastPos = query.find_first_not_of(' ', tempPos1); //getting variable name
				tempPos1 = query.find_first_of(',', lastPos); //TODO:REMEMBER TO CHECK FOR ANY OTHER DELIMITER IN VARIABLE NAME
				tempPos2 = query.find_first_of(';', lastPos);
				if (tempPos2 < tempPos1 || tempPos1 < 0) //if "," comes before ";", there are more variables of same type to declare
				{	
					userVariables.push_back(make_pair(query.substr(lastPos, tempPos2 - lastPos), variableType));
					break;
				}
				userVariables.push_back(make_pair(query.substr(lastPos, tempPos1 - lastPos), variableType));
				tempPos1++; //skipping over , token
			}

			lastPos = query.find_first_not_of(' ', tempPos2 + 1); //skip over the ; token
			nextPos = query.find_first_of(' ', lastPos);
		}
	}

	return;
}

