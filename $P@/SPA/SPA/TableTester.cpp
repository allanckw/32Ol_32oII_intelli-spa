//#pragma once
//#include "stdafx.h"
//#include "ASTNode.h"
//#include "StmtNode.h"
//#include "VARTable.h"
//#include "PROCTable.h"
//#include "ExprNode.h"
//#include "Parser.h"
//#include "ModifiesTable.h"
//#include "UsesTable.h"
//#include "CallsTable.h"
//
//using namespace std;
//
//void testCalls();
//void testModifies();
//void testUses();
//
//int main(int argc, char* arg[])
//{
//	//Assuming a program with 5 procedures, statements, variables
//
//	testCalls();
//	cout<<"--------------------------------------------"<<endl;
//	testModifies();
//	cout<<"--------------------------------------------"<<endl;
//	testUses();
//
//	system("PAUSE");
//
//	return 0;
//}
//
//void testModifies()
//{

//	MT.insertStmtModifies(1, 1);
//	MT.insertStmtModifies(1, 2);
//	MT.insertProcModifies(1, 3);
//	MT.insertProcModifies(1, 4);
//	MT.insertStmtModifies(2, 0);
//	MT.insertStmtModifies(2, 3);
//	MT.insertProcModifies(2, 1);
//	MT.insertProcModifies(2, 2);
//	MT.insertStmtModifies(3, 2);
//	MT.insertStmtModifies(3, 3);
//	MT.insertProcModifies(3, 0);
//	MT.insertProcModifies(3, 4);
//	MT.insertStmtModifies(4, 0);
//	MT.insertStmtModifies(4, 3);
//	MT.insertProcModifies(4, 2);
//	MT.insertProcModifies(4, 1);
//	//after insertions are finished, call optimize function
//	MT.optimizeModifiesTable();
//
//	//displaying all the preprocessed stuff
//	MT.displayModifiesStmtTable();
//	MT.displayModifiesProcTable();
//	MT.displayModifiedByStmtTable();
//	MT.displayModifiedByProcTable();
//
//	//Query Tests
//	vector<PROC> test;
//	int count = 0, testStmt = 0, testProc, testVar;
//	test = MT.getModifiedByStmt(testStmt);
//	cout<<"Variables modified by STMT "<<testStmt<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testStmt = 2;
//	count = 0;
//	test = MT.getModifiedByStmt(testStmt);
//	cout<<"Variables modified by STMT "<<testStmt<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 1;
//	count = 0;
//	test = MT.getModifiedByProc(testProc);
//	cout<<"Variables modified by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 3;
//	count = 0;
//	test = MT.getModifiedByProc(testProc);
//	cout<<"Variables modified by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 1;
//	count = 0;
//	test = MT.getModifiesStmt(testVar);
//	cout<<"Statements that modify VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 2;
//	count = 0;
//	test = MT.getModifiesStmt(testVar);
//	cout<<"Statements that modify VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 2;
//	count = 0;
//	test = MT.getModifiesProc(testVar);
//	cout<<"Procedures that modify VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 3;
//	count = 0;
//	test = MT.getModifiesProc(testVar);
//	cout<<"Procedures that modify VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//}
//
//void testUses()
//{
//	cout<<"-Testing Uses Table-"<<endl;
//	UsesTable UT;
//	//insertions
//	UT.insertStmtUses(0, 0);
//	UT.insertStmtUses(0, 2);
//	UT.insertProcUses(0, 3);
//	UT.insertProcUses(0, 4);
//	UT.insertStmtUses(1, 1);
//	UT.insertStmtUses(1, 2);
//	UT.insertProcUses(1, 3);
//	UT.insertProcUses(1, 4);
//	UT.insertStmtUses(2, 0);
//	UT.insertStmtUses(2, 3);
//	UT.insertProcUses(2, 1);
//	UT.insertProcUses(2, 2);
//	UT.insertStmtUses(3, 2);
//	UT.insertStmtUses(3, 3);
//	UT.insertProcUses(3, 0);
//	UT.insertProcUses(3, 4);
//	UT.insertStmtUses(4, 0);
//	UT.insertStmtUses(4, 3);
//	UT.insertProcUses(4, 2);
//	UT.insertProcUses(4, 1);
//	//after insertions are finished, call optimize function
//	UT.optimizeUsesTable();
//
//	//displaying all the preprocessed stuff
//	UT.displayUsedInStmtTable();
//	UT.displayUsedInProcTable();
//	UT.displayUsedByStmtTable();
//	UT.displayUsedByProcTable();
//
//	//Query Tests
//	vector<PROC> test;
//	int count = 0, testStUT = 0, testProc, testVar;
//	test = UT.getUsedByStmt(testStUT);
//	cout<<"Variables modified by STMT "<<testStUT<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testStUT = 2;
//	count = 0;
//	test = UT.getUsedByStmt(testStUT);
//	cout<<"Variables used by STMT "<<testStUT<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 1;
//	count = 0;
//	test = UT.getUsedByProc(testProc);
//	cout<<"Variables used by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 3;
//	count = 0;
//	test = UT.getUsedByProc(testProc);
//	cout<<"Variables used by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 1;
//	count = 0;
//	test = UT.getUsedInStmt(testVar);
//	cout<<"Statements that use VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 2;
//	count = 0;
//	test = UT.getUsedInStmt(testVar);
//	cout<<"Statements that use VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 2;
//	count = 0;
//	test = UT.getUsedInProc(testVar);
//	cout<<"Procedures that use VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testVar = 3;
//	count = 0;
//	test = UT.getUsedInProc(testVar);
//	cout<<"Procedures that use VAR "<<testVar<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//}
//
//void testCalls()
//{
//	cout<<"-Testing Calls Table-"<<endl;
//	CallsTable CT;
//	//insertions
//	CT.insertCalls(0, 1);
//	CT.insertCalls(0, 2);
//	CT.insertCalls(0, 4);
//	CT.insertCalls(1, 2);
//	CT.insertCalls(1, 3);
//	CT.insertCalls(1, 0);
//	CT.insertCalls(2, 1);
//	CT.insertCalls(2, 3);
//	CT.insertCalls(2, 4);
//	CT.insertCalls(3, 0);
//	CT.insertCalls(3, 2);
//	CT.insertCalls(3, 4);
//	CT.insertCalls(4, 1);
//	CT.insertCalls(4, 2);
//	CT.insertCalls(4, 3);
//	//after insertions are finished, call optimize function
//	CT.optimizeCallsTable();
//
//	//displaying all the preprocessed stuff
//	CT.displayCallsTable();
//	CT.displayCalledByTable();
//	CT.displayCalledFromTable();
//	CT.displayCalledByStarTable();
//	CT.displayCalledFromStarTable();
//
//	//Query Tests
//	vector<PROC> test;
//	int count = 0, testProc = 0;
//	test = CT.getCalledBy(testProc);
//	cout<<"Procedures called by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 2;
//	count = 0;
//	test = CT.getCalledBy(testProc);
//	cout<<"Procedures called by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 1;
//	count = 0;
//	test = CT.getCalledFrom(testProc);
//	cout<<"Procedures that call PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 3;
//	count = 0;
//	test = CT.getCalledFrom(testProc);
//	cout<<"Procedures that call PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 1;
//	count = 0;
//	test = CT.getCalledByStar(testProc);
//	cout<<"Procedures called* by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 4;
//	count = 0;
//	test = CT.getCalledByStar(testProc);
//	cout<<"Procedures called* by PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 2;
//	count = 0;
//	test = CT.getCalledFromStar(testProc);
//	cout<<"Procedures that call* PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//
//	testProc = 4;
//	count = 0;
//	test = CT.getCalledFromStar(testProc);
//	cout<<"Procedures that call* PROC "<<testProc<<": ";
//	while (count < test.size())
//	{
//		cout<<test.at(count)<<" ";
//		count++;
//	}
//	cout<<endl;
//}