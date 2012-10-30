#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TestQueryTreeBuilder: 
	public CPPUNIT_NS::TestFixture // Note 2 
{

		CPPUNIT_TEST_SUITE(TestQueryTreeBuilder); // Note 3 
		CPPUNIT_TEST(TestQueryTreeBuilderBuilder);
		CPPUNIT_TEST_SUITE_END();
public:
	TestQueryTreeBuilder(void);
	~TestQueryTreeBuilder(void);
	void TestQueryTreeBuilderBuilder();
};