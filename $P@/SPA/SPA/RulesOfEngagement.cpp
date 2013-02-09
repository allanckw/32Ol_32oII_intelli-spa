#pragma once
#include "RulesOfEngagement.h"
#include "PKB.h"
#include "Helper.h"

unordered_map<string, unordered_set<RulesOfEngagement::QueryRelations>>
	RulesOfEngagement::tokenToRel;
unordered_map<string, RulesOfEngagement::QueryVar> RulesOfEngagement::tokenToVar;
unordered_map<RulesOfEngagement::QueryVar, set<string>> RulesOfEngagement::allowableConditions;
unordered_map<string, RulesOfEngagement::QueryVar> RulesOfEngagement::conditionTypes;
unordered_map<RulesOfEngagement::QueryRelations, set<RulesOfEngagement::QueryVar>>
	RulesOfEngagement::allowableFirstArgument;
unordered_map<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>
	RulesOfEngagement::privilegedFirstArgument;
unordered_map<RulesOfEngagement::QueryRelations, set<RulesOfEngagement::QueryVar>>
	RulesOfEngagement::allowableSecondArgument;
unordered_map<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar>
	RulesOfEngagement::privilegedSecondArgument;
unordered_set<RulesOfEngagement::QueryRelations> RulesOfEngagement::allowableSelfReference;
unordered_map<RulesOfEngagement::QueryRelations, bool> RulesOfEngagement::emptyRel;
unordered_map<RulesOfEngagement::QueryRelations, RulesOfEngagement::isRelation>
	RulesOfEngagement::relationMap;
unordered_map<RulesOfEngagement::QueryRelations, RulesOfEngagement::relationFamily>
	RulesOfEngagement::relationByMap;
unordered_map<RulesOfEngagement::QueryRelations, RulesOfEngagement::relationFamily>
	RulesOfEngagement::relationFromMap;
unordered_map<RulesOfEngagement::QueryVar, RulesOfEngagement::getAllTypes> RulesOfEngagement::typeMap;

