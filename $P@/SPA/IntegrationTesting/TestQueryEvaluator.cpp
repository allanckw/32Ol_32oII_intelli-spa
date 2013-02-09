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
	doon("prog_line n2;  Select n2 such that Next(9, n2)"); 
}

void TestQueryEvaluator::doon(string s)
{
	
	vector<PROG_LINE> x = PKB::next.getNextBy(9);
	


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
