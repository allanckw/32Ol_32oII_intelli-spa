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

	allowableSelfReference.insert(NextStar);
	allowableSelfReference.insert(Affects);
	allowableSelfReference.insert(AffectsStar);

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

	relationByMap[ModifiesStmt] = &modifiesStmtBy;
	relationByMap[ModifiesProc] = &modifiesProcBy;
	relationByMap[UsesStmt] = &usesStmtBy;
	relationByMap[UsesProc] = &usesProcBy;
	relationByMap[Calls] = &callsBy;
	relationByMap[CallsStar] = &callsStarBy;
	relationByMap[Follows] = &followsBy;
	relationByMap[FollowsStar] = &followsStarBy;
	relationByMap[Parent] = &parentBy;
	relationByMap[ParentStar] = &parentStarBy;
	relationByMap[Next] = &nextBy;
	relationByMap[NextStar] = &nextStarBy;
	relationByMap[Affects] = &affectsBy;
	relationByMap[AffectsStar] = &affectsStarBy;

	relationFromMap[ModifiesStmt] = &modifiesStmtFrom;
	relationFromMap[ModifiesProc] = &modifiesProcFrom;
	relationFromMap[UsesStmt] = &usesStmtFrom;
	relationFromMap[UsesProc] = &usesProcFrom;
	relationFromMap[Calls] = &callsFrom;
	relationFromMap[CallsStar] = &callsStarFrom;
	relationFromMap[Follows] = &followsFrom;
	relationFromMap[FollowsStar] = &followsStarFrom;
	relationFromMap[Parent] = &parentFrom;
	relationFromMap[ParentStar] = &parentStarFrom;
	relationFromMap[Next] = &nextFrom;
	relationFromMap[NextStar] = &nextStarFrom;
	relationFromMap[Affects] = &affectsFrom;
	relationFromMap[AffectsStar] = &affectsStarFrom;
	
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
/*inline */int RulesOfEngagement::convertArgumentToInteger(const RulesOfEngagement::QueryRelations type,
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

/**
* Method that helps to convert the value stored in AnswerTable to the proper description.
* @param type type of synonym
* @param integer integer to be converted
* @return the string that the integer was representing
*/
/*inline */string RulesOfEngagement::convertIntegerToArgument(
	const RulesOfEngagement::QueryVar type, const int integer)
{
	switch (type) {
	case RulesOfEngagement::Procedure:
		return PKB::procedures.getPROCName(integer);
	case RulesOfEngagement::Variable:
		return PKB::variables.getVARName(integer);
	default:
		return Helper::intToString(integer);
	}
}

/**
* Method that determines if there exists any of assign/while/if in the program.
* @param var type of variable
* @return true if there exists such a variable
*/
bool RulesOfEngagement::isExistType(RulesOfEngagement::QueryVar var)
{
	switch (var) {
	case Assign:
		return (PKB::assignTable.size() > 0);
	case If:
		return (PKB::ifTable.size() > 0);
	case While:
		return (PKB::whileTable.size() > 0);
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
	//return PKB::next.isNext(x, y);
	if (x < 0 || x > PKB::maxProgLines || y < 0 || y > PKB::maxProgLines)
		return false;
	MyCFG* s1 = PKB::bigTable[x];
	if (x < s1->last)
		return y == x + 1;
	MyCFG* child;
	queue<MyCFG*> children;
	children.push(s1);
	while (!children.empty()) {
		MyCFG* curr = children.front();
		children.pop();
		switch (curr->type) {
		case MyCFG::Dummy:
		case MyCFG::Normal:
			child = curr->children.oneChild;
			if (child == NULL)
				break;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				if (y == child->first)
					return true;
			}
			break;
		case MyCFG::While:
			child = curr->children.whileChildren.whileIn;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				if (y == child->first)
					return true;
			}
			child = curr->children.whileChildren.whileOut;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				if (y == child->first)
					return true;
			}
			break;
		case MyCFG::If:
			child = curr->children.ifChildren.ifThen;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				if (y == child->first)
					return true;
			}
			child = curr->children.ifChildren.ifElse;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				if (y == child->first)
					return true;
			}
			break;
		}
	}
	return false;
}

