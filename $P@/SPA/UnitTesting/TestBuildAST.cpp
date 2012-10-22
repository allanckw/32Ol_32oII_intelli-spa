#include "TestBuildAST.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
#include "../SPA/Parser.h"
#include "../SPA/ASTNode.h"
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

	CPPUNIT_ASSERT_EQUAL(0, PKB::rootNode->getValue());
	CPPUNIT_ASSERT_EQUAL(true,PKB::rootNode->isRoot());
	
	ASTNode* procNode1 = PKB::rootNode->getChild(0);
	ASTNode* whileNode=procNode1->getChild(0)->getChild(1);

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Procedure, procNode1->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::While, whileNode->getType());

	//StmtNode* whileNode=PKB::rootNode->getChild(0)->getChild(1);
	VARIndex vi=PKB::rootNode->getChild(0)->getChild(1)->getValue();

	string var = PKB::variables.getVARName(0);
	CPPUNIT_ASSERT(var == "x");
}
