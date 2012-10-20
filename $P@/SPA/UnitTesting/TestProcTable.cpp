#include "TestProcTable.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestProcTable); // Note 4 

void TestProcTable::setUp()
{
}

void TestProcTable::tearDown()
{
}

void TestProcTable::testInsertAndRetrieveProcs()
{  
	PKB::procedures.insertPROC("asd");
	PKB::procedures.insertPROC("y");

	CPPUNIT_ASSERT_EQUAL(0, PKB::procedures.getPROCIndex("asd"));
	CPPUNIT_ASSERT_EQUAL(1, PKB::procedures.getPROCIndex("y"));

	string PROC = PKB::procedures.getPROCName(0);
	CPPUNIT_ASSERT(PROC == "asd");

	PROC = PKB::procedures.getPROCName(1);
	CPPUNIT_ASSERT(PROC == "y");

	CPPUNIT_ASSERT_EQUAL(-1, PKB::procedures.getPROCIndex("qq"));

	//PROC = PKB::procedures.getPROCName(2);
	//CPPUNIT_ASSERT(PROC == "y");
}