bool RulesOfEngagement::isNextStar(int x, int y)
{
	//return PKB::next.isNextStar(x, y);
	if (x < 0 || x > PKB::maxProgLines || y < 0 || y > PKB::maxProgLines)
		return false;
	const MyCFG* s1 = PKB::bigTable[x];
	if (y > x && y <= s1->last)
		return true;
	const MyCFG* s2 = PKB::bigTable[y];
	if (s1->proc != s2->proc)
		return false;
	if (s1->whileAncestor != NULL && s1->whileAncestor == s2->whileAncestor)
		return true;
	const IntervalList * list = s1->nextList;
	if (list == NULL)
		return false;
	if (y < list->first) {
		list = list->prev;
		while (list != NULL && y < list->first)
			list = list->prev;
		if (list == NULL || y > list->last)
			return false;
		return true;
	} else if (y > list->last) {
		list = list->next;
		while (list != NULL && y > list->last)
			list = list->next;
		if (list == NULL || y < list->first)
			return false;
		return true;
	}
	return true;
}

bool RulesOfEngagement::isAffects(int x, int y)
{
	//return PKB::affects.isAffects(x, y);
	if (x < 0 || x > PKB::maxProgLines || y < 0 || y > PKB::maxProgLines)
		return false;
	if (PKB::assignTable.count(x) == 0 || PKB::assignTable.count(y) == 0)
		//throw SPAException("Both arguments to Affects must be assignments");
		return false; //TODO: double check with cristina

	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(x)[0];

	{
		bool ok = false;
		const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(y);
		for (auto it = usesVar.begin(); it != usesVar.end(); it++)
			if (modifiesVar == *it) {
				ok = true;
				break;
			}
		if (!ok)
			return false;
	}

	MyCFG* s1 = PKB::bigTable[x];

	if (y <= s1->last) {
		for (int i = x + 1; i < y; i++)
			if (PKB::assignTable.count(i) > 0) {
				if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
					return false;
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (modifiesVar == *it)
						return false;
			}
		return true;
	}

	const MyCFG* s2 = PKB::bigTable[y];
	if (s1->proc != s2->proc)
		return false;

	const IntervalList* list = s1->nextList;
	if (list == NULL)
		return false;
	if (y < list->first) {
		list = list->prev;
		while (list != NULL && y < list->first)
			list = list->prev;
		if (list == NULL || y > list->last)
			return false;
	} else if (y > list->last) {
		list = list->next;
		while (list != NULL && y > list->last)
			list = list->next;
		if (list == NULL || y < list->first)
			return false;
	}

	for (int i = x + 1; i <= s1->last; i++)
		if (PKB::assignTable.count(i) > 0) {
			if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
				return false;
		} else if (PKB::callTable.count(i) > 0) {
			const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
			for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
				if (modifiesVar == *it)
					return false;
		}

	queue<MyCFG*> search;
	unordered_set<MyCFG*> seen;

	switch (s1->type) {
	case MyCFG::Normal:
		if (s1->children.oneChild != NULL)
			search.push(s1->children.oneChild);
		break;

	case MyCFG::While:
		if (s1->children.whileChildren.whileIn->first <= y
			&& y < s1->children.whileChildren.whileOut->first)
			search.push(s1->children.whileChildren.whileIn);
		else
			search.push(s1->children.whileChildren.whileOut);
		break;

	case MyCFG::If:
		if (s1->children.ifChildren.ifThen->first <= y
			&& y < s1->children.ifChildren.ifElse->first)
			search.push(s1->children.ifChildren.ifThen);
		else if (s1->children.ifChildren.ifElse->first <= y
			&& y < s1->children.ifChildren.ifLater->first)
			search.push(s1->children.ifChildren.ifElse);
		else {
			search.push(s1->children.ifChildren.ifThen);
			search.push(s1->children.ifChildren.ifElse);
		}
		break;
	}

	while (!search.empty()) {
		MyCFG* currCFG = search.front();
		search.pop();
		if (seen.count(currCFG) > 0)
			continue;
		seen.insert(currCFG);

		if (currCFG->first != 0 && currCFG->modifySet.count(modifiesVar) > 0) {
			if (y <= currCFG->last) {
				for (int i = currCFG->first; i < y; i++) {
					if (PKB::assignTable.count(i) > 0) {
						if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
							return false;
					} else if (PKB::callTable.count(i) > 0) { //interprocedural will change here
						const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
						for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
							if (modifiesVar == *it)
								return false;
					}
				}
				return true;
			}
			break;
		}
		
		switch (currCFG->type) {
		case MyCFG::Normal:
			if (currCFG->children.oneChild != NULL)
				search.push(currCFG->children.oneChild);
			break;

		case MyCFG::While:
			if (currCFG->children.whileChildren.whileIn->first <= y
				&& y < currCFG->children.whileChildren.whileOut->first)
				search.push(currCFG->children.whileChildren.whileIn);
			else
				search.push(currCFG->children.whileChildren.whileOut);
			break;

		case MyCFG::If:
			if (currCFG->children.ifChildren.ifThen->first <= y
				&& y < currCFG->children.ifChildren.ifElse->first)
				search.push(currCFG->children.ifChildren.ifThen);
			else if (currCFG->children.ifChildren.ifElse->first <= y
				&& y < currCFG->children.ifChildren.ifLater->first)
				search.push(currCFG->children.ifChildren.ifElse);
			else {
				search.push(currCFG->children.ifChildren.ifThen);
				search.push(currCFG->children.ifChildren.ifElse);
			}
			break;
		}
	}
	return false; //shouldn't reach this point
}

