#include "TestBuildAST.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
#include "../SPA/Parser.h"
#include "../SPA/ASTNode.h"
#include "../SPA/StmtLstNode.h"
#include "../SPA/StmtNode.h"
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
	Parser* p = new Parser("C:\\temp\\Source.txt");

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

	p->buildAST();

	system("PAUSE");
	//Insert assertions here..
	VARIndex vi;
	string var;
	PROCIndex pi;
	string proc;

	CPPUNIT_ASSERT_EQUAL(0, PKB::rootNode->getValue());
	CPPUNIT_ASSERT_EQUAL(true,PKB::rootNode->isRoot());
	
	ASTNode* procNode = PKB::rootNode->getChild(0);
	StmtLstNode* stmtListNode = dynamic_cast<StmtLstNode* >(procNode->getChild(0));
	StmtNode* assignNode = dynamic_cast<StmtNode* >(stmtListNode->getChild(0));
	ExprNode* oprNode = dynamic_cast<ExprNode* >(assignNode->getChild(1));

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, oprNode->getType());
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), oprNode->getValue());

	StmtNode* callNode=dynamic_cast<StmtNode* >(stmtListNode->getChild(2));
	
	//ASTNode* parentNode=childNode->getParent();
	
	StmtNode* whileNode=dynamic_cast<StmtNode* >(stmtListNode->getChild(1));
	StmtLstNode* stmtListNode2 = dynamic_cast<StmtLstNode* >(whileNode->getChild(1));
	StmtNode* childNode=dynamic_cast<StmtNode* >(stmtListNode2->getChild(0));
	StmtNode* parentNode=dynamic_cast<StmtNode* >(childNode->getParent());

	
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Procedure, procNode->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::StmtLst, stmtListNode->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Assign, assignNode->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::While, whileNode->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Assign, childNode->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Call, callNode->getType());
	

	//vi=oprNode->getChild(0)->getValue();
	//var = PKB::variables.getVARName(vi);
	//CPPUNIT_ASSERT(var == "yas");

	CPPUNIT_ASSERT_EQUAL(whileNode->getStmtNumber(),parentNode->getStmtNumber());
	//CPPUNIT_ASSERT_EQUAL(stmtListNode2);

	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "lain");

	pi=callNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");

	procNode=PKB::rootNode->getChild(1);
	pi=procNode->getValue();
	proc=PKB::procedures.getPROCName(pi);
	CPPUNIT_ASSERT(proc == "Noob");

	vi=assignNode->getValue();
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "x");

	vi=whileNode->getValue(); 
	var = PKB::variables.getVARName(vi);
	CPPUNIT_ASSERT(var == "i");
}

