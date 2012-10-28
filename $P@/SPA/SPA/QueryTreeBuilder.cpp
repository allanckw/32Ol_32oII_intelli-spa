#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"
#include "QueryPreprocessor.h"
#include "QueryTreeNode.h"
#include "QueryRelNode.h"
#include "SelectNode.h"
#include "QueryProjectNode.h"
#include "QueryCondNode.h"
#include "QueryTreeBuilder.h"
#include "QueryEnums.h"

void QueryTreeBuilder::buildQueryTree(unordered_map<int, vector<string>> userVariables, 
	unordered_map<int, vector<string>> selectVariables, 
	unordered_map<int, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>> relationships,
	unordered_map<int, pair<pair<QueryEnums::QueryVar, string>, string>> conditions)
{
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> relationship;
	pair<pair<QueryEnums::QueryVar, string>, string> condition;
	QueryEnums::QueryRel relationshipType;
	QueryEnums::QueryCond conditionType;
	QueryEnums::QueryVar currentVariableType;
	vector<string> currentVariables;
	unordered_map<int, vector<string>> selectVars;
	vector<QueryTreeNode*> qtCluster;
	
	for (auto it = relationships.begin(); it != relationships.end(); it++)
	{
		relationshipType = (QueryEnums::QueryRel) (*it).first;
		relationship = (*it).second;
		QueryRelNode* rNode = new QueryRelNode(relationshipType, relationship);
		for (auto it2 = selectVariables.begin(); it2 != selectVariables.end(); it2++)
		{
			currentVariableType = (QueryEnums::QueryVar) (*it2).first;
			currentVariables = (*it2).second;
			for (int i = 0; i < currentVariables.size(); i++)
			{
				if (currentVariables.at(i).compare(rNode->getFirstVariableName()) == 0 ||
					currentVariables.at(i).compare(rNode->getSecondVariableName()) == 0)
					selectVars[currentVariableType].push_back(currentVariables.at(i));
			}
			SelectNode* sNode = new SelectNode(selectVars);
			QueryProjectNode* pNode = new QueryProjectNode();
			
			//Form cluster
			rNode->setParent(sNode);
			sNode->setChild(rNode);
			sNode->setParent(pNode);
			pNode->setChild(sNode);
			qtCluster.push_back(rNode);
			qtCluster.push_back(sNode);
			qtCluster.push_back(pNode);
			//Throw cluster into query tree
			queryTree.push_back(qtCluster);
			//clear cluster for next insertion
			qtCluster.clear();
		}
	}

	for (auto it = conditions.begin(); it != conditions.end(); it++)
	{
		conditionType = (QueryEnums::QueryCond) (*it).first;
		condition = (*it).second;
		QueryCondNode* cNode = new QueryCondNode(conditionType, condition);
		for (auto it2 = selectVariables.begin(); it2 != selectVariables.end(); it2++)
		{
			currentVariableType = (QueryEnums::QueryVar) (*it2).first;
			currentVariables = (*it2).second;
			for (int i = 0; i < currentVariables.size(); i++)
			{
				if (currentVariables.at(i).compare(cNode->getConditionVariableName()) == 0)
					selectVars[currentVariableType].push_back(currentVariables.at(i));
			}
			SelectNode* sNode = new SelectNode(selectVars);
			QueryProjectNode* pNode = new QueryProjectNode();
			
			//Form cluster
			cNode->setParent(sNode);
			sNode->setChild(cNode);
			sNode->setParent(pNode);
			pNode->setChild(sNode);
			qtCluster.push_back(cNode);
			qtCluster.push_back(sNode);
			qtCluster.push_back(pNode);
			//Throw cluster into query tree
			queryTree.push_back(qtCluster);
			//clear cluster for next insertion
			qtCluster.clear();
		}
	}

	return;
}

vector<vector<QueryTreeNode*>> QueryTreeBuilder::getQueryTree()
{
	return queryTree;
}