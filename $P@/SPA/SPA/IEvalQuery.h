#pragma once
#include "stdafx.h"
#include "QueryTreeBuilder.h"
#include "QueryEnums.h"
#include "QueryTreeNode.h"
#include "QuerySelNode.h"
#include "QueryRelNode.h"
#include "QueryLastSelNode.h"
#include "QueryProjectNode.h"
#include "QueryPatternNode.h"
#include "ASTNode.h"

class IEvalQuery
{
private:

	vector<QueryProjectNode*> projects;

	bool allStmtsFirst, allStmtsSecond, allProcsFirst, allProcsSecond, allVarsFirst, allVarsSecond;
	vector<string> answer;

	unordered_set<int> currentFirstIndices, currentSecondIndices;
	void populateVariableIndices(QueryEnums::QueryVar, int index);

	QueryEnums::QueryVar currentFirstVariableType, currentSecondVariableType;
	string currentFirstVariableName, currentSecondVariableName;

	vector<QueryTreeNode*> currentCluster;
	QueryTreeNode* currentNode;
	QueryTreeNode::QTNodeType currentNodeType;

	QueryPatternNode* currentPatternNode;
	QueryEnums::QueryReladition currentReladitionType;

	QueryRelNode* currentRelationshipNode;
	int currentFirstVariableNo, currentSecondVariableNo;
	bool firstNumber, secondNumber; 
	bool firstFixedProcedure , secondFixedProcedure , secondFixedVariable;
	vector<int> firstVariableAnswer, secondVariableAnswer;
	bool projectBool, boolAnswer; //projectBool = true means that the reladition should return bool instead of vecctor answer

	QuerySelNode* currentSelNode;
	vector<pair<QueryEnums::QueryVar, string>> selected;
	QueryLastSelNode* lastSelNode;
	QueryEnums::QueryVar selectType;

	vector<string> vecMatch;
	bool isSub;
	bool TryMatch(ASTNode*, string, vector<string>, bool);

	//testing?
	unordered_map<string, vector<int>> variableAnswers;
	vector<string> bigAnswerHeaders, tempBigAnswerHeaders;
	vector<vector<int>> bigAnswerIndices, tempBigAnswerIndices;
	void cartesianUntilGoMad();
	bool isSynonym(string&, QueryEnums::QueryVar);
	bool finalBoolAnswer;

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

	void IEvalQuery::resetAll();

	//TODO: FOR CS3202
	//Affects, Affects*
	//Next, Next*

public:
	vector<string> evaluateQuery(QueryTree);
};