bool RulesOfEngagement::isAffectsStar(int x, int y)
{
	//return PKB::affects.isAffectsStar(x, y);
	if (x < 0 || x > PKB::maxProgLines || y < 0 || y > PKB::maxProgLines)
		return false;
	if (PKB::assignTable.count(x) == 0 || PKB::assignTable.count(y) == 0)
		//throw SPAException("Both arguments to Affects must be assignments");
		return false; //TODO: double check with cristina

	const vector<VAR>& modifiesVarVector = PKB::modifies.getModifiedByStmt(x);
	unordered_set<VAR> modifiesVarSet(modifiesVarVector.begin(), modifiesVarVector.end());
	const MyCFG* s1 = PKB::bigTable[x];
	const MyCFG* s2 = PKB::bigTable[y];
	if (s1->proc != s2->proc)
		return false;

	if (y <= s1->last && s1->whileAncestor == NULL) {
		if (x >= y)
			return false;
		for (int i = x + 1; i < y; i++)
			if (PKB::assignTable.count(i) > 0) {
				const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
				const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (modifiesVarSet.count(modifiesVar)) { //this statement kills value of some prev
					bool match = false;
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							if (modifiesVar == *it) { //overall effect -> do nothing
								match = true;
								break;
							}
						}
					if (!match) { //kill previous value
						modifiesVarSet.erase(modifiesVar);
						if (modifiesVarSet.size() == 0)
							return false;
					}
				} else { //this statement does not kill anything
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							modifiesVarSet.insert(modifiesVar); //overall effect -> add it in
							break;
						}
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0) {
						modifiesVarSet.erase(*it);
						if (modifiesVarSet.size() == 0)
							return false;
					}
			}
		const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(y);
		for (auto it = usesVar.begin(); it != usesVar.end(); it++)
			if (modifiesVarSet.count(*it) > 0)
				return true;
		return false;
	} else {
		const IntervalList* list = s1->nextList;
		if (list == NULL)
			return false;
		if (y < list->first) {
			list = list->prev;
			while (list != NULL && y < list->first)
				list = list->prev;
			if (list == NULL || y > list->last)
				return false;
		} else if (y > list->last) {
			list = list->next;
			while (list != NULL && y > list->last)
				list = list->next;
			if (list == NULL || y < list->first)
				return false;
		}

		for (int i = x + 1; i <= s1->last; i++)
			if (PKB::assignTable.count(i) > 0) {
				const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
				const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (modifiesVarSet.count(modifiesVar)) { //this statement kills value of some prev
					bool match = false;
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							if (modifiesVar == *it) { //overall effect -> do nothing
								match = true;
								break;
							}
						}
					if (!match) //kill previous value
						modifiesVarSet.erase(modifiesVar);
				} else { //this statement does not kill anything
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							modifiesVarSet.insert(modifiesVar); //overall effect -> add it in
							break;
						}
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0)
						modifiesVarSet.erase(*it);
			}
	}

	queue<pair<MyCFG*, unordered_set<int>>> search;
	unordered_map<MyCFG*, unordered_set<int>> seen;
	
	switch (s1->type) {
	case MyCFG::Normal:
		if (s1->children.oneChild != NULL)
			search.push(pair<MyCFG*, unordered_set<int>>(s1->children.oneChild, modifiesVarSet));
		break;

	case MyCFG::While:
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.whileChildren.whileIn, modifiesVarSet));
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.whileChildren.whileOut, modifiesVarSet));
		break;

	case MyCFG::If:
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.ifChildren.ifThen, modifiesVarSet));
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.ifChildren.ifElse, modifiesVarSet));
		break;
	}

	while (!search.empty()) {
		pair<MyCFG*, unordered_set<int>> currPair = search.front();
		search.pop();
		MyCFG* currCFG = currPair.first;
		unordered_set<int>& currVar = currPair.second;
		if (seen.count(currCFG) > 0) {
			unordered_set<int>& seenVar = seen[currCFG];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				currVar.erase(*it);
			if (currVar.size() == 0)
				continue;
			for (auto it = currVar.begin(); it != currVar.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<MyCFG*, unordered_set<int>>(currCFG, currVar));

		bool step = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (currVar.count(*it) > 0) {
				step = true;
				break;
			}
		if (!step)
			for (auto it = currCFG->useSet.begin(); it != currCFG->useSet.end(); it++)
				if (currVar.count(*it) > 0) {
					step = true;
					break;
				}

		if (step)
			for (int i = currCFG->first; i <= currCFG->last; i++)
				if (PKB::assignTable.count(i) > 0) {
					const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
					const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (currVar.count(modifiesVar)) { //this statement kills value of some prev
						bool match = false;
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								if (modifiesVar == *it) { //overall effect -> do nothing
									if (i == y)
										return true;
									match = true;
									break;
								}
							}
						if (!match) //kill previous value
							currVar.erase(modifiesVar);
					} else { //this statement does not kill anything
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								if (i == y)
									return true;
								currVar.insert(modifiesVar); //overall effect -> add it in
								break;
							}
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
						if (currVar.count(*it) > 0)
							currVar.erase(*it);
				}
		
		switch (currCFG->type) {
		case MyCFG::Normal:
			if (currCFG->children.oneChild != NULL)
				search.push(pair<MyCFG*, unordered_set<int>>(currCFG->children.oneChild, currVar));
			break;

		case MyCFG::While:
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.whileChildren.whileIn, currVar));
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.whileChildren.whileOut, currVar));
			break;

		case MyCFG::If:
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.ifChildren.ifThen, currVar));
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.ifChildren.ifElse, currVar));
			break;
		}
	}
	return false; //shouldn't reach this point
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
	int ans = PKB::follows.getFollowsBy(x);
	if (ans >= 0)
		return vector<int>(1, ans);
	else
		return vector<int>();
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
	//return PKB::next.getNext(x);
	if (x < 0 || x > PKB::maxProgLines)
		return vector<int>();
	MyCFG* s1 = PKB::bigTable[x];
	if (x < s1->last)
		return vector<int>(1, x + 1);
	vector<int> answer;
	MyCFG* child;
	queue<MyCFG*> children;
	children.push(s1);
	while (!children.empty()) {
		MyCFG* curr = children.front();
		children.pop();
		switch (curr->type) {
		case MyCFG::Dummy:
		case MyCFG::Normal:
			child = curr->children.oneChild;
			if (child == NULL)
				break;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			break;
		case MyCFG::While:
			child = curr->children.whileChildren.whileIn;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			child = curr->children.whileChildren.whileOut;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			break;
		case MyCFG::If:
			child = curr->children.ifChildren.ifThen;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			child = curr->children.ifChildren.ifElse;
			if (child->type == MyCFG::Dummy) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			break;
		}
	}
	return answer;
}

