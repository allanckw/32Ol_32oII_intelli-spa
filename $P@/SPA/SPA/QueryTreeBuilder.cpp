#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"
#include "QueryPreprocessor.h"
#include "QueryTreeNode.h"
#include "RelationshipNode.h"
#include "SelectNode.h"
#include "ProjectNode.h"
#include "ConditionNode.h"
#include "QueryTreeBuilder.h"

void QueryTreeBuilder::buildQueryTree(unordered_map<int, vector<string>> userVariables, vector<string> selectVariables,
					unordered_map<int, vector<vector<string>>> conditions, unordered_map<int, vector<vector<string>>> relationships)
{
	vector<vector<string>> reladition;
	QueryPreprocessor::garyOak relationshipType;
	QueryPreprocessor::rorona conditionType;
	string currentVariable;
	vector<string> selectVars;
	vector<QueryTreeNode*> qtCluster;
	
	for (auto it = relationships.begin(); it != relationships.end(); it++)
	{
		relationshipType = (QueryPreprocessor::garyOak) (*it).first;
		reladition = (*it).second;
		for (int x = 0; x < reladition.size(); x++)
		{
			RelationshipNode* rNode = new RelationshipNode(relationshipType, reladition.at(x));
			for (int y = 0; y < selectVariables.size(); y++)
			{
				currentVariable = selectVariables.at(y);
				if (currentVariable.compare(rNode->getFirstVariable()) == 0 || currentVariable.compare(rNode->getSecondVariable()) == 0)
					selectVars.push_back(currentVariable);
			}
			SelectNode* sNode = new SelectNode(selectVars);
			ProjectNode* pNode = new ProjectNode();
			
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
		conditionType = (QueryPreprocessor::rorona) (*it).first;
		reladition = (*it).second; //iterator returns a pair
		for (int x = 0; x < reladition.size(); x++)
		{
			ConditionNode* cNode = new ConditionNode(conditionType, reladition.at(x));
			for (int y = 0; y < selectVariables.size(); y++)
			{
				currentVariable = selectVariables.at(y);
				if (currentVariable.compare(cNode->getConditionVariable()) == 0)
					selectVars.push_back(currentVariable);
			}
			SelectNode* sNode = new SelectNode(selectVars);
			ProjectNode* pNode = new ProjectNode();
			
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