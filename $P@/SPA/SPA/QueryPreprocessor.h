#pragma once
#include "stdAfx.h"
#include "RulesOfEngagement.h"
class QueryPreprocessor
{
public:
	static void validate(const string& query);
	static string getToken(const string& query, int& pos);
	static string getToken2(string query, int& pos);
	static void matchToken(const string& query, int& pos, const string& match);
};

