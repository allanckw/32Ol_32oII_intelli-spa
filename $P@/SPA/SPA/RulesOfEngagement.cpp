#pragma once
#include "RulesOfEngagement.h"
#include "PKB.h"
#include "Helper.h"

unordered_map<string, RulesOfEngagement::QueryRelations> RulesOfEngagement::tokenToRel;
unordered_map<string, RulesOfEngagement::QueryVar> RulesOfEngagement::tokenToVar;
unordered_map<RulesOfEngagement::QueryVar, set<string>> RulesOfEngagement::allowableConditions;
unordered_map<string, RulesOfEngagement::QueryVar> RulesOfEngagement::conditionTypes;
unordered_map<RulesOfEngagement::QueryRelations, set<RulesOfEngagement::QueryVar>> RulesOfEngagement::allowableFirstArgument;
unordered_map<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar> RulesOfEngagement::privilegedFirstArgument;
unordered_map<RulesOfEngagement::QueryRelations, set<RulesOfEngagement::QueryVar>> RulesOfEngagement::allowableSecondArgument;
unordered_map<RulesOfEngagement::QueryRelations, RulesOfEngagement::QueryVar> RulesOfEngagement::privilegedSecondArgument;
unordered_set<RulesOfEngagement::QueryRelations> RulesOfEngagement::allowableSelfReference;
typedef bool(*isRelation)(int, int);
typedef vector<int>(*getAllTypes)();
unordered_map<RulesOfEngagement::QueryRelations, bool> RulesOfEngagement::emptyRel;
unordered_map<RulesOfEngagement::QueryRelations, isRelation> RulesOfEngagement::relationMap;
unordered_map<RulesOfEngagement::QueryVar, getAllTypes> RulesOfEngagement::typeMap;

