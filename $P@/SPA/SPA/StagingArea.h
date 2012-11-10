#pragma once
#include "stdafx.h"
#include "SPAException.h"
#include "PKB.h"
#include "QueryEnums.h"

class StagingArea
{
private:
	unordered_map<string, QueryEnums::QueryVar> individualType; //((a1,Assign),(a2,Assign),(a3,Assign))
	unordered_map<string, int> individualPos; //((a1,0),(a2,1),(a3,2))
	vector<bool> individualInit; //(false,true,false)
	vector<set<int>> individualTables; //( (), (1,2,3), () )
	
	vector<string> productList; //(a1,s2,v1)
	unordered_map<string, int> productListSet; //((a1,0),(s2,1),(v1,2))
	set<vector<int>> productTable; //( (1,2,0 = 'x'), (2,3,1 = 'y'), (1,3,2 = 'z') )

	void stage(string synonym);

public:
	int getIndividualIndex(string);
	int getProductIndex(string);

	void declareSynonym(string synonym, QueryEnums::QueryVar type);
	void initIndividualTable(string synonym, set<int> table);
	//void cartesianFirst(int index1, int index2); //full cartesian product - only done one time
	void cartesian(int index); //full cartesian product index to productTable
	//void joinFirst(int index1, int index2, vector<int> (*f)(int));
	bool join(string synonym1, string synonym2, vector<int> (*f)(int));
	set<vector<int>> project(vector<int> indices);
}