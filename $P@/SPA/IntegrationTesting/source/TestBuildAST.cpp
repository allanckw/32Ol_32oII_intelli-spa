#include "TestBuildAST.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
#include "../SPA/Parser.h"
#include "../SPA/ASTNode.h"
#include "../SPA/ASTStmtLstNode.h"
#include "../SPA/ASTStmtNode.h"
#include "../SPA/AssignmentParser.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestBuildAST); // Note 4 


void TestBuildAST::setUp()
{
}

void TestBuildAST::tearDown()
{
}

void TestBuildAST::testASTBuilder()
{  
	try{
	//Parser* p = new Parser("C:\\temp\\Source.txt");
	//Parser* p = new Parser("C:\\temp\\Source2.txt");
	Parser* p = new Parser("C:\\temp\\Source3.txt");
	int templine =0;
	int tempindex=0;
	for(int i=0; i<Parser::tokenized_codes.size(); i++)
	{
		vector<string> inner =  Parser::tokenized_codes.at(i);
		for(int j=0; j<inner.size(); j++)
		{
			cout<<inner.at(j)<<" ";
		}
		cout << endl;
	}
	//Parser::tokenized_codes
	//Check if all variables are in the vartable, use testvartable as a reference
	//Check if all procedures are in the proctable, use testproctable as a reference
	//try
	//{
	
		p->buildAST();
		PKB::rootNode;

		this->contTest = true;
	}catch (exception& e) {
		cout << e.what() << endl;
		this->contTest = false;
	} 
	//}
	//catch(SPAException ex)
	//{
	//	return;
	//}

	system("PAUSE");
}

void TestBuildAST::testASTFirstProcedureSource1()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;
	ASTStmtNode* parentNode;

	CPPUNIT_ASSERT_EQUAL(0, PKB::rootNode->getValue());
	CPPUNIT_ASSERT_EQUAL(true,PKB::rootNode->isRoot());

	//First Procedure
	ASTNode* procNode = PKB::rootNode->getChild(0);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));
	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "lain");

	//First Assignment Node
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(1,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");
	
	//First While Loop
	ASTStmtNode* whileNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(2,whileNode->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	//First Assignment Node in while loop
	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(3,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "yas");

	//Second Assignment Node in while loop
	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(1));
	CPPUNIT_ASSERT_EQUAL(4,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "t");

	//Nested While Loop in While Loop
	ASTStmtNode* whileNode2 = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(2));
	CPPUNIT_ASSERT_EQUAL(5,whileNode2->getStmtNumber());
	vi=whileNode2->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "q");
	//Test parent of nested while loop
	parentNode =dynamic_cast<ASTStmtNode* >(whileNode2->getParent());
	CPPUNIT_ASSERT_EQUAL(whileNode,parentNode);

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode2->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(6,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");
	//Test Parent of Assigned in Nested While Loop
	parentNode =dynamic_cast<ASTStmtNode* >(assignNode->getParent());
	CPPUNIT_ASSERT_EQUAL(whileNode2,parentNode);

	//Call Node
	ASTStmtNode* callNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(2));
	CPPUNIT_ASSERT_EQUAL(7,callNode->getStmtNumber());
	pi=callNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");
	}
}

void TestBuildAST::testASTSecondProcedureSource1()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;

	ASTNode* procNode = PKB::rootNode->getChild(1);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));

	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");
	
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(8,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "z");
	
	assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(9,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");
	}
}

void TestBuildAST::testASTFirstProcedureSource2()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;
	ASTStmtNode* parentNode;

	CPPUNIT_ASSERT_EQUAL(0, PKB::rootNode->getValue());
	CPPUNIT_ASSERT_EQUAL(true,PKB::rootNode->isRoot());

	//First Procedure
	ASTNode* procNode = PKB::rootNode->getChild(0);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));
	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "lain");

	//First Assignment Node
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(1,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");
	
	//First While Loop
	ASTStmtNode* whileNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(2,whileNode->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	//First Assignment Node in while loop
	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(3,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	//Call Node
	ASTStmtNode* callNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(2));
	CPPUNIT_ASSERT_EQUAL(4,callNode->getStmtNumber());
	pi=callNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");
	}
}

void TestBuildAST::testASTSecondProcedureSource2()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;

	ASTNode* procNode = PKB::rootNode->getChild(1);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));

	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");
	
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(5,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "z");
	
	assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(6,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	//Call Node
	ASTStmtNode* callNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(2));
	CPPUNIT_ASSERT_EQUAL(7,callNode->getStmtNumber());
	pi=callNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noober");
	}
}

