#pragma once
#include "StagingArea.h"

void StagingArea::declareSynonym(string synonym, QueryEnums::QueryVar type)
{
	individualType[synonym] = type;
	individualPos[synonym] = individualPos.size();
	individualInit.push_back(false);
}

void StagingArea::stage(string synonym)
{
	switch(individualType[synonym]) {
	/*case Constant
		Procedure,
		Stmt,
		Assign,
		Call,
		While,
		If,
		Variable,*/
	}
}

void StagingArea::initIndividualTable(string synonym, set<int> table)
{
	if (individualPos.count(synonym) == 0)
		throw SPAException("Very weird indeed");
	int pos = individualPos[synonym];
	if (individualInit[pos])
		throw SPAException("Very very weird indeed");
	individualInit[pos] = true;
	individualTables.push_back(table);
}

bool StagingArea::join(string synonym1, string synonym2, vector<int> (*f)(int))
{
	if (productListSet.count(synonym1) > 0 && productListSet.count(synonym2) > 0) { //do intersection
		int index1 = productListSet[synonym1];
		int index2 = productListSet[synonym2];
		unordered_map<int, vector<int>> memoise;
		vector<set<vector<int>>::iterator> toBeDestroyed;
		for (auto it = productTable.begin(); it != productTable.end(); it++) {
			int currentSynonym1 = (*it)[index1];
			if (memoise.count(currentSynonym1) == 0)
				memoise[currentSynonym1] = (*f)(currentSynonym1);
			bool match = false;
			for (auto it2 = memoise[currentSynonym1].begin(); it2 != memoise[currentSynonym1].end(); it2++)
				if ((*it)[index2] == *it2) {
					match = true;
					break;
				}
			if (!match)
				toBeDestroyed.push_back(it);
		}
		for (auto it = toBeDestroyed.rbegin(); it != toBeDestroyed.rend(); it++)
			productTable.erase(*it);
		return true;
	} else if (productListSet.count(synonym1) > 0 || productListSet.count(synonym2) > 0) {
		int index1, index2;
		if (productListSet.count(synonym1) > 0) { //an alternative way would be to have a disjoint set data structure in Staging Area, so we can have multiple productTables. but this might blow up the space
			index1 = productListSet[synonym1];
			if (individualPos.count(synonym2) == 0) {

			} else
				index2 = individualPos[synonym2];
		} else {
			index1 = productListSet[synonym2];
			index2 = individualPos[synonym1];
		}
		unordered_map<int, vector<int>> memoise;
		for (auto it = productTable.begin(); it != productTable.end(); it++) {
			int currentSynonym1 = (*it)[index1];
			if (memoise.count(currentSynonym1) == 0)
				memoise[currentSynonym1] = (*f)(currentSynonym1);
			bool match = false;
			for (auto it2 = memoise[currentSynonym1].begin(); it2 != memoise[currentSynonym1].end(); it2++)
				if ((*it)[index2] == *it2) {
					match = true;
					break;
				}
			if (!match)
				toBeDestroyed.push_back(it);
		}
		set<vector<int>> newProductTable;
		return true;
	} else
		return false;
}