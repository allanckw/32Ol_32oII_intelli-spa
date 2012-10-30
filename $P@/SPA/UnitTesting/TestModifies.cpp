#include "TestModifies.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestModifies); // Note 4 


void TestModifies::setUp()
{
}

void TestModifies::tearDown()
{
}

void TestModifies::testInsertAndRetrieveModifies()
{  
	PKB::modifies.insertStmtModifies(1, 1);
	PKB::modifies.insertStmtModifies(1, 2);
	PKB::modifies.insertProcModifies(1, 3);
	PKB::modifies.insertProcModifies(1, 4);
	PKB::modifies.insertStmtModifies(2, 0);
	PKB::modifies.insertStmtModifies(2, 3);
	PKB::modifies.insertProcModifies(2, 1);
	PKB::modifies.insertProcModifies(2, 2);
	PKB::modifies.insertStmtModifies(3, 2);
	PKB::modifies.insertStmtModifies(3, 3);
	PKB::modifies.insertProcModifies(3, 0);
	PKB::modifies.insertProcModifies(3, 4);
	PKB::modifies.insertStmtModifies(4, 0);
	PKB::modifies.insertStmtModifies(4, 3);
	PKB::modifies.insertProcModifies(4, 2);
	PKB::modifies.insertProcModifies(4, 1);
	//after insertions are finished, call optimize function
	PKB::modifies.optimizeModifiesTable();
}