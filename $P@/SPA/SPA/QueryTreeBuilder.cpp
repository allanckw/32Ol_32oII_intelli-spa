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
#include "QueryPatternNode.h"
#include "QueryTreeBuilder.h"
#include "QueryEnums.h"


void QueryTreeBuilder::buildQueryTree(unordered_map<int, vector<string>> userVariables, 
	unordered_map<int, vector<string>> selectVariables, 
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> relationships,
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> conditions,
	vector<pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> patterns)
{
	queryTree.clear();

	vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>> currentRelationships, currentConditions;
	pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>> relationship, condition;
	pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>> pattern;
	//unordered_map<int, vector<string>> selectVariablesLeft = selectVariables;
	QueryEnums::QueryReladition reladitionType;
	QueryEnums::QueryVar currentVariableType;
	vector<string> currentVariables;
	unordered_map<int, vector<string>> selectVars;
	vector<QueryTreeNode*> qtCluster;

	for (auto it = relationships.begin(); it != relationships.end(); it++)
	{
		reladitionType = (QueryEnums::QueryReladition) (*it).first;
		currentRelationships = (*it).second;
		for (int i = 0; i < currentRelationships.size(); i++)
		{
			relationship = currentRelationships.at(i);
			QueryRelNode* rNode = new QueryRelNode(reladitionType, relationship);
			/*for (auto it = selectVariables.begin(); it != selectVariables.end(); it++)
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
			}*/
			//selectVars.clear();
			QueryProjectNode* pNode = new QueryProjectNode();
			
			//Form cluster
			rNode->setParent(pNode);
			//sNode->setChild(rNode);
			//sNode->setParent(pNode);
			pNode->setChild(rNode);
			qtCluster.push_back(rNode);
			//qtCluster.push_back(sNode);
			qtCluster.push_back(pNode);
			//Throw cluster into query tree
			queryTree.push_back(qtCluster);
			//clear cluster for next insertion
			qtCluster.clear();
		}
	}
	

	for (auto it = conditions.begin(); it != conditions.end(); it++)
	{
		reladitionType = (QueryEnums::QueryReladition) (*it).first;
		currentConditions = (*it).second;
		for (int i = 0; i < currentConditions.size(); i++)
		{
			condition = currentConditions.at(i);
			QueryCondNode* cNode = new QueryCondNode(reladitionType, condition);
			/*for (auto it2 = selectVariables.begin(); it2 != selectVariables.end(); it2++)
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
			}
			selectVars.clear();
			*/

			QueryProjectNode* pNode = new QueryProjectNode();
			
			//Form cluster
			cNode->setParent(pNode);
			//sNode->setChild(cNode);
			//sNode->setParent(pNode);
			pNode->setChild(cNode);
			qtCluster.push_back(cNode);
			//qtCluster.push_back(sNode);
			qtCluster.push_back(pNode);
			//Throw cluster into query tree
			queryTree.push_back(qtCluster);
			//clear cluster for next insertion
			qtCluster.clear();
	
		}
	}

	for (int i = 0; i < patterns.size(); i++)
	{
		pattern = patterns.at(i);
		QueryPatternNode* paNode = new QueryPatternNode(pattern);
		/*for (auto it = selectVariables.begin(); it != selectVariables.end(); it++)
		{
			currentVariableType = (QueryEnums::QueryVar) (*it).first;
			currentVariables = (*it).second;
			for (int i = 0; i < currentVariables.size(); i++)
			{
				if (currentVariables.at(i).compare(paNode->getPatternVariableName()) == 0 ||
					currentVariables.at(i).compare(paNode->getFirstPatternParameterName()) == 0)
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
			selectVars.clear();
		}*/

		QueryProjectNode* pNode = new QueryProjectNode();

		//Form cluster
		paNode->setParent(pNode);
		//sNode->setChild(paNode);
		//sNode->setParent(pNode);
		pNode->setChild(paNode);
		qtCluster.push_back(paNode);
		//qtCluster.push_back(sNode);
		qtCluster.push_back(pNode);
		//Throw cluster into query tree
		queryTree.push_back(qtCluster);
		//clear cluster for next insertion
		qtCluster.clear();
	}

	//Create a dummy cluster for remaining select variables not found in relationships/conditions
	QueryRelNode* blankRNode = new QueryRelNode();
	QueryLastSelNode* sNode = new QueryLastSelNode(selectVariables); //getting all select variables now
	//Form cluster
	blankRNode->setParent(sNode);
	sNode->setChild(blankRNode);
	qtCluster.push_back(blankRNode);
	qtCluster.push_back(sNode);
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

