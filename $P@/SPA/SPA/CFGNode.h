#pragma once
#include "StdAfx.h"
#include "SPAException.h"

class CFGNode
{
public:
	typedef enum NodeType { 
		StandardNode,
		WhileNode,
		IfNode,
		DummyNode
	};

	CFGNode(NodeType, PROG_LINE start, PROG_LINE end, PROC p);

	//link node1->node2 in the cfg
	void setLink(CFGNode* node);

	void setEndProgramLine(PROG_LINE p);

	void setStartNode();

	bool isStartNode();

	void setEndNode();

	bool isEndNode();

	bool isDummy();

	NodeType getType();

	void addNextNode(CFGNode* node);

	void addPreviousNode(CFGNode* node);

	vector<CFGNode*> getPreviousNodes();

	//vector<CFGNode*> getAllPreviousNodes();

	vector<CFGNode*> getNextNodes();

	//vector<CFGNode*> getAllNextNodes();

	PROC getProcedure();

	vector<PROG_LINE> getProgramLines();

	bool isNext(PROG_LINE p1, PROG_LINE p2);

private:
	PROG_LINE starting, ending;

	vector<CFGNode*> nextNodes, prevNodes;

	PROC belongingTo;

	NodeType type;

	bool isStart, isEnd;

	bool isNext(PROG_LINE p1, PROG_LINE p2, CFGNode* nextNode);
};