void TestBuildAST::testASTThirdProcedureSource2()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;
	

	ASTNode* procNode = PKB::rootNode->getChild(2);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));

	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noober");
	
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(8,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "z");
	
	assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(9,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	//first while loop
	ASTStmtNode* whileNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(2));
	CPPUNIT_ASSERT_EQUAL(10,whileNode->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(11,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(1));
	CPPUNIT_ASSERT_EQUAL(12,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "y");

	//second while loop
	ASTStmtNode* whileNode2 = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(2));
	CPPUNIT_ASSERT_EQUAL(13,whileNode2->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode2->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(14,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode2->getChild(1)->getChild(1));
	CPPUNIT_ASSERT_EQUAL(15,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "y");

	////third while loop
	//ASTStmtNode* whileNode3=dynamic_cast<ASTStmtNode* >(whileNode2->getChild(1)->getChild(2));
	//CPPUNIT_ASSERT_EQUAL(16,whileNode3->getStmtNumber());
	//vi=whileNode->getValue();
	//var = PKB::variables.getVARName(vi);
	//CPPUNIT_ASSERT(var == "i");

	//assignNode = dynamic_cast<ASTStmtNode* >(whileNode3->getChild(1)->getChild(0));
	//CPPUNIT_ASSERT_EQUAL(17,assignNode->getStmtNumber());
	//vi=assignNode->getValue();
	//var = PKB::variables.getVARName(vi);
	//CPPUNIT_ASSERT(var == "z");

	//assignNode = dynamic_cast<ASTStmtNode* >(whileNode3->getChild(1)->getChild(1));
	//CPPUNIT_ASSERT_EQUAL(18,assignNode->getStmtNumber());
	//vi=assignNode->getValue();
	//var = PKB::variables.getVARName(vi);
	//CPPUNIT_ASSERT(var == "x");

	////fourth while loop
	//ASTStmtNode* whileNode4=dynamic_cast<ASTStmtNode* >(whileNode3->getChild(1)->getChild(2));
	//CPPUNIT_ASSERT_EQUAL(19,whileNode4->getStmtNumber());
	//vi=whileNode->getValue();
	//var = PKB::variables.getVARName(vi);
	//CPPUNIT_ASSERT(var == "i");

	//assignNode = dynamic_cast<ASTStmtNode* >(whileNode4->getChild(1)->getChild(0));
	//CPPUNIT_ASSERT_EQUAL(20,assignNode->getStmtNumber());
	//vi=assignNode->getValue();
	//var = PKB::variables.getVARName(vi);
	//CPPUNIT_ASSERT(var == "xas");

	//assignNode = dynamic_cast<ASTStmtNode* >(whileNode4->getChild(1)->getChild(1));
	//CPPUNIT_ASSERT_EQUAL(21,assignNode->getStmtNumber());
	//vi=assignNode->getValue();
	//var = PKB::variables.getVARName(vi);
	//CPPUNIT_ASSERT(var == "ydd");
	}
}

void TestBuildAST::testASTFirstProcedureSource3()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;
	ASTStmtNode* parentNode;

	CPPUNIT_ASSERT_EQUAL(0, PKB::rootNode->getValue());
	CPPUNIT_ASSERT_EQUAL(true,PKB::rootNode->isRoot());

	//First Procedure
	ASTNode* procNode = PKB::rootNode->getChild(0);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));
	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "lain");

	//First Assignment Node
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(1,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");
	
	//First While Loop
	ASTStmtNode* whileNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(2,whileNode->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	//First Assignment Node in while loop
	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(3,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	//Call Node
	ASTStmtNode* callNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(2));
	CPPUNIT_ASSERT_EQUAL(4,callNode->getStmtNumber());
	pi=callNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");
	}
}

void TestBuildAST::testASTSecondProcedureSource3()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;

	ASTNode* procNode = PKB::rootNode->getChild(1);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));

	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");
	
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(5,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "z");
	
	assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(6,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	//Call Node
	ASTStmtNode* callNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(2));
	CPPUNIT_ASSERT_EQUAL(7,callNode->getStmtNumber());
	pi=callNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noober");
	}
}

void TestBuildAST::testASTThirdProcedureSource3()
{
	if(this->contTest)
	{
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;
	

	ASTNode* procNode = PKB::rootNode->getChild(2);
	ASTStmtLstNode* stmtListNode = dynamic_cast<ASTStmtLstNode* >(procNode->getChild(0));

	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noober");
	
	ASTStmtNode* assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(0));
	CPPUNIT_ASSERT_EQUAL(8,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "z");
	
	assignNode = dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(1));
	CPPUNIT_ASSERT_EQUAL(9,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	//first while loop
	ASTStmtNode* whileNode=dynamic_cast<ASTStmtNode* >(stmtListNode->getChild(2));
	CPPUNIT_ASSERT_EQUAL(10,whileNode->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(11,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(1));
	CPPUNIT_ASSERT_EQUAL(12,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "y");

	//second while loop
	ASTStmtNode* whileNode2 = dynamic_cast<ASTStmtNode* >(whileNode->getChild(1)->getChild(2));
	CPPUNIT_ASSERT_EQUAL(13,whileNode2->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode2->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(14,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode2->getChild(1)->getChild(1));
	CPPUNIT_ASSERT_EQUAL(15,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "y");

	//third while loop
	ASTStmtNode* whileNode3=dynamic_cast<ASTStmtNode* >(whileNode2->getChild(1)->getChild(2));
	CPPUNIT_ASSERT_EQUAL(16,whileNode3->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode3->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(17,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "z");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode3->getChild(1)->getChild(1));
	CPPUNIT_ASSERT_EQUAL(18,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	//fourth while loop
	ASTStmtNode* whileNode4=dynamic_cast<ASTStmtNode* >(whileNode3->getChild(1)->getChild(2));
	CPPUNIT_ASSERT_EQUAL(19,whileNode4->getStmtNumber());
	vi=whileNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode4->getChild(1)->getChild(0));
	CPPUNIT_ASSERT_EQUAL(20,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "xas");

	assignNode = dynamic_cast<ASTStmtNode* >(whileNode4->getChild(1)->getChild(1));
	CPPUNIT_ASSERT_EQUAL(21,assignNode->getStmtNumber());
	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "ydd");
	}
}