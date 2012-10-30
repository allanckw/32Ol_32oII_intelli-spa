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
#include "QueryTreeBuilder.h"
#include "QueryEnums.h"

//int main(int argc, char* arg[])
//{
//	vector<string> tokens; 
//	QueryPreprocessor QPP;
//	QueryParser QP;
//	QueryTreeBuilder QTB;
//	vector<vector<QueryTreeNode*>> QT;
//	vector<QueryTreeNode*> cluster;
//	vector<pair<QueryEnums::QueryVar, string>> selected;
//	QueryTreeNode::QTNodeType NT;
//	QueryProjectNode* p;
//	QuerySelNode* sn;
//	tokens = QP.tokenize("assignment a1, a2; statement s1, s2; select a1 with s1.stmt# = 5 and modifies(a1, _) with s2.procname = 'hi' and uses(a2, a1)");
//	QPP.preProcess(tokens);
//	QTB.buildQueryTree(QPP.getUserVariables(), QPP.getSelectVariables(), QPP.getRelationships(), QPP.getConditions());
//	QT = QTB.getQueryTree();
//	
//	for (int i = 0; i < QT.size(); i++)
//	{
//		cluster = QT.at(i);
//		for (int y = 0; y < cluster.size(); y++)
//		{
//			NT = cluster.at(y)->getNodeType();
//			if (NT == QueryTreeNode::Project)
//			{
//				p = new QueryProjectNode();
//				p = (QueryProjectNode*) cluster.at(y);
//				cout<<"Project Node: ";
//				for (int j = 0; j < p->getAnswer().size(); j++)
//					cout<<p->getAnswer().at(j)<<" ";
//				cout<<endl;
//			}
//			else if (NT == QueryTreeNode::Select)
//			{
//				selected = ((QuerySelNode*) cluster.at(y))->getSelectedVariables();
//				cout<<"Select Node: ";
//				for (int i = 0; i < selected.size(); i++)
//					cout<<"("<<selected.at(i).first<<", "<<selected.at(i).second<<")";
//				cout<<endl;
//			}
//			else if (NT == QueryTreeNode::Relationship)
//			{
//				cout<<"Relationship: "<<endl;
//				cout<<"Type: ";
//				cout<<((QueryRelNode*) cluster.at(y))->getRelationshipType()<<endl;
//				cout<<"([";
//				cout<<((QueryRelNode*) cluster.at(y))->getFirstVariableType();
//				cout<<", ";
//				cout<<((QueryRelNode*) cluster.at(y))->getFirstVariableName();
//				cout<<"] , [";
//				cout<<((QueryRelNode*) cluster.at(y))->getSecondVariableType();
//				cout<<", ";
//				cout<<((QueryRelNode*) cluster.at(y))->getSecondVariableName();
//				cout<<"])"<<endl;
//			}
//			else if (NT == QueryTreeNode::Condition)
//			{
//				cout<<"Condition: "<<endl;
//				cout<<"Attribute: ";
//				cout<<((QueryCondNode*) cluster.at(y))->getConditionAttribute()<<endl;
//				cout<<"(";
//				cout<<((QueryCondNode*) cluster.at(y))->getConditionVariableType();
//				cout<<", ";
//				cout<<((QueryCondNode*) cluster.at(y))->getConditionVariableName();
//				cout<<") , ";
//				cout<<((QueryCondNode*) cluster.at(y))->getConditionBoundary();
//				cout<<endl;
//			}
//		}
//		cout<<endl;
//	}
//
//	system("PAUSE");
//	return 0;
//}

void QueryTreeBuilder::buildQueryTree(unordered_map<int, vector<string>> userVariables, 
	unordered_map<int, vector<string>> selectVariables, 
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> relationships,
	unordered_map<int, vector<pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>>> conditions)
{
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
							cout<<selectVariablesLeft[currentVariableType].size();
							selectVariablesLeft[currentVariableType].erase(selectVariablesLeft[currentVariableType].begin() + i);
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

	//Create a dummy node for remaining select variables not found in relationships/conditions
	
	if (!(selectVariablesLeft.begin() == selectVariablesLeft.end()))
	{
		QueryRelNode* blankRNode = new QueryRelNode();
		QuerySelNode* sNode = new QuerySelNode(selectVariables);
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
	}

	return;
}

vector<vector<QueryTreeNode*>> QueryTreeBuilder::getQueryTree()
{
	return queryTree;
}

