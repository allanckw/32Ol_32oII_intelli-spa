#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
#include <vector>
class TestQueryEvaluator: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
		CPPUNIT_TEST_SUITE(TestQueryEvaluator); // Note 3 
		CPPUNIT_TEST(TestQueryEvaluatorBuilder);
		CPPUNIT_TEST_SUITE_END();
	
public:
	TestQueryEvaluator(void);
	~TestQueryEvaluator(void);
	void TestQueryEvaluatorBuilder();
};