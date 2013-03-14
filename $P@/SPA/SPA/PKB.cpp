#pragma once
#include "PKB.h"


vector<pair<RulesOfEngagement::QueryRelations, int>> PKB::sortorder;

//Store the root of the AST
ASTNode* PKB::rootNode;

//For each procedure, there will be one CFGHead 
vector<CFGNode*> PKB::CFGHeads;


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

unordered_map<int, ASTNode*> PKB::assignNodes;
unordered_map<int, ASTNode*> PKB::callNodes;
unordered_map<int, ASTNode*> PKB::ifNodes;
unordered_map<int, ASTNode*> PKB::whileNodes;

vector<ASTNode*> PKB::oprNodes;
vector<ASTNode*> PKB::varNodes;
vector<ASTNode*> PKB::stmtLstNodes;
vector<ASTNode*> PKB::constNodes;


vector<pair<STMT, STMT>> PKB::TheBeginningAndTheEnd;
unordered_map<int, vector<STMT>> PKB::constantsTable;


void PKB::addToCFGList(CFGNode* node)
{
	PKB::CFGHeads.push_back(node);
}

