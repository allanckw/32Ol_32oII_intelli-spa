#pragma once
#include "ASTNode.h"
#include "ASTExprNode.h"
#include "stdAfx.h"


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
		Plus,
		Minus,
		Times,
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
		NextBip,
		NextBipStar,
		Affects,
		AffectsStar,
		AffectsBip,
		AffectsBipStar,
		PatternModifies,
		PatternSecond,
		PatternThird,
		PatternUses,
		Contains,
		ContainsStar,
		Sibling
	};
	
	static unordered_map<string, unordered_set<QueryRelations>> tokenToRel;
	static unordered_map<string, QueryVar> tokenToVar;
	static unordered_map<QueryVar, set<string>> allowableConditions;
	static unordered_map<string, QueryVar> conditionTypes;
	static unordered_map<QueryVar, bool> formOfASTNode;

	static unordered_map<QueryRelations, set<QueryVar>> allowableFirstArgument;
	static unordered_map<QueryRelations, QueryVar> privilegedFirstArgument;
	static unordered_map<QueryRelations, set<QueryVar>> allowableSecondArgument;
	static unordered_map<QueryRelations, QueryVar> privilegedSecondArgument;
	static unordered_set<QueryRelations> allowableSelfReference;
	static unordered_map<QueryRelations, bool> takesInASTNode;

	static int convertArgumentToInteger(const QueryRelations type,
		const bool first, const string& arg);
	static string convertIntegerToArgument(const QueryVar type,
		const string& condition, const int);
	static unordered_set<ASTNode*> convertIntegerToASTNode(const QueryVar type, const int);

	static bool isExistType(QueryVar var);

	typedef bool(*isRelation)(int, int);
	static isRelation getRelation(QueryRelations rel);
	static unordered_map<QueryRelations, bool> emptyRel;
	
	typedef bool(*isRelation2)(const ASTNode * const, const ASTNode * const);
	static isRelation2 getRelation2(QueryRelations rel);

	typedef vector<int>(*relationFamily)(int);
	static relationFamily getRelationByFamily(QueryRelations rel);
	static relationFamily getRelationFromFamily(QueryRelations rel);

	typedef const vector<ASTNode*>(*relation2Family)(const ASTNode * const);
	static relation2Family getRelation2ByFamily(QueryRelations rel);
	static relation2Family getRelation2FromFamily(QueryRelations rel);

	typedef vector<int>(*getAllTypes)();
	static getAllTypes getType(QueryVar type);

	typedef vector<ASTNode*>(*getAllTypes2)();
	static getAllTypes2 getType2(QueryVar type);

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
	static bool isNext(int x, int y);
	static bool isNextStar(int x, int y);
	static bool isNextBip(int x, int y);
	static bool isNextBipStar(int x, int y);
	static bool isAffects(int x, int y);
	static bool isAffectsStar(int x, int y);
	static bool isAffectsBip(int x, int y);
	static bool isAffectsBipStar(int x, int y);
	static bool isPatternModifies(int x, int y);
	
	static unordered_map<QueryRelations, isRelation2> relation2Map;
	static bool isSibling(const ASTNode * const x, const ASTNode * const y);
	static bool isContains(const ASTNode * const x, const ASTNode * const y);
	static bool isContainsStar(const ASTNode * const x, const ASTNode * const y);
	static bool isPatternSecond(const ASTNode * const x, const ASTNode * const y);
	static bool isPatternThird(const ASTNode * const x, const ASTNode * const y);

	static unordered_map<QueryRelations, relationFamily> relationByMap;
	static vector<int> modifiesStmtBy(int x);
	static vector<int> modifiesProcBy(int x);
	static vector<int> usesStmtBy(int x);
	static vector<int> usesProcBy(int x);
	static vector<int> callsBy(int x);
	static vector<int> callsStarBy(int x);
	static vector<int> followsBy(int x);
	static vector<int> followsStarBy(int x);
	static vector<int> parentBy(int x);
	static vector<int> parentStarBy(int x);
	static vector<int> nextBy(int x);
	static vector<int> nextStarBy(int x);
	static vector<int> nextBipBy(int x);
	static vector<int> nextBipStarBy(int x);
	static vector<int> affectsBy(int x);
	static vector<int> affectsStarBy(int x);
	static vector<int> affectsBipBy(int x);
	static vector<int> affectsBipStarBy(int x);

	static unordered_map<QueryRelations, relation2Family> relation2ByMap;
	static const vector<ASTNode*> containsBy(const ASTNode * const x);
	static const vector<ASTNode*> containsStarBy(const ASTNode * const x);
	static const vector<ASTNode*> siblingBy(const ASTNode * const x);

	static unordered_map<QueryRelations, relationFamily> relationFromMap;
	static vector<int> modifiesStmtFrom(int y);
	static vector<int> modifiesProcFrom(int y);
	static vector<int> usesStmtFrom(int y);
	static vector<int> usesProcFrom(int y);
	static vector<int> callsFrom(int y);
	static vector<int> callsStarFrom(int y);
	static vector<int> followsFrom(int y);
	static vector<int> followsStarFrom(int y);
	static vector<int> parentFrom(int y);
	static vector<int> parentStarFrom(int y);
	static vector<int> nextFrom(int y);
	static vector<int> nextStarFrom(int y);
	static vector<int> nextBipFrom(int y);
	static vector<int> nextBipStarFrom(int y);
	static vector<int> affectsFrom(int y);
	static vector<int> affectsStarFrom(int y);
	static vector<int> affectsBipFrom(int y);
	static vector<int> affectsBipStarFrom(int y);

	static unordered_map<QueryRelations, relation2Family> relation2FromMap;
	static const vector<ASTNode*> containsFrom(const ASTNode * const y);
	static const vector<ASTNode*> containsStarFrom(const ASTNode * const y);

	//static vector<int> getStmtSiblings(int x);

	static unordered_map<QueryVar, getAllTypes> typeMap;
	static vector<int> getAllProc();
	static vector<int> getAllStmtList();
	static vector<int> getAllStmt();
	static vector<int> getAllVar();
	static vector<int> getAllAssign();
	static vector<int> getAllConstant();
	static vector<int> getAllWhile();
	static vector<int> getAllIf();
	static vector<int> getAllCall();
	
	static unordered_map<QueryVar, getAllTypes2> type2Map;
	static vector<ASTNode*> getAllPlusNodes();
	static vector<ASTNode*> getAllMinusNodes();
	static vector<ASTNode*> getAllTimesNodes();

	/*static vector<ASTNode*> getAllVarNodes();
	static vector<ASTNode*> getAllProcNodes();
	static vector<ASTNode*> getAllConstantNodes();
	
	//get all var nodes that has the value of v
	static vector<ASTNode*> getAllVarNodes(VAR v);
	//get all proc nodes that has the value of p
	//static vector<ASTNode*> getAllProcNodes(PROC p);
	//get all const nodes that has the value of c
	static vector<ASTNode*> getAllConstantNodes(int c);

	static ASTNode* getStmtNode(STMT s);
	static vector<ASTNode*> getAllStmtLstNodes();

	static vector<ASTNode*> getAllAssignNodes();
	static vector<ASTNode*> getAllWhileNodes();
	static vector<ASTNode*> getAllIfNodes();
	static vector<ASTNode*> getAllCallNodes();

	//get all call nodes that call procedures p
	static vector<ASTNode*> getAllCallNodes(PROC p);*/
	
public:
	enum PatternLHSType { 
		PLWildcard, PLStringVariable
	};

	enum PatternRHSType { 
		PRWildcard, PRSub, PRNoSub
	};
	
	static bool satisfyPattern(const int index, const RulesOfEngagement::PatternRHSType RHS,
		const string& RHSVarName, const ASTExprNode* RHSexprs);
private:
	static bool TryMatch(ASTNode* testedNode, PatternRHSType RHS, const ASTExprNode* RHSexpr);
	static bool MatcherTree(const ASTNode* Original, const ASTNode* Pattern);//, bool isSub);
};