void RulesOfEngagement::initialise()
{
	tokenToRel["Calls"] = Calls;
	tokenToRel["Calls*"] = CallsStar;
	tokenToRel["Modifies"] = Modifies;
	tokenToRel["Uses"] = Uses;
	tokenToRel["Parent"] = Parent;
	tokenToRel["Parent*"] = ParentStar;
	tokenToRel["Follows"] = Follows;
	tokenToRel["Follows*"] = FollowsStar;
	tokenToRel["Next"] = Next;
	tokenToRel["Next*"] = NextStar;
	tokenToRel["Affects"] = Affects;
	tokenToRel["Affects*"] = AffectsStar;

	tokenToVar["procedure"] = Procedure;
	tokenToVar["stmt"] = Statement;
	tokenToVar["prog_line"] = Statement; //Lame, but prog_line and statement are interchangable
	tokenToVar["assign"] = Assign;
	tokenToVar["constant"] = Constant;
	tokenToVar["while"] = While;
	tokenToVar["variable"] = Variable;
	tokenToVar["call"] = Call;
	tokenToVar["if"] = If;

	allowableConditions[Procedure].insert("procName");
	allowableConditions[Call].insert("procName");
	allowableConditions[Variable].insert("varName");
	allowableConditions[Constant].insert("value");
	allowableConditions[Statement].insert("stmtNo");
	allowableConditions[Assign].insert("stmtNo");
	allowableConditions[While].insert("stmtNo");
	allowableConditions[If].insert("stmtNo");
	allowableConditions[Call].insert("stmtNo");

	conditionTypes["procName"] = String;
	conditionTypes["varName"] = String;
	conditionTypes["value"] = Integer;
	conditionTypes["stmtNo"] = Integer;

	allowableFirstArgument[Modifies].insert(Procedure);
	allowableFirstArgument[Modifies].insert(Statement);
	allowableFirstArgument[Modifies].insert(Assign);
	allowableFirstArgument[Modifies].insert(Call);
	allowableFirstArgument[Modifies].insert(While);
	allowableFirstArgument[Modifies].insert(If);
	allowableFirstArgument[Modifies].insert(Integer);
	allowableFirstArgument[Modifies].insert(String);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(ModifiesStmt, Statement));
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(ModifiesProc, Procedure));
	allowableSecondArgument[Modifies].insert(Variable);
	allowableSecondArgument[Modifies].insert(WildCard);
	allowableSecondArgument[Modifies].insert(String);
	/*allowableSecondArgument[ModifiesStmt].insert(Variable);
	allowableSecondArgument[ModifiesStmt].insert(WildCard);
	allowableSecondArgument[ModifiesProc].insert(Variable);
	allowableSecondArgument[ModifiesProc].insert(WildCard);*/
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ModifiesStmt, Variable));
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ModifiesProc, Variable));
	
	allowableFirstArgument[Uses].insert(Procedure);
	allowableFirstArgument[Uses].insert(Statement);
	allowableFirstArgument[Uses].insert(Assign);
	allowableFirstArgument[Uses].insert(Call);
	allowableFirstArgument[Uses].insert(While);
	allowableFirstArgument[Uses].insert(If);
	allowableFirstArgument[Uses].insert(Integer);
	allowableFirstArgument[Uses].insert(String);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(UsesStmt, Statement));
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(UsesProc, Procedure));
	allowableSecondArgument[Uses].insert(Variable);
	allowableSecondArgument[Uses].insert(WildCard);
	allowableSecondArgument[Uses].insert(String);
	/*allowableSecondArgument[UsesStmt].insert(Variable);
	allowableSecondArgument[UsesStmt].insert(WildCard);
	allowableSecondArgument[UsesProc].insert(Variable);
	allowableSecondArgument[UsesProc].insert(WildCard);*/
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(UsesStmt, Variable));
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
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Parent, Statement));
	allowableSecondArgument[NextStar].insert(Statement);
	allowableSecondArgument[NextStar].insert(Assign);
	allowableSecondArgument[NextStar].insert(Call);
	allowableSecondArgument[NextStar].insert(While);
	allowableSecondArgument[NextStar].insert(If);
	allowableSecondArgument[NextStar].insert(WildCard);
	allowableSecondArgument[NextStar].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ParentStar, Statement));
	
	allowableFirstArgument[Affects].insert(Statement);
	allowableFirstArgument[Affects].insert(Assign);
	allowableFirstArgument[Affects].insert(WildCard);
	allowableFirstArgument[Affects].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Parent, Statement));
	allowableSecondArgument[Affects].insert(Statement);
	allowableSecondArgument[Affects].insert(Assign);
	allowableSecondArgument[Affects].insert(WildCard);
	allowableSecondArgument[Affects].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ParentStar, Statement));
	
	allowableFirstArgument[AffectsStar].insert(Statement);
	allowableFirstArgument[AffectsStar].insert(Assign);
	allowableFirstArgument[AffectsStar].insert(WildCard);
	allowableFirstArgument[AffectsStar].insert(Integer);
	privilegedFirstArgument.insert(pair<QueryRelations, QueryVar>(Parent, Statement));
	allowableSecondArgument[AffectsStar].insert(Statement);
	allowableSecondArgument[AffectsStar].insert(Assign);
	allowableSecondArgument[AffectsStar].insert(WildCard);
	allowableSecondArgument[AffectsStar].insert(Integer);
	privilegedSecondArgument.insert(pair<QueryRelations, QueryVar>(ParentStar, Statement));

	allowableSelfReference.insert(Next);

	emptyRel[Modifies] = PKB::modifies.empty();
	emptyRel[Uses] = PKB::uses.empty();
	emptyRel[Calls] = emptyRel[CallsStar] = PKB::calls.empty();
	emptyRel[Follows] = emptyRel[FollowsStar] = PKB::follows.empty();
	emptyRel[Parent] = emptyRel[ParentStar] = PKB::parent.empty();

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
	/*relationMap[Next] = &is<>;
	relationMap[NextStar] = &is<>;
	relationMap[Affects] = &is<>;
	relationMap[AffectsStar] = &is<>;
	relationMap[Pattern] = &is<>;*/
	
	typeMap[Statement] = &getAllStmt;
	typeMap[Variable] = &getAllVar;
	typeMap[Procedure] = &getAllProc;
	typeMap[Assign] = &getAllAssign;
	typeMap[Constant] = &getAllConstant;
	typeMap[While] = &getAllWhile;
	typeMap[If] = &getAllIf;
	typeMap[Call] = &getAllCall;
}

