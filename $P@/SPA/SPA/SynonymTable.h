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

	//e.g. stmt s; procedure p; select s such that modifies(s, p) -> synRelFirst[s] = {procedure}
	vector<vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>>>
		synRelGenericFirst;

	//e.g. stmt s; select s such that follows(s, 2) -> synRelFirst[s] = {follows,2}
	vector<vector<pair<RulesOfEngagement::QueryRelations, string>>> synRelSpecificFirst;

	vector<vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>>>
		synRelGenericSecond;
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

	void setFirstGeneric(string name, RulesOfEngagement::QueryRelations relation,
		RulesOfEngagement::QueryVar specific);
	vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>>
		getAllFirstGeneric(string name);

	void setFirstSpecific(string name, RulesOfEngagement::QueryRelations relation, string specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>> getAllFirstSpecific(string name);

	void setSecondGeneric(string name, RulesOfEngagement::QueryRelations relation,
		RulesOfEngagement::QueryVar specific);
	vector<pair<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>>
		getAllSecondGeneric(string name);

	void setSecondSpecific(string name, RulesOfEngagement::QueryRelations relation, string specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>> getAllSecondSpecific(string name);
};