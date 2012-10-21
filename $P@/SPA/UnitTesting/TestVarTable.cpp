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
	CPPUNIT_ASSERT_EQUAL(2, PKB::variables.getVARIndex("i"));
	CPPUNIT_ASSERT_EQUAL(3, PKB::variables.getVARIndex("z"));

	string var = PKB::variables.getVARName(0);
	CPPUNIT_ASSERT(var == "x");

	var = PKB::variables.getVARName(1);
	CPPUNIT_ASSERT(var == "yas");
	
	var = PKB::variables.getVARName(2);
	CPPUNIT_ASSERT(var == "i");

	var = PKB::variables.getVARName(3);
	CPPUNIT_ASSERT(var == "z");

	CPPUNIT_ASSERT_EQUAL(-1, PKB::variables.getVARIndex("qq"));

	//var = PKB::variables.getVARName(2);
	//CPPUNIT_ASSERT(var == "y");
	
}

