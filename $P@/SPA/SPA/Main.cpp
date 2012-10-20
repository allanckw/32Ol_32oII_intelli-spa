#pragma once
#include "stdafx.h"
#include "ASTNode.h"
#include "StmtNode.h"
#include "VARTable.h"
#include "PROCTable.h"
#include "ExprNode.h"
#include "Parser.h"
#include "PKB.h"

#include <sstream>
using namespace std;


bool IsName(std::string s) //first char of name cannot be digit
{
	//Check if first char of name is digit or character
	int output; 
	istringstream (s.at(0)) >> output;
	if (output == -858993460)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Reason why i put procIdx is in a while there could be a call statement possibilities
StmtNode* processWhile(int *i, Index procIdx)
{
	stack<char> brackets;
	int currline =*i;
	int tempindex=0;
	int* line=&currline;
	int* index = &tempindex;

	//Get Control Variable
	vector<string> inner=Parser::tokenized_codes.at(*line);
		
	string varName = inner.at(1);

	if ((IsName(varName))==false)
	{
		throw SPAException("Invalid Name!");
	}

	VARIndex vi=PKB::variables.getVARIndex(varName);
	StmtNode* stmtNode = new StmtNode(*line,ASTNode::NodeType::While, vi);
	//Get Next Line
	(*line)++;
	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);
		*index = 0;
		while(*index < inner.size())
		{
			string keyword=inner.at(*index);

			if (keyword=="procedure")
			{
				//throw exception, procedure does not exist in procedure
			}

			//do bracket Matching
			if (keyword=="{")
			{
				brackets.push('{');
				if (brackets.size()>1)
				{
					//throw exception
				}
			}
			else if (keyword=="}")
			{
				brackets.pop();
				if(brackets.size()==0)
				{
					(*i)=(*line);
					return stmtNode;
				}
			}
			
				
			if(*index==0)
			{			
				if (keyword=="call")
				{
					stmtNode->AddChild(processCall(line, procIdx));
				}
				else if(keyword=="while")
				{
					stmtNode->AddChild(processWhile(line, procIdx));
				}
				//else if(keyword=="if")
				//{
				//	//procNode->AddChild(processWhile(line));
				//}
				else//no other keyword
				{
					//variable which is assignment go to next token to check
				}
			}
			else
			{
				if(keyword=="=")//check for assignment Statement
				{
					//processAssignmentNode
				}
			}
			//cout<<inner.at(*index)<<" ";
			(*index)++;
		}
		(*line)++;
	}

	//Not reachable
	//To Throw exception as it pass over all the line
	//(*i)=(*line);
	return NULL;
}

//Reason why i put procIdx is in a while there could not be a self call
StmtNode* processCall(int *i, Index procIdx)
{
	vector<string> inner=Parser::tokenized_codes.at(*i);
	PROCIndex pi = PKB::procedures.getPROCIndex(inner.at(1));
	if(pi==-1)
	{
		//Throw exception, Procedure not in Source
	}
	
	if(pi==procIdx)
	{
		//Throw exception, Procedure cannot self call
	}

	StmtNode* stmtCall=new StmtNode(*i,ASTNode::NodeType::Call,pi);
	return stmtCall;
}

ASTNode* processProcedure(int *i)
{
	stack<char> brackets;
	int currline =*i;
	int tempindex=0;
	int* line=&currline;
	int* index = &tempindex;

	vector<string> inner=Parser::tokenized_codes.at(*line);
	//Note To KW:To add in PROCIndex for constructor
	ASTNode* procNode = new ASTNode(ASTNode::NodeType::Procedure);
	string procName = inner.at(1);

	if ((IsName(procName))==false)
	{
		throw SPAException("Invalid Name!");
	}

	PROCIndex pi=PKB::procedures.getPROCIndex(procName);
	//Get Next Line
	(*line)++;
	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);
		*index = 0;
		while(*index < inner.size())
		{
			string keyword=inner.at(*index);

			if (keyword=="procedure")
			{
				//throw exception, procedure does not exist in procedure
			}

			//do bracket Matching
			if (keyword=="{")
			{
				brackets.push('{');
				if (brackets.size()>1)
				{
					//throw exception
				}
			}
			else if (keyword=="}")
			{
				brackets.pop();
				if(brackets.size()==0)
				{
					(*i)=(*line);
					return procNode;
				}
			}
			
				
			if(*index==0)
			{			
				if (keyword=="call")
				{
					procNode->AddChild(processCall(line, pi));
				}
				else if(keyword=="while")
				{
					procNode->AddChild(processWhile(line, pi));
				}
				//else if(keyword=="if")
				//{
				//	//procNode->AddChild(processWhile(line));
				//}
				else//no other keyword
				{
					//variable which is assignment go to next token to check
				}
			}
			else
			{
				if(keyword=="=")//check for assignment Statement
				{
					//processAssignmentNode
				}
			}
			//cout<<inner.at(*index)<<" ";
			(*index)++;
		}
		(*line)++;
	}

	//Not reachable
	//To Throw exception as it pass over all the line
	//(*i)=(*line);
	return NULL;
}

int main(int argc, char* arg[])
{

	vector<string> codings;

	string line;

	//Testing Code
	/*
	try{
		ExprNode* test = new ExprNode(ASTNode::NodeType::Variable, "X");
		cout<< test->getType() << endl;
		test->AddChild(new ExprNode(ASTNode::NodeType::Constant, 10), 1);
		test->AddChild(new ExprNode(ASTNode::NodeType::Constant, 5), 1);
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}*/

	//end testing code - Allan

	try{
	  ifstream myfile ("C:\\spa\\CS3201test6.txt");
	  
	  if (myfile.is_open())
	  {
		while ( myfile.good() )
		{
		  getline (myfile,line);
	
		  codings.push_back(line);
		}
		myfile.close();
	  }

	  else cout << "Unable to open file"<<endl; 

	  //tolenize

	  int templine =0;
	  int tempindex=0;
	  int* line=&templine;
	  int* index = &tempindex;

	  vector<vector<string>> data;

	  Parser p(codings);
	  data = Parser::tokenized_codes;
	  
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
					  ASTNode::rootNode->AddChild(processProcedure(line);
				  }
			  }
			  else
			  {
				   if (inner.at(*index) == "procedure") 
				   {
					   ASTNode::rootNode->AddChild(processProcedure(line);
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
	catch (exception& e) {
		cout << e.what() << endl;
		return 0;
	}

}


