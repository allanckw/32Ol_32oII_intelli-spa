#pragma once
#include "TestVarTable.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestVarTable); // Note 4 

void TestVarTable::setUp()
{
}

void TestVarTable::tearDown()
{
}

void TestVarTable::testInsertAndRetrieveVars()
{  


	CPPUNIT_ASSERT_EQUAL(0, PKB::variables.getVARIndex("x"));
	CPPUNIT_ASSERT_EQUAL(1, PKB::variables.getVARIndex("yas"));

	string var = PKB::variables.getVARName(0);
	CPPUNIT_ASSERT(var == "x");

	var = PKB::variables.getVARName(1);
	CPPUNIT_ASSERT(var == "yas");


	CPPUNIT_ASSERT_EQUAL(-1, PKB::variables.getVARIndex("qq"));

	//var = PKB::variables.getVARName(2);
	//CPPUNIT_ASSERT(var == "y");
	
	CPPUNIT_ASSERT_EQUAL(5, PKB::variables.getSize());
}

