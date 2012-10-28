#pragma once
// Note 1
#include <cppunit/extensions/HelperMacros.h>
class TestBuildAST: 
	public CPPUNIT_NS::TestFixture // Note 2 
{
	CPPUNIT_TEST_SUITE(TestBuildAST); // Note 3 
	CPPUNIT_TEST(testASTBuilder);
	
	//CPPUNIT_TEST(testASTFirstProcedureSource1);
	//CPPUNIT_TEST(testASTSecondProcedureSource1);
	
	//CPPUNIT_TEST(testASTFirstProcedureSource2);
	//CPPUNIT_TEST(testASTSecondProcedureSource2);
	//CPPUNIT_TEST(testASTThirdProcedureSource2);
	
	CPPUNIT_TEST(testASTFirstProcedureSource3);
	CPPUNIT_TEST(testASTSecondProcedureSource3);
	CPPUNIT_TEST(testASTThirdProcedureSource3);
	CPPUNIT_TEST_SUITE_END();
private:
	bool contTest;
public:
	void setUp();
	void tearDown();

	// method to test the assigning and retrieval of procs
	void testASTBuilder();
	//additional if needed..
	void testASTFirstProcedureSource1();
	void testASTSecondProcedureSource1();

	void testASTFirstProcedureSource2();
	void testASTSecondProcedureSource2();
	void testASTThirdProcedureSource2();

	void testASTFirstProcedureSource3();
	void testASTSecondProcedureSource3();
	void testASTThirdProcedureSource3();
	//void watever();
};

