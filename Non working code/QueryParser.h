#pragma once
#include "stdafx.h"

class QueryParser
{
private:
	static vector<string> tokens;

public:
	static vector<string> tokenize(string);
};