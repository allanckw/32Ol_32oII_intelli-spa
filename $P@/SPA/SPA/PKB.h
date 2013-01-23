#pragma once
#include "stdAfx.h"
#include "PROCTable.h"
#include "VARTable.h"
#include "ASTNode.h"
#include "ModifiesTable.h"
#include "CallsTable.h"
#include "UsesTable.h"
#include "FollowsTable.h"
#include "ParentTable.h"
#include "CFGNode.h"

class PKB
{
public:
	static ASTNode* rootNode;
	static void addToCFGList(CFGNode* start, PROC p);

	static int maxProgLines;

	static PROCTable procedures;
	static VARTable variables;
	
	static UsesTable uses;
	static ModifiesTable modifies;
	static CallsTable calls;

	static FollowsTable follows;
	static ParentTable parent;

	static vector<ASTNode::NodeType> statementTable;
	static unordered_set<STMT> assignTable;
	static unordered_set<STMT> callTable;
	static unordered_set<STMT> whileTable;
	static unordered_set<STMT> ifTable;
	static vector<pair<STMT, STMT>> TheBeginningAndTheEnd;
	static unordered_map<int, vector<STMT>> constantsTable;
	static unordered_map<int, ASTNode*> assignNodes;

	static CFGNode* getCFGHead(PROC p);

private:
	static vector<pair<CFGNode*, PROC>> CFGHeads;
	
};

