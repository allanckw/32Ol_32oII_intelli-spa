#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
class TestModifies: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
	CPPUNIT_TEST_SUITE(TestModifies); // Note 3 
	CPPUNIT_TEST(testInsertAndRetrieveModifies);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();
	// method to test the assigning and retrieval of procs
	void testInsertAndRetrieveModifies();
};