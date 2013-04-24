#pragma once
#include "stdAfx.h"
#include "RulesOfEngagement.h"

class Synonym
{
public:
	string synName;
	RulesOfEngagement::QueryVar synType;
	bool selected;
	int synClassIndex;
	unordered_map<string, string> synAttributesSpecific;
	//unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>> synAttributesGeneric;
	unordered_set<RulesOfEngagement::QueryRelations> synSelfReference;

	//e.g. stmt s; Select s such that modifies(s, _)
	//-> stmt s; variable v; Select s such that modifies(s, v) (implicitly)
	//-> synRelGenericFirst[s] = {modifies(, implicit)} //variable is implicit
	unordered_set<RulesOfEngagement::QueryRelations> synRelGenericFirst;

	//e.g. stmt s; select s such that follows(s, 2) -> synRelFirst[s] = {follows,2}
	vector<pair<RulesOfEngagement::QueryRelations, string>> synRelSpecificFirst;

	unordered_set<RulesOfEngagement::QueryRelations> synRelGenericSecond;
	vector<pair<RulesOfEngagement::QueryRelations, string>> synRelSpecificSecond;

	vector<RulesOfEngagement::Relation> relations;
	vector<RulesOfEngagement::Condition> conditions;
	vector<string> synPattern;
	
	Synonym(const string& name, const RulesOfEngagement::QueryVar type);
};