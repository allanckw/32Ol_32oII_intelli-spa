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
	vector<unordered_map<string, string>> synAttributesSpecific;
	vector<unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>>
		synAttributesGeneric;
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
	int size() const;

	void insert(const string& name, RulesOfEngagement::QueryVar type);
	vector<string> getAllNames() const;
	bool isInTable(string name) const;
	vector<string> getAllOfType(RulesOfEngagement::QueryVar type) const;
	RulesOfEngagement::QueryVar getType(string) const;
	int synonymIndex(string name) const;
	void changeType(string name, RulesOfEngagement::QueryVar type);

	void setSelected(string name);
	vector<string> getAllSelected() const;
	bool isSelected(string name) const;

	void putIntoClass(string name, int classIndex);
	int inClass(string name) const;

	bool setSpecificAttribute(string name, string condition, string attribute);
	//string getSpecificAttribute(string name, string condition) const;
	unordered_map<string, string> getAllSpecificAttributes(const string& name) const;

	void setGenericAttribute(string name, string ownAttribute,
		RulesOfEngagement::QueryVar otherVariable, string otherAttribute);
	unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>
		getAllGenericAttributes(const string& name) const;

	void setSelfReference(string name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllSelfReferences(string name) const;

	void setFirstGeneric(string name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllFirstGeneric(string name) const;

	void setFirstSpecific(string name, RulesOfEngagement::QueryRelations relation, string specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>> getAllFirstSpecific(string name) const;

	void setSecondGeneric(string name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllSecondGeneric(string name) const;

	void setSecondSpecific(string name, RulesOfEngagement::QueryRelations relation, string specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>> getAllSecondSpecific(string name) const;
};