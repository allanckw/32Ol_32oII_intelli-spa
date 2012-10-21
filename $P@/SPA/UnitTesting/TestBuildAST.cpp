#include "TestBuildAST.h"
#include <cppunit/config/SourcePrefix.h>
#include "../SPA/PKB.h"
#include "../SPA/Parser.h"
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestBuildAST); // Note 4 


void TestBuildAST::setUp()
{
}

void TestBuildAST::tearDown()
{
}

void TestBuildAST::testASTBuilder()
{  
	Parser* p = new Parser("C:\\temp\\Source.txt");

	int templine =0;
	int tempindex=0;
	for(int i=0; i<Parser::tokenized_codes.size(); i++)
	{
		vector<string> inner =  Parser::tokenized_codes.at(i);
		for(int j=0; j<inner.size(); j++)
		{
			cout<<inner.at(j)<<" ";
		}
		cout << endl;
	}
	//Parser::tokenized_codes
	//Check if all variables are in the vartable, use testvartable as a reference
	//Check if all procedures are in the proctable, use testproctable as a reference

	//p->buildAST();

	system("PAUSE");
	//Insert assertions here..



}
