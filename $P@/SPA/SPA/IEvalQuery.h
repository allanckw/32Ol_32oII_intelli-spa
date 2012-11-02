#pragma once
#include "stdafx.h"
#include "QueryTreeBuilder.h"
#include "QueryEnums.h"
#include "QueryTreeNode.h"
#include "QuerySelNode.h"
#include "QueryRelNode.h"
#include "QueryLastSelNode.h"
#include "ASTNode.h"

class IEvalQuery
{
private:

	bool allStmtsFirst, allStmtsSecond, allProcsFirst, allProcsSecond, allVarsFirst, allVarsSecond;
	list<string> answer;

	unordered_set<int> currentFirstIndices, currentSecondIndices;
	void populateVariableIndices(QueryEnums::QueryVar, int index);

	vector<QueryTreeNode*> currentCluster;
	QueryTreeNode* currentNode;
	QueryTreeNode::QTNodeType currentNodeType;

	QueryRelNode* currentRelationshipNode;
	QueryEnums::QueryRel currentRelationshipType;
	QueryEnums::QueryVar currentFirstVariableType, currentSecondVariableType;
	string currentFirstVariableName, currentSecondVariableName;
	int currentFirstVariableNo, currentSecondVariableNo;
	bool firstNumber, secondNumber; 
	bool firstFixedProcedure , secondFixedProcedure , secondFixedVariable;
	vector<string> firstVariableAnswer, secondVariableAnswer;
	bool projectBool, boolAnswer; //projectBool = true means that the reladition should return bool instead of vecctor answer

	QuerySelNode* currentSelNode;
	vector<pair<QueryEnums::QueryVar, string>> selected;
	QueryLastSelNode* lastSelNode;

	vector<int> stmtLineTrue;
	vector<string> vecMatch;
	bool isSub;
	bool TryMatch(ASTNode*, string, vector<string>, bool);

	//Methods For Evaluating Relationships
	void IEvalQuery::EvaluateModifies();

	void IEvalQuery::EvaluateUses();
	
	void IEvalQuery::EvaluateParent();
	
	void IEvalQuery::EvaluateParentStar();
	
	void IEvalQuery::EvaluateFollows();
	
	void IEvalQuery::EvaluateFollowsStar();

	void IEvalQuery::EvaluateCalls();

	void IEvalQuery::EvaluateCallsStar();

	void IEvalQuery::EvaluatePattern();

	void IEvalQuery::initNewQuery();

	//TODO: FOR CS3202
	//Affects, Affects*
	//Next, Next*

public:
	list<string> evaluateQuery(QueryTree);
};