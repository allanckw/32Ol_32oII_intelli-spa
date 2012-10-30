#include "TestQueryTreeBuilder.h"
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
#include "../SPA/QueryProjectNode.h"
#include "../SPA/QuerySelNode.h"
#include "../SPA/QueryRelNode.h"
#include "../SPA/SingleQueryEvaluator.h"
#include "../SPA/QueryEnums.h"

// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION(TestQueryTreeBuilder); // Note 4 

TestQueryTreeBuilder::TestQueryTreeBuilder(void)
{
}


TestQueryTreeBuilder::~TestQueryTreeBuilder(void)
{
}

void TestQueryTreeBuilder::TestQueryTreeBuilderBuilder()
{  

	vector<string> tokens; 
	QueryPreprocessor QPP;
	QueryParser QP;
	QueryTreeBuilder QTB;
	vector<vector<QueryTreeNode*>> QT;
	vector<pair<QueryEnums::QueryVar, string>> selected;
	vector<QueryTreeNode*> cluster;
	QueryTreeNode::QTNodeType NT;
	vector<string> ans;
	tokens = QP.tokenize("stmt s1; select s1 such that follows(3, s1)");
	QPP.preProcess(tokens);
	QTB.buildQueryTree(QPP.getUserVariables(), QPP.getSelectVariables(), QPP.getRelationships(), QPP.getConditions());
	QT = QTB.getQueryTree();

		cluster = QT.at(0);
			NT = cluster.at(0)->getNodeType();
			if (NT == QueryTreeNode::Project)
			{
				/*
				p = new QueryProjectNode();
				p = (QueryProjectNode*) cluster.at(y);
				cout<<"Project Node: ";
				for (int j = 0; j < p->getAnswer().size(); j++)
					cout<<p->getAnswer().at(j)<<" ";
				cout<<endl;*/
			}
			else if (NT == QueryTreeNode::Select)
			{
				/*
				selected = ((QuerySelNode*) cluster.at(y))->getSelectedVariables();
				cout<<"Select Node: ";
				for (int i = 0; i < selected.size(); i++)
					cout<<"("<<selected.at(i).first<<", "<<selected.at(i).second<<")";
				cout<<endl;*/
			}
			else if (NT == QueryTreeNode::Relationship)
			{
				//int a = (int) ((QueryRelNode*) cluster.at(0))->getFirstVariableType();
				//CPPUNIT_ASSERT_EQUAL(3, a);
				QueryEnums::QueryVar a = ((QueryRelNode*) cluster.at(0))->getSecondVariableType();
				CPPUNIT_ASSERT(a == QueryEnums::Stmt);
			}
}