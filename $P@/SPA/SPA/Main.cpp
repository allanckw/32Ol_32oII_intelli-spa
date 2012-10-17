#pragma once
#include "stdafx.h"
#include "ASTNode.h"
#include "StmtNode.h"
#include "VARTable.h"
#include "PROCTable.h"
#include "ExprNode.h"
#include "Parser.h"

using namespace std;


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
		  //cout << line << endl;
	
		  codings.push_back(line);

		 // codes.append(line);
		}
		myfile.close();
	  }

	  else cout << "Unable to open file"<<endl; 

	  //tolenize

	  int templine =0;
	  int tempindex=0;
	  int* line=&templine;
	  int* index = &tempindex;

	  ASTNode* root;

	  vector<vector<string>> data;

	  Parser p(codings);
	  data = Parser::tokenized_codes;

	  //Vector of code form..
	  //while(*line < Parser::tokenized_codes.size())
	  //{
		 // vector<string> inner =  Parser::tokenized_codes.at(*line);
		 // *index = 0;
		 // while(*index < inner.size())
		 // {
			//  //TODO: For Kai, Build the AST From Here
			//  if (*index == 0 && *line == 0 )
			//  {
			//	  if (inner.at(*index) == "procedure") //Assume case sensitive
			//	  {
			//		  PROCIndex i = PROCTable::procedures.getPROCIndex(inner.at(1));
			//		  if (i != 0)
			//		  {
			//			  //not the first procedure as parsed...
			//		  }
			//		  ASTNode::rootNode->SetRoot(i);
			//	  }
			//  }
			//  //cout<<inner.at(*index)<<" ";
			//  (*index)++;
		 // }
		 // cout<<endl;
		 //// (*line)++;

	  //}

	  system("PAUSE");
	}
	catch (exception& e) {
		cout << e.what() << endl;
		return 0;
	}

}


