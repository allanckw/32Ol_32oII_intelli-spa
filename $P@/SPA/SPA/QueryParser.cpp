#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"

vector<string> QueryParser::tokens;



vector<string> QueryParser::tokenize(string query)
{
	tokens.clear();
	string delimiters = (";,().="), spaces(" \t");
	string currentStr, tempStr;
	int lastpos, pos;
	lastpos = query.find_first_not_of(spaces, 0);
	pos = query.find_first_of(spaces, lastpos);
	while (lastpos < query.size() && pos <= query.size())
	{
		currentStr = query.substr(lastpos, pos - lastpos);
		for (int i = 0; i < currentStr.size(); i++)
		{
			if (currentStr.at(i) == '\n') 
			{
				if (!tempStr.find('\n'))  //for now, exterminate all \n from result string
					tokens.push_back(tempStr);
				tempStr.clear();
			}
			else if (delimiters.find(currentStr.at(i)) != string::npos)
			{
				tokens.push_back(tempStr);
				tempStr.clear();
				tempStr += currentStr.at(i);
				tokens.push_back(tempStr);
				tempStr.clear();
			}
			else
				tempStr += currentStr.at(i);
		}
		if (!tempStr.empty())
			tokens.push_back(tempStr);
		
		tempStr.clear();
		lastpos = query.find_first_not_of(spaces, lastpos + currentStr.size());
		pos =  query.find_first_of(spaces, lastpos);
		if (pos > query.size()) //check if there are no more spaces beyond the last element
			pos = query.size();
	}

	for (auto it = tokens.begin(); it != tokens.end(); it++)
	{
		if ((*it).compare("") == 0)
			it = tokens.erase(it);
	}
	return tokens;
}