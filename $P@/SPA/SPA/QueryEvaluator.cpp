#pragma once
#include "QueryEvaluator.h"
#include "StdAfx.h"
#include "PKB.h"
#include "QueryParser.h"
#include "QueryPreprocessor.h"
#include "QueryTreeBuilder.h"
#include "QueryTreeNode.h"
#include "QueryRelNode.h"
#include "QueryEnums.h"
#include "PROCTable.h"
#include "QueryProjectNode.h"
#include "CallsTable.h"

QueryEvaluator::QueryEvaluator(void)
{
}

vector<string> QueryEvaluator::intersection(vector<string> v1, vector<string> v2) 
{
	vector<string> desiredAnswer;
	unordered_map<string, bool> store;
	for (int i = 0; i < v1.size(); i++) {
		store[v1[i]] = true;
	}
	for (int i = 0; i < v2.size(); i++) {
		if (store[v2[i]] == true) {
			desiredAnswer.push_back(v2[i]);
		}
	}
	return desiredAnswer;
}

vector<string> QueryEvaluator::returnAnswer() {
	QueryPreprocessor querypreprocessor;
	unordered_map<int, vector<string>> userVariables = querypreprocessor.getUserVariables();
	for (auto it = userVariables.begin(); it != userVariables.end(); it++) {
		vector<string> variableNames = (*it).second;
		for (int i = 0; i < variableNames.size(); i++) {
			string currentVariableName = variableNames[i];
			answer[currentVariableName].push_back("false");
		}
	}
	QueryTreeBuilder querytreebuilder;
	vector<vector<QueryTreeNode*>> querytree = querytreebuilder.getQueryTree();

	for (int i = 0; i < querytree.size(); i++) {
		vector<QueryTreeNode*> querycluster = querytree[i];
		QueryTreeNode* currentnode = querycluster[0];
		QueryTreeNode::QTNodeType nodetype = currentnode->getNodeType();
		if (nodetype == QueryTreeNode::Relationship) {
			
			QueryRelNode* rnode = (QueryRelNode*) currentnode;
			QueryEnums::QueryRel rtype = rnode->getRelationshipType();
			if (nodetype == QueryTreeNode::Relationship) {
				QueryRelNode* rnode = (QueryRelNode*) currentnode;
				QueryEnums::QueryRel rtype = rnode->getRelationshipType();
				QueryEnums::QueryVar firsttype = rnode->getFirstVariableType();
				QueryEnums::QueryVar secondtype = rnode->getSecondVariableType();

			switch(rtype) {
				case QueryEnums::Calls: 
					if (firsttype != QueryEnums::QueryVar::Procedure || secondtype != QueryEnums::QueryVar::Procedure) {
						throw new SPAException("Hanwei, you screwed up!");
					} else {
						querycluster.pop_back();
						vector<pair<string, string>> newans;
						for (int i = 0; i < PKB::procedures.getSize(); i++) {
							PROCIndex index = (PROCIndex) i;
							string curr_name = PKB::procedures.getPROCName(index);
							vector<PROC> getcalledby = PKB::calls.getCalledBy(index);
							if (!getcalledby.empty()) {
								for (int j = 0; j < getcalledby.size(); j++) {
									PROCIndex index2 = (PROCIndex) j;
									string next_name = PKB::procedures.getPROCName(index2);
									newans.push_back(make_pair(curr_name, next_name));
								}
							}
						}
						QueryProjectNode* newprojnode = new QueryProjectNode(newans);
						querycluster.push_back(newprojnode);
					}
					break; 
				case QueryEnums::CallsStar: 
						if (firsttype != QueryEnums::QueryVar::Procedure || secondtype != QueryEnums::QueryVar::Procedure) {
							throw new SPAException("Hanwei, you screwed up!");
						} else {
					
							querycluster.pop_back();
							vector<pair<string, string>> newans;
					
							for (int i = 0; i < PKB::procedures.getSize(); i++) {
								PROCIndex index = (PROCIndex) i;
								string curr_name = PKB::procedures.getPROCName(index);
								vector<PROC> getcalledbystar = PKB::calls.getCalledByStar(index);
								if (!getcalledbystar.empty()) {
									for (int j = 0; j < getcalledbystar.size(); j++) {
										PROCIndex index2 = (PROCIndex) j;
										string next_name = PKB::procedures.getPROCName(index2);
										newans.push_back(make_pair(curr_name, next_name));
									}
								}
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
				
							}
						}
						break; 
						
					case QueryEnums::Follows: 
						if (firsttype != QueryEnums::QueryVar::Stmt || secondtype != QueryEnums::QueryVar::Stmt) {
							querycluster.pop_back();
							vector<pair<string, string>> newans;
							newans.push_back(make_pair("false", "false"));
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
				
						} else {
							int maxlines = PKB::maxProgLines;
							querycluster.pop_back();
							vector<pair<int, int>> newans;
					
							for (int i = 0; i < maxlines; i++) {
								STMT index = i + 1;
								int followed = PKB::follows.getFollowsBy(index);
								if (followed != -1) {
									newans.push_back(make_pair(index, followed));
								}
							}
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
						}
						break; 
						
					case QueryEnums::FollowsStar: 
						if (firsttype != QueryEnums::QueryVar::Stmt || secondtype != QueryEnums::QueryVar::Stmt) {
							querycluster.pop_back();
							vector<pair<string, string>> newans;
							newans.push_back(make_pair("false", "false"));
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
						} else {
							int maxlines = PKB::maxProgLines;
							querycluster.pop_back();
							vector<pair<int, int>> newans;
							for (int i = 0; i < maxlines; i++) {
								int index = i + 1;
								vector<STMT> getfollowsbystar = PKB::follows.getFollowsStarBy(index);
								if (!getfollowsbystar.empty()) {
									for (int j = 0; j < getfollowsbystar.size(); j++) {
										newans.push_back(make_pair(index, getfollowsbystar[j]));
									}
								}
							}
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
						}
				
						break; 
					case QueryEnums::Parent: 
						if (firsttype != QueryEnums::QueryVar::Stmt || secondtype != QueryEnums::QueryVar::Stmt) {
							querycluster.pop_back();
							vector<pair<string, string>> newans;
							newans.push_back(make_pair("false", "false"));
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
						} else {
							int maxlines = PKB::maxProgLines;
							querycluster.pop_back();
							vector<pair<int, int>> newans;
							for (int i = 0; i < maxlines; i++) {
								STMT index = i + 1;
								vector<STMT> children = PKB::parent.getChildren(index);
								if (!children.empty()) {
									for (int j = 0; j < children.size(); j++) {
									newans.push_back(make_pair(index, children[j]));
									}
								}
							}
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
						}
						break; 

					case QueryEnums::ParentStar: 
						if (firsttype != QueryEnums::QueryVar::Stmt || secondtype != QueryEnums::QueryVar::Stmt) {
							querycluster.pop_back();
							vector<pair<string, string>> newans;
							newans.push_back(make_pair("false", "false"));
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
						} else {
							int maxlines = PKB::maxProgLines;
							querycluster.pop_back();
							vector<pair<int, int>> newans;
							for (int i = 0; i < maxlines; i++) {
								int index = i + 1;
								vector<STMT> getchildrenstar = PKB::parent.getChildrenStar(index);
								if (!getchildrenstar.empty()) {
									for (int j = 0; j < getchildrenstar.size(); j++) {
										newans.push_back(make_pair(index, getchildrenstar[j]));
									}
								}
							}
							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
							querycluster.push_back(newprojnode);
					}
					break; 
					case QueryEnums::Uses: 
						break; 
					case QueryEnums::Modifies: 
						break; 
				}
			}
		}
	}
}
