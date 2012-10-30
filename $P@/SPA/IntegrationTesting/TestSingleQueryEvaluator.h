#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
#include <vector>
class TestSingleQueryEvaluator: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
		CPPUNIT_TEST_SUITE(TestSingleQueryEvaluator); // Note 3 
		CPPUNIT_TEST(TestSingleQueryEvaluatorBuilder);
		CPPUNIT_TEST_SUITE_END();
	
public:
	TestSingleQueryEvaluator(void);
	~TestSingleQueryEvaluator(void);
	void TestSingleQueryEvaluatorBuilder();
};