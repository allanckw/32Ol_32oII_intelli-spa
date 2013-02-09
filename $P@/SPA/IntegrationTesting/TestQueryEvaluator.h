#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
#include "../SPA/PKB.h"
#include "../SPA/MultiQueryEval.h"
#include "../SPA/stdAfx.h"
#include "../SPA/PKBController.h"

class TestQueryEvaluator: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
		CPPUNIT_TEST_SUITE(TestQueryEvaluator); // Note 3 
		CPPUNIT_TEST(TestQueryEvaluatorM);
		CPPUNIT_TEST_SUITE_END();
	
public:
	TestQueryEvaluator(void);
	~TestQueryEvaluator(void);
	void TestQueryEvaluatorM();
	void TestQueryEvaluator::doon(string s);
};