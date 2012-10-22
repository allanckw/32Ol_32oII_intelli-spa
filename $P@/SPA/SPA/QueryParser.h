#pragma once
#include "stdafx.h"

class QueryParser
{
private:
	vector<string> tokens;

public:
	void tokenize(string);
	vector<string> getTokens();
};