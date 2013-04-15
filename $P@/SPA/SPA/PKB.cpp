#pragma once
#include "PKB.h"


vector<pair<RulesOfEngagement::QueryRelations, int>> PKB::sortorder;

//Store the root of the AST
ASTNode* PKB::rootNode;

//For each procedure, there will be one CFGHead and CFGTail
vector<CFGNode*> PKB::CFGHeads;
vector<CFGNode*> PKB::CFGTails;


VARTable PKB::variables;
PROCTable PKB::procedures;

FollowsTable PKB::follows;
ParentTable PKB::parent;

UsesTable PKB::uses;
ModifiesTable PKB::modifies;
CallsTable PKB::calls;

AffectsTable PKB::affects;
NextTable PKB::next;

int PKB::maxProgLines;

vector<ASTNode::NodeType> PKB::statementTable;
unordered_set<STMT> PKB::assignTable;
unordered_set<STMT> PKB::callTable;
unordered_set<STMT> PKB::whileTable;
unordered_set<STMT> PKB::ifTable;

unordered_set<STMT> PKB::statementListTable;
vector<ASTNode*> PKB::statementNodes;
vector<StmtRef> PKB::stmtRefMap;

unordered_map<STMT, ASTNode*> PKB::assignNodes;
unordered_map<STMT, ASTNode*> PKB::callNodes;
unordered_map<STMT, ASTNode*> PKB::whileNodes;
unordered_map<STMT, ASTNode*> PKB::ifNodes;
unordered_map<ASTNode*, STMT> PKB::assignNodesBack;
unordered_map<ASTNode*, STMT> PKB::callNodesBack;
unordered_map<ASTNode*, STMT> PKB::whileNodesBack;
unordered_map<ASTNode*, STMT> PKB::ifNodesBack;

vector<vector<ASTNode*>> PKB::oprNodes;
vector<vector<ASTNode*>> PKB::varNodes;
vector<ASTNode*> PKB::stmtLstNodes;
unordered_map<int, vector<ASTNode*>> PKB::constNodes;


vector<pair<STMT, STMT>> PKB::TheBeginningAndTheEnd;
unordered_map<int, vector<STMT>> PKB::constantsTable;

/**
* This method will be used to get the all the ASTNode of the type
* @param type the NodeType to get
* @return list of ASTNode that is of the type
*/
vector<ASTNode*> PKB::getNodes(ASTNode::NodeType type ){
	vector<ASTNode*> result;
	switch (type)
	{
	case ASTNode::Procedure:
	{
			return PKB::rootNode->getChildren();
	}
	case ASTNode::StmtLst:
		 {
			 return PKB::stmtLstNodes;
		 }
	case ASTNode::Assign:
		{
			for (auto it = PKB::assignNodes.begin(); it != PKB::assignNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	case ASTNode::Call:
		{
			for (auto it = PKB::callNodes.begin(); it != PKB::callNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	case ASTNode::While:
		{
			for (auto it = PKB::whileNodes.begin(); it != PKB::whileNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	case ASTNode::If:
		{
			for (auto it = PKB::ifNodes.begin(); it != PKB::ifNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	/*case ASTNode::Operator:
		{
			return PKB::oprNodes;
		}
	 case ASTNode::Variable:
		 {
			 return PKB::varNodes;
		 }
	 case ASTNode::Constant:
		 {
			 return PKB::constNodes;
		 }*/
	 

		return result;
	}
}

/**
* This method will be used to get the all the ASTNode of the type with specified value
* @param type the NodeType to get
* @param value the value of the Node to get
* @return list of ASTNode that is of the type and value
*/
vector<ASTNode*> PKB::getNodes(ASTNode::NodeType type, int value)
{
	vector<ASTNode*> result;
	switch (type)
	{
	case ASTNode::Procedure:
	{
		for (int i = 0; i < PKB::rootNode->getChildren().size(); i++){
			if (PKB::rootNode->getChildren().at(i)->getValue() == value)
					result.push_back(PKB::rootNode->getChildren().at(i));
		}
	}
	case ASTNode::Call:
		{
			for (auto it = PKB::callNodes.begin(); it != PKB::callNodes.end(); it++){
				if ((*it).second->getValue() == value)
					result.push_back((*it).second);
			}
		}
	/*case ASTNode::Operator:
		{
			for (int i = 0; i < PKB::oprNodes.size(); i++){
				if (PKB::oprNodes.at(i)->getValue() == value)
					result.push_back(PKB::oprNodes.at(i));
			}
		}
	 case ASTNode::Variable:
		 {
			for (int i = 0; i < PKB::varNodes.size(); i++){
				if (PKB::varNodes.at(i)->getValue() == value)
					result.push_back(PKB::varNodes.at(i));
			}
		 }
	 case ASTNode::Constant:
		 {
			for (int i = 0; i < PKB::constNodes.size(); i++){
				if (PKB::constNodes.at(i)->getValue() == value)
					result.push_back(PKB::constNodes.at(i));
			}
		 }*/
	 
	}
		
	return result;
}
