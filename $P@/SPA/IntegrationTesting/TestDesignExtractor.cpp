#include "TestDesignExtractor.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
#include "../SPA/Parser.h"
#include "../SPA/ASTNode.h"
#include "../SPA/ASTStmtLstNode.h"
#include "../SPA/ASTStmtNode.h"
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
	Parser* p = new Parser("C:\\Documents and Settings\\UltraSuperboy\\My Documents\\NUS\\CS3201\\ToMod\\3201_3202_SPA\\Source.txt");//test src above
	
		p->buildAST();
		PKB::rootNode;

		//this->contTest = true;
	}catch (exception& e) {
		cout << e.what() << endl;
		//this->contTest = false;
	} 

	DesignExtractor::extractDesign();
	CPPUNIT_ASSERT_EQUAL(1,PKB::follows.getFollowsFrom(2)); //fixed

	int a = PKB::follows.getFollowsBy(2);//invalid ans shld be 4 the table also dont have 2->4 //fixed
	CPPUNIT_ASSERT_EQUAL(4,PKB::follows.getFollowsBy(2));

	vector<VAR> b = PKB::follows.getFollowsStarBy(2);//wrong ans shld be 4 //fixed
	CPPUNIT_ASSERT_EQUAL(4,PKB::follows.getFollowsStarBy(2).at(0));

	vector<VAR> c = PKB::follows.getFollowsStarFrom(2);
	
	CPPUNIT_ASSERT_EQUAL(1,PKB::follows.getFollowsStarFrom(2).at(0));
	
	/////////////////////////////////////////////////////
	PKB::variables;
	PKB::procedures;

	vector<VAR> d = PKB::uses.getUsedByProc(PKB::procedures.getPROCIndex("lain"));//y only 0, shld have 0,1,2,3,4 //fixed
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"),PKB::uses.getUsedByProc(PKB::procedures.getPROCIndex("lain")).at(0));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("yas"),PKB::uses.getUsedByProc(PKB::procedures.getPROCIndex("lain")).at(1));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("i"),PKB::uses.getUsedByProc(PKB::procedures.getPROCIndex("lain")).at(2));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("q"),PKB::uses.getUsedByProc(PKB::procedures.getPROCIndex("lain")).at(3));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("z"),PKB::uses.getUsedByProc(PKB::procedures.getPROCIndex("lain")).at(4));

	



	PKB::uses;
	vector<VAR> tempvars1 = PKB::uses.getUsedByStmt(3);//invalid ans, 3 get used by x and q which are index 0 and 3 but this return nth
														//still wrong give i and q instead of x and q
														 //fixed
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), PKB::uses.getUsedByStmt(3).at(0));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("q"), PKB::uses.getUsedByStmt(3).at(1));

	vector<VAR> e = PKB::uses.getUsedInProc(PKB::procedures.getPROCIndex("lain"));
	//method parameter wrong and return statment wrong. eiter that or maybe is getUsedByProc wrong

	vector<VAR> f = PKB::uses.getUsedInStmt(3);
	//ans are ok
	//isnt this the same as getUsedByStmt wats the diff?
	//I think you are mixing up usedBy and usesIn. usedInStmt(3) is x and q.
	//UsesByStmt(3) returns all statements using variable q.
	//CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"), PKB::uses.getUsedInStmt(3).at(0));
	//CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("q"), PKB::uses.getUsedInStmt(3).at(1));

	


	////////////////////////////////////////////////////
	PKB::modifies;

	vector<VAR> modedata0 = PKB::modifies.getModifiedByProc(PKB::procedures.getPROCIndex("lain"));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"),PKB::modifies.getModifiedByProc(PKB::procedures.getPROCIndex("lain")).at(0));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("z"),PKB::modifies.getModifiedByProc(PKB::procedures.getPROCIndex("lain")).at(1));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("y"),PKB::modifies.getModifiedByProc(PKB::procedures.getPROCIndex("lain")).at(2));

	vector<VAR> modedata1 = PKB::modifies.getModifiedByStmt(3);
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"),PKB::modifies.getModifiedByStmt(3).at(0));

	vector<VAR> modedata2 = PKB::modifies.getModifiedByStmt(4);//shld be 0,4,5 only 0 //fixed
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("x"),PKB::modifies.getModifiedByStmt(4).at(0));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("z"),PKB::modifies.getModifiedByStmt(4).at(1));
	CPPUNIT_ASSERT_EQUAL(PKB::variables.getVARIndex("y"),PKB::modifies.getModifiedByStmt(4).at(2));


	vector<PROC> modedata3 = PKB::modifies.getModifiesProc(PKB::variables.getVARIndex("x"));
	//ok but call didnt return //fixed
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("lain"),PKB::modifies.getModifiesProc(PKB::variables.getVARIndex("x")).at(0));
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("Noob"),PKB::modifies.getModifiesProc(PKB::variables.getVARIndex("x")).at(1));
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("Noober"),PKB::modifies.getModifiesProc(PKB::variables.getVARIndex("x")).at(2));

	vector<STMT> modedata4 = PKB::modifies.getModifiesStmt(PKB::variables.getVARIndex("y"));
	//ok but not yet fully done as call procedure didnt return
	//fixed - the answers are in reversed order. 4,7,10,12,13,15
	CPPUNIT_ASSERT_EQUAL(4, PKB::modifies.getModifiesStmt(PKB::variables.getVARIndex("y")).at(0));
	CPPUNIT_ASSERT_EQUAL(7, PKB::modifies.getModifiesStmt(PKB::variables.getVARIndex("y")).at(1));
	CPPUNIT_ASSERT_EQUAL(10, PKB::modifies.getModifiesStmt(PKB::variables.getVARIndex("y")).at(2));
	CPPUNIT_ASSERT_EQUAL(12, PKB::modifies.getModifiesStmt(PKB::variables.getVARIndex("y")).at(3));
	CPPUNIT_ASSERT_EQUAL(13, PKB::modifies.getModifiesStmt(PKB::variables.getVARIndex("y")).at(4));
	CPPUNIT_ASSERT_EQUAL(15, PKB::modifies.getModifiesStmt(PKB::variables.getVARIndex("y")).at(5));
	

	vector<VAR> tempvars2 = PKB::modifies.getModifiedByStmt(1);	//threwed error //fixed
	CPPUNIT_ASSERT_EQUAL(0, PKB::modifies.getModifiedByStmt(1).at(0));
	///////////////////////////////////////////////////
	PKB::parent;
	CPPUNIT_ASSERT_EQUAL(2,PKB::parent.getParent(3));//ok
	CPPUNIT_ASSERT_EQUAL(true,PKB::parent.isParent(2,3));//ok
	
	CPPUNIT_ASSERT_EQUAL(3, PKB::parent.getChildren(2).at(0));//ok

	CPPUNIT_ASSERT_EQUAL(true,PKB::parent.isParentStar(10,15));

	vector<STMT> modedata5 = PKB::parent.getParentStar(15);
	CPPUNIT_ASSERT_EQUAL(13,PKB::parent.getParentStar(15).at(0));
	CPPUNIT_ASSERT_EQUAL(10,PKB::parent.getParentStar(15).at(1));

	//13,10
	vector<STMT> modedata6 = PKB::parent.getChildrenStar(10);
	//11-15

	CPPUNIT_ASSERT_EQUAL(11,PKB::parent.getChildrenStar(10).at(0));
	CPPUNIT_ASSERT_EQUAL(12,PKB::parent.getChildrenStar(10).at(1));
	CPPUNIT_ASSERT_EQUAL(13,PKB::parent.getChildrenStar(10).at(2));
	CPPUNIT_ASSERT_EQUAL(14,PKB::parent.getChildrenStar(10).at(3));
	CPPUNIT_ASSERT_EQUAL(15,PKB::parent.getChildrenStar(10).at(4));
	

	///////////////////////////////////////////////////
	int temp2 = PKB::procedures.getPROCIndex("Noob");
	vector<PROC> temp = PKB::calls.getCalledFrom(PKB::procedures.getPROCIndex("Noob"));
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("lain"),PKB::calls.getCalledFrom(PKB::procedures.getPROCIndex("Noob")).at(0));

	vector<PROC> temp4 = PKB::calls.getCalledFromStar(PKB::procedures.getPROCIndex("Noober"));
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("Noob"),PKB::calls.getCalledFromStar(PKB::procedures.getPROCIndex("Noober")).at(1));
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("lain"),PKB::calls.getCalledFromStar(PKB::procedures.getPROCIndex("Noober")).at(0));

	vector<PROC> temp1 = PKB::calls.getCalledBy(PKB::procedures.getPROCIndex("lain"));

	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("Noob"),PKB::calls.getCalledBy(PKB::procedures.getPROCIndex("lain")).at(0));

	vector<PROC> temp3 = PKB::calls.getCalledByStar(PKB::procedures.getPROCIndex("lain"));

	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("Noob"),PKB::calls.getCalledByStar(PKB::procedures.getPROCIndex("lain")).at(0));
	CPPUNIT_ASSERT_EQUAL(PKB::procedures.getPROCIndex("Noober"),PKB::calls.getCalledByStar(PKB::procedures.getPROCIndex("lain")).at(1));

	int zz =0;
	//invalid ans please fix //fixed
	//if (noProcs - 1 >= p)<==inside your getcalledfrom function not sure wat noprocs is for i am guessing its number of procs
	//	answer = optimizedCalledFromTable[p];
	//
	/////////////////////////////////////////////////////////////////


	
}