/**
* Stores all the tables with the relevant information.
*/
void RulesOfEngagement::initialise()
{
	tokenToRel["Calls"].insert(Calls);
	tokenToRel["Calls*"].insert(CallsStar);
	tokenToRel["Modifies"].insert(ModifiesStmt);
	tokenToRel["Modifies"].insert(ModifiesProc);
	tokenToRel["Uses"].insert(UsesStmt);
	tokenToRel["Uses"].insert(UsesProc);
	tokenToRel["Parent"].insert(Parent);
	tokenToRel["Parent*"].insert(ParentStar);
	tokenToRel["Follows"].insert(Follows);
	tokenToRel["Follows*"].insert(FollowsStar);
	tokenToRel["Next"].insert(Next);
	tokenToRel["Next*"].insert(NextStar);
	tokenToRel["Affects"].insert(Affects);
	tokenToRel["Affects*"].insert(AffectsStar);

	tokenToVar["procedure"] = Procedure;
	tokenToVar["stmtLst"] = Statement_List;
	tokenToVar["stmt"] = Statement;
	tokenToVar["assign"] = Assign;
	tokenToVar["call"] = Call;
	tokenToVar["while"] = While;
	tokenToVar["if"] = If;
	tokenToVar["prog_line"] = Statement; 
	tokenToVar["variable"] = Variable;
	tokenToVar["constant"] = Constant;

	allowableConditions[Procedure].insert("procName");
	allowableConditions[Call].insert("procName");
	allowableConditions[Variable].insert("varName");
	allowableConditions[Constant].insert("value");
	allowableConditions[Statement].insert("stmt#");
	allowableConditions[Assign].insert("stmt#");
	allowableConditions[While].insert("stmt#");
	allowableConditions[If].insert("stmt#");
	allowableConditions[Call].insert("stmt#");

	conditionTypes["procName"] = String;
	conditionTypes["varName"] = String;
	conditionTypes["value"] = Integer;
	conditionTypes["stmt#"] = Integer;

	allowableFirstArgument[ModifiesStmt].insert(Statement);
	allowableFirstArgument[ModifiesStmt].insert(Assign);
	allowableFirstArgument[ModifiesStmt].insert(Call);
	allowableFirstArgument[ModifiesStmt].insert(While);
	allowableFirstArgument[ModifiesStmt].insert(If);
	allowableFirstArgument[ModifiesStmt].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(ModifiesStmt, Statement));
	allowableSecondArgument[ModifiesStmt].insert(Variable);
	allowableSecondArgument[ModifiesStmt].insert(WildCard);
	allowableSecondArgument[ModifiesStmt].insert(String);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ModifiesStmt, Variable));

	allowableFirstArgument[ModifiesProc].insert(Procedure);
	allowableFirstArgument[ModifiesProc].insert(String);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(ModifiesProc, Procedure));
	allowableSecondArgument[ModifiesProc].insert(Variable);
	allowableSecondArgument[ModifiesProc].insert(WildCard);
	allowableSecondArgument[ModifiesProc].insert(String);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ModifiesProc, Variable));
	
	allowableFirstArgument[UsesStmt].insert(Statement);
	allowableFirstArgument[UsesStmt].insert(Assign);
	allowableFirstArgument[UsesStmt].insert(Call);
	allowableFirstArgument[UsesStmt].insert(While);
	allowableFirstArgument[UsesStmt].insert(If);
	allowableFirstArgument[UsesStmt].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(UsesStmt, Statement));
	allowableSecondArgument[UsesStmt].insert(Variable);
	allowableSecondArgument[UsesStmt].insert(WildCard);
	allowableSecondArgument[UsesStmt].insert(String);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(UsesStmt, Variable));

	allowableFirstArgument[UsesProc].insert(Procedure);
	allowableFirstArgument[UsesProc].insert(String);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(UsesProc, Procedure));
	allowableSecondArgument[UsesProc].insert(Variable);
	allowableSecondArgument[UsesProc].insert(WildCard);
	allowableSecondArgument[UsesProc].insert(String);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(UsesProc, Variable));

	allowableFirstArgument[Calls].insert(Procedure);
	allowableFirstArgument[Calls].insert(WildCard);
	allowableFirstArgument[Calls].insert(String);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Calls, Procedure));
	allowableSecondArgument[Calls].insert(Procedure);
	allowableSecondArgument[Calls].insert(WildCard);
	allowableSecondArgument[Calls].insert(String);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(Calls, Procedure));

	allowableFirstArgument[CallsStar].insert(Procedure);
	allowableFirstArgument[CallsStar].insert(WildCard);
	allowableFirstArgument[CallsStar].insert(String);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(CallsStar, Procedure));
	allowableSecondArgument[CallsStar].insert(Procedure);
	allowableSecondArgument[CallsStar].insert(WildCard);
	allowableSecondArgument[CallsStar].insert(String);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(CallsStar, Procedure));
	
	allowableFirstArgument[Parent].insert(Statement);
	allowableFirstArgument[Parent].insert(Assign);
	allowableFirstArgument[Parent].insert(Call);
	allowableFirstArgument[Parent].insert(While);
	allowableFirstArgument[Parent].insert(If);
	allowableFirstArgument[Parent].insert(WildCard);
	allowableFirstArgument[Parent].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Parent, Statement));
	allowableSecondArgument[Parent].insert(Statement);
	allowableSecondArgument[Parent].insert(Assign);
	allowableSecondArgument[Parent].insert(Call);
	allowableSecondArgument[Parent].insert(While);
	allowableSecondArgument[Parent].insert(If);
	allowableSecondArgument[Parent].insert(WildCard);
	allowableSecondArgument[Parent].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(Parent, Statement));
	
	allowableFirstArgument[ParentStar].insert(Statement);
	allowableFirstArgument[ParentStar].insert(Assign);
	allowableFirstArgument[ParentStar].insert(Call);
	allowableFirstArgument[ParentStar].insert(While);
	allowableFirstArgument[ParentStar].insert(If);
	allowableFirstArgument[ParentStar].insert(WildCard);
	allowableFirstArgument[ParentStar].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(ParentStar, Statement));
	allowableSecondArgument[ParentStar].insert(Statement);
	allowableSecondArgument[ParentStar].insert(Assign);
	allowableSecondArgument[ParentStar].insert(Call);
	allowableSecondArgument[ParentStar].insert(While);
	allowableSecondArgument[ParentStar].insert(If);
	allowableSecondArgument[ParentStar].insert(WildCard);
	allowableSecondArgument[ParentStar].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ParentStar, Statement));
	
	allowableFirstArgument[Follows].insert(Statement);
	allowableFirstArgument[Follows].insert(Assign);
	allowableFirstArgument[Follows].insert(Call);
	allowableFirstArgument[Follows].insert(While);
	allowableFirstArgument[Follows].insert(If);
	allowableFirstArgument[Follows].insert(WildCard);
	allowableFirstArgument[Follows].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Follows, Statement));
	allowableSecondArgument[Follows].insert(Statement);
	allowableSecondArgument[Follows].insert(Assign);
	allowableSecondArgument[Follows].insert(Call);
	allowableSecondArgument[Follows].insert(While);
	allowableSecondArgument[Follows].insert(If);
	allowableSecondArgument[Follows].insert(WildCard);
	allowableSecondArgument[Follows].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(Follows, Statement));
	
	allowableFirstArgument[FollowsStar].insert(Statement);
	allowableFirstArgument[FollowsStar].insert(Assign);
	allowableFirstArgument[FollowsStar].insert(Call);
	allowableFirstArgument[FollowsStar].insert(While);
	allowableFirstArgument[FollowsStar].insert(If);
	allowableFirstArgument[FollowsStar].insert(WildCard);
	allowableFirstArgument[FollowsStar].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(FollowsStar, Statement));
	allowableSecondArgument[FollowsStar].insert(Statement);
	allowableSecondArgument[FollowsStar].insert(Assign);
	allowableSecondArgument[FollowsStar].insert(Call);
	allowableSecondArgument[FollowsStar].insert(While);
	allowableSecondArgument[FollowsStar].insert(If);
	allowableSecondArgument[FollowsStar].insert(WildCard);
	allowableSecondArgument[FollowsStar].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(FollowsStar, Statement));
	
	allowableFirstArgument[Next].insert(Statement);
	allowableFirstArgument[Next].insert(Assign);
	allowableFirstArgument[Next].insert(Call);
	allowableFirstArgument[Next].insert(While);
	allowableFirstArgument[Next].insert(If);
	allowableFirstArgument[Next].insert(WildCard);
	allowableFirstArgument[Next].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Next, Statement));
	allowableSecondArgument[Next].insert(Statement);
	allowableSecondArgument[Next].insert(Assign);
	allowableSecondArgument[Next].insert(Call);
	allowableSecondArgument[Next].insert(While);
	allowableSecondArgument[Next].insert(If);
	allowableSecondArgument[Next].insert(WildCard);
	allowableSecondArgument[Next].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(Next, Statement));
	
	allowableFirstArgument[NextStar].insert(Statement);
	allowableFirstArgument[NextStar].insert(Assign);
	allowableFirstArgument[NextStar].insert(Call);
	allowableFirstArgument[NextStar].insert(While);
	allowableFirstArgument[NextStar].insert(If);
	allowableFirstArgument[NextStar].insert(WildCard);
	allowableFirstArgument[NextStar].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(NextStar, Statement));
	allowableSecondArgument[NextStar].insert(Statement);
	allowableSecondArgument[NextStar].insert(Assign);
	allowableSecondArgument[NextStar].insert(Call);
	allowableSecondArgument[NextStar].insert(While);
	allowableSecondArgument[NextStar].insert(If);
	allowableSecondArgument[NextStar].insert(WildCard);
	allowableSecondArgument[NextStar].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(NextStar, Statement));
	
	allowableFirstArgument[Affects].insert(Statement);
	allowableFirstArgument[Affects].insert(Assign);
	allowableFirstArgument[Affects].insert(WildCard);
	allowableFirstArgument[Affects].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Affects, Statement));
	allowableSecondArgument[Affects].insert(Statement);
	allowableSecondArgument[Affects].insert(Assign);
	allowableSecondArgument[Affects].insert(WildCard);
	allowableSecondArgument[Affects].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(Affects, Statement));
	
	allowableFirstArgument[AffectsStar].insert(Statement);
	allowableFirstArgument[AffectsStar].insert(Assign);
	allowableFirstArgument[AffectsStar].insert(WildCard);
	allowableFirstArgument[AffectsStar].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(AffectsStar, Statement));
	allowableSecondArgument[AffectsStar].insert(Statement);
	allowableSecondArgument[AffectsStar].insert(Assign);
	allowableSecondArgument[AffectsStar].insert(WildCard);
	allowableSecondArgument[AffectsStar].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(AffectsStar, Statement));
	
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(PatternModifies, Variable));

	allowableSelfReference.insert(Next);

	emptyRel[ModifiesStmt] = PKB::modifies.isEmpty();
	emptyRel[ModifiesProc] = PKB::modifies.isEmpty();
	emptyRel[UsesStmt] = PKB::uses.isEmpty();
	emptyRel[UsesProc] = PKB::uses.isEmpty();
	emptyRel[Calls] = emptyRel[CallsStar] = PKB::calls.isEmpty();
	emptyRel[Follows] = emptyRel[FollowsStar] = PKB::follows.isEmpty();
	emptyRel[Parent] = emptyRel[ParentStar] = PKB::parent.isEmpty();

	relationMap[ModifiesStmt] = &isModifiesStmt;
	relationMap[ModifiesProc] = &isModifiesProc;
	relationMap[UsesStmt] = &isUsesStmt;
	relationMap[UsesProc] = &isUsesProc;
	relationMap[Calls] = &isCalls;
	relationMap[CallsStar] = &isCallsStar;
	relationMap[Follows] = &isFollows;
	relationMap[FollowsStar] = &isFollowsStar;
	relationMap[Parent] = &isParent;
	relationMap[ParentStar] = &isParentStar;
	relationMap[Next] = &isNext;
	relationMap[NextStar] = &isNextStar;
	relationMap[Affects] = &isAffects;
	relationMap[AffectsStar] = &isAffectsStar;
	relationMap[PatternModifies] = &isPatternModifies;

	relationMap[ModifiesStmt] = &isModifiesStmt;
	relationMap[ModifiesProc] = &isModifiesProc;
	relationMap[UsesStmt] = &isUsesStmt;
	relationMap[UsesProc] = &isUsesProc;
	relationMap[Calls] = &isCalls;
	relationMap[CallsStar] = &isCallsStar;
	relationMap[Follows] = &isFollows;
	relationMap[FollowsStar] = &isFollowsStar;
	relationMap[Parent] = &isParent;
	relationMap[ParentStar] = &isParentStar;
	relationMap[Next] = &isNext;
	relationMap[NextStar] = &isNextStar;
	relationMap[Affects] = &isAffects;
	relationMap[AffectsStar] = &isAffectsStar;
	relationMap[PatternModifies] = &isPatternModifies;
	
	typeMap[Statement] = &getAllStmt;
	typeMap[Variable] = &getAllVar;
	typeMap[Procedure] = &getAllProc;
	typeMap[Assign] = &getAllAssign;
	typeMap[Constant] = &getAllConstant;
	typeMap[While] = &getAllWhile;
	typeMap[If] = &getAllIf;
	typeMap[Call] = &getAllCall;
}

