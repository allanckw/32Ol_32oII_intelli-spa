#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
class TestBuildAST: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
	CPPUNIT_TEST_SUITE(TestBuildAST); // Note 3 
	CPPUNIT_TEST(testASTBuilder);
	//CPPUNIT_TEST(watver);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();

	// method to test the assigning and retrieval of procs
	void testASTBuilder();
	//additional if needed..
	//void watever();
};

