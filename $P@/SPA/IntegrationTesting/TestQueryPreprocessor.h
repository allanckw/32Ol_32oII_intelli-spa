#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TestQueryPreprocessor: 
	public CPPUNIT_NS::TestFixture // Note 2 
{

		CPPUNIT_TEST_SUITE(TestQueryPreprocessor); // Note 3 
		CPPUNIT_TEST(TestQueryPreprocessorBuilder);
		CPPUNIT_TEST_SUITE_END();
public:
	TestQueryPreprocessor(void);
	~TestQueryPreprocessor(void);
	void TestQueryPreprocessorBuilder();
};