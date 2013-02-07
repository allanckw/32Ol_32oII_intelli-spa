#pragma once
#include "StdAfx.h"

class Helper
{
public:
	static string intToString(int i);
	static string charToString(char c);
	static int stringToInt(string s);
	static bool isNumber(string s);
	static bool contains(vector<PROG_LINE> list, PROG_LINE s1);
};

