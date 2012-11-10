#pragma once
#include "stdafx.h"
#include "MultiQueryTree.h"
#include "StagingArea.h"
#include "PKB.h"

class MultiQueryEval
{
private:

public:
	vector<string> evaluateQuery(MultiQueryTree);
	void evaluateStarred(vector<vector<int>> *answers, MultiQueryTree reladition);
	void evaluateUnstarred(vector<vector<int>> *answers, MultiQueryTree reladition);

};