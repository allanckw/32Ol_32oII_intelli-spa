#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>

class TestVarTable : 
	public CPPUNIT_NS::TestFixture // Note 2 
{
	CPPUNIT_TEST_SUITE(TestVarTable); // Note 3 
	CPPUNIT_TEST(testInsertAndRetrieveVars);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	// method to test the assigning and retrieval of vars
	void testInsertAndRetrieveVars();
};

