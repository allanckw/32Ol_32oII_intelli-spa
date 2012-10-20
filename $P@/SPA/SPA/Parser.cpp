#include "StdAfx.h"
#include "Parser.h"
#include "PKB.h"
#include "SPAException.h"

//TODO: For Nick, put your parser into this controller
vector<vector<string>> Parser::tokenized_codes;

stack<char> brackets;

Parser::Parser(vector<string> codings)
{
	string line;

	int currentline = 0;

	while(currentline < codings.size()){
		  line = codings.at(currentline);
		  vector<string> lststr = tokenizer(line);//tokenize the line one by one
		  Parser::tokenized_codes.push_back(lststr);//store the tokenized line
		  currentline++;
	}

	if(brackets.size() != 0){
		throw SPAException("Error during Parsing, Invalid Bracket matching");//error, bracket matching fail
	}
	else
	{
		PKB::maxProgLines = currentline;
	}
	  
}

Parser::~Parser(void)
{
}


bool Parser::IsEmpty(string str)
{
	for(int index=0;index<str.size();index++)
	{
		if(str.at(index)!= ' ')
			return false;
	}
	return true;
}


bool Parser::is_number(const std::string& str)
{
    std::string::const_iterator currentIterator = str.begin();
    while (currentIterator != str.end() && isdigit(*currentIterator)) ++currentIterator;
    return !str.empty() && currentIterator == str.end();
}

void Parser::AddTables(vector<string> list, string newtoken)
{
	int size = list.size();
	if(newtoken == "while" || newtoken == "if" || newtoken == "procedure" || newtoken == "+" || newtoken == "-" || 
		newtoken == "*" ||  newtoken == "=" ||newtoken == "call"||newtoken == "then"||newtoken == "else"||newtoken == ";")//found a reserve token
	{
		return;
	}
	else if(newtoken[0] == '{' || newtoken[0] == '}')//found a bracket
	{
		
		if(newtoken[0] == '{' )
		{
			brackets.push('{');//push open braket to stack
		}
		else if(newtoken[0] == '}' )
		{
			
			if(brackets.size() == 0)
			{
				throw SPAException("Error during Parsing, Invalid Bracket matching");//error bracket matching
				
			}
			else if(brackets.top() != '{')
			{
				throw SPAException("Error during Parsing, Invalid Bracket matching"); //error bracket matching
			}
			brackets.pop();//pop the bracket from the stack
			
		}
		
	}
	else if(!isdigit(newtoken[0]))//if currentoken is not a number hence may be a variable or procedure
	{
		if(size != 0 && list.at(size-1) == "procedure")
			PKB::procedures.insertPROC(newtoken);
		else if(size == 0 || list.at(size-1) != "call")
			PKB::variables.insertVAR(newtoken);
			
	}
	else
	{
		if(size != 0 && list.at(size-1) == "procedure")
			throw SPAException("Error during Parsing, Invalid Procedure Starting Number");//error procedure start with a number
		else if(!is_number(newtoken))
		{
			throw SPAException("Error during Parsing, Invalid Variable Starting Number");//error variable start with a number
		}
	}

}

vector<string> Parser::tokenizer(string line)//split the string into tokens
{
	vector<string> list;
	string delimiter = " -+*;{}";//delimiter
	int position = 0;//starting position
	int startindex = -1;
	int endindex = -1;

	
	do//loop thru the string
	{

		startindex = line.find_first_not_of(delimiter,position);

		if(endindex != -1 && endindex<line.size())
			{
				string tempstr1; //temp str to store subset of currently working substring
				if(startindex == -1)
				{
					tempstr1 = line.substr(endindex,line.size() - endindex);
				}
				else
				{
					tempstr1 = line.substr(endindex,startindex - endindex);
				}
				for(int i=0;i<tempstr1.size();)
				{
					string tempstr2 = tempstr1.substr(0,1);
					tempstr1 = tempstr1.substr(1,tempstr1.size()-1);
					if(tempstr2 != " ")
					{
						AddTables(list,tempstr2);
						list.push_back(tempstr2);
					}
				}
			}

			endindex = line.find_first_of(delimiter,startindex);

			position = endindex;
		
			if(startindex != -1)
			{
				string tempstr = line.substr(startindex,endindex-startindex);
				AddTables(list,tempstr);
				list.push_back(tempstr);			
			}

	}while(startindex != -1 && position < line.size() && endindex != -1);
	
	return list;
}

