#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"
#include "QueryPreprocessor.h"
#include "QueryTreeNode.h"
#include "QueryRelNode.h"
#include "QuerySelNode.h"
#include "QueryProjectNode.h"
#include "QueryCondNode.h"
#include "QueryLastSelNode.h"
#include "QueryTreeBuilder.h"
#include "QueryEnums.h"


void QueryTreeBuilder::buildQueryTree(unordered_map<int, vector<string>> userVariables, 
	unordered_map<int, vector<string>> selectVariables, 
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> relationships,
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> conditions)
{
	queryTree.clear();

	vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>> currentRelationships, currentConditions;
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> relationship, condition;
	unordered_map<int, vector<string>> selectVariablesLeft = selectVariables;
	QueryEnums::QueryRel relationshipType;
	QueryEnums::QueryCond conditionType;
	QueryEnums::QueryVar currentVariableType;
	vector<string> currentVariables;
	unordered_map<int, vector<string>> selectVars;
	vector<QueryTreeNode*> qtCluster;

	for (auto it = relationships.begin(); it != relationships.end(); it++)
	{
		relationshipType = (QueryEnums::QueryRel) (*it).first;
		currentRelationships = (*it).second;
		for (int i = 0; i < currentRelationships.size(); i++)
		{
			relationship = currentRelationships.at(i);
			QueryRelNode* rNode = new QueryRelNode(relationshipType, relationship);
			for (auto it = selectVariables.begin(); it != selectVariables.end(); it++)
			{
				currentVariableType = (QueryEnums::QueryVar) (*it).first;
				currentVariables = (*it).second;
				for (int i = 0; i < currentVariables.size(); i++)
				{
					if (currentVariables.at(i).compare(rNode->getFirstVariableName()) == 0 ||
						currentVariables.at(i).compare(rNode->getSecondVariableName()) == 0)
					{
						selectVars[currentVariableType].push_back(currentVariables.at(i));
						if(!(selectVariablesLeft[currentVariableType].size() == 0))
						{
							selectVariablesLeft[currentVariableType].erase
								(selectVariablesLeft[currentVariableType].begin() + i);
						}
					}
				}
				QuerySelNode* sNode = new QuerySelNode(selectVars);
				QueryProjectNode* pNode = new QueryProjectNode();
				selectVars.clear();
			
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
	}
	

	for (auto it = conditions.begin(); it != conditions.end(); it++)
	{
		conditionType = (QueryEnums::QueryCond) (*it).first;
		currentConditions = (*it).second;
		for (int i = 0; i < currentConditions.size(); i++)
		{
			condition = currentConditions.at(i);
			QueryCondNode* cNode = new QueryCondNode(conditionType, condition);
			for (auto it2 = selectVariables.begin(); it2 != selectVariables.end(); it2++)
			{
				currentVariableType = (QueryEnums::QueryVar) (*it2).first;
				currentVariables = (*it2).second;
				for (int j = 0; j < currentVariables.size(); j++)
				{
					if (currentVariables.at(j).compare(cNode->getConditionVariableName()) == 0)
					{
						selectVars[currentVariableType].push_back(currentVariables.at(j));
						if(!(selectVariablesLeft[currentVariableType].size() == 0))
							selectVariablesLeft[currentVariableType].erase(selectVariablesLeft[currentVariableType].begin() + i);
					}
				}
				QuerySelNode* sNode = new QuerySelNode(selectVars);
				QueryProjectNode* pNode = new QueryProjectNode();
				selectVars.clear();
			
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
	}

	//Create a dummy cluster for remaining select variables not found in relationships/conditions
	QueryRelNode* blankRNode = new QueryRelNode();
	QueryLastSelNode* sNode = new QueryLastSelNode(selectVariablesLeft); //can be empty
	QueryProjectNode* blankPNode = new QueryProjectNode();
	//Form cluster
	blankRNode->setParent(sNode);
	sNode->setChild(blankRNode);
	sNode->setParent(blankPNode);
	blankPNode->setChild(sNode);
	qtCluster.push_back(blankRNode);
	qtCluster.push_back(sNode);
	qtCluster.push_back(blankPNode);
	//Throw cluster into query tree
	queryTree.push_back(qtCluster);
	//clear cluster for next insertion
	qtCluster.clear();

	return;
}

QueryTree QueryTreeBuilder::getQueryTree()
{
	return queryTree;
}

