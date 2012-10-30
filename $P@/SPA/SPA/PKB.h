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

class PKB
{
public:
	static int maxProgLines;

	static PROCTable procedures;
	static VARTable variables;
	static ASTNode* rootNode;

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


};

