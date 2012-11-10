#pragma once
#include "stdAfx.h"

class test
{
private:
	unordered_map<string, vector<int>> variableAnswers;
	vector<string> variableRecord; //keeps track of how many variables to erase values from

public:
	void evaluate();
};

//stmt s1, s2, s3; select s1 such that follows(s1, s2) and parent(s2, s3)

//assume no 2 variables can be of the same name
//vector<int> relatedVariables;
//relatedVariables.push_back(s1);
//relatedVariables.push_back(s2);
//relatedVariables.push_back(s3);
//
//unordered_map<string, vector<int>> variableAnswers;
//
//evaluate follows (s1, s2)
//populate firstvariableanswer, secondvariableanswer
//variableAnswers[s1] = firstvariableanswer
//variableAnswers[s2] = secondvariableanswer -> these things are already a pair, kinda
//
//use variableAnswers[s2]
//to evaluate parent(s2, s3)
//for every false case, delete variableAnswers[s2].at(count) as well as variableAnswers[s1].at(count)
//for every true case, push in s3
//case: if s1 is not a vector of answers but instead a fixed int index: just delete from s2
//case: s1 and s2 are both numbers: true and false
//case: s3 is a number: delete from s2 and s1, but dont add anything else for s3





