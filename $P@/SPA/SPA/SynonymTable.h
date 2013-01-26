#pragma once
#include "stdAfx.h"
#include "RulesOfEngagement.h"

class SynonymTable
{
private:
	vector<string> synName;
	vector<RulesOfEngagement::QueryVar> synType;
	vector<bool> selected;
	vector<int> synClassIndex;
	vector<unordered_map<string, string>> synAttributes;
	vector<unordered_set<RulesOfEngagement::QueryRelations>> synSelfReference;

	//e.g. stmt s; Select s such that modifies(s, _)
	//-> stmt s; variable v; Select s such that modifies(s, v) (implicitly)
	//-> synRelGenericFirst[s] = {modifies(, implicit)} //variable is implicit
	vector<unordered_set<RulesOfEngagement::QueryRelations>> synRelGenericFirst;

	//e.g. stmt s; select s such that follows(s, 2) -> synRelFirst[s] = {follows,2}
	vector<vector<pair<RulesOfEngagement::QueryRelations, string>>> synRelSpecificFirst;

	vector<unordered_set<RulesOfEngagement::QueryRelations>> synRelGenericSecond;
	vector<vector<pair<RulesOfEngagement::QueryRelations, string>>> synRelSpecificSecond;

	unordered_map<string, int> stringToIndex;
	unordered_map<RulesOfEngagement::QueryVar, vector<int>> typeToIndices;

	vector<int> selectedIndices;
	
public:
	int size();

	void insert(string name, RulesOfEngagement::QueryVar type);
	vector<string> getAllNames();
	bool isInTable(string name);
	vector<string> getAllOfType(RulesOfEngagement::QueryVar type);
	RulesOfEngagement::QueryVar getType(string);
	int synonymIndex(string name);
	void changeType(string name, RulesOfEngagement::QueryVar type);

	void setSelected(string name);
	vector<string> getAllSelected();
	bool isSelected(string name);

	void putIntoClass(string name, int classIndex);
	int inClass(string name);

	bool setAttribute(string name, string condition, string attribute);
	string getAttribute(string name, string condition);
	unordered_map<string, string> getAllAttributes(string name);

	void setSelfReference(string name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllSelfReferences(string name);

	void setFirstGeneric(string name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllFirstGeneric(string name);

	void setFirstSpecific(string name, RulesOfEngagement::QueryRelations relation, string specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>> getAllFirstSpecific(string name);

	void setSecondGeneric(string name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllSecondGeneric(string name);

	void setSecondSpecific(string name, RulesOfEngagement::QueryRelations relation, string specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>> getAllSecondSpecific(string name);
};