#pragma once
#include "stdafx.h"

class QueryParser
{
private:
	vector<string> tokens;

public:
	vector<string> tokenize(string);
};