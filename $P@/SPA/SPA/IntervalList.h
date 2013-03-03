#pragma once
#include "StdAfx.h"

class IntervalList
{
public:
	const int first, last;
	IntervalList *prev, *next;

	IntervalList(const int first, const int last);
	IntervalList(const int first, const int last, IntervalList* prev, IntervalList* next);

	const IntervalList* Union(const int first, const int last) const;
};