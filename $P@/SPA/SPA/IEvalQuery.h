#pragma once
#include "stdafx.h"
#include "QueryTreeBuilder.h"

class IEvalQuery
{
private:

	//Copied from previous evaluator.. not sure how to use yet tho -.-
	unordered_map<string, pair<bool, vector<string>>> answer;
	vector<string> intersection(vector<string>, vector<string>);
	vector<string> listfirst(vector<pair<string, string>>);
	vector<string> listsecond(vector<pair<string, string>>);

	//Methods For Evaluating Relationships
	void IEvalQuery::EvaluateModifies();

	void IEvalQuery::EvaluateUses();
	
	void IEvalQuery::EvaluateParent();
	
	void IEvalQuery::EvaluateParentStar();
	
	void IEvalQuery::EvaluateFollows();
	
	void IEvalQuery::EvaluateFollowsStar();

	void IEvalQuery::EvaluateCalls();

	void IEvalQuery::EvaluateCallStar();

	//TODO: FOR CS3202
	//Affects, Affects*
	//Next, Next*
	//Etc..

public:

	static string evaluateQuery(QueryTree qt);
};

