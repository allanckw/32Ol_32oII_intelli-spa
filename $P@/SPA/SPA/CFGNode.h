#pragma once
#include "StdAfx.h"
#include "SPAException.h"

class CFGNode
{
private:
	bool isStart, isEnd;

	PROG_LINE starting, ending;

	CFGNode *nextNode, *prevNode;

	PROC belongingTo;

public:
	CFGNode(PROG_LINE start, PROG_LINE end, PROC p);

	//link node1->node2 in the cfg
	void setLink(CFGNode* node1, CFGNode* node2);

	void setAsStartNode();
	
	void setAsEndNode();

	bool isStartNode();

	bool isEndNode();

	void setNextNode(CFGNode* node);

	void setPreviousNode(CFGNode* node);

	CFGNode* getPreviousNode();

	vector<CFGNode*> getAllPreviousNodes();

	CFGNode* getNextNode();

	vector<CFGNode*> getAllNextNodes();

	PROC getProcedure();

	vector<PROG_LINE> getProgramLines();
	
	~CFGNode(void);
};

