#include "TestQueryEvaluator.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PQLAffectsProcessor.h"
#include <time.h>


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
	PKBController::initializePKB("F:\\3201_3202\\SPA\\pewpewpew.txt");
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
	ASTNode* x = PKB::rootNode;
	//PKB::uses.displayUsesTables();
	//doon("stmt s2,s3,s4,s5;constant c;assign s1,a;while w; if iff; variable v; Select <a,s1,w,iff> such that Affects*(a, s1) and Follows* (s1, s2) and Next*(s2,s3) such that Parent*(s3,s4) and Uses(s2,\"x\") and Modifies (s3,\"z\") pattern a(v,_\"x\"_) pattern w(v,_) pattern iff(v,_,_) with c.value = 0");
	//doon("assign a; variable v; while w1, w2; select a pattern a(v,_\"k\"_) such that Parent(w1,a) and Follows(w2,w1)");
	//doon("assign a; Select a such that  Uses ( a  ,\"a\")") ;// pattern a ( a , \"l\")");
	//doon("assign a; Select BOOLEAN such that Follows(1,1) or Follows(1,2)");
	//Follows 1,1 is false, Follows (1,2) and Follows (1, 1) is false;
	//doon("assign a; Select BOOLEAN such that Follows(1,1) or                      (Follows(1,2) and Follows(1,  1))");
	//doon("Select <s1,s2> such that Affects*(s1, s2) and Affects*(s3, s4)");
	//doon("stmt s; stmt t; Select s such that Follows(s, 2) or Follows(s, 243)"); // 1 and 103

	//doon("stmt s; stmt t; Select s such that Follows(s, 2) or ((Follows(s, 243) and Follows(1,1)))");//1
	
	//doon("procedure p1,p2; assign a1,a2,a3,a4; Select <p1.procName, a2> such that Contains*(p1,a1) with a1.stmt#=a2.stmt# pattern a2(\"m\",_\"b\"_)");
	//doon("assign a; while w;  Select 	a pattern a(_,_\"z\"_) such that Follows*(w,a)");
	//doon("Select 	a pattern a(_,_\"z\"_) such that Follows*(w,a)");
	const clock_t begin_time = clock();
	//doon("prog_line n; assign a, b; stmt s1,s2; Select n such that Affects*(a,n) and Affects*(b,n) and Affects*(s1,s2) and Next*(6,n) and Next* (n,48) with b.stmt#=1");
	doon("assign a1, a2; Select a2 such that AffectsBip*(1, a2)");

	//std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}


void TestQueryEvaluator::doon(string s)
{

	cout << s << endl;
	try {
		list<string> answer;
		PQLController::evaluateQuery(s, answer);
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
