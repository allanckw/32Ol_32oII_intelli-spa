#pragma once
#include "stdafx.h"
#include "ASTNode.h"
#include "StmtNode.h"
#include "VARTable.h"
#include "PROCTable.h"
#include "ExprNode.h"
#include "Parser.h"

using namespace std;

stack<char> brackets;


bool IsEmpty(string str)
{
	for(int i=0;i<str.size();i++)
	{
		if(str.at(i)!= ' ')
			return false;
	}
	return true;
}
void error(int i)
{
	cout<<"error: ";
	switch(i)
	{
	case 1:
		cout<<"bracket matching fail : "<<endl;
		break;
	case 2:
		cout<<"bracket matching fail: "<<endl;
		break;
	case 3:
		cout<<"var starting number: "<<endl;
		break;
	case 4:
		cout<<"proc starting number: "<<endl;
		break;
	default:
		cout<<"just error"<<endl;
		break;
	}
	
	system("PAUSE");
	throw 20;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void AddTables(vector<string> list, string newtoken)
{
	int size = list.size();
	if(newtoken == "while" || newtoken == "if" || newtoken == "procedure" || newtoken == "+" || newtoken == "-" || 
		newtoken == "*" ||  newtoken == "=" ||newtoken == "call"||newtoken == "then"||newtoken == "else"||newtoken == ";")
	{
		return;
	}
	else if(newtoken[0] == '{' || newtoken[0] == '}')
	{
		
		if(newtoken[0] == '{' )
		{
			brackets.push('{');
		}
		else if(newtoken[0] == '}' )
		{
			
			if(brackets.size() == 0)
			{
				error(1);
				//throw exception("Error!, Incorrect Bracket Matching");
			}
			else if(brackets.top() != '{')
			{
				error(2);
			}
			brackets.pop();
			
		}
		//add var table
	}
	else if(!isdigit(newtoken[0]))
	{
		if(size != 0 && list.at(size-1) == "procedure")
			PROCTable::procedures.insertPROC(newtoken);
		else if(size == 0 || list.at(size-1) != "call")
			VARTable::variables.insertVAR(newtoken);
			//throw exception("Error!, first character of procedure cannot be a integer");
	}
	else
	{
		if(size != 0 && list.at(size-1) == "procedure")
			error(4);
		else if(!is_number(newtoken))
		{
			error(3);
		}
	}

}

vector<string> tokenizer3(string line)
{
	vector<string> list;
	string delimiter = " -+/*;{}";
	int position = 0;
	int startindex = -1;
	int endindex = -1;

	
	do
	{

		startindex = line.find_first_not_of(delimiter,position);

		if(endindex != -1 && endindex<line.size())
			{
				string temp;
				if(startindex == -1)
				{
					temp = line.substr(endindex,line.size() - endindex);
				}
				else
				{
					temp = line.substr(endindex,startindex - endindex);
				}
				for(int i=0;i<temp.size();)
				{
					string tempstr = temp.substr(0,1);
					temp = temp.substr(1,temp.size()-1);
					if(tempstr != " ")
					{
						AddTables(list,tempstr);
						list.push_back(tempstr);
					}
				}
			}

		endindex = line.find_first_of(delimiter,startindex);

		position = endindex;
		
		if(startindex != -1)
		{
			string temp = line.substr(startindex,endindex-startindex);
			AddTables(list,temp);
			list.push_back(temp);			
		}

	}while(startindex != -1 && position < line.size() && endindex != -1);
	
	return list;
}


int main(int argc, char* arg[])
{

	vector<string> codings;

	string line;

	//Testing Code
	
	try{
		ExprNode* test = new ExprNode(ASTNode::NodeType::Variable, "X");
		cout<< test->getType() << endl;
		test->AddChild(new ExprNode(ASTNode::NodeType::Constant, 10), 1);
		test->AddChild(new ExprNode(ASTNode::NodeType::Constant, 5), 1);
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	//end testing code - Allan

	try{
	  ifstream myfile ("C:\\spa\\CS3201test6.txt");
	  
	  if (myfile.is_open())
	  {
		while ( myfile.good() )
		{
		  getline (myfile,line);
		  //cout << line << endl;
	
		  codings.push_back(line);

		 // codes.append(line);
		}
		myfile.close();
	  }

	  else cout << "Unable to open file"<<endl; 

	  int currentline = 0;
	  int totalsize = 0;
	  while(currentline < codings.size())
	  {
		  line = codings.at(currentline);
		  vector<string> aa = tokenizer3(line);
		  Parser::tokenized_codes.push_back(aa);
		  currentline++;
	  }
	  int templine =0;
	  int tempindex=0;
	  int* line=&templine;
	  int* index = &tempindex;

	  ASTNode* root;

		if(brackets.size() != 0)
		{
			error(1);
			//throw exception("Error!, Incorrect Bracket Matching");
		}

	  vector<vector<string>> data;

	  //Vector of code form..
	  while(*line < Parser::tokenized_codes.size())
	  {
		  vector<string> inner =  Parser::tokenized_codes.at(*line);
		  *index = 0;
		  while(*index < inner.size())
		  {
			  //TODO: For Kai, Build the AST From Here
			  if (*index == 0 && *line == 0 )
			  {
				  if (inner.at(*index) == "procedure") //Assume case sensitive
				  {
					  PROCIndex i = PROCTable::procedures.getPROCIndex(inner.at(1));
					  if (i != 0)
					  {
						  //not the first procedure as parsed...
					  }
					  ASTNode::rootNode->SetRoot(i);
				  }
			  }
			  //cout<<inner.at(*index)<<" ";
			  (*index)++;
		  }
		  cout<<endl;
		 // (*line)++;

	  }

	  system("PAUSE");
	}
	catch(int e)
	{
		return 0;
	}

}


