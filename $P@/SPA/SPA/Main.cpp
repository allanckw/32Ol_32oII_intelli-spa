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

	  system("PAUSE");
	}
	catch (exception& e) {
		cout << e.what() << endl;
		return 0;
	}

}


