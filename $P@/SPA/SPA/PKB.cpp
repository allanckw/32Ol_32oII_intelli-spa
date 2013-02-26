#pragma once
#include "PKB.h"


vector<pair<RulesOfEngagement::QueryRelations, int>> PKB::sortorder;

//Store the root of the AST
ASTNode* PKB::rootNode;

//For each procedure, there will be one CFGHead 
vector<pair<CFGNode*, PROC>> PKB::CFGHeads;
vector<MyCFG*> PKB::MyCFGHeads;

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
unordered_map<int, ASTNode*> PKB::assignNodes;
unordered_set<STMT> PKB::callTable;
unordered_map<int, ASTNode*> PKB::callNodes;
unordered_set<STMT> PKB::whileTable;
unordered_map<int, ASTNode*> PKB::whileNodes;
unordered_set<STMT> PKB::ifTable;
unordered_map<int, ASTNode*> PKB::ifNodes;
unordered_set<STMT> PKB::statementListTable;
vector<ASTNode*> PKB::statementNodes;
vector<StmtRef> PKB::stmtRefMap;
vector<MyCFG*> PKB::bigTable;
vector<pair<STMT, STMT>> PKB::TheBeginningAndTheEnd;
unordered_map<int, vector<STMT>> PKB::constantsTable;

void PKB::addToCFGList(CFGNode* start, PROC p)
{
	if (!start->isStartNode())
		throw SPAException("The CFGNode must be of type StartNode to be added to CFGLists in the program!!!");

	else if (!PKB::procedures.isExists(p))
		throw SPAException("Procedure not found in source!p");

	else{
		pair<CFGNode*, PROC> p(start, p);
		PKB::CFGHeads.push_back(p);
	}
}

void PKB::addToCFGList(MyCFG* node)
{
	PKB::MyCFGHeads.push_back(node);
}

CFGNode* PKB::getCFGHead(PROC p)
{
	for (auto it = PKB::CFGHeads.begin() ; it != PKB::CFGHeads.end(); it++)
	{
		pair<CFGNode*, PROC> iten = *it;
		if (iten.second == p)
			return iten.first;
	}

	return NULL;
}