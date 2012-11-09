#pragma once
#include "stdafx.h"

class StagingArea
{
private:
	vector<string> individualList; //(a1,a2,a3)
	unordered_map<string, int> individualListSet; //((a1,0),(a2,1),(a3,2))
	vector<bool> individualInit; //(false,true,false)
	vector<set<int>> individualTables; //( (), (1,2,3), () )
	
	vector<string> productList; //(a1,s2,v1)
	unordered_map<string, int> productListSet; //((a1,0),(s2,1),(v1,2))
	set<vector<int>> productTable; //( (1,2,'x'), (2,3,'y'), (1,3,'z') )

public:
	int getIndividualIndex(string);
	int getProductIndex(string);

	void initIndividualTable(string var, set<int>);
	void cartesian(int index1, int index2); //full cartesian product - only done one time
	void cartesian(int index); //full cartesian product index to productTable
	void joinFirst(int index1, int index2, vector<int> index1Matches);
	void join(int individualIndex, int productIndex, vector<int> individualIndexMatches);
	set<vector<int>> project(vector<int> indices);
}