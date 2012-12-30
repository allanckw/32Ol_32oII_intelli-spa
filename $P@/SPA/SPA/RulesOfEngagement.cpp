#pragma once
#include "RulesOfEngagement.h"
#include "PKB.h"

typedef bool(*isRelation)(int, int);
typedef vector<int>(*getAllTypes)();
unordered_map<QueryEnums::QueryReladition, isRelation> RulesOfEngagement::relationMap;
unordered_map<QueryEnums::QueryVar, getAllTypes> RulesOfEngagement::typeMap;

//relation map
RulesOfEngagement::isRelation RulesOfEngagement::getRelation(QueryEnums::QueryReladition rel)
{
	if (relationMap.empty()) {
		relationMap[QueryEnums::ModifiesStmt] = &isModifiesStmt;
		relationMap[QueryEnums::ModifiesProc] = &isModifiesProc;
		relationMap[QueryEnums::UsesStmt] = &isUsesStmt;
		relationMap[QueryEnums::UsesProc] = &isUsesProc;
		relationMap[QueryEnums::Calls] = &isCalls;
		relationMap[QueryEnums::CallsStar] = &isCallsStar;
		relationMap[QueryEnums::Follows] = &isFollows;
		relationMap[QueryEnums::FollowsStar] = &isFollowsStar;
		relationMap[QueryEnums::Parent] = &isParent;
		relationMap[QueryEnums::ParentStar] = &isParentStar;
		/*relationMap[QueryEnums::Next] = &is<>;
		relationMap[QueryEnums::NextStar] = &is<>;
		relationMap[QueryEnums::Affects] = &is<>;
		relationMap[QueryEnums::AffectsStar] = &is<>;
		relationMap[QueryEnums::Pattern] = &is<>;*/
	}

	return relationMap[rel];
}

bool RulesOfEngagement::isModifiesStmt(int x, int y)
{
	return PKB::modifies.isModifiedStmt(x, y);
}

bool RulesOfEngagement::isModifiesProc(int x, int y)
{
	return PKB::modifies.isModifiedProc(x, y);
}

bool RulesOfEngagement::isUsesStmt(int x, int y)
{
	return PKB::uses.isUsedStmt(x, y);
}

bool RulesOfEngagement::isUsesProc(int x, int y)
{
	return PKB::uses.isUsedProc(x, y);
}

bool RulesOfEngagement::isCalls(int x, int y)
{
	return PKB::calls.isCalled(x, y);
}

bool RulesOfEngagement::isCallsStar(int x, int y)
{
	return PKB::calls.isCalledStar(x, y);
}

bool RulesOfEngagement::isFollows(int x, int y)
{
	return PKB::follows.isFollows(x, y);
}

bool RulesOfEngagement::isFollowsStar(int x, int y)
{
	return PKB::follows.isFollowsStar(x, y);
}

bool RulesOfEngagement::isParent(int x, int y)
{
	return PKB::parent.isParent(x, y);
}

bool RulesOfEngagement::isParentStar(int x, int y)
{
	return PKB::parent.isParentStar(x, y);
}

/*template

bool RulesOfEngagement::is<Rel>(int x, int y)
{
	return PKB::<table>.is<Rel>(x, y);
}
*/
//end relation map

bool RulesOfEngagement::satisfyPattern(int index, int modifiesVar, string usesVar)
{
	return isModifiesStmt(index, modifiesVar)
		&& isUsesStmt(index, PKB::variables.getVARIndex(usesVar)); //to be changed
}

//type map
RulesOfEngagement::getAllTypes RulesOfEngagement::getType(QueryEnums::QueryVar type)
{
	if (typeMap.empty()) {
		typeMap[QueryEnums::Stmt] = &getAllStmt;
		typeMap[QueryEnums::Variable] = &getAllVar;
		typeMap[QueryEnums::Procedure] = &getAllProc;
		typeMap[QueryEnums::Assign] = &getAllAssign;
		typeMap[QueryEnums::Constant] = &getAllConstant;
		typeMap[QueryEnums::While] = &getAllWhile;
		typeMap[QueryEnums::If] = &getAllIf;
		typeMap[QueryEnums::Call] = &getAllCall;
	}

	return typeMap[type];
}

vector<int> RulesOfEngagement::getAllStmt()
{
	int maxProgLines = PKB::maxProgLines;
	vector<int> answer;
	for (int i = 1; i <= maxProgLines; i++)
		answer.push_back(i);
	return answer;
}

vector<int> RulesOfEngagement::getAllVar()
{
	int maxVar = PKB::variables.getSize();
	vector<int> answer;
	for (int i = 0; i < maxVar; i++)
		answer.push_back(i);
	return answer;
}

vector<int> RulesOfEngagement::getAllProc()
{
	int maxProcs = PKB::procedures.getSize();
	vector<int> answer;
	for (int i = 0; i < maxProcs; i++)
		answer.push_back(i);
	return answer;
}

vector<int> RulesOfEngagement::getAllAssign()
{
	vector<int> answer;
	for (auto it = PKB::assignTable.begin(); it != PKB::assignTable.end(); it++)
		answer.push_back(*it);
	return answer;
}

vector<int> RulesOfEngagement::getAllConstant()
{
	vector<int> answer;
	for (auto it = PKB::constantsTable.begin(); it != PKB::constantsTable.end(); it++)
		answer.push_back((*it).first);
	return answer;
}

vector<int> RulesOfEngagement::getAllWhile()
{
	vector<int> answer;
	for (auto it = PKB::whileTable.begin(); it != PKB::whileTable.end(); it++)
		answer.push_back(*it);
	return answer;
}

vector<int> RulesOfEngagement::getAllIf()
{
	vector<int> answer;
	for (auto it = PKB::ifTable.begin(); it != PKB::ifTable.end(); it++)
		answer.push_back(*it);
	return answer;
}

vector<int> RulesOfEngagement::getAllCall()
{
	vector<int> answer;
	for (auto it = PKB::callTable.begin(); it != PKB::callTable.end(); it++)
		answer.push_back(*it);
	return answer;
}
/*template

vector<int> RulesOfEngagement::getAll<Type>()
{
	int max<> = PKB::<>;
	vector<int> answer;
	for (int i = 1; i <= max<>; i++)
		answer.push_back(i);
	return answer;
}
*/
//end type map