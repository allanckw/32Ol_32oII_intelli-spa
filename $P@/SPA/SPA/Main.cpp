#pragma once
#include "stdafx.h"

#include "VARTable.h"
#include "PROCTable.h"
#include "ExprNode.h"
#include "Parser.h"
#include "PKB.h"

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
	  ifstream myfile ("C:\\temp\\zzz.txt");
	  
	  if (myfile.is_open()) {
		while ( myfile.good() )		{		  
			getline (myfile,line);
			codings.push_back(line);
		}
		myfile.close();
	  }

	  else cout << "Unable to open file"<<endl; 

	  //tokenize
	  Parser p(codings);
	  p.buildAST();
	  
	}catch (exception& e) {
		cout << e.what() << endl;
	}
	system("PAUSE");
	return 0;
}


