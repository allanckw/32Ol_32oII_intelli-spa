#include "TestCallsTable.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestCallsTable); // Note 4 


void TestCallsTable::setUp()
{
}

void TestCallsTable::tearDown()
{
}

void TestCallsTable::testInsertAndRetrieveCalls()
{  
	PKB::procedures.insertPROC("asd");
	PKB::procedures.insertPROC("y");

	//Fill in the Calls Table and test~

}

