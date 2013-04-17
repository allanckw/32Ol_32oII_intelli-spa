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
	for (size_t i = 0; i < s.size(); i++)
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

/**
* Check that p1 contain inside the list
* @param list the list to be check
* @param p1 the integer to be check whether inside the list
* @return True if the list contain p1, false otherwise
*/
bool Helper::contains(vector<int> list, int p1)
{
	if(find(list.begin(),list.end(), p1) != list.end())
		return true;
	else
		return false;
}

string Helper::trim(const string& str, string whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

string Helper::reduce(const string& str, string fill, string whitespace)
{
    // trim first
    auto result = trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != string::npos)
    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}