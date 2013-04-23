#pragma once
#include "Synonym.h"
#include "SPAException.h"

Synonym::Synonym(const string& name, const RulesOfEngagement::QueryVar type)
	: synName(name), synType(type), selected(false), synClassIndex(-1),
	synAttributesSpecific(unordered_map<string, string>()),
	//synAttributesGeneric(
	//unordered_map<string, unordered_map<RulesOfEngagement::QueryVar, string>>()),
	synSelfReference(unordered_set<RulesOfEngagement::QueryRelations>()),
	synRelGenericFirst(unordered_set<RulesOfEngagement::QueryRelations>()),
	synRelSpecificFirst(vector<pair<RulesOfEngagement::QueryRelations, string>>()),
	synRelGenericSecond(unordered_set<RulesOfEngagement::QueryRelations>()),
	synRelSpecificSecond(vector<pair<RulesOfEngagement::QueryRelations, string>>()),
	synPattern(vector<string>())
{
}