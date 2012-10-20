#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
class TestCallsTable : 
	public CPPUNIT_NS::TestFixture // Note 2 
{
public:
	CPPUNIT_TEST_SUITE(TestCallsTable); // Note 3 
	CPPUNIT_TEST(testInsertAndRetrieveCalls);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	// method to test the assigning and retrieval of procs
	void testInsertAndRetrieveCalls();
};

