#pragma once
#include "RulesOfEngagement.h"
#include "PKB.h"

unordered_map<string, RulesOfEngagement::QueryVar> RulesOfEngagement::tokenToType;
unordered_map<RulesOfEngagement::QueryVar, set<string>> RulesOfEngagement::allowableConditions;
unordered_map<RulesOfEngagement::QueryReladition, set<RulesOfEngagement::QueryVar>> RulesOfEngagement::allowableFirstArgument;
unordered_map<RulesOfEngagement::QueryReladition, set<RulesOfEngagement::QueryVar>> RulesOfEngagement::allowableSecondArgument;
typedef bool(*isRelation)(int, int);
typedef vector<int>(*getAllTypes)();
unordered_map<RulesOfEngagement::QueryReladition, isRelation> RulesOfEngagement::relationMap;
unordered_map<RulesOfEngagement::QueryVar, getAllTypes> RulesOfEngagement::typeMap;

void RulesOfEngagement::initialise()
{
	tokenToType["procedure"] = Procedure;
	tokenToType["stmt"] = Statement;
	tokenToType["prog_line"] = Statement; //Lame, but prog_line and statement are interchangable
	tokenToType["assign"] = Assign;
	tokenToType["constant"] = Constant;
	tokenToType["while"] = While;
	tokenToType["variable"] = Variable;
	tokenToType["call"] = Call;
	tokenToType["if"] = If;

	allowableConditions[Procedure].insert("procName");
	allowableConditions[Call].insert("procName");
	allowableConditions[Variable].insert("varName");
	allowableConditions[Constant].insert("Value");
	allowableConditions[Statement].insert("stmtNo");
	allowableConditions[Assign].insert("stmtNo");
	allowableConditions[While].insert("stmtNo");
	allowableConditions[If].insert("stmtNo");
	allowableConditions[Call].insert("stmtNo");

	allowableFirstArgument[Modifies].insert(Procedure);
	allowableFirstArgument[Modifies].insert(Statement);
	allowableFirstArgument[Modifies].insert(Assign);
	allowableFirstArgument[Modifies].insert(Call);
	allowableFirstArgument[Modifies].insert(While);
	allowableFirstArgument[Modifies].insert(If);
	allowableSecondArgument[ModifiesStmt].insert(Variable);
	allowableSecondArgument[ModifiesStmt].insert(WildCard);
	allowableSecondArgument[ModifiesProc].insert(Variable);
	allowableSecondArgument[ModifiesProc].insert(WildCard);
	
	allowableFirstArgument[Uses].insert(Procedure);
	allowableFirstArgument[Uses].insert(Statement);
	allowableFirstArgument[Uses].insert(Assign);
	allowableFirstArgument[Uses].insert(Call);
	allowableFirstArgument[Uses].insert(While);
	allowableFirstArgument[Uses].insert(If);
	allowableSecondArgument[UsesStmt].insert(Variable);
	allowableSecondArgument[UsesStmt].insert(WildCard);
	allowableSecondArgument[UsesProc].insert(Variable);
	allowableSecondArgument[UsesProc].insert(WildCard);

	allowableFirstArgument[Calls].insert(Procedure);
	allowableFirstArgument[Calls].insert(WildCard);
	allowableSecondArgument[Calls].insert(Procedure);
	allowableSecondArgument[Calls].insert(WildCard);

	allowableFirstArgument[CallsStar].insert(Procedure);
	allowableFirstArgument[CallsStar].insert(WildCard);
	allowableSecondArgument[CallsStar].insert(Procedure);
	allowableSecondArgument[CallsStar].insert(WildCard);
	
	allowableFirstArgument[Parent].insert(Statement);
	allowableFirstArgument[Parent].insert(Assign);
	allowableFirstArgument[Parent].insert(Call);
	allowableFirstArgument[Parent].insert(While);
	allowableFirstArgument[Parent].insert(If);
	allowableFirstArgument[Parent].insert(WildCard);
	allowableSecondArgument[Parent].insert(Statement);
	allowableSecondArgument[Parent].insert(Assign);
	allowableSecondArgument[Parent].insert(Call);
	allowableSecondArgument[Parent].insert(While);
	allowableSecondArgument[Parent].insert(If);
	allowableSecondArgument[Parent].insert(WildCard);
	
	allowableFirstArgument[ParentStar].insert(Statement);
	allowableFirstArgument[ParentStar].insert(Assign);
	allowableFirstArgument[ParentStar].insert(Call);
	allowableFirstArgument[ParentStar].insert(While);
	allowableFirstArgument[ParentStar].insert(If);
	allowableFirstArgument[ParentStar].insert(WildCard);
	allowableSecondArgument[ParentStar].insert(Statement);
	allowableSecondArgument[ParentStar].insert(Assign);
	allowableSecondArgument[ParentStar].insert(Call);
	allowableSecondArgument[ParentStar].insert(While);
	allowableSecondArgument[ParentStar].insert(If);
	allowableSecondArgument[ParentStar].insert(WildCard);
	
	allowableFirstArgument[Follows].insert(Statement);
	allowableFirstArgument[Follows].insert(Assign);
	allowableFirstArgument[Follows].insert(Call);
	allowableFirstArgument[Follows].insert(While);
	allowableFirstArgument[Follows].insert(If);
	allowableFirstArgument[Follows].insert(WildCard);
	allowableSecondArgument[Follows].insert(Statement);
	allowableSecondArgument[Follows].insert(Assign);
	allowableSecondArgument[Follows].insert(Call);
	allowableSecondArgument[Follows].insert(While);
	allowableSecondArgument[Follows].insert(If);
	allowableSecondArgument[Follows].insert(WildCard);
	
	allowableFirstArgument[FollowsStar].insert(Statement);
	allowableFirstArgument[FollowsStar].insert(Assign);
	allowableFirstArgument[FollowsStar].insert(Call);
	allowableFirstArgument[FollowsStar].insert(While);
	allowableFirstArgument[FollowsStar].insert(If);
	allowableFirstArgument[FollowsStar].insert(WildCard);
	allowableSecondArgument[FollowsStar].insert(Statement);
	allowableSecondArgument[FollowsStar].insert(Assign);
	allowableSecondArgument[FollowsStar].insert(Call);
	allowableSecondArgument[FollowsStar].insert(While);
	allowableSecondArgument[FollowsStar].insert(If);
	allowableSecondArgument[FollowsStar].insert(WildCard);
	
	allowableFirstArgument[Next].insert(Statement);
	allowableFirstArgument[Next].insert(Assign);
	allowableFirstArgument[Next].insert(Call);
	allowableFirstArgument[Next].insert(While);
	allowableFirstArgument[Next].insert(If);
	allowableFirstArgument[Next].insert(WildCard);
	allowableSecondArgument[Next].insert(Statement);
	allowableSecondArgument[Next].insert(Assign);
	allowableSecondArgument[Next].insert(Call);
	allowableSecondArgument[Next].insert(While);
	allowableSecondArgument[Next].insert(If);
	allowableSecondArgument[Next].insert(WildCard);
	
	allowableFirstArgument[NextStar].insert(Statement);
	allowableFirstArgument[NextStar].insert(Assign);
	allowableFirstArgument[NextStar].insert(Call);
	allowableFirstArgument[NextStar].insert(While);
	allowableFirstArgument[NextStar].insert(If);
	allowableFirstArgument[NextStar].insert(WildCard);
	allowableSecondArgument[NextStar].insert(Statement);
	allowableSecondArgument[NextStar].insert(Assign);
	allowableSecondArgument[NextStar].insert(Call);
	allowableSecondArgument[NextStar].insert(While);
	allowableSecondArgument[NextStar].insert(If);
	allowableSecondArgument[NextStar].insert(WildCard);
	
	allowableFirstArgument[Affects].insert(Statement);
	allowableFirstArgument[Affects].insert(Assign);
	allowableFirstArgument[Affects].insert(WildCard);
	allowableSecondArgument[Affects].insert(Statement);
	allowableSecondArgument[Affects].insert(Assign);
	allowableSecondArgument[Affects].insert(WildCard);
	
	allowableFirstArgument[AffectsStar].insert(Statement);
	allowableFirstArgument[AffectsStar].insert(Assign);
	allowableFirstArgument[AffectsStar].insert(WildCard);
	allowableSecondArgument[AffectsStar].insert(Statement);
	allowableSecondArgument[AffectsStar].insert(Assign);
	allowableSecondArgument[AffectsStar].insert(WildCard);

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

//relation map
RulesOfEngagement::isRelation RulesOfEngagement::getRelation(RulesOfEngagement::QueryReladition rel)
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

bool RulesOfEngagement::satisfyPattern(int index, int modifiesVar, string usesVar)
{
	if (!isModifiesStmt(index, modifiesVar))
		return false;

	static unordered_map<int, unordered_map<string, bool>> map;
	if (map.count(modifiesVar) > 0 && map[modifiesVar].count(usesVar) > 0)
		return map[modifiesVar][usesVar];
	return map[modifiesVar][usesVar] = tryMatch(PKB::assignNodes[index], usesVar, vector<string>(), false); //to be changed
}

//RHS for now handles patterns in the form of "a" or _"a"_
bool RulesOfEngagement::tryMatch(ASTNode* testedNode, string targetVar,vector<string> incCodes, bool isSubsTree)
{
	return true;
	/*if(!(testedNode->getType() == ASTNode::Assign))
		throw SPAException("Error, this node not an assignNode");

	if(!testedNode->isHasChildren())
		throw SPAException("Assignment no child error!");

	bool leftTrue = true;

	if(!allVarsFirst)
	{
		currentFirstVariableNo = PKB::variables.getVARIndex(targetVar);
		if(!(targetVar.compare("_") == 0) && currentFirstVariableNo != testedNode->getChild(0)->getValue())
		{
			return false;
		}
		////////at this point left hand side is ok
	}
	ASTNode* head= testedNode->getChild(1);

	bool rightTrue = false;

	if(incCodes.at(0).compare("_") == 0)
		rightTrue = true;

	if(leftTrue && rightTrue)
		return true;


	int rightInt = PKB::variables.getVARIndex(incCodes.at(0));

	if(!isSubsTree)//if not a subtree, since we only handle 1 variable so right side must be a variable if is true
	{
		if(head->getType() != ASTNode::Variable)//right node is not a variable = auto fail
			return false;
		else if(rightInt == head->getValue()) //right side value is same as rightint
			return true;
		else
			return false; //if not equal return false
	}
	stack<ASTNode*> nodesStack; //if subtree = true

	/*nodesStack.push(head->getChild(0));

	nodesStack.push(head->getChild(1));
	*/
	/*nodesStack.push(head);

	while(nodesStack.size() > 0)
	{
		if(nodesStack.top()->getType() == ASTNode::Operator)
		{
			ASTNode* tempnode = nodesStack.top();
			nodesStack.pop();
			nodesStack.push(tempnode->getChild(1));//add right side in

			nodesStack.push(tempnode->getChild(0));//add left side in
		}
		else if(nodesStack.top()->getType() == ASTNode::Variable || nodesStack.top()->getType() == ASTNode::Constant)
		{
			//assume is subtree

			if(nodesStack.top()->getType() == ASTNode::Variable && rightInt == nodesStack.top()->getValue())
			{
				return true;
			}
			nodesStack.pop();
		}
		else
		{
			throw SPAException("Error! invalid node kind in operator");
		}
	}

	return false;*/
}

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