#include "TestProcTable.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION(TestProcTable); // Note 4 

void TestProcTable::setUp()
{
}

void TestProcTable::tearDown()
{
}

void TestProcTable::testInsertAndRetrieveProcs()
{  
	PKB::procedures.insertPROC("lain");
	PKB::procedures.insertPROC("Noob");

	CPPUNIT_ASSERT_EQUAL(0, PKB::procedures.getPROCIndex("lain"));
	CPPUNIT_ASSERT_EQUAL(1, PKB::procedures.getPROCIndex("Noob"));

	string PROC = PKB::procedures.getPROCName(0);
	CPPUNIT_ASSERT(PROC == "lain");

	PROC = PKB::procedures.getPROCName(1);
	CPPUNIT_ASSERT(PROC == "Noob");

	CPPUNIT_ASSERT_EQUAL(-1, PKB::procedures.getPROCIndex("qq"));

	CPPUNIT_ASSERT_EQUAL(2, PKB::procedures.getSize());

}