/**
* Method that helps to convert an argument to the correct value to be stored in AnswerTable.
* To be used for relations.
* @param type type of relation
* @param first whether the argument is the first or second argument
* @param arg argument to be converted
* @return the integer representing the argument
*/
int RulesOfEngagement::convertArgumentToInteger(const RulesOfEngagement::QueryRelations type,
	const bool first, const string& arg)
{
	if (first) {
		switch (type) {
		case ModifiesStmt:
		case UsesStmt:
			return Helper::stringToInt(arg);
		case ModifiesProc:
		case UsesProc:
		case Calls:
		case CallsStar:
			return PKB::procedures.getPROCIndex(arg.substr(1, arg.length() - 2));
		default:
			return Helper::stringToInt(arg);
		}
	} else {
		switch (type) {
		case ModifiesStmt:
		case ModifiesProc:
		case UsesStmt:
		case UsesProc:
		case PatternModifies:
			return PKB::variables.getVARIndex(arg.substr(1, arg.length() - 2));
		case Calls:
		case CallsStar:
			return PKB::procedures.getPROCIndex(arg.substr(1, arg.length() - 2));
		default:
			return Helper::stringToInt(arg);
		}
	}
}

//relation map
/**
* The reason for the shortness of the code in MultiQueryEval.
* Takes in the relation type and returns a function pointer that can be called to evaluate
* the satisfiablilty of the two arguments.
* @param rel type of relation
* @return the function pointer
*/
RulesOfEngagement::isRelation RulesOfEngagement::getRelation(RulesOfEngagement::QueryRelations rel)
{
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

bool RulesOfEngagement::isPatternModifies(int x, int y)
{
	return PKB::statementNodes[x]->getChild(0)->getValue() == y;
}

bool RulesOfEngagement::isNext(int x, int y)
{
	return PKB::next.isNext(x, y);
}

bool RulesOfEngagement::isNextStar(int x, int y)
{
	return PKB::next.isNextStar(x, y);
}

bool RulesOfEngagement::isAffects(int x, int y)
{
	return PKB::affects.isAffects(x, y);
}

bool RulesOfEngagement::isAffectsStar(int x, int y)
{
	return PKB::affects.isAffectsStar(x, y);
}

/*template

bool RulesOfEngagement::is<Rel>(int x, int y)
{
	return PKB::<table>.is<Rel>(x, y);
}
*/

/**
* The reason for the shortness of the code in MultiQueryEval.
* Takes in the relation type and returns a function pointer that can be called to return
* a list of values that satisfy the relation.
* e.g. getRelationByFamily(rel)(x) -> list of y such that rel(x, y) is true
* @param rel type of relation
* @return the function pointer
*/
RulesOfEngagement::relationFamily RulesOfEngagement::getRelationByFamily(QueryRelations rel)
{
	return relationByMap[rel];
}

vector<int> RulesOfEngagement::modifiesStmtBy(int x)
{
	return PKB::modifies.getModifiedByStmt(x);
}

vector<int> RulesOfEngagement::modifiesProcBy(int x)
{
	return PKB::modifies.getModifiedByProc(x);
}

vector<int> RulesOfEngagement::usesStmtBy(int x)
{
	return PKB::uses.getUsedByStmt(x);
}

vector<int> RulesOfEngagement::usesProcBy(int x)
{
	return PKB::uses.getUsedByProc(x);
}

vector<int> RulesOfEngagement::callsBy(int x)
{
	return PKB::calls.getCalledBy(x);
}

vector<int> RulesOfEngagement::callsStarBy(int x)
{
	return PKB::calls.getCalledByStar(x);
}

vector<int> RulesOfEngagement::followsBy(int x)
{
	return vector<int>(PKB::follows.getFollowsBy(x));
}

vector<int> RulesOfEngagement::followsStarBy(int x)
{
	return PKB::follows.getFollowsStarBy(x);
}

vector<int> RulesOfEngagement::parentBy(int x)
{
	return PKB::parent.getChildren(x);
}

vector<int> RulesOfEngagement::parentStarBy(int x)
{
	return PKB::parent.getChildrenStar(x);
}

vector<int> RulesOfEngagement::nextBy(int x)
{
	return PKB::next.getNext(x);
}

vector<int> RulesOfEngagement::nextStarBy(int x)
{
	return PKB::next.getNextStar(x);
}

vector<int> RulesOfEngagement::affectsBy(int x)
{
	return PKB::affects.getAffectsBy(x);
}

vector<int> RulesOfEngagement::affectsStarBy(int x)
{
	return PKB::affects.getAffectsByStar(x);
}

/**
* The reason for the shortness of the code in MultiQueryEval.
* Takes in the relation type and returns a function pointer that can be called to return
* a list of values that satisfy the relation.
* e.g. getRelationFromFamily(rel)(y) -> list of x such that rel(x, y) is true
* @param rel type of relation
* @return the function pointer
*/
RulesOfEngagement::relationFamily RulesOfEngagement::getRelationFromFamily(QueryRelations rel)
{
	return relationFromMap[rel];
}

vector<int> RulesOfEngagement::modifiesStmtFrom(int y)
{
	return PKB::modifies.getModifiesStmt(y);
}

vector<int> RulesOfEngagement::modifiesProcFrom(int y)
{
	return PKB::modifies.getModifiesProc(y);
}

vector<int> RulesOfEngagement::usesStmtFrom(int y)
{
	return PKB::uses.getUsedInStmt(y);
}

vector<int> RulesOfEngagement::usesProcFrom(int y)
{
	return PKB::uses.getUsedInProc(y);
}

vector<int> RulesOfEngagement::callsFrom(int y)
{
	return PKB::calls.getCalledFrom(y);
}

vector<int> RulesOfEngagement::callsStarFrom(int y)
{
	return PKB::calls.getCalledFromStar(y);
}

vector<int> RulesOfEngagement::followsFrom(int y)
{
	return vector<int>(PKB::follows.getFollowsFrom(y));
}

vector<int> RulesOfEngagement::followsStarFrom(int y)
{
	return PKB::follows.getFollowsStarFrom(y);
}

vector<int> RulesOfEngagement::parentFrom(int y)
{
	return PKB::parent.getChildren(y);
}

vector<int> RulesOfEngagement::parentStarFrom(int y)
{
	return PKB::parent.getChildrenStar(y);
}

vector<int> RulesOfEngagement::nextFrom(int y)
{
	return PKB::next.getPrevious(y);
}

vector<int> RulesOfEngagement::nextStarFrom(int y)
{
	return PKB::next.getNextStar(y);
}

vector<int> RulesOfEngagement::affectsFrom(int y)
{
	return PKB::affects.getAffectsFrom(y);
}

vector<int> RulesOfEngagement::affectsStarFrom(int y)
{
	return PKB::affects.getAffectsFromStar(y);
}
//end relation map

//type map
/**
* Takes in the synonym type and returns a function that when called, will produce all
* values that are of that synonym type, in integer form.
* @param rel type of variable
* @return the function pointer
*/
RulesOfEngagement::getAllTypes RulesOfEngagement::getType(RulesOfEngagement::QueryVar type)
{
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

//pattern
/**
* This method will be used to check whether this assignment is valid
with the pattern being checked
* @param index index of the assingent being checked
* @param RHS the right hand side's type
* @param RHSVarName right hand side's variable name
* @param RHSexprs a right hand side expression tree
* @return true if the assignment satisfied the pattern, and false otherwise
*/
bool RulesOfEngagement::satisfyPattern(const int index, const RulesOfEngagement::PatternRHSType RHS,
	const string& RHSVarName, const ASTExprNode* RHSexprs)
{
	/*static unordered_map<int, unordered_map<string, bool>> map;
	if (map.count(index) > 0 && map[index].count(RHSVarName) > 0)
		return map[index][RHSVarName];*/

	return /*map[index][RHSVarName] = */TryMatch(PKB::assignNodes[index], RHS, RHSexprs);
}

//RHS for now handles patterns in the form of "a" or _"a"_
/**
* this method will return the result if whether an AST node is valid
with a pattern
* @param testedNode the AST node being tested
* @param RHS the right hand side's type
* @param RHSVarName right hand side's variable name
* @param RHSexprs a right hand side expression tree
* @return true if the assignment satisfied the pattern, and false otherwise
*/
bool RulesOfEngagement::TryMatch(ASTNode* testedNode,
	RulesOfEngagement::PatternRHSType RHS, const ASTExprNode* pattern)
{
	ASTNode* head = testedNode->getChild(1);
	stack<ASTNode*> nodesStack;
	nodesStack.push(head);

	if(RHS == RulesOfEngagement::PRSub) {
		while(nodesStack.size() > 0) {
			const ASTNode* tempnode = nodesStack.top();
			nodesStack.pop();

			if (MatcherTree(tempnode,pattern))
				return true; 

			if(tempnode->getType() == ASTNode::Operator) {
				nodesStack.push(tempnode->getChild(1));//add right side in
				nodesStack.push(tempnode->getChild(0));//add left side in
			}
		}
		return false;
	} else
		return MatcherTree(head,pattern);
}

/**
* This method will be used to check whether an expression tree is
identical or is a subtree of another expression tree
* @param Original original expression tree
* @param Original expression tree made from the pattern's expression
* @return true if the two expression trees are the same, and false otherwise
*/
bool RulesOfEngagement::MatcherTree(const ASTNode* Original, const ASTNode* Pattern)//, bool isSub)
{
	return (Original->getType() == Pattern->getType() &&
		Original->getValue() == Pattern->getValue() &&
		(Original->getType() == ASTNode::Constant ||
		Original->getType() == ASTNode::Variable ||
		(Original->getType() == ASTNode::Operator &&
			MatcherTree(Original->getChild(0), Pattern->getChild(0)) &&
			MatcherTree(Original->getChild(1), Pattern->getChild(1)))));
}
//end pattern