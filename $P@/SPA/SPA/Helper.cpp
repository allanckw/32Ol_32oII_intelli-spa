#pragma once
#include "Helper.h"
#include <sstream>

/**
* Converts an integer to a string
* @param i	  The integer to convert
* @return the converted string
*/
string Helper::intToString(int i)
{
	stringstream ss;
	ss<<i;
	return ss.str();
}

/**
* Converts a character to a string
* @param i	 The character to convert
* @return the converted string
*/
string Helper::charToString(char c)
{
	stringstream ss;
	ss<<c;
	return ss.str();
}

/**
* Converts a string to an integer
* @param s	 The string to convert
* @return the converted integer
*/
int Helper::stringToInt(string s)
{
	int ans = 0;
	for (unsigned int i = 0; i < s.size(); i++)
		ans = ans * 10 + (s[i] - '0');
	return ans;
}

/**
* Check that a string is a number
* @param s	  The string to check
* @return True if it is a number, false otherwise
*/
bool Helper::isNumber(string s)
{
    auto it = s.begin();
    while (it != s.end() && isdigit(*it)) 
		it++;
    return !s.empty() && it == s.end();
}

bool Helper::contains(vector<int> list, int p1)
{
	if(std::find(list.begin(),list.end(), p1) != list.end())
		return true;
	else
		return false;
}