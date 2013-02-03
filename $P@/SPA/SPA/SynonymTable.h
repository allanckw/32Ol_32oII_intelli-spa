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
	int getSize() const;

	void insert(const string& name, RulesOfEngagement::QueryVar type);
	vector<string> getAllNames() const;
	bool isInTable(const string& name) const;
	vector<string> getAllOfType(RulesOfEngagement::QueryVar type) const;
	RulesOfEngagement::QueryVar getType(const string&) const;
	int getSynonymIndex(const string& name) const;
	//void changeType(string name, RulesOfEngagement::QueryVar type);

	void setSelected(const string& name);
	vector<string> getAllSelected() const;
	bool isSelected(const string& name) const;

	void putIntoClass(const string& name, int classIndex);
	int getClass(const string& name) const;

	bool setSpecificAttribute(const string& name, const string& condition, const string& attribute);
	//string getSpecificAttribute(string name, string condition) const;
	unordered_map<string, string> getAllSpecificAttributes(const string& name) const;

	void setGenericAttribute(const string& name, const string& ownAttribute,
		RulesOfEngagement::QueryVar otherVariable, const string& otherAttribute);
	unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>
		getAllGenericAttributes(const string& name) const;

	void setSelfReference(const string& name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllSelfReferences(const string& name) const;

	void setFirstGeneric(const string& name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllFirstGeneric(const string& name) const;

	void setFirstSpecific(const string& name,
		RulesOfEngagement::QueryRelations relation, const string& specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>>
		getAllFirstSpecific(const string& name) const;

	void setSecondGeneric(const string& name, RulesOfEngagement::QueryRelations relation);
	unordered_set<RulesOfEngagement::QueryRelations> getAllSecondGeneric(const string& name) const;

	void setSecondSpecific(const string& name,
		RulesOfEngagement::QueryRelations relation, const string& specific);
	vector<pair<RulesOfEngagement::QueryRelations, string>>
		getAllSecondSpecific(const string& name) const;
};