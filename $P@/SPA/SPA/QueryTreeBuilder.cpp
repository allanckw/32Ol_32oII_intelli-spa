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
#include "PalletTown.h"

void QueryTreeBuilder::buildQueryTree(unordered_map<int, vector<string>> userVariables, 
	unordered_map<int, vector<string>> selectVariables, 
	unordered_map<int, pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>>> relationships,
	unordered_map<int, pair<pair<PalletTown::ashKetchum, string>, string>> conditions)
{
	pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>> relationship;
	pair<pair<PalletTown::ashKetchum, string>, string> condition;
	PalletTown::garyOak relationshipType;
	PalletTown::mistyWaterflower conditionType;
	PalletTown::ashKetchum currentVariableType;
	vector<string> currentVariables;
	unordered_map<int, vector<string>> selectVars;
	vector<QueryTreeNode*> qtCluster;
	
	for (auto it = relationships.begin(); it != relationships.end(); it++)
	{
		relationshipType = (PalletTown::garyOak) (*it).first;
		relationship = (*it).second;
		RelationshipNode* rNode = new RelationshipNode(relationshipType, relationship);
		for (auto it2 = selectVariables.begin(); it2 != selectVariables.end(); it2++)
		{
			currentVariableType = (PalletTown::ashKetchum) (*it2).first;
			currentVariables = (*it2).second;
			for (int i = 0; i < currentVariables.size(); i++)
			{
				if (currentVariables.at(i).compare(rNode->getFirstVariableName()) == 0 ||
					currentVariables.at(i).compare(rNode->getSecondVariableName()) == 0)
					selectVars[currentVariableType].push_back(currentVariables.at(i));
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
		conditionType = (PalletTown::mistyWaterflower) (*it).first;
		condition = (*it).second;
		ConditionNode* cNode = new ConditionNode(conditionType, condition);
		for (auto it2 = selectVariables.begin(); it2 != selectVariables.end(); it2++)
		{
			currentVariableType = (PalletTown::ashKetchum) (*it2).first;
			currentVariables = (*it2).second;
			for (int i = 0; i < currentVariables.size(); i++)
			{
				if (currentVariables.at(i).compare(cNode->getConditionVariableName()) == 0)
					selectVars[currentVariableType].push_back(currentVariables.at(i));
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

vector<vector<QueryTreeNode*>> QueryTreeBuilder::getQueryTree()
{
	return queryTree;
}