vector<int> RulesOfEngagement::nextStarBy(int x)
{
	//return PKB::next.getNextStar(x);
	if (x < 0 || x > PKB::maxProgLines)
		return vector<int>();
	vector<int> answer;
	MyCFG* s1 = PKB::bigTable[x];
	for (int i = x + 1; i <= s1->last; i++)
		answer.push_back(i);
	const IntervalList * node = s1->nextList;
	if (node != NULL) {
		for (int i = node->first; i <= node->last; i++)
			answer.push_back(i);
		const IntervalList * prevNode = node->prev;
		while (prevNode != NULL) {
			for (int i = prevNode->first; i <= prevNode->last; i++)
				answer.push_back(i);
		}
		node = node->next;
		while (node != NULL) {
			for (int i = node->first; i <= node->last; i++)
				answer.push_back(i);
			node = node->next;
		}
	}
	return answer;
}

vector<int> RulesOfEngagement::affectsBy(int x)
{
	//return PKB::affects.getAffectsBy(x);
	if (x < 0 || x > PKB::maxProgLines)
		return vector<int>();
	if (PKB::assignTable.count(x) == 0)
		return vector<int>(); //TODO: double check with cristina

	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(x)[0];
	MyCFG* s1 = PKB::bigTable[x];
	unordered_set<int> answer;

	if (s1->useSet.count(modifiesVar) > 0)
		for (int i = x + 1; i <= s1->last; i++)
			if (PKB::assignTable.count(i) > 0) {
				const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
				for (auto it = stmtUsesVar.begin(); it != stmtUsesVar.end(); it++)
					if (modifiesVar == *it) {
						answer.insert(i);
						break;
					}
				if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
					return vector<int>(answer.begin(), answer.end());
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (modifiesVar == *it)
						return vector<int>(answer.begin(), answer.end());
			}
	
	queue<MyCFG*> search;
	unordered_set<MyCFG*> seen;
	
	switch (s1->type) {
	case MyCFG::Normal:
		if (s1->children.oneChild != NULL)
			search.push(s1->children.oneChild);
		break;

	case MyCFG::While:
		search.push(s1->children.whileChildren.whileIn);
		search.push(s1->children.whileChildren.whileOut);
		break;

	case MyCFG::If:
		search.push(s1->children.ifChildren.ifThen);
		search.push(s1->children.ifChildren.ifElse);
		break;
	}

	while (!search.empty()) {
		s1 = search.front();
		search.pop();
		if (seen.count(s1) > 0)
			continue;
		seen.insert(s1);

		bool continueCFG;
		/*if (s1->first == 0)
			continueCFG = true;
		else */if (s1->modifySet.count(modifiesVar) == 0) {
			if (s1->useSet.count(modifiesVar) > 0)
				for (int i = s1->first; i <= s1->last; i++)
					if (PKB::assignTable.count(i) > 0) {
						const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
						for (auto it = stmtUsesVar.begin(); it != stmtUsesVar.end(); it++)
							if (modifiesVar == *it) {
								answer.insert(i);
								break;
							}
					}
			continueCFG = true;
		} else { //some statement will modify the variable
			if (s1->useSet.count(modifiesVar) > 0)
				for (int i = s1->first; i <= s1->last; i++)
					if (PKB::assignTable.count(i) > 0) {
						const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
						for (auto it = stmtUsesVar.begin(); it != stmtUsesVar.end(); it++)
							if (modifiesVar == *it) {
								answer.insert(i);
								break;
							}
						if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
							break;
					} else if (PKB::callTable.count(i) > 0) {
						bool ok = true;
						const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
						for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
							if (modifiesVar == *it) {
								ok = false;
								break;
							}
						if (!ok)
							break;
					}
			continueCFG = false;
		}

		if (continueCFG)
			switch (s1->type) {
			case MyCFG::Normal:
				if (s1->children.oneChild != NULL)
					search.push(s1->children.oneChild);
				break;

			case MyCFG::While:
				search.push(s1->children.whileChildren.whileIn);
				search.push(s1->children.whileChildren.whileOut);
				break;

			case MyCFG::If:
				search.push(s1->children.ifChildren.ifThen);
				search.push(s1->children.ifChildren.ifElse);
				break;
			}
	}
	return vector<int>(answer.begin(), answer.end());
}

