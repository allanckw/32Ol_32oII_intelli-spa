#pragma once
#include "stdafx.h"
#include <set>
#include "QueryPreprocessor.h"
#include "QueryTreeBuilder.h"

class QueryEvaluator
{
private: static unordered_map<string, pair<bool, vector<string>>> answer;
	vector<string> intersection(vector<string>, vector<string>);
	vector<string> listfirst(vector<pair<string, string>>);
	vector<string> listsecond(vector<pair<string, string>>);

public:
	QueryEvaluator(void);
	vector<string> returnAnswer(QueryPreprocessor, QueryTreeBuilder);
	~QueryEvaluator(void);
};

