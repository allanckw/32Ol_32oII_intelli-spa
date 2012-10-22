#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
class TestAssnParser : 
	public CPPUNIT_NS::TestFixture // Note 2 

{
	CPPUNIT_TEST_SUITE(TestAssnParser); // Note 3 
	CPPUNIT_TEST(testAssignmentParsing);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testAssignmentParsing();
};