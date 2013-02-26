#pragma once
#include "MyCFG.h"

MyCFG::MyCFG(PROC proc) : proc(proc), first(0), last(0), type(Dummy), whileAncestor(NULL),
	nextList(), prevList(), modifySet(), useSet(), parents(), children()
{
}

MyCFG::MyCFG(MyCFG* parent) : proc(parent->proc), first(0), last(0), type(Dummy), 
	whileAncestor(parent->whileAncestor), nextList(), modifySet(), useSet(), children(),
	parents(1, parent)
{
	if (parent->prevList == NULL)
		prevList = new IntervalList(parent->first, parent->last);
	else
		prevList = parent->prevList->Union(parent->first, parent->last);
}