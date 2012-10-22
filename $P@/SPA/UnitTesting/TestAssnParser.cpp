#include "TestAssnParser.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/ExprNode.h"
#include "../SPA/ASTNode.h"
#include "../SPA/AssignmentParser.h"
#include "../SPA/PKB.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestAssnParser); // Note 4 


void TestAssnParser::setUp()
{
}

void TestAssnParser::tearDown()
{
}

void TestAssnParser::testAssignmentParsing()
{  
	//PKB::variables.insertVAR("x");
	//PKB::variables.insertVAR("y");
	//vector<string> expr;
	//expr.push_back("x");
	//expr.push_back("+");
	//expr.push_back("y");
	//expr.push_back(";");

	//ExprNode* opr = AssignmentParser::processAssignment(expr);

	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType());
	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(0)->getType());
	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(1)->getType());


	//expr.clear();
	/////*
	////		+
	////	x		*
	////		y		2
	////*/
	//expr.push_back("x");
	//expr.push_back("+");
	//expr.push_back("y");
	//expr.push_back("*");
	//expr.push_back("2");
	//expr.push_back(";");

	//opr = AssignmentParser::processAssignment(expr);

	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType()); //+
	//CPPUNIT_ASSERT_EQUAL(getOprType("+"), opr->getValue());

	////x
	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(0)->getType()); 
	//CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), opr->getChild(0)->getValue());

	//ExprNode* times = opr;
	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, times->getType());
	//CPPUNIT_ASSERT_EQUAL(getOprType("*"), times->getValue());

	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, times->getChild(0)->getType()); 
	//CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("y"), times->getChild(0)->getValue());

	//CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, times->getChild(1)->getType()); 
	//CPPUNIT_ASSERT_EQUAL(2, times->getChild(1)->getValue());
}

int getOprType(string s)
{
	if (s=="+")
		return 0;
	else if (s=="-")
		return 1;
	else if (s=="*")
		return 2;
}