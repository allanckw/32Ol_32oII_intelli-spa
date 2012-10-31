#include "TestSingleQueryEvaluator.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
#include "../SPA/Parser.h"
#include "../SPA/ASTNode.h"
#include "../SPA/ASTStmtLstNode.h"
#include "../SPA/ASTStmtNode.h"
#include "../SPA/AssignmentParser.h"
#include "../SPA/DesignExtractor.h"
#include "../SPA/PKB.h"
#include "../SPA/QueryParser.h"
#include "../SPA/QueryPreprocessor.h"
#include "../SPA/QueryTreeBuilder.h"
#include "../SPA/SingleQueryEvaluator.h"
#include "../SPA/IEvalQuery.h"
#include "../SPA/QueryEnums.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestSingleQueryEvaluator); // Note 4 

TestSingleQueryEvaluator::TestSingleQueryEvaluator(void)
{
}


TestSingleQueryEvaluator::~TestSingleQueryEvaluator(void)
{
}

void TestSingleQueryEvaluator::TestSingleQueryEvaluatorBuilder()
{  
	try{
	Parser* p = new Parser("F:\\3201_3202\\SPA\\Source.txt");//test src above
	
		p->buildAST();
		PKB::rootNode;

		//this->contTest = true;
	}catch (exception& e) {
		cout << e.what() << endl;
		//this->contTest = false;
	} 

	DesignExtractor::extractDesign();
	vector<string> tokens; 
	QueryPreprocessor QPP;
	QueryParser QP;
	QueryTreeBuilder QTB;
	vector<pair<QueryEnums::QueryVar, string>> selected;
	vector<string> ans;

	/////////////////
	//Follows tests//

	tokens = QP.tokenize("stmt s1; select s1 such that follows(3, s1)");
	//tokens = QP.tokenize("stmt s1; select s1 such that follows(s1, 4)");
	//tokens = QP.tokenize("stmt s1; select s1 such that follows(9, s1)");
	//tokens = QP.tokenize("stmt s1; select s1 such that follows(2, s1)");
	//tokens = QP.tokenize("stmt s1; select s1 such that follows*(3, s1)");

	//End Follows tests//
	/////////////////////

	////////////////
	//Parent tests//

	//tokens = QP.tokenize("stmt s1; select s1 such that parent(2, s1)");
	//tokens = QP.tokenize("stmt s1; select s1 such that parent*(2, s1)");
	//tokens = QP.tokenize("stmt s1; select s1 such that parent(s1, 8)");

	//End Parent tests//
	////////////////////

	//////////////////
	//Modifies tests//

	//tokens = QP.tokenize("stmt s1; select s1 such that modifies(s1, \"x\")");
	//tokens = QP.tokenize("proc s1; select s1 such that modifies(s1, \"x\")");
	//tokens = QP.tokenize("var s1; select s1 such that modifies(3, s1)");
	//tokens = QP.tokenize("var s1; select s1 such that modifies(\"lain\", s1)");

	//End Modifies tests//
	//////////////////////

	//////////////
	//Uses tests//

	//tokens = QP.tokenize("stmt s1; select s1 such that uses(s1, \"x\")");
	//tokens = QP.tokenize("proc p1; select p1 such that uses(p1, \"x\")");
	//tokens = QP.tokenize("var v1; select v1 such that uses(1, v1)");
	//tokens = QP.tokenize("var v1; select v1 such that uses(\"lain\", v1)");

	//End Uses tests//
	//////////////////

	///////////////
	//Calls tests//

	tokens = QP.tokenize("proc p1, p2; select p1 such that calls(p1, p2)");

	QPP.preProcess(tokens);
	QTB.buildQueryTree(QPP.getUserVariables(), QPP.getSelectVariables(), QPP.getRelationships(), QPP.getConditions());
	IEvalQuery EQ;
	vector<vector<QueryTreeNode*>> QT;
	QT = QTB.getQueryTree();
	ans = EQ.evaluateQuery(QT);

	//CPPUNIT_ASSERT(ans.empty() == false);
	//CPPUNIT_ASSERT(ans.empty() == true);

	//string a = ans.at(0);

	///////////////////
	//Follows asserts//

	//CPPUNIT_ASSERT(a == "4");
	//CPPUNIT_ASSERT(ans.size() == 1);

	//CPPUNIT_ASSERT(a.compare("3"));
	//CPPUNIT_ASSERT(ans.size() == 1);

	//CPPUNIT_ASSERT(a == "None");
	//CPPUNIT_ASSERT(ans.size() == 1);

	//CPPUNIT_ASSERT(a.compare("7"));
	//CPPUNIT_ASSERT(ans.size() == 1);

	/*CPPUNIT_ASSERT(a == "4");
	string a2 = ans.at(1);
	CPPUNIT_ASSERT(a2 == "5");
	CPPUNIT_ASSERT(ans.size() == 2);*/

	//End follows asserts//
	///////////////////////

	//////////////////
	//Parent asserts//

	/*CPPUNIT_ASSERT(a == "3");
	string a2 = ans.at(1);
	CPPUNIT_ASSERT(a2 == "4");
	string a3 = ans.at(2);
	CPPUNIT_ASSERT(a3 == "5");
	CPPUNIT_ASSERT(ans.size() == 3);*/

	/*CPPUNIT_ASSERT(a == "3");
	string a2 = ans.at(1);
	CPPUNIT_ASSERT(a2 == "4");
	string a3 = ans.at(2);
	CPPUNIT_ASSERT(a3 == "5");
	string a4 = ans.at(3);
	CPPUNIT_ASSERT(a4 == "6");
	CPPUNIT_ASSERT(ans.size() == 4);*/

	/*CPPUNIT_ASSERT(a == "None");
	CPPUNIT_ASSERT(ans.size() == 1);*/

	//End Parent asserts//
	//////////////////////

	/////////////////////
	//Modifies asserts///

	//CPPUNIT_ASSERT(a == "1");
	//string a2 = ans.at(1);
	//int a2n = atoi(a2.c_str());
	//CPPUNIT_ASSERT_EQUAL(2, a2n);
	//string a3 = ans.at(2);
	//CPPUNIT_ASSERT(a3 == "5");
	/*string a4 = ans.at(3);
	CPPUNIT_ASSERT(a4 == "6");
	string a5 = ans.at(4);
	CPPUNIT_ASSERT(a5 == "9");
	CPPUNIT_ASSERT(ans.size() == 5);*/

	//Test case fail: procs that modify x only got Noob, dont have lain, or Lain
	//string a2 = ans.at(1);
	//CPPUNIT_ASSERT(a.compare("lain") == 0);
	//CPPUNIT_ASSERT(a2.compare("Noob") == 0);
	//CPPUNIT_ASSERT(ans.size() == 2);

	//CPPUNIT_ASSERT(a.compare("yas"));
	//CPPUNIT_ASSERT(ans.size() == 1);

	//CPPUNIT_ASSERT(a.compare("x"));
	//string a2 = ans.at(1);
	//CPPUNIT_ASSERT(a2.compare("t"));
	//CPPUNIT_ASSERT(ans.size() == 2);

	//End Modifies asserts//
	////////////////////////

	////////////////
	//Uses asserts//

	/*int an = atoi(a.c_str());
	CPPUNIT_ASSERT_EQUAL(2, an);
	string a2 = ans.at(1);
	CPPUNIT_ASSERT(a2 == "4");
	string a3 = ans.at(2);
	CPPUNIT_ASSERT(a3 == "5");
	string a4 = ans.at(3);
	CPPUNIT_ASSERT(a4 == "6");
	CPPUNIT_ASSERT(ans.size() == 4);*/

	//CPPUNIT_ASSERT(a.compare("lain"));
	//CPPUNIT_ASSERT(ans.size() == 1);

	//CPPUNIT_ASSERT(a == "yas");
	//CPPUNIT_ASSERT(ans.size() == 1);
	
	/*CPPUNIT_ASSERT(a == "yas");
	string a2 = ans.at(1);
	CPPUNIT_ASSERT(a2 == "i");
	string a3 = ans.at(2);
	CPPUNIT_ASSERT(a3 == "q");
	string a4 = ans.at(3);
	CPPUNIT_ASSERT(a4.compare("x"));
	CPPUNIT_ASSERT(ans.size() == 4);*/

	//End Uses asserts//
	////////////////////

	/////////////////
	//Calls asserts//

	//CPPUNIT_ASSERT(a.compare("lain"));
	//CPPUNIT_ASSERT(ans.size() == 1);

	//End Calls Asserts//
	/////////////////////
}