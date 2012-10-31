#pragma once
#include "stdafx.h"
#include "QueryTreeBuilder.h"
#include "QueryEnums.h"
#include "QueryTreeNode.h"
#include "QuerySelNode.h"
#include "QueryRelNode.h"

class IEvalQuery
{
private:

	//Copied from previous evaluator.. not sure how to use yet tho -.-
	bool allStmtsFirst, allStmtsSecond, allProcsFirst, allProcsSecond, allVarsSecond;
	vector<string> answer;

	unordered_set<STMT> currentFirstIndices, currentSecondIndices;
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
	bool firstFixedProcedure , secondFixedProcedure , secondFixedVariable ;
	vector<string> boolAnswer;
	vector<string> firstVariableAnswer, secondVariableAnswer;
	bool projectBool ;

	QuerySelNode* currentSelNode;
	vector<pair<QueryEnums::QueryVar, string>> selected;


	//Methods For Evaluating Relationships
	void IEvalQuery::EvaluateModifies();

	void IEvalQuery::EvaluateUses();
	
	void IEvalQuery::EvaluateParent();
	
	void IEvalQuery::EvaluateParentStar();
	
	void IEvalQuery::EvaluateFollows();
	
	void IEvalQuery::EvaluateFollowsStar();

	void IEvalQuery::EvaluateCalls();

	void IEvalQuery::EvaluateCallsStar();

	void IEvalQuery::initNewQuery();

	//TODO: FOR CS3202
	//Affects, Affects*
	//Next, Next*
	//Etc..

public:
	vector<string> evaluateQuery(QueryTree qt);
};

