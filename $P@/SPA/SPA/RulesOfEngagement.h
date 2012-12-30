#pragma once
#include "QueryEnums.h"

class RulesOfEngagement
{
public:
	typedef bool(*isRelation)(int, int);
	static isRelation getRelation(QueryEnums::QueryReladition rel);
	static bool satisfyPattern(int index, int modifiesVar, string usesVar);

	typedef vector<int>(*getAllTypes)();
	static getAllTypes getType(QueryEnums::QueryVar type);

//private:
	static unordered_map<QueryEnums::QueryReladition, isRelation> relationMap;
	static bool isModifiesStmt(int x, int y);
	static bool isModifiesProc(int x, int y);
	static bool isUsesStmt(int x, int y);
	static bool isUsesProc(int x, int y);
	static bool isCalls(int x, int y);
	static bool isCallsStar(int x, int y);
	static bool isFollows(int x, int y);
	static bool isFollowsStar(int x, int y);
	static bool isParent(int x, int y);
	static bool isParentStar(int x, int y);
	/*template
	static bool is<Rel>(int x, int y);
	*/

	static unordered_map<QueryEnums::QueryVar, getAllTypes> typeMap;
	static vector<int> getAllStmt();
	static vector<int> getAllVar();
	static vector<int> getAllProc();
	static vector<int> getAllAssign();
	static vector<int> getAllConstant();
	static vector<int> getAllWhile();
	static vector<int> getAllIf();
	static vector<int> getAllCall();
	/*template
	static vector<int> getAll<Type>();
	*/
};