#pragma once
#include "StdAfx.h"
#include "IntervalList.h"

class MyCFG
{
public:
	enum Type { If, While, /*WhilePrev, */Normal, /*End, */Dummy };
	struct WhileChildren {
		MyCFG* whileIn;
		MyCFG* whileOut;
	};
	struct IfChildren {
		MyCFG* ifThen;
		MyCFG* ifElse;
		MyCFG* ifLater;
	};
	union Children {
		MyCFG* oneChild;
		WhileChildren whileChildren;
		IfChildren ifChildren;
	};

	PROC proc;
	PROG_LINE first, last;
	Type type;
	MyCFG* whileAncestor;
	const IntervalList *nextList, *prevList;
	unordered_set<VAR> modifySet, useSet;
	vector<MyCFG*> parents;
	Children children;

	MyCFG(PROC proc);
	MyCFG(MyCFG* parent);
};