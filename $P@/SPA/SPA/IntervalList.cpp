#pragma once
#include "IntervalList.h"

/**
* Constructor for a standalone IntervalList
* @param first the first program line
* @param last the last program line
*/
IntervalList::IntervalList(const int first, const int last)
	: first(first), last(last), prev(0), next(0)
{
}

/**
* Constructor for an whose prev and next are already defined.
* @param first the first program line
* @param last the last program line
* @param prev the IntervalList preceding this one
* @param next the IntervalList after this one
*/
IntervalList::IntervalList(const int first, const int last, IntervalList* prev, IntervalList* next)
	: first(first), last(last), prev(prev), next(next)
{
	if (prev != NULL && prev->next == NULL)
		prev->next = this;
	if (next != NULL && next->prev == NULL)
		next->prev = this;
}

/**
* Takes an IntervalList and unions it with a new set of program lines.
* Combines consecutive program lines together.
* @param first the first program line
* @param last the last program line
* @return A pointer to the new IntervalList
*/
const IntervalList* IntervalList::Union(const int first, const int last) const {
	const IntervalList* node = this;
	while (true) {
		if (last + 1 < node->first) {
			if (node->prev == NULL) {
				return new IntervalList(node->first,
					node->last, new IntervalList(first, last), node->next);
			} else
				node = node->prev;
		} else if (first < node->first) {
			if (last <= node->last) {
				const int actualLast = node->last;
				IntervalList* const actualNext = node->next;
				while (true) {
					if (node->prev == NULL || first > node->prev->last + 1) {
						return new IntervalList(first, actualLast, node->prev, actualNext);
					} else if (first >= node->prev->first) {
						return new IntervalList(node->prev->first,
							actualLast, node->prev->prev, actualNext);
					} else {
						node = node->prev;
					}
				}
			} else { //last > node->last
				int actualFirst;
				IntervalList *actualPrev;
				while (true) {
					if (node->prev == NULL || first > node->prev->last + 1) {
						actualFirst = first;
						break;
					} else if (first >= node->prev->first) {
						actualFirst = node->prev->first;
						actualPrev = node->prev->prev;
						break;
					} else {
						node = node->prev;
					}
				}
				while (true) {
					if (node->next == NULL || last < node->next->first - 1) {
						return new IntervalList(actualFirst, last, actualPrev, node->next);
					} else if (last <= node->next->last) {
						return new IntervalList(actualFirst,
							node->next->last, actualPrev, node->next->next);
					} else {
						node = node->next;
					}
				}
			}
		} else if (first > node->last + 1) {
			if (node->next == NULL) {
				return new IntervalList(node->first,
					node->last, node->prev, new IntervalList(first, last));
			} else
				node = node->next;
		} else if (last > node->last) { //and (node->first <= first <= node->last + 1)
			const int actualFirst = node->first;
			IntervalList* const actualPrev = node->prev;
			while (true) {
				if (node->next == NULL || last < node->next->first - 1) {
					return new IntervalList(actualFirst, last, actualPrev, node->next);
				} else if (last <= node->next->last) {
					return new IntervalList(actualFirst,
						node->next->last, actualPrev, node->next->next);
				} else {
					node = node->next;
				}
			}
		} else
			return this;
	}
}