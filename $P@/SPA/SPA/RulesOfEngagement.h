#pragma once
#include "QueryEnums.h"
#include "ASTNode.h"

class RulesOfEngagement
{
public:
	static void initialise();

	enum QueryVar { //variable QueryEnums
		Constant,
		Procedure,
		Statement,
		Assign,
		Call,
		While,
		If,
		Variable,
		ProgLine,
		//Expression,
		WildCard
	};

	enum QueryReladition { //relationship QueryEnums
		Modifies,
		ModifiesStmt,
		ModifiesProc,
		Uses,
		UsesStmt,
		UsesProc,
		Calls,
		CallsStar,
		Parent,
		ParentStar,
		Follows,
		FollowsStar,
		Next,
		NextStar,
		Affects,
		AffectsStar,
		Pattern
	};
	
	static unordered_map<string, QueryVar> tokenToType;
	static unordered_map<QueryVar, set<string>> allowableConditions;
	static unordered_map<QueryReladition, set<QueryVar>> allowableFirstArgument;
	static unordered_map<QueryReladition, set<QueryVar>> allowableSecondArgument;

	typedef bool(*isRelation)(int, int);
	static isRelation getRelation(QueryReladition rel);
	static bool satisfyPattern(int index, int modifiesVar, string usesVar);
private:
	static bool tryMatch(ASTNode* testedNode, string targetVar,vector<string> incCodes, bool isSubsTree);
public:
	typedef vector<int>(*getAllTypes)();
	static getAllTypes getType(QueryVar type);

	static unordered_map<QueryReladition, isRelation> relationMap;
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

	static unordered_map<QueryVar, getAllTypes> typeMap;
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