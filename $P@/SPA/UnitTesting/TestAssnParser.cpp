#include "TestAssnParser.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/ASTExprNode.h"
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

int getOprType(string s)
{
	if (s=="+")
		return 0;
	else if (s=="-")
		return 1;
	else if (s=="*")
		return 2;
}


void TestAssnParser::testAssignmentParsing()
{  

	PKB::variables.insertVAR("x");
	PKB::variables.insertVAR("yas");

	vector<string> expr;
	expr.push_back("x");
	expr.push_back("+");
	expr.push_back("yas");
	expr.push_back(";");

	ASTExprNode* opr = AssignmentParser::processAssignment(expr);

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(0)->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(1)->getType());

	CPPUNIT_ASSERT_EQUAL(0, opr->getChild(0)->getValue());
	CPPUNIT_ASSERT_EQUAL(1, opr->getChild(1)->getValue());

	CPPUNIT_ASSERT_EQUAL(getOprType("+"), opr->getValue());

	expr.clear();
	///*
	//		+
	//	x		*
	//		y		2
	//*/
	expr.push_back("x");
	expr.push_back("+");
	expr.push_back("yas");
	expr.push_back("*");
	expr.push_back("2");
	expr.push_back(";");


	opr = AssignmentParser::processAssignment(expr);
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType()); //+
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), opr->getValue());

	//x
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), opr->getChild(0)->getValue());

	ASTExprNode* times = dynamic_cast<ASTExprNode* >(opr->getChild(1));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, times->getType());
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), times->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, times->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), times->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, times->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(2, times->getChild(1)->getValue());

	expr.clear();
	//x+yas*2+3*yas
	expr.push_back("x");
	expr.push_back("+");
	expr.push_back("yas");
	expr.push_back("*");
	expr.push_back("2");
	expr.push_back("+");
	expr.push_back("3");
	expr.push_back("*");
	expr.push_back("yas");
	expr.push_back(";");

	//					 +
	//			      x		  +
	//					 *		    *
	//				yas		2	 3	   yas

	//Sub Tree Root +
	ASTNode* opr1 = AssignmentParser::processAssignment(expr);
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr1->getType()); //+
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), opr1->getValue());
	
	//X Node
	ASTNode* subTree1 = opr1->getChild(0);
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, subTree1->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), subTree1->getValue());

	// + Node
	ASTNode* subTree2 = opr1->getChild(1);
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, subTree2->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), subTree2->getValue());

	//Sub Tree y*2
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, subTree2->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), subTree2->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, subTree2->getChild(0)->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), subTree2->getChild(0)->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, subTree2->getChild(0)->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(2, subTree2->getChild(0)->getChild(1)->getValue());
	//End Sub Tree y*2

	//Sub Tree 3*y
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, subTree2->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), subTree2->getChild(1)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, subTree2->getChild(1)->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(3, subTree2->getChild(1)->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, subTree2->getChild(1)->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), subTree2->getChild(1)->getChild(1)->getValue());
	//End Sub Tree 3*y


	//-------------------------------------------------------------------------------------------------------------------
	//Bracket Assignment Testing
	//-------------------------------------------------------------------------------------------------------------------
	expr.clear();
	expr.push_back("(");
	expr.push_back("x");
	expr.push_back("+");
	expr.push_back("1");
	expr.push_back(")");
	expr.push_back(";");

	opr = AssignmentParser::processAssignment(expr);

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(0)->getType());
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, opr->getChild(1)->getType());

	CPPUNIT_ASSERT_EQUAL(0, opr->getChild(0)->getValue());
	CPPUNIT_ASSERT_EQUAL(1, opr->getChild(1)->getValue());

	CPPUNIT_ASSERT_EQUAL(getOprType("+"), opr->getValue());


	expr.clear();
	//		*
	//	+		33	
	//x   yas	
	
	expr.push_back("(");
	expr.push_back("x");
	expr.push_back("+");
	expr.push_back("yas");
	expr.push_back(")");
	expr.push_back("*");
	expr.push_back("33");
	expr.push_back(";");

	opr = AssignmentParser::processAssignment(expr);
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType()); //*
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), opr->getValue());

	ASTExprNode* plus = dynamic_cast<ASTExprNode* >(opr->getChild(0));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, plus->getType()); //+
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), plus->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant,  opr->getChild(1)->getType());
	CPPUNIT_ASSERT_EQUAL(33, opr->getChild(1)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, plus->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), plus->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, plus->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), plus->getChild(1)->getValue());

	expr.clear();
	//x+(yas*(16+11*x))

	expr.push_back("x");
	expr.push_back("+");
	expr.push_back("(");
	expr.push_back("yas");
	expr.push_back("*");
	expr.push_back("(");
	expr.push_back("16");
	expr.push_back("+");
	expr.push_back("11");
	expr.push_back("*");
	expr.push_back("x");
	expr.push_back(")");
	expr.push_back(")");
	expr.push_back(";");

	//		+
	// x		*
	//		yas		+
	//			16		*
	//				11		x

	opr = AssignmentParser::processAssignment(expr);
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType()); //+
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), opr->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, opr->getChild(0)->getType());
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), opr->getChild(0)->getValue());

	times = dynamic_cast<ASTExprNode* >(opr->getChild(1));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, times->getType());
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), times->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, times->getChild(0)->getType());
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), times->getChild(0)->getValue());

	plus =  dynamic_cast<ASTExprNode* >(times->getChild(1));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, plus->getType());
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), plus->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, plus->getChild(0)->getType());
	CPPUNIT_ASSERT_EQUAL(16, plus->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, plus->getChild(1)->getType());
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), plus->getChild(1)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, plus->getChild(1)->getChild(0)->getType());
	CPPUNIT_ASSERT_EQUAL(11, plus->getChild(1)->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, plus->getChild(1)->getChild(1)->getType());
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), plus->getChild(1)->getChild(1)->getValue());

	expr.clear();
	
	expr.push_back("(");
	expr.push_back("(");
	expr.push_back("x");
	expr.push_back("*");
	expr.push_back("16");
	expr.push_back(")");
	expr.push_back("+");
	expr.push_back("(");
	expr.push_back("11");
	expr.push_back("+");
	expr.push_back("yas");
	expr.push_back(")");
	expr.push_back(")");

	expr.push_back("-");

	expr.push_back("(");
	expr.push_back("(");
	expr.push_back("x");
	expr.push_back("-");
	expr.push_back("19");
	expr.push_back(")");
	expr.push_back("+");
	expr.push_back("(");
	expr.push_back("4");
	expr.push_back("*");
	expr.push_back("yas");
	expr.push_back(")");
	expr.push_back(")");

	//((x * 16) + (11 + yas)) - ((x - 19) + (4 * yas))
	//				-
	//		+				+
	//	*		+		-		*
	//x	 16  11  yas  x   19  4   yas

	//Left Sub Tree
	opr = AssignmentParser::processAssignment(expr);
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, opr->getType()); //-
	CPPUNIT_ASSERT_EQUAL(getOprType("-"), opr->getValue());

	plus = dynamic_cast<ASTExprNode* >(opr->getChild(0));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, plus->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), plus->getValue());

	times  = dynamic_cast<ASTExprNode* >(plus->getChild(0));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, times->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), times->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, times->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), times->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, times->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(16, times->getChild(1)->getValue());

	times  = dynamic_cast<ASTExprNode* >(plus->getChild(1));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, times->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), times->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, times->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(11, times->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, times->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), times->getChild(1)->getValue());
	

	//Right Sub Tree
	plus = dynamic_cast<ASTExprNode* >(opr->getChild(1));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, plus->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("+"), plus->getValue());

	times  = dynamic_cast<ASTExprNode* >(plus->getChild(0));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, times->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("-"), times->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, times->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), times->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, times->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(19, times->getChild(1)->getValue());

	times  = dynamic_cast<ASTExprNode* >(plus->getChild(1));
	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Operator, times->getType()); 
	CPPUNIT_ASSERT_EQUAL(getOprType("*"), times->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Constant, times->getChild(0)->getType()); 
	CPPUNIT_ASSERT_EQUAL(4, times->getChild(0)->getValue());

	CPPUNIT_ASSERT_EQUAL(ASTNode::NodeType::Variable, times->getChild(1)->getType()); 
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"), times->getChild(1)->getValue());
}