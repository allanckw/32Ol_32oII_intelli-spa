#include "TestQueryEvaluator.h"
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
#include "../SPA/QueryEvaluator.h"
#include "../SPA/QueryEnums.h"

// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION(TestQueryEvaluator); // Note 4 

TestQueryEvaluator::TestQueryEvaluator(void)
{
}


TestQueryEvaluator::~TestQueryEvaluator(void)
{
}

void TestQueryEvaluator::TestQueryEvaluatorBuilder()
{  
	try{
	Parser* p = new Parser("D:\\SPA\\lalala\\Source.txt");//test src above
	
		p->buildAST();
		PKB::rootNode;
	}catch (exception& e) {
		cout << e.what() << endl;
	} 

	DesignExtractor::extractDesign();
	vector<string> tokens; 
	QueryPreprocessor QPP;
	QueryParser QP;
	QueryTreeBuilder QTB;
	vector<vector<QueryTreeNode*>> QT;
	vector<QueryTreeNode*> cluster;
	vector<pair<QueryEnums::QueryVar, string>> selected;
	QueryTreeNode::QTNodeType NT;
	QueryEvaluator* QE = new QueryEvaluator();
	vector<string> ans;
	tokens = QP.tokenize("stmt s1, s2; select s1 such that follows(s2, s1)");
	QPP.preProcess(tokens);
	QTB.buildQueryTree(QPP.getUserVariables(), QPP.getSelectVariables(), QPP.getRelationships(), QPP.getConditions());
	QT = QTB.getQueryTree();
	ans = QE->returnAnswer(QPP, QTB);

	CPPUNIT_ASSERT(ans.empty() == true);

	//CPPUNIT_ASSERT(a.empty() == true);
	//CPPUNIT_ASSERT(a == "2");
}
