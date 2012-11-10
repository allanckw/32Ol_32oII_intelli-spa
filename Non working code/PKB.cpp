#pragma once
#include "PKB.h"

ASTNode* PKB::rootNode;

VARTable PKB::variables;
PROCTable PKB::procedures;

FollowsTable PKB::follows;
ParentTable PKB::parent;

UsesTable PKB::uses;
ModifiesTable PKB::modifies;
CallsTable PKB::calls;

int PKB::maxProgLines;

vector<ASTNode::NodeType> PKB::statementTable;
unordered_set<STMT> PKB::assignTable;
unordered_set<STMT> PKB::callTable;
unordered_set<STMT> PKB::whileTable;
unordered_set<STMT> PKB::ifTable;
vector<pair<STMT, STMT>> PKB::TheBeginningAndTheEnd;
unordered_map<VAR, vector<STMT>> PKB::constantsTable;