vector<int> RulesOfEngagement::affectsStarBy(int x)
{
	//return PKB::affects.getAffectsByStar(x);
	if (x < 0 || x > PKB::maxProgLines)
		return vector<int>();
	if (PKB::assignTable.count(x) == 0)
		//throw SPAException("Both arguments to Affects must be assignments");
		return vector<int>(); //TODO: double check with cristina

	vector<VAR>& modifiesVarVector = PKB::modifies.getModifiedByStmt(x);
	unordered_set<VAR> modifiesVarSet(modifiesVarVector.begin(), modifiesVarVector.end());
	const MyCFG* s1 = PKB::bigTable[x];
	unordered_set<int> answer;

	for (int i = x + 1; i <= s1->last; i++)
		if (PKB::assignTable.count(i) > 0) {
			const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
			const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
			if (modifiesVarSet.count(modifiesVar)) { //this statement kills value of some prev
				bool match = false;
				for (auto it = usesVar.begin(); it != usesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
						answer.insert(i);
						if (modifiesVar == *it) { //overall effect -> do nothing
							match = true;
							break;
						}
					}
				if (!match) { //kill previous value
					modifiesVarSet.erase(modifiesVar);
					if (modifiesVarSet.size() == 0)
						return vector<int>(answer.begin(), answer.end());
				}
			} else { //this statement does not kill anything
				for (auto it = usesVar.begin(); it != usesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
						answer.insert(i); //overall effect -> add it in
						modifiesVarSet.insert(modifiesVar);
						break;
					}
			}
		} else if (PKB::callTable.count(i) > 0) {
			const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
			for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
				if (modifiesVarSet.count(*it) > 0) {
					modifiesVarSet.erase(*it);
					if (modifiesVarSet.size() == 0)
						return vector<int>(answer.begin(), answer.end());
				}
		}

	queue<pair<MyCFG*, unordered_set<int>>> search;
	unordered_map<MyCFG*, unordered_set<int>> seen;
	
	switch (s1->type) {
	case MyCFG::Normal:
		if (s1->children.oneChild != NULL)
			search.push(pair<MyCFG*, unordered_set<int>>(s1->children.oneChild, modifiesVarSet));
		break;

	case MyCFG::While:
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.whileChildren.whileIn, modifiesVarSet));
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.whileChildren.whileOut, modifiesVarSet));
		break;

	case MyCFG::If:
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.ifChildren.ifThen, modifiesVarSet));
		search.push(pair<MyCFG*, unordered_set<int>>(
			s1->children.ifChildren.ifElse, modifiesVarSet));
		break;
	}

	while (!search.empty()) {
		pair<MyCFG*, unordered_set<int>> currPair = search.front();
		search.pop();
		MyCFG* currCFG = currPair.first;
		unordered_set<int>& currVar = currPair.second;
		if (seen.count(currCFG) > 0) {
			unordered_set<int>& seenVar = seen[currCFG];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				currVar.erase(*it);
			if (currVar.size() == 0)
				continue;
			for (auto it = currVar.begin(); it != currVar.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<MyCFG*, unordered_set<int>>(currCFG, currVar));

		bool step = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (currVar.count(*it) > 0) {
				step = true;
				break;
			}
		if (!step)
			for (auto it = currCFG->useSet.begin(); it != currCFG->useSet.end(); it++)
				if (currVar.count(*it) > 0) {
					step = true;
					break;
				}

		if (step)
			for (int i = currCFG->first; i <= currCFG->last; i++)
				if (PKB::assignTable.count(i) > 0) {
					const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
					const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (currVar.count(modifiesVar)) { //this statement kills value of some prev
						bool match = false;
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								answer.insert(i);
								if (modifiesVar == *it) { //overall effect -> do nothing
									match = true;
									break;
								}
							}
						if (!match) { //kill previous value
							currVar.erase(modifiesVar);
							if (currVar.size() == 0)
								break;
						}
					} else { //this statement does not kill anything
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								answer.insert(i);
								currVar.insert(modifiesVar); //overall effect -> add it in
								break;
							}
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
						if (currVar.count(*it) > 0) {
							currVar.erase(*it);
							if (currVar.size() == 0)
								break;
						}
				}
		
		switch (currCFG->type) {
		case MyCFG::Normal:
			if (currCFG->children.oneChild != NULL)
				search.push(pair<MyCFG*, unordered_set<int>>(currCFG->children.oneChild, currVar));
			break;

		case MyCFG::While:
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.whileChildren.whileIn, currVar));
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.whileChildren.whileOut, currVar));
			break;

		case MyCFG::If:
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.ifChildren.ifThen, currVar));
			search.push(pair<MyCFG*, unordered_set<int>>(
				currCFG->children.ifChildren.ifElse, currVar));
			break;
		}
	}
	return vector<int>(answer.begin(), answer.end());
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
	int ans = PKB::follows.getFollowsFrom(y);
	if (ans >= 0)
		return vector<int>(1, ans);
	else
		return vector<int>();
}

