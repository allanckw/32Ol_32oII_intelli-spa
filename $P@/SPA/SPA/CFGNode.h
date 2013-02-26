#pragma once
#include "StdAfx.h"
#include "IntervalList.h"

class CFGNode
{
public:
	enum NodeType { 
		StandardNode,
		WhileNode,
		IfNode,
		DummyNode
	};

	struct WhileChildren {
		CFGNode* whileIn;
		CFGNode* whileOut;
	};
	struct IfChildren {
		CFGNode* ifThen;
		CFGNode* ifElse;
		CFGNode* ifLater;
	};
	union Children {
		CFGNode* oneChild;
		WhileChildren whileChildren;
		IfChildren ifChildren;
	};

	PROC proc;
	PROG_LINE first, last;
	NodeType type;
	CFGNode* whileAncestor;
	const IntervalList *nextList, *prevList;
	unordered_set<VAR> modifySet, useSet;
	vector<CFGNode*> parents;
	Children children;

	CFGNode(PROC proc);
	CFGNode(CFGNode* parent);
};


//class CFGNode
//{
//public:
//	typedef enum NodeType { 
//		StandardNode,
//		WhileNode,
//		IfNode,
//		DummyNode
//	};
//

//	CFGNode(NodeType, PROG_LINE start, PROG_LINE end, PROC p);
//
//	//link node1->node2 in the cfg
//	void setLink(CFGNode* node);
//
//	void setEndProgramLine(PROG_LINE p);
//
//	void setStartNode();
//
//	bool isStartNode();
//
//	void setEndNode();
//
//	bool isEndNode();
//
//	bool isDummy();
//
//	NodeType getType();
//
//	void addNextNode(CFGNode* node);
//
//	void addPreviousNode(CFGNode* node);
//
//	vector<CFGNode*> getPreviousNodes();
//
//	vector<CFGNode*> getNextNodes();
//
//	PROC getProcedure();
//
//	vector<PROG_LINE> getProgramLines();
//
//	PROG_LINE getStartLine();
//
//	PROG_LINE getEndLine();
//
//	bool isProgLineBelongto(PROG_LINE);
//
//private:
//	PROG_LINE starting, ending;
//
//	vector<CFGNode*> nextNodes, prevNodes;
//
//	PROC belongingTo;
//
//	NodeType type;
//
//	bool isStart, isEnd;
//};

