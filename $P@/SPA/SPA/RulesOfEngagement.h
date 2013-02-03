#pragma once
#include "ASTNode.h"
#include "ASTExprNode.h"

class RulesOfEngagement
{
public:
	static void initialise();

	enum QueryVar { //variable QueryEnums
		Procedure,
		Statement_List,
		Statement,
		Assign,
		Call,
		While,
		If,
		Prog_Line,
		Variable,
		Constant,
		WildCard,
		Integer,
		String
	};

	enum QueryRelations { //relationship QueryEnums
		//Modifies,
		ModifiesStmt,
		ModifiesProc,
		//Uses,
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
		PatternModifies,
		PatternUses
	};
	
	static unordered_map<string, unordered_set<QueryRelations>> tokenToRel;
	static unordered_map<string, QueryVar> tokenToVar;
	static unordered_map<QueryVar, set<string>> allowableConditions;
	static unordered_map<string, QueryVar> conditionTypes;
	static unordered_map<QueryRelations, set<QueryVar>> allowableFirstArgument;
	static unordered_map<QueryRelations, QueryVar> privilegedFirstArgument;
	static unordered_map<QueryRelations, set<QueryVar>> allowableSecondArgument;
	static unordered_map<QueryRelations, QueryVar> privilegedSecondArgument;
	static unordered_set<QueryRelations> allowableSelfReference;

	static int convertArgumentToInteger(const QueryRelations type,
		const bool first, const string& arg);

	typedef bool(*isRelation)(int, int);
	static isRelation getRelation(QueryRelations rel);
	static unordered_map<QueryRelations, bool> emptyRel;

	typedef vector<int>(*getAllTypes)();
	static getAllTypes getType(QueryVar type);

private:
	static unordered_map<QueryRelations, isRelation> relationMap;
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
	static bool isPatternModifies(int x, int y);
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
	
public:
	enum PatternLHSType { 
		PLWildcard, PLStringVariable
	};

	enum PatternRHSType { 
		PRWildcard, PRSub, PRNoSub
	};
	
	static bool satisfyPattern(const int index, const RulesOfEngagement::PatternRHSType RHS,
		const string& RHSVarName, const ASTExprNode* RHSexprs);
	/*static bool satisfyPattern(int index, int modifiesVar,
		RulesOfEngagement::PatternRHSType RHS, string RHSVarName, ASTExprNode* RHSexprs);*/
private:
	static bool TryMatch(ASTNode* testedNode, PatternRHSType RHS, const ASTExprNode* RHSexpr);
	static bool MatcherTree(const ASTNode* Original, const ASTNode* Pattern);//, bool isSub);
};