vector<int> RulesOfEngagement::followsStarFrom(int y)
{
	return PKB::follows.getFollowsStarFrom(y);
}

vector<int> RulesOfEngagement::parentFrom(int y)
{
	int ans = PKB::parent.getParent(y);
	if (ans >= 0)
		return vector<int>(1, ans);
	else
		return vector<int>();
}

vector<int> RulesOfEngagement::parentStarFrom(int y)
{
	return PKB::parent.getParentStar(y);
}

vector<int> RulesOfEngagement::nextFrom(int y)
{
	//return PKB::next.getPrevious(y);
	if (y < 0 || y > PKB::maxProgLines)
		return vector<int>();
	MyCFG* s2 = PKB::bigTable[y];
	if (y > s2->first)
		return vector<int>(1, y - 1);
	vector<int> answer;
	MyCFG* parent;
	queue<MyCFG*> parents;
	parents.push(s2);
	while (!parents.empty()) {
		MyCFG* curr = parents.front();
		parents.pop();
		for (auto it = curr->parents.begin(); it != curr->parents.end(); it++) {
			parent = *it;
			switch (parent->type) {
			case MyCFG::Dummy:
				parents.push(parent);
				break;
			default:
				answer.push_back(parent->last);
			}
		}
	}
	return answer;
}