int RulesOfEngagement::convertArgumentToInteger(RulesOfEngagement::QueryRelations& type,
	const bool first, const string& arg)
{
	if (first) {
		switch (type) {
		case Modifies:
			if (Helper::isNumber(arg)) {
				type = ModifiesStmt;
				return Helper::stringToInt(arg);
			} else {
				type = ModifiesProc;
				return PKB::procedures.getPROCIndex(arg);
			}
		case Uses:
			if (Helper::isNumber(arg)) {
				type = UsesStmt;
				return Helper::stringToInt(arg);
			} else {
				type = UsesProc;
				return PKB::procedures.getPROCIndex(arg.substr(1, arg.length() - 2));
			}
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

/*template

bool RulesOfEngagement::is<Rel>(int x, int y)
{
	return PKB::<table>.is<Rel>(x, y);
}
*/
//end relation map

//type map
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
bool RulesOfEngagement::satisfyPattern(int index, int modifiesVar,
	RulesOfEngagement::PatternRHSType RHS, string RHSVarName, ASTExprNode* RHSexprs)
{
	if (modifiesVar >= 0 && !isModifiesStmt(index, modifiesVar))
		return false;

	static unordered_map<int, unordered_map<string, bool>> map;
	if (map.count(index) > 0 && map[index].count(RHSVarName) > 0)
		return map[index][RHSVarName];

	return map[index][RHSVarName] = TryMatch(PKB::assignNodes[index], RHS, RHSexprs);
}

//RHS for now handles patterns in the form of "a" or _"a"_
bool RulesOfEngagement::TryMatch(ASTNode* testedNode,
	RulesOfEngagement::PatternRHSType RHS, ASTExprNode* RHSexpr)
{
	ASTNode* head= testedNode->getChild(1);

	//int rightInt = PKB::variables.getVARIndex(incCodes.at(0));

	//if(!isSubsTree)//if not a subtree, since we only handle 1 variable so right side must be a variable if is true
	//{
	//	if(head->getType() != ASTNode::Variable)//right node is not a variable = auto fail
	//		return false;
	//	else if(rightInt == head->getValue()) //right side value is same as rightint
	//		return true;
	//	else
	//		return false; //if not equal return false
	//}
	stack<ASTNode*> nodesStack; 

	//nodesStack.push(head->getChild(0));

	//nodesStack.push(head->getChild(1));
	
	nodesStack.push(head);
	ASTNode* pattern = RHSexpr;

	if(RHS != RulesOfEngagement::PRSub)
		return MatcherTree(head,pattern);
	else
	{
		while(nodesStack.size() > 0)
		{

			ASTNode* tempnode = nodesStack.top();
			nodesStack.pop();
							/*
							ASTNode* temp = nodesStack.top();
				nodesStack.pop();
				int counter = 0;

				ASTStmtLstNode* t =	(ASTStmtLstNode*)temp;

				for(int j=0;j<t->getSize();j++)
				{
					ASTNode* tempushnode = temp->getChild(j);
					nodesStack.push(tempushnode);
				}
							*/
			//
				//check here
			//if()
			//ASTNode* tempnode
		
			//RHSexpr->

			if (MatcherTree(tempnode,pattern))//,isSub))
			{
				return true; 
			}
			//ASTExprNode* RHSexpr
			//
			//

			if(tempnode->getType() == ASTNode::Operator)
			{
				nodesStack.push(tempnode->getChild(1));//add right side in

				nodesStack.push(tempnode->getChild(0));//add left side in
			}
			//else if(nodesStack.top()->getType() == ASTNode::Variable || nodesStack.top()->getType() == ASTNode::Constant)
			//{
			//	//assume is subtree

			//	if(nodesStack.top()->getType() == ASTNode::Variable && rightInt == nodesStack.top()->getValue())
			//	{
			//		return true;
			//	}
			//	nodesStack.pop();
			//}
			//else
			//{
			//	throw SPAException("Error! invalid node kind in operator");
			//}
		}
	}
	return false;
}

bool RulesOfEngagement::MatcherTree(ASTNode* Original, ASTNode* Pattern)//, bool isSub)
{
	if (Original->getType() != Pattern->getType() || Original->getValue() != Pattern->getValue())
		return false;

	if (Original->getType() == ASTNode::NodeType::Constant)
		return true;
	else if (Original->getType() == ASTNode::NodeType::Variable)
		return true;
	else if (Original->getType() == ASTNode::NodeType::Operator) {
		if (!MatcherTree(Original->getChild(0),Pattern->getChild(0)));//, isSub);
			return false;
		return MatcherTree(Original->getChild(1),Pattern->getChild(1));//, isSub);
	}

	return false;
}
//end pattern