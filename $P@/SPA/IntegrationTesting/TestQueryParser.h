#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TestQueryParser: 
	public CPPUNIT_NS::TestFixture // Note 2 
{

		CPPUNIT_TEST_SUITE(TestQueryParser); // Note 3 
		CPPUNIT_TEST(TestQueryParserBuilder);
		CPPUNIT_TEST_SUITE_END();
public:
	TestQueryParser(void);
	~TestQueryParser(void);
	void TestQueryParserBuilder();
};

