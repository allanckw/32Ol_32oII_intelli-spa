#pragma once
#include "stdafx.h"
#include <set>
#include "PalletTown.h"

class QueryPreprocessor
{
private:
	set<string> keywords;
	set<string> relationshipPalletTown;
	set<string> variablePalletTown;
	set<string> conditionAttributePalletTown;
	unordered_map<int, vector<string>> userVariables;
	unordered_map<int, vector<string>> selectVariables;
	unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>>>> relationships;
	unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, string>>> conditions;
	
public:
	QueryPreprocessor();
	void preProcess(vector<string>);
	bool isName(string);
	unordered_map<int, vector<string>> getUserVariables();
	unordered_map<int, vector<string>> getSelectVariables();
	unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>>>> 
		getRelationships();
	unordered_map<int, vector<pair<pair<PalletTown::ashKetchum, string>, string>>> getConditions();
};