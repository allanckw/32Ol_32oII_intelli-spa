#include "TestQueryPreprocessor.h"
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
#include "../SPA/QueryEnums.h"

// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION(TestQueryPreprocessor); // Note 4 

TestQueryPreprocessor::TestQueryPreprocessor(void)
{
}

TestQueryPreprocessor::~TestQueryPreprocessor(void)
{
}

void TestQueryPreprocessor::TestQueryPreprocessorBuilder()
{
	vector<string> tokens; 
	QueryPreprocessor QPP;
	QueryParser QP;
	QueryTreeBuilder QTB;
	vector<pair<QueryEnums::QueryVar, string>> selected;
	vector<string> ans;
	tokens = QP.tokenize("stmt s1; select s1 such that follows*(3, s1)");
	QPP.preProcess(tokens);
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> r = 
		QPP.getRelationships();
	
	//Test for searching through all relationships for their contents and types
	for (auto it = r.begin(); it != r.end(); it++)
	{
		CPPUNIT_ASSERT((*it).first == QueryEnums::FollowsStar);
		for (int x = 0; x < (*it).second.size(); x++)
		{
			CPPUNIT_ASSERT((*it).second.at(x).second.first == QueryEnums::Stmt);
		}
	}
}