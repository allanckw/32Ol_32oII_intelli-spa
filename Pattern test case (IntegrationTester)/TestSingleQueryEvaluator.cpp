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
	Parser* p = new Parser("D:\\SPA\\lalala\\Source.txt"); //Please remember to change file directory
	
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
	QueryTree QT;
	IEvalQuery* EQ = new IEvalQuery();
	vector<string> a;
	int size;

	//tokens = QP.tokenize("var v1, v2; stmt s1, s2; while w; Select s1 such that Modifies(s1, v1)");// and Uses(s1, v2)
	tokens = QP.tokenize("assign a; stmt s; Select a pattern a (\"x\", _\"q\"_)"); //returns 3 and 6, 3 is wrong
	QPP.preProcess(tokens);
	QTB.buildQueryTree(QPP.getUserVariables(), QPP.getSelectVariables(), QPP.getRelationships(), QPP.getConditions(), QPP.getPatterns());
	QT = QTB.getQueryTree();
	a = EQ->evaluateQuery(QT);
	//                                                                                                                                                                                                                                                                                                                              	CPPUNIT_ASSERT(a.at(0) == "4");
	size = a.size();
	CPPUNIT_ASSERT_EQUAL(1, size);
}