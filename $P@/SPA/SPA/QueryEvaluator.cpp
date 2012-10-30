#pragma once
#include "StdAfx.h"
//#include "QueryEvaluator.h"
//#include "PKB.h"
//#include "QueryParser.h"
//#include "QueryPreprocessor.h"
//#include "QueryTreeBuilder.h"
//#include "QueryTreeNode.h"
//#include "QueryRelNode.h"
//#include "QueryCondNode.h"
//#include "QuerySelNode.h"
//#include "QueryEnums.h"
//#include "PROCTable.h"
//#include "QueryProjectNode.h"
//#include "CallsTable.h"
//#include "ASTNode.h"
//
////int main(int argc, char* arg[])
////{
////	vector<string> tokens; 
////	QueryPreprocessor QPP;
////	QueryParser QP;
////	QueryTreeBuilder QTB;
////	vector<vector<QueryTreeNode*>> QT;
////	vector<QueryTreeNode*> cluster;
////	vector<pair<QueryEnums::QueryVar, string>> selected;
////	QueryTreeNode::QTNodeType NT;
////	QueryProjectNode* p;
////	QuerySelNode* sn;
////	QueryEvaluator* QE = new QueryEvaluator();
////	vector<string> ans;
////	tokens = QP.tokenize("assign a1; select a1 such that follows(a1, 10)");
////	QPP.preProcess(tokens);
////	QTB.buildQueryTree(QPP.getUserVariables(), QPP.getSelectVariables(), QPP.getRelationships(), QPP.getConditions());
////	QT = QTB.getQueryTree();
////	ans = QE->returnAnswer(QPP, QTB);
////	
////	for (int i = 0; i < ans.size(); i++)
////		cout<<ans.at(i)<<" ";
////
////	cout<<endl;
////
////	system("PAUSE");
////	return 0;
////}
//
//QueryEvaluator::QueryEvaluator(void)
//{
//}
//
//vector<string> QueryEvaluator::intersection(vector<string> v1, vector<string> v2) 
//{
//	vector<string> desiredAnswer;
//	unordered_map<string, bool> store;
//	for (int i = 0; i < v1.size(); i++) {
//		store[v1[i]] = true;
//	}
//	for (int i = 0; i < v2.size(); i++) {
//		if (store[v2[i]] == true) {
//			desiredAnswer.push_back(v2[i]);
//		}
//	}
//	return desiredAnswer;
//}
//
//vector<string> QueryEvaluator::listfirst(vector<pair<string, string>> v) {
//	vector<string> ans;
//	if (!v.empty()) {
//		for (int i = 0; i < v.size(); i++) {
//			pair<string, string> current = v[i];
//			string first = current.first;
//			if (std::find(ans.begin(), ans.end(), first) != ans.end()) {
//				ans.push_back(first);
//			}
//		}
//	}
//	return ans;
//}
//
//vector<string> QueryEvaluator::listsecond(vector<pair<string, string>> v) {
//	vector<string> ans;
//	if (!v.empty()) {
//		for (int i = 0; i < v.size(); i++) {
//			pair<string, string> current = v[i];
//			string second = current.second;
//			if (std::find(ans.begin(), ans.end(), second) != ans.end()) {
//				ans.push_back(second);
//			}
//		}
//	}
//	return ans;
//}
//
//vector<string> QueryEvaluator::returnAnswer(QueryPreprocessor querypreprocessor, QueryTreeBuilder querytreebuilder) {
//	unordered_map<int, vector<string>> userVariables = querypreprocessor.getUserVariables();
//	for (auto it = userVariables.begin(); it != userVariables.end(); it++) {
//		vector<string> variableNames = (*it).second;
//		for (int i = 0; i < variableNames.size(); i++) {
//			string currentVariableName = variableNames[i];
//			answer[currentVariableName].first = false;
//			vector<string> newvector;
//			answer[currentVariableName].second = newvector;
//		}
//	}
//	vector<vector<QueryTreeNode*>> querytree = querytreebuilder.getQueryTree();
//	for (int i = 0; i < querytree.size(); i++) {
//		vector<QueryTreeNode*> querycluster = querytree[i];
//		QueryTreeNode* currentnode = querycluster[0];
//		QueryTreeNode::QTNodeType nodetype = currentnode->getNodeType();
//		
//		if (nodetype == QueryTreeNode::Relationship) {
//			QueryRelNode* rnode = (QueryRelNode*) currentnode;
//			QueryEnums::QueryRel rtype = rnode->getRelationshipType();
//			switch(rtype) {
//					case QueryEnums::Calls: {
//				
//				QueryEnums::QueryVar firsttype = rnode->getFirstVariableType();
//				QueryEnums::QueryVar secondtype = rnode->getSecondVariableType();
//				querycluster.pop_back();
//				vector<pair<string, string>> newans;
//				
//				if (firsttype != QueryEnums::Procedure || secondtype != QueryEnums::Procedure) {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//				} else {
//					string firstname = rnode->getFirstVariableName();
//					string secondname = rnode->getSecondVariableName();
//					if ((char*) firstname[0] == "\"" && (char*) secondname[0] == "\"") {
//						PROCIndex firstproc = PKB::procedures.getPROCIndex(firstname);
//						PROCIndex secondproc = PKB::procedures.getPROCIndex(secondname);
//						if (PKB::calls.isCalled(firstproc, secondproc)) {
//							newans.push_back(make_pair(firstname, secondname));
//						} 
//							QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//							querycluster.push_back(newprojnode);
//					} else if ((char*) firstname[0] == "\"" && (char*) secondname[0] != "\"") {
//						PROCIndex firstproc = PKB::procedures.getPROCIndex(firstname);
//						vector<PROC> listofproc = PKB::calls.getCalledBy(firstproc);
//						if (!listofproc.empty()) {
//							for (int i = 0; i< listofproc.size(); i++) {
//						string currentprocname = PKB::procedures.getPROCName(i);
//						newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if ((char*) firstname[0] != "\"" && (char*) secondname[0] == "\"") {
//						PROCIndex secondproc = PKB::procedures.getPROCIndex(secondname);
//						vector<PROC> listofproc = PKB::calls.getCalledFrom(secondproc);
//						if (!listofproc.empty()) {
//							for (int i = 0; i< listofproc.size(); i++) {
//						string currentprocname = PKB::procedures.getPROCName(i);
//						newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//						for (int i = 0; i < PKB::procedures.getSize(); i++) {
//						PROCIndex index = (PROCIndex) i;
//						string curr_name = PKB::procedures.getPROCName(index);
//						vector<PROC> getcalledby = PKB::calls.getCalledBy(index);
//						if (!getcalledby.empty()) {
//							for (int j = 0; j < getcalledby.size(); j++) {
//								PROCIndex index2 = (PROCIndex) j;
//								string next_name = PKB::procedures.getPROCName(index2);
//								newans.push_back(make_pair(curr_name, next_name));
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				} 
//				break; 
//			} case QueryEnums::CallsStar: {
//				
//				QueryEnums::QueryVar firsttype = rnode->getFirstVariableType();
//				QueryEnums::QueryVar secondtype = rnode->getSecondVariableType();
//				querycluster.pop_back();
//				vector<pair<string, string>> newans;
//				
//				if (firsttype != QueryEnums::Procedure || secondtype != QueryEnums::Procedure) {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//				} else {
//					string firstname = rnode->getFirstVariableName();
//					string secondname = rnode->getSecondVariableName();
//					if ((char*) firstname[0] == "\"" && (char*) secondname[0] == "\"") {
//						PROCIndex firstproc = PKB::procedures.getPROCIndex(firstname);
//						PROCIndex secondproc = PKB::procedures.getPROCIndex(secondname);
//						if (PKB::calls.isCalledStar(firstproc, secondproc)) {
//							newans.push_back(make_pair(firstname, secondname));
//						} 
//						QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//						querycluster.push_back(newprojnode);
//					} else if ((char*) firstname[0] == "\"" && (char*) secondname[0] != "\"") {
//						PROCIndex firstproc = PKB::procedures.getPROCIndex(firstname);
//						vector<PROC> listofproc = PKB::calls.getCalledByStar(firstproc);
//						if (!listofproc.empty()) {
//							for (int i = 0; i< listofproc.size(); i++) {
//						string currentprocname = PKB::procedures.getPROCName(i);
//						newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if ((char*) firstname[0] != "\"" && (char*) secondname[0] == "\"") {
//						PROCIndex secondproc = PKB::procedures.getPROCIndex(secondname);
//						vector<PROC> listofproc = PKB::calls.getCalledFromStar(secondproc);
//						if (!listofproc.empty()) {
//							for (int i = 0; i< listofproc.size(); i++) {
//						string currentprocname = PKB::procedures.getPROCName(i);
//						newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//					for (int i = 0; i < PKB::procedures.getSize(); i++) {
//						PROCIndex index = (PROCIndex) i;
//						string curr_name = PKB::procedures.getPROCName(index);
//						vector<PROC> getcalledby = PKB::calls.getCalledByStar(index);
//						if (!getcalledby.empty()) {
//							for (int j = 0; j < getcalledby.size(); j++) {
//								PROCIndex index2 = (PROCIndex) j;
//								string next_name = PKB::procedures.getPROCName(index2);
//								newans.push_back(make_pair(curr_name, next_name));
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				} 
//				break; 
//			} case QueryEnums::Follows: {
//				QueryEnums::QueryVar firsttype = rnode->getFirstVariableType();
//				QueryEnums::QueryVar secondtype = rnode->getSecondVariableType();
//				querycluster.pop_back();
//				vector<pair<string, string>> newans;
//				if (firsttype == QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					{
//					string firstname = rnode->getFirstVariableName();
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = atoi(secondname.c_str());
//						if (PKB::follows.isFollows(firststmt,secondstmt)) {
//							newans.push_back(make_pair(firstname, secondname));
//						} 
//						QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//						querycluster.push_back(newprojnode);
//					} else if (querypreprocessor.isNumber(firstname) && !querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = PKB::follows.getFollowsBy(firststmt);
//						if (secondstmt != -1) {
//							ostringstream convert;
//							convert << secondstmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (!querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::follows.getFollowsFrom(secondstmt);
//						if (firststmt != -1) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//						int maxlines = PKB::maxProgLines;
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				} 
//				} else if (firsttype == QueryEnums::Stmt && secondtype != QueryEnums::Stmt) {
//					string firstname = rnode->getFirstVariableName();
//					if (querypreprocessor.isNumber(firstname)) {
//						if (secondtype == QueryEnums::Assign) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = PKB::follows.getFollowsBy(firststmt);
//						if (secondstmt != -1) {
//							if (PKB::statementTable[secondstmt - 1] == ASTNode::Assign) {
//							ostringstream convert;
//							convert << secondstmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::While) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = PKB::follows.getFollowsBy(firststmt);
//						if (secondstmt != -1) {
//							if (PKB::statementTable[secondstmt - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << secondstmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = PKB::follows.getFollowsBy(firststmt);
//						if (secondstmt != -1) {
//							if (PKB::statementTable[secondstmt - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << secondstmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::Call) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = PKB::follows.getFollowsBy(firststmt);
//						if (secondstmt != -1) {
//							if (PKB::statementTable[secondstmt - 1] == ASTNode::Call) {
//							ostringstream convert;
//							convert << secondstmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						if (secondtype == QueryEnums::Assign) {
//					int maxlines = PKB::assignTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsFrom(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << followed;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsFrom(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << followed;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsFrom(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << followed;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::Call) {
//					int maxlines = PKB::callTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsFrom(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << followed;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else if (firsttype != QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(secondname)) {
//						if (firsttype == QueryEnums::Assign) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::follows.getFollowsFrom(secondstmt);
//						if (firststmt != -1) {
//							if (PKB::statementTable[firststmt - 1] == ASTNode::Assign) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::While) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::follows.getFollowsFrom(secondstmt);
//						if (firststmt != -1) {
//							if (PKB::statementTable[firststmt - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::If) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::follows.getFollowsFrom(secondstmt);
//						if (firststmt != -1) {
//							if (PKB::statementTable[firststmt - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);						
//						} else if (firsttype == QueryEnums::Call) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::follows.getFollowsFrom(secondstmt);
//						if (firststmt != -1) {
//							if (PKB::statementTable[firststmt - 1] == ASTNode::Call) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						if (firsttype == QueryEnums::Assign) {
//					int maxlines = PKB::assignTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::Call) {
//					int maxlines = PKB::callTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else {
//					if (firsttype == QueryEnums::Assign) {
//					int maxlines = PKB::assignTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							if (secondtype == QueryEnums::Assign) {
//								if (PKB::statementTable[followed - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[followed - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[followed - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[followed - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (firsttype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							if (secondtype == QueryEnums::Assign) {
//								if (PKB::statementTable[followed - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[followed - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[followed - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[followed - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (firsttype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							if (secondtype == QueryEnums::Assign) {
//								if (PKB::statementTable[followed - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[followed - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[followed - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[followed - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (firsttype == QueryEnums::Call) {
//					int maxlines = PKB::callTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int followed = PKB::follows.getFollowsBy(index);
//						if (followed != -1) {
//							if (secondtype == QueryEnums::Assign) {
//								if (PKB::statementTable[followed - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[followed - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[followed - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[followed - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << followed;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				break;
//			} case QueryEnums::FollowsStar: {
//				QueryEnums::QueryVar firsttype = rnode->getFirstVariableType();
//				QueryEnums::QueryVar secondtype = rnode->getSecondVariableType();
//				querycluster.pop_back();
//				vector<pair<string, string>> newans;
//				if (firsttype == QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					{
//					string firstname = rnode->getFirstVariableName();
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = atoi(secondname.c_str());
//						if (PKB::follows.isFollowsStar(firststmt,secondstmt)) {
//							newans.push_back(make_pair(firstname, secondname));
//						} 
//						QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//						querycluster.push_back(newprojnode);
//					} else if (querypreprocessor.isNumber(firstname) && !querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::follows.getFollowsStarBy(firststmt);
//						if (!secondstmt.empty()) {
//							for (int i = 0; i < secondstmt.size(); i++) {
//								STMT current = secondstmt[i];
//							if (current != -1) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//								}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (!querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::follows.getFollowsStarFrom(secondstmt);
//						if (!firststmt.empty()) {
//							for (int i = 0; i < firststmt.size(); i++) {
//								STMT current = firststmt[i];
//							if (current != -1) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//								}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//						int maxlines = PKB::maxProgLines;
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				} 
//				} else if (firsttype == QueryEnums::Stmt && secondtype != QueryEnums::Stmt) {
//					string firstname = rnode->getFirstVariableName();
//					if (querypreprocessor.isNumber(firstname)) {
//						if (secondtype == QueryEnums::Assign) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::follows.getFollowsStarBy(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//							STMT current = secondstmt[j];
//							if (current != -1) {
//							if (PKB::statementTable[current - 1] == ASTNode::Assign) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::While) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::follows.getFollowsStarBy(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//							STMT current = secondstmt[j];
//							if (current != -1) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::follows.getFollowsStarBy(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//							STMT current = secondstmt[j];
//							if (current != -1) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::Call) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::follows.getFollowsStarBy(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//							STMT current = secondstmt[j];
//							if (current != -1) {
//							if (PKB::statementTable[current - 1] == ASTNode::Call) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						if (secondtype == QueryEnums::Assign) {
//					int maxlines = PKB::assignTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarFrom(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << current;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarFrom(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << current;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarFrom(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << current;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::Call) {
//					int maxlines = PKB::callTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarFrom(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << current;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else if (firsttype != QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(secondname)) {
//						if (firsttype == QueryEnums::Assign) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::follows.getFollowsStarFrom(secondstmt);
//						if (!firststmt.empty()) {
//							for (int j = 0; j < firststmt.size(); j++) {
//								STMT index = firststmt[j];
//							if (index != -1) {
//							if (PKB::statementTable[index - 1] == ASTNode::Assign) {
//							ostringstream convert;
//							convert << index;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::While) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::follows.getFollowsStarFrom(secondstmt);
//						if (!firststmt.empty()) {
//							for (int j = 0; j < firststmt.size(); j++) {
//								STMT index = firststmt[j];
//							if (index != -1) {
//							if (PKB::statementTable[index - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << index;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::If) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::follows.getFollowsStarFrom(secondstmt);
//						if (!firststmt.empty()) {
//							for (int j = 0; j < firststmt.size(); j++) {
//								STMT index = firststmt[j];
//							if (index != -1) {
//							if (PKB::statementTable[index - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << index;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);						
//						} else if (firsttype == QueryEnums::Call) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::follows.getFollowsStarFrom(secondstmt);
//						if (!firststmt.empty()) {
//							for (int j = 0; j < firststmt.size(); j++) {
//								STMT index = firststmt[j];
//							if (index != -1) {
//							if (PKB::statementTable[index - 1] == ASTNode::Call) {
//							ostringstream convert;
//							convert << index;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						if (firsttype == QueryEnums::Assign) {
//					int maxlines = PKB::assignTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::Call) {
//					int maxlines = PKB::callTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else {
//					if (firsttype == QueryEnums::Assign) {
//					int maxlines = PKB::assignTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							if (secondtype == QueryEnums::Assign) {
//							if (PKB::statementTable[current - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[current - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//							}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (firsttype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							if (secondtype == QueryEnums::Assign) {
//							if (PKB::statementTable[current - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[current - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//							}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (firsttype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							if (secondtype == QueryEnums::Assign) {
//							if (PKB::statementTable[current - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[current - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//							}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (firsttype == QueryEnums::Call) {
//					int maxlines = PKB::callTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::follows.getFollowsStarBy(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							if (secondtype == QueryEnums::Assign) {
//							if (PKB::statementTable[current - 1] == ASTNode::Assign) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::Call) {
//							if (PKB::statementTable[current - 1] == ASTNode::Call) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							}
//							}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				break;
//			} case QueryEnums::Parent: {
//				QueryEnums::QueryVar firsttype = rnode->getFirstVariableType();
//				QueryEnums::QueryVar secondtype = rnode->getSecondVariableType();
//				querycluster.pop_back();
//				vector<pair<string, string>> newans;
//				if (firsttype == QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					{
//					string firstname = rnode->getFirstVariableName();
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = atoi(secondname.c_str());
//						if (PKB::parent.isParent(firststmt,secondstmt)) {
//							newans.push_back(make_pair(firstname, secondname));
//						} 
//						QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//						querycluster.push_back(newprojnode);
//					} else if (querypreprocessor.isNumber(firstname) && !querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::parent.getChildren(firststmt);
//						if (!secondstmt.empty()) {
//							for (int i = 0; i < secondstmt.size(); i++) {
//								STMT current = secondstmt[i];
//							if (current != -1) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//								}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (!querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::parent.getParent(secondstmt);
//						if (firststmt != -1) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//						int maxlines = PKB::maxProgLines;
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int parent = PKB::parent.getParent(index);
//						if (parent != -1) {
//							ostringstream convert1, convert2;
//							convert1 << parent;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				} 
//				} else if (firsttype == QueryEnums::Stmt && secondtype != QueryEnums::Stmt) {
//					string firstname = rnode->getFirstVariableName();
//					if (querypreprocessor.isNumber(firstname)) {
//						if (secondtype == QueryEnums::While) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::parent.getChildren(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//								STMT child = secondstmt[j];
//							if (child != -1) {
//							if (PKB::statementTable[child - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << child;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::parent.getChildren(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//								STMT child = secondstmt[j];
//							if (child != -1) {
//							if (PKB::statementTable[child - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << child;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						if (secondtype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int parent = PKB::parent.getParent(index);
//						if (parent != -1) {
//							ostringstream convert1, convert2;
//							convert1 << parent;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int parent = PKB::parent.getParent(index);
//						if (parent != -1) {
//							ostringstream convert1, convert2;
//							convert1 << parent;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else if (firsttype != QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(secondname)) {
//						if (firsttype == QueryEnums::While) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::parent.getParent(secondstmt);
//						if (firststmt != -1) {
//							if (PKB::statementTable[firststmt - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::If) {
//						STMT secondstmt = atoi(secondname.c_str());
//						STMT firststmt = PKB::parent.getParent(secondstmt);
//						if (firststmt != -1) {
//							if (PKB::statementTable[firststmt - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << firststmt;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);						
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						if (secondtype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> children = PKB::parent.getChildren(index);
//						if (!children.empty()) {
//							for (int j = 0; j < children.size(); j++) {
//								STMT child = children[j];
//							if (child != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << child;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> children = PKB::parent.getChildren(index);
//						if (!children.empty()) {
//							for (int j = 0; j < children.size(); j++) {
//								STMT child = children[j];
//							if (child != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << child;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else {
//					if (secondtype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int parent = PKB::follows.getFollowsBy(index);
//						if (parent != -1) {
//							if (firsttype == QueryEnums::While) {
//							if (PKB::statementTable[parent - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << parent;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (firsttype == QueryEnums::If) {
//							if (PKB::statementTable[parent - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << parent;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} 
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (secondtype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						int parent = PKB::follows.getFollowsBy(index);
//						if (parent != -1) {
//							if (firsttype == QueryEnums::While) {
//							if (PKB::statementTable[parent - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << parent;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (firsttype == QueryEnums::If) {
//							if (PKB::statementTable[parent - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << parent;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} 
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				break;
//			} case QueryEnums::ParentStar: {
//				QueryEnums::QueryVar firsttype = rnode->getFirstVariableType();
//				QueryEnums::QueryVar secondtype = rnode->getSecondVariableType();
//				querycluster.pop_back();
//				vector<pair<string, string>> newans;
//				if (firsttype == QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					{
//					string firstname = rnode->getFirstVariableName();
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						STMT secondstmt = atoi(secondname.c_str());
//						if (PKB::parent.isParentStar(firststmt,secondstmt)) {
//							newans.push_back(make_pair(firstname, secondname));
//						} 
//						QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//						querycluster.push_back(newprojnode);
//					} else if (querypreprocessor.isNumber(firstname) && !querypreprocessor.isNumber(secondname)) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::parent.getChildrenStar(firststmt);
//						if (!secondstmt.empty()) {
//							for (int i = 0; i < secondstmt.size(); i++) {
//								STMT current = secondstmt[i];
//							if (current != -1) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//								}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (!querypreprocessor.isNumber(firstname) && querypreprocessor.isNumber(secondname)) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::parent.getParentStar(secondstmt);
//						if (!firststmt.empty()) {
//							for (int i = 0; i < firststmt.size(); i++) {
//								STMT current = firststmt[i];
//							if (current != -1) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//								}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//						int maxlines = PKB::maxProgLines;
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::parent.getChildrenStar(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				} 
//				} else if (firsttype == QueryEnums::Stmt && secondtype != QueryEnums::Stmt) {
//					string firstname = rnode->getFirstVariableName();
//					if (querypreprocessor.isNumber(firstname)) {
//						if (secondtype == QueryEnums::While) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::parent.getChildrenStar(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//							STMT current = secondstmt[j];
//							if (current != -1) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//						STMT firststmt = atoi(firstname.c_str());
//						vector<STMT> secondstmt = PKB::parent.getChildrenStar(firststmt);
//						if (!secondstmt.empty()) {
//							for (int j = 0; j < secondstmt.size(); j++) {
//							STMT current = secondstmt[j];
//							if (current != -1) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << current;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(firstname, currentprocname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						if (secondtype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::parent.getParentStar(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << current;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::parent.getParentStar(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << current;
//							convert2 << index;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else if (firsttype != QueryEnums::Stmt && secondtype == QueryEnums::Stmt) {
//					string secondname = rnode->getSecondVariableName();
//					if (querypreprocessor.isNumber(secondname)) {
//						 if (firsttype == QueryEnums::While) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::parent.getParentStar(secondstmt);
//						if (!firststmt.empty()) {
//							for (int j = 0; j < firststmt.size(); j++) {
//								STMT index = firststmt[j];
//							if (index != -1) {
//							if (PKB::statementTable[index - 1] == ASTNode::While) {
//							ostringstream convert;
//							convert << index;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (firsttype == QueryEnums::If) {
//						STMT secondstmt = atoi(secondname.c_str());
//						vector<STMT> firststmt = PKB::parent.getParentStar(secondstmt);
//						if (!firststmt.empty()) {
//							for (int j = 0; j < firststmt.size(); j++) {
//								STMT index = firststmt[j];
//							if (index != -1) {
//							if (PKB::statementTable[index - 1] == ASTNode::If) {
//							ostringstream convert;
//							convert << index;
//							string currentprocname = convert.str();
//							newans.push_back(make_pair(currentprocname, secondname));
//							}
//						}
//							}
//						}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);						
//						} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					} else {
//						 if (secondtype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::parent.getChildrenStar(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						} else if (secondtype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::parent.getChildren(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//								STMT current = followed[j];
//							if (current != -1) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//						}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}  else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//						}
//					}
//				} else { 
//					if (firsttype == QueryEnums::While) {
//					int maxlines = PKB::whileTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::parent.getChildrenStar(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} 
//							}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else if (firsttype == QueryEnums::If) {
//					int maxlines = PKB::ifTable.size();
//					for (int i = 0; i < maxlines; i++) {
//						STMT index = i + 1;
//						vector<STMT> followed = PKB::parent.getChildrenStar(index);
//						if (!followed.empty()) {
//							for (int j = 0; j < followed.size(); j++) {
//							STMT current = followed[j];
//							if (current != -1) {
//							if (secondtype == QueryEnums::While) {
//							if (PKB::statementTable[current - 1] == ASTNode::While) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} else if (secondtype == QueryEnums::If) {
//							if (PKB::statementTable[current - 1] == ASTNode::If) {
//							ostringstream convert1, convert2;
//							convert1 << index;
//							convert2 << current;
//							string firstans = convert1.str();
//							string secondans = convert2.str();
//							newans.push_back(make_pair(firstans, secondans));
//								}
//							} 
//							}
//							}
//						}
//					}
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					} else {
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				break;
//			}
//			case QueryEnums::Modifies: break;
//			case QueryEnums::Uses: break;
//			case QueryEnums::Next: break;
//			case QueryEnums::NextStar: break;
//			case QueryEnums::Affects: break;
//			case QueryEnums::AffectsStar: break;
//			default: break;
//			}
//		} else {
//			QueryCondNode* cnode = (QueryCondNode*) currentnode;
//			QueryEnums::QueryCond ctype = cnode->getConditionAttribute();
//			querycluster.pop_back();
//			vector<string> newans;
//			QueryEnums::QueryVar vartype = cnode->getConditionVariableType();
//			switch (ctype) {
//			case QueryEnums::ProcName: {
//				if (vartype != QueryEnums::Procedure) {
//				QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//				querycluster.push_back(newprojnode);
//				} else {
//				string procname = cnode->getConditionBoundaryName();
//				PROCIndex procindex = PKB::procedures.getPROCIndex(procname);
//				if (procindex != -1) {
//					newans.push_back(procname);
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//				}
//				}
//								break;
//				}
//			case QueryEnums::VarName: {
//				if (vartype != QueryEnums::Variable) {
//				QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//				querycluster.push_back(newprojnode);
//				} else {
//				string varname = cnode->getConditionBoundaryName();
//				PROCIndex varindex = PKB::variables.getVARIndex(varname);
//				if (varindex != -1) {
//					newans.push_back(varname);
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//				}
//				}
//								break;
//			}
//			case QueryEnums::StmtNo: {
//				if (vartype == QueryEnums::Stmt) {
//				string stmt = cnode->getConditionBoundaryName();
//				STMT stmtno = atoi(stmt.c_str());
//				if (!(stmtno > PKB::maxProgLines)) {
//					newans.push_back(stmt);
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//				}
//				} else if (vartype == QueryEnums::Assign) {
//				string stmt = cnode->getConditionBoundaryName();
//				STMT stmtno = atoi(stmt.c_str());
//				if (!(stmtno > PKB::maxProgLines)) {
//					if (PKB::statementTable[stmtno] == ASTNode::Assign) {
//					newans.push_back(stmt);
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				} else if (vartype == QueryEnums::While) {
//				string stmt = cnode->getConditionBoundaryName();
//				STMT stmtno = atoi(stmt.c_str());
//				if (!(stmtno > PKB::maxProgLines)) {
//					if (PKB::statementTable[stmtno] == ASTNode::While) {
//					newans.push_back(stmt);
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				} else if (vartype == QueryEnums::If) {
//				string stmt = cnode->getConditionBoundaryName();
//				STMT stmtno = atoi(stmt.c_str());
//				if (!(stmtno > PKB::maxProgLines)) {
//					if (PKB::statementTable[stmtno] == ASTNode::If) {
//					newans.push_back(stmt);
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				}  else if (vartype == QueryEnums::Call) {
//				string stmt = cnode->getConditionBoundaryName();
//				STMT stmtno = atoi(stmt.c_str());
//				if (!(stmtno > PKB::maxProgLines)) {
//					if (PKB::statementTable[stmtno] == ASTNode::Call) {
//					newans.push_back(stmt);
//					QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//					querycluster.push_back(newprojnode);
//					}
//				}
//				} else {
//									QueryProjectNode* newprojnode = new QueryProjectNode(newans);
//				querycluster.push_back(newprojnode);
//				}
//								break;
//			} 
//			case QueryEnums::Value: break;
//			default: break;
//			}
//			}
//	}
//	for (int i = 0; i < querytree.size(); i++) {
//		vector<QueryTreeNode*> querycluster = querytree[i];
//		QueryTreeNode* currentnode = querycluster[0];
//		QueryTreeNode::QTNodeType nodetype = currentnode->getNodeType();
//		if (nodetype == QueryTreeNode::Relationship) {
//			QueryRelNode* rnode = (QueryRelNode*) currentnode;
//			QueryProjectNode* projnode = (QueryProjectNode*) querycluster[2];
//			vector<pair<string, string>> listofanswers = projnode->getPairAnswer();
//			string firstname = rnode->getFirstVariableName();
//			string secondname = rnode->getSecondVariableName();
//			vector<string> listfirst = QueryEvaluator::listfirst(listofanswers);
//			vector<string> listsecond = QueryEvaluator::listsecond(listofanswers);
//			if (!answer[firstname].first) {
//				answer[firstname].first = true;
//				answer[firstname].second = listfirst;
//			} else {
//			vector<string> storedanswer1 = answer[firstname].second;
//			answer[firstname].second = QueryEvaluator::intersection(listfirst, storedanswer1);
//			} if (!answer[secondname].first) {
//				answer[secondname].first = true;
//				answer[secondname].second = listsecond;
//			} else {
//			vector<string> storedanswer2 = answer[secondname].second;
//			answer[secondname].second = QueryEvaluator::intersection(listsecond, storedanswer2);
//			}
//		} else {
//			QueryCondNode* cnode = (QueryCondNode*) currentnode;
//			QueryProjectNode* projnode = (QueryProjectNode*) querycluster[2];
//			vector<string> listofanswers = projnode->getAnswer();
//			string varname = cnode->getConditionVariableName();
//			if (!answer[varname].first) {
//				answer[varname].first = true;
//				answer[varname].second = listofanswers;
//			} else {
//			vector<string> storedanswer = answer[varname].second;
//			answer[varname].second = QueryEvaluator::intersection(listofanswers, storedanswer);
//			} 
//		}
//	}
//	unordered_map<int, vector<string>> selectVariables = querypreprocessor.getSelectVariables();
//	auto it = selectVariables.begin(); // ASSUME THAT WE ARE SELECTING ONLY ONE ELEMENT. FAILS IF WE ARE DOING MULTIPLE.
//	vector<string> variableNames = (*it).second;
//	string varName = variableNames[0]; // ASSUME THAT WE ARE SELECTING ONLY ONE ELEMENT. FAILS IF WE ARE DOING MULTIPLE.
//	vector<string> queryanswer = answer[varName].second;
//	return queryanswer;
//}