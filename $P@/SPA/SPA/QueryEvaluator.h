#pragma once
#include "stdafx.h"


class QueryEvaluator
{
private: static unordered_map<string, vector<string>> answer;

public:
	QueryEvaluator(void);
	vector<string> intersection(vector<string> v1, vector<string> v2);
	vector<string> returnAnswer();
	~QueryEvaluator(void);
};

