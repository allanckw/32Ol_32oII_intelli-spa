#include "TestDesignExtractor.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
#include "../SPA/Parser.h"
#include "../SPA/ASTNode.h"
#include "../SPA/StmtLstNode.h"
#include "../SPA/StmtNode.h"
#include "../SPA/AssignmentParser.h"
#include "../SPA/DesignExtractor.h"
#include "../SPA/PKB.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestDesignExtractor); // Note 4 

TestDesignExtractor::TestDesignExtractor(void)
{
}


TestDesignExtractor::~TestDesignExtractor(void)
{
}

void TestDesignExtractor::TestDesignExtractorBuilder()
{  

	
	try{
//Source DE Test1.txt
		/*
procedure lain {
	x = yas;
	while i {
		x = x + q * 2;	
	}
	call Noob;
}

procedure Noob{
	z = 1;
	x = z;
	call Noober;
}

procedure Noober{
	z = 1;
		*/
		////////////////////////////////////////////ji kai, this code failed dounno y i only tried put nested while loop, did i typo something?
//Source DE Test2.txt
/*
procedure lain {
	x = yas;
	while i {
		x = x + q * 2;	
	}
	call Noob;
}

procedure Noob{
	z = 1;
	x = z;
	call Noober;
}

procedure Noober{
	z = 1;
	x = z;
	while i {
		x = 1;
		y = 2;
		while i {
			x = 2;
			y = 3;
		}
	}
}
}*/
	Parser* p = new Parser("C:\\temp\\Source DE Test1.txt");//test src above
	
		p->buildAST();
		PKB::rootNode;

		//this->contTest = true;
	}catch (exception& e) {
		cout << e.what() << endl;
		//this->contTest = false;
	} 

	DesignExtractor::extractDesign();
	int temp1 =PKB::follows.getFollowsFrom(2);//error, tried always give no ans.//example 2 is fellowed by 3 and 2 is fellowed from 1

	/////////////////////////////////////////////////////


	PKB::uses;
	vector<VAR> tempvars1 = PKB::uses.getUsedByStmt(3);//invalid ans, 3 get used by x and q which are index 0 and 3 but this return nth
	////////////////////////////////////////////////////
	PKB::modifies;
	//vector<VAR> tempvars2 = PKB::modifies.getModifiedByStmt(1);	//threwed error
	///////////////////////////////////////////////////
	PKB::parent;
	CPPUNIT_ASSERT_EQUAL(2,PKB::parent.getParent(3));//ok
	CPPUNIT_ASSERT_EQUAL(true,PKB::parent.isParent(2,3));//ok
	
	CPPUNIT_ASSERT_EQUAL(3, PKB::parent.getChildren(2).at(0));//ok


	//vector<STMT> lol = PKB::parent.getChildrenStar(10);not yet tried


	///////////////////////////////////////////////////
	int temp2 = PKB::procedures.getPROCIndex("Noob");
	vector<PROC> temp = PKB::calls.getCalledFrom(PKB::procedures.getPROCIndex("Noob"));
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("lain"),PKB::calls.getCalledFrom(PKB::procedures.getPROCIndex("Noob")).at(0));
	//invalid ans please fix
	//if (noProcs - 1 >= p)<==inside your getcalledfrom function not sure wat noprocs is for i am guessing its number of procs
	//	answer = optimizedCalledFromTable[p];
	//
	/////////////////////////////////////////////////////////////////


	
}
