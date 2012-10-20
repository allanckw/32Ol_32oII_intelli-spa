#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
class TestParent: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
	CPPUNIT_TEST_SUITE(TestParent); // Note 3 
	CPPUNIT_TEST(testInsertAndRetrieveParent);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();
	// method to test the assigning and retrieval of procs
	void testInsertAndRetrieveParent();
};
