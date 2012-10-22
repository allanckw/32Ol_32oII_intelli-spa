#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"

//**For testing purposes only
//int main(int argc, char* arg[])
//{
//	string query;
//
//	//cout<<"Enter query plz:"<<endl;
//	//cin>>query;
//
//	QueryParser QP;
//	QP.tokenize("assignment a1, a2; statement s1, s2; select a such that s1.stmt# = 5 and uses(a1, a2)\n");
//	for (int i = 0; i < QP.getTokens().size(); i++)
//		cout<<QP.getTokens().at(i)<<" ";
//	cout<<endl;
//	QP.tokenize("assignment a1, a2; statement s1, s2; select a such that modifies(a1, a2)\n");
//	for (int i = 0; i < QP.getTokens().size(); i++)
//		cout<<QP.getTokens().at(i)<<" ";
//	cout<<endl;
//
//	system("PAUSE");
//	return 0;
//}

void QueryParser::tokenize(string query)
{
	tokens.clear();
	string delimiters = (";,()");
	char endline = '\n';
	string currentstr, tempstr;
	int lastpos, pos;
	lastpos = query.find_first_not_of(' ', 0);
	pos = query.find_first_of(' ', lastpos);
	while (lastpos < query.size() && pos <= query.size())
	{
		currentstr = query.substr(lastpos, pos - lastpos);
		for (int i = 0; i < currentstr.size(); i++)
		{
			if (currentstr.at(i) == '\n') 
			{
				if (!tempstr.find(endline))  //for now, exterminate all \n from result string
					tokens.push_back(tempstr);
				tempstr.clear();
			}
			else if (delimiters.find(currentstr.at(i)) != string::npos)
			{
				tokens.push_back(tempstr);
				tempstr.clear();
				tempstr += currentstr.at(i);
				tokens.push_back(tempstr);
				tempstr.clear();
			}
			else
				tempstr += currentstr.at(i);
		}
		if (!tempstr.empty())
			tokens.push_back(tempstr);
		
		tempstr.clear();
		lastpos = query.find_first_not_of(' ', lastpos + currentstr.size());
		pos =  query.find_first_of(' ', lastpos);
		if (pos > query.size()) //check if there are no more spaces beyond the last element
			pos = query.size();
	}
}

vector<string> QueryParser::getTokens(){
	return tokens;
}