#pragma once
#include "StdAfx.h"
#include "PKB.h"
#include "ASTNode.h"
#include "ASTStmtLstNode.h"
#include "ASTStmtNode.h"
#include "ASTExprNode.h"
#include "CallsTable.h"
#include "ModifiesTable.h"
#include "UsesTable.h"
#include "ParentTable.h"
#include "FollowsTable.h"

class DesignExtractor
{
private:
	static int totalNumOfProcs;
	static unordered_map <PROC, unordered_set<PROC> > toProcAdjList;
	static unordered_map <PROC, unordered_set<PROC> > fromProcAdjList;
	static unordered_map <PROC, int> procCount;
	static unordered_map <PROC, vector< stack<ASTStmtNode*> > > savestate;


	static void buildFirstRound();
	static void buildOtherTables(PROC);
	static void CompleteExtraction();

	static void initializeStatisticalSortSize();
	
		
	struct sort_pred {
		bool operator()(const pair<RulesOfEngagement::QueryRelations, int> &i, 
			const pair<RulesOfEngagement::QueryRelations, int> &j) {
			return i.second < j.second;
		}
	};

public:
	static void extractDesign();
};