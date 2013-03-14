#include "TestQueryEvaluator.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PQLAffectsProcessor.h"


//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestQueryEvaluator); // Note 4 

TestQueryEvaluator::TestQueryEvaluator(void)
{
}


TestQueryEvaluator::~TestQueryEvaluator(void)
{
}

void display(vector<int> x)
{
	if (x.size() == 0)
		cout<< "NONE";

	 for (int i = 0; i < x.size(); i++)
		cout << x[i] << " ";

	 cout << endl;
}
void TestQueryEvaluator::TestQueryEvaluatorM()
{  
	PKBController::initializePKB("F:\\3201_3202\\SPA\\source1.txt");
	//doon("assign a; Select BOOLEAN such that Affects(1, 2)"); 
	//doon("assign a; Select BOOLEAN such that Affects(1, 3)"); 
	//doon("assign a; Select BOOLEAN such that Affects(1, 5)"); 
	//doon("assign a; Select BOOLEAN such that Affects(2, 6)"); 
	//doon("assign a; Select BOOLEAN such that Affects(2, 8)"); 
	//doon("assign a; Select BOOLEAN such that Affects(3, 12)"); 
	//doon("assign a; Select BOOLEAN such that Affects(3, 13)"); 
	//doon("Select BOOLEAN such that Affects*(1, 8)"); 
	//doon("Select BOOLEAN such that Affects*(11, 14)"); 
	//doon("Select BOOLEAN such that Affects*(20, 20)"); 
	vector<ASTNode*> x = PKB::varNodes;

	doon("stmt s2,s3,s4,s5;constant c;assign s1,a;while w; if iff; variable v; Select <a,s1,w,iff> such that Affects*(a, s1) and Follows* (s1, s2) and Next*(s2,s3) such that Parent*(s3,s4) and Uses(s2,\"x\") and Modifies (s3,\"z\") pattern a(v,_\"x\"_) pattern w(v,_) pattern iff(v,_,_) with c.value = 0");

}


void TestQueryEvaluator::doon(string s)
{

	cout << s << endl;
	try {
		vector<string> answer = PQLController::evaluateQuery(s);
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