vector<int> RulesOfEngagement::nextStarFrom(int y)
{
	//return PKB::next.getPreviousStar(y);
	if (y < 0 || y > PKB::maxProgLines)
		return vector<int>();
	vector<int> answer;
	MyCFG* s2 = PKB::bigTable[y];
	for (int i = s2->first; i < y; i++)
		answer.push_back(i);
	const IntervalList * node = s2->prevList;
	if (node != NULL) {
		for (int i = node->first; i <= node->last; i++)
			answer.push_back(i);
		const IntervalList * prevNode = node->prev;
		while (prevNode != NULL) {
			for (int i = prevNode->first; i <= prevNode->last; i++)
				answer.push_back(i);
			prevNode = prevNode->prev;
		}
		node = node->next;
		while (node != NULL) {
			for (int i = node->first; i <= node->last; i++)
				answer.push_back(i);
			node = node->next;
		}
	}
	return answer;
}

vector<int> RulesOfEngagement::affectsFrom(int y)
{
	//return PKB::affects.getAffectsFrom(y);
	if (y < 0 || y > PKB::maxProgLines)
		return vector<int>();
	if (PKB::assignTable.count(y) == 0)
		return vector<int>(); //TODO: double check with cristina

	const vector<VAR>& usesVarVector = PKB::uses.getUsedByStmt(y);
	unordered_set<VAR> usesVarSet(usesVarVector.begin(), usesVarVector.end());
	MyCFG* s2 = PKB::bigTable[y];
	unordered_set<int> answer;

	bool toStep = false;
	for (auto it = s2->modifySet.begin(); it != s2->modifySet.end(); it++)
		if (usesVarSet.count(*it) > 0) {
			toStep = true;
			break;
		}
	if (toStep)
		for (int i = y - 1; i >= s2->first; i--)
			if (PKB::assignTable.count(i) > 0) {
				const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (usesVarSet.count(stmtModifiesVar) > 0) {
					answer.insert(i);
					usesVarSet.erase(stmtModifiesVar);
					if (usesVarSet.size() == 0)
						return vector<int>(answer.begin(), answer.end());
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (usesVarSet.count(*it) > 0) {
						usesVarSet.erase(*it);
						if (usesVarSet.size() == 0)
							return vector<int>(answer.begin(), answer.end());
					}
			}
	
	queue<pair<MyCFG*, unordered_set<int>>> search;
	list<pair<MyCFG*, unordered_set<int>>> ifQueue;
	unordered_set<MyCFG*> seen;
	for (auto it = s2->parents.begin(); it != s2->parents.end(); it++)
		search.push(pair<MyCFG*, unordered_set<int>>(*it, usesVarSet));

	while (!(search.empty() && ifQueue.empty())) {
		pair<MyCFG*, unordered_set<int>> currPair;
		MyCFG* currCFG;
		unordered_set<int> usesVar;
		if (search.empty()) {
			currPair = ifQueue.front();
			ifQueue.erase(ifQueue.begin());
			currCFG = currPair.first;
			usesVar = currPair.second;
		} else {
			currPair = search.front();
			search.pop();
			currCFG = currPair.first;
			if (currCFG->type == MyCFG::If) {
				bool match = false;
				for (auto it = ifQueue.begin(); it != ifQueue.end(); it++)
					if (currCFG = it->first) {
						match = true;
						usesVar = currPair.second;
						usesVar.insert(it->second.begin(), it->second.end());
						break;
					}
				if (!match) {
					ifQueue.push_back(currPair);
					continue;
				}
			} else
				usesVar = currPair.second;
		}
		if (seen.count(currCFG) > 0)
			continue;
		seen.insert(currCFG);
		
		bool toStep = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (usesVar.count(*it) > 0) {
				toStep = true;
				break;
			}
		if (toStep) { //some statement will modify some of the used variables
			for (int i = currCFG->last; i >= currCFG->first; i--)
				if (PKB::assignTable.count(i) > 0) {
					const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (usesVar.count(stmtModifiesVar) > 0) {
						answer.insert(i);
						usesVar.erase(stmtModifiesVar);
						if (usesVar.size() == 0)
							break;
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++) {
						if (usesVar.count(*it) > 0) {
							usesVar.erase(*it);
							if (usesVar.size() == 0)
								break;
						}
					}
					if (usesVar.size() == 0)
						break;
				}
		}

		if (usesVar.size() != 0)
			for (auto it = currCFG->parents.begin(); it != currCFG->parents.end(); it++)
				search.push(pair<MyCFG*, unordered_set<int>>(*it, usesVar));
	}
	return vector<int>(answer.begin(), answer.end());
}

vector<int> RulesOfEngagement::affectsStarFrom(int y)
{
	//return PKB::affects.getAffectsFromStar(y);
	if (y < 0 || y > PKB::maxProgLines)
		return vector<int>();
	if (PKB::assignTable.count(y) == 0)
		return vector<int>(); //TODO: double check with cristina

	const vector<VAR>& usesVarVector = PKB::uses.getUsedByStmt(y);
	unordered_set<VAR> usesVarSet(usesVarVector.begin(), usesVarVector.end());
	const MyCFG* s2 = PKB::bigTable[y];
	unordered_set<int> answer;

	bool toStep = false;
		for (auto it = s2->modifySet.begin(); it != s2->modifySet.end(); it++)
			if (usesVarSet.count(*it) > 0) {
				toStep = true;
				break;
			}
	if (toStep)
		for (int i = y - 1; i >= s2->first; i--)
			if (PKB::assignTable.count(i) > 0) {
				const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (usesVarSet.count(stmtModifiesVar) > 0) {
					answer.insert(i);
					usesVarSet.erase(stmtModifiesVar);
					const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
					usesVarSet.insert(stmtUsesVar.begin(), stmtUsesVar.end());
					if (usesVarSet.size() == 0)
						return vector<int>(answer.begin(), answer.end());
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (usesVarSet.count(*it) > 0) {
						usesVarSet.erase(*it);
						if (usesVarSet.size() == 0)
							return vector<int>(answer.begin(), answer.end());
					}
			}

	queue<pair<MyCFG*, unordered_set<int>>> search;
	unordered_map<MyCFG*, unordered_set<int>> seen;
	for (auto it = s2->parents.begin(); it != s2->parents.end(); it++)
		search.push(pair<MyCFG*, unordered_set<int>>(*it, usesVarSet));

	while (!search.empty()) {
		pair<MyCFG*, unordered_set<int>> currPair = search.front();
		search.pop();
		MyCFG* currCFG = currPair.first;
		unordered_set<int>& currVar = currPair.second;
		if (seen.count(currCFG) > 0) {
			unordered_set<int>& seenVar = seen[currCFG];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				currVar.erase(*it);
			if (currVar.size() == 0)
				continue;
			for (auto it = currVar.begin(); it != currVar.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<MyCFG*, unordered_set<int>>(currCFG, currVar));

		bool toStep = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (currVar.count(*it) > 0) {
				toStep = true;
				break;
			}
		if (toStep)
			for (int i = currCFG->last; i >= currCFG->first; i--)
				if (PKB::assignTable.count(i) > 0) {
					const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (currVar.count(stmtModifiesVar) > 0) {
						answer.insert(i);
						currVar.erase(stmtModifiesVar);
						const vector<VAR>& stmtcurrVar = PKB::uses.getUsedByStmt(i);
						currVar.insert(stmtcurrVar.begin(), stmtcurrVar.end());
						if (currVar.size() == 0)
							break;
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++) {
						if (currVar.count(*it) > 0) {
							currVar.erase(*it);
							if (currVar.size() == 0)
								break;
						}
						if (currVar.size() == 0)
							break;
					}
				}
		
		if (currVar.size() != 0)
			for (auto it = currCFG->parents.begin(); it != currCFG->parents.end(); it++)
				search.push(pair<MyCFG*, unordered_set<int>>(*it, currVar));
	}
	return vector<int>(answer.begin(), answer.end());
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

vector<int> RulesOfEngagement::getAllStmtList()
{
	vector<int> answer;
	for (auto it = PKB::statementListTable.begin(); it != PKB::statementListTable.end(); it++)
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