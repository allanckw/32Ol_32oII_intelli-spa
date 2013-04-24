#pragma once
#include "stdAfx.h"
#include "RulesOfEngagement.h"
#include "Synonym.h"

class SynonymTable
{
private:
	vector<Synonym> synonyms;

	unordered_map<string, int> stringToIndex;
	unordered_map<RulesOfEngagement::QueryVar, vector<int>> typeToIndices;

	vector<int> selectedIndices;
	
public:
	int getSize() const;

	void insert(const string& name, const RulesOfEngagement::QueryVar type);
	Synonym getSynonym(const string& name) const;
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

	/*void setGenericAttribute(const string& name, const string& ownAttribute,
		RulesOfEngagement::QueryVar otherVariable, const string& otherAttribute);
	unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>
		getAllGenericAttributes(const string& name) const;*/

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

	void setPattern(const string& name, const string& expression);
	vector<string> getAllPattern(const string& name) const;

	void setRelation(const RulesOfEngagement::Relation rel);
	void doneRelation(const RulesOfEngagement::Relation rel);
	void setCondition(const RulesOfEngagement::Condition cond);
	void doneCondition(const RulesOfEngagement::Condition cond);
	bool stillActive(const string& name) const;

	unordered_map<string, string> alias();
};