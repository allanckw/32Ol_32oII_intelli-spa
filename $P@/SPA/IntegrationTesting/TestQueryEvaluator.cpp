#include "TestQueryEvaluator.h"
#include <cppunit/config/SourcePrefix.h>



//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestQueryEvaluator); // Note 4 

TestQueryEvaluator::TestQueryEvaluator(void)
{
}


TestQueryEvaluator::~TestQueryEvaluator(void)
{
}

void TestQueryEvaluator::TestQueryEvaluatorM()
{  
	PKBController::initializePKB("F:\\3201_3202\\SPA\\Source4.txt");
	doon("prog_line n;  Select n such that Next*(10, n) and Next*(n, 1)"); 
}

void TestQueryEvaluator::doon(string s)
{
	bool a = PKB::next.isNextStar(1, 3);
	bool b = PKB::uses.isUsedStmt(3, 0);
	cout << a << endl << b << endl;;
	
	bool x = PKB::affects.isAffects(1,3);//PKB::affects.isAffects(1,4);
	bool y = PKB::affects.isAffects(9,12);
	//vector<PROG_LINE> y = PKB::next.getPreviousStar(9);
	cout << x << endl << y << endl;;


	cout << s << endl;
	try {
		vector<string> answer = MultiQueryEval::evaluateQuery(s);
		set<string> answer2;
		for (auto it = answer.begin(); it != answer.end(); it++)
			answer2.insert(*it);
		for (auto it = answer2.begin(); it != answer2.end(); it++)
			cout << *it << endl;
		cout << endl;
	} catch (SPAException& e) {
		cout << e.what();
	}
}
