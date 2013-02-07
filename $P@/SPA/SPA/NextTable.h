#pragma once
#include "StdAfx.h"

class NextTable
{
private:
    class Next
	{
	private:
		PROG_LINE p1, p2;
		bool next;

	public:
		Next(PROG_LINE p1, PROG_LINE p2, bool next)
		{
			this->p1 = p1;
			this->p2 = p2;
			this->next = next;
		}

		bool isNext()
		{
			return this->next;
		}
		
		PROG_LINE getP1()
		{
			return this->p1;
		}

		PROG_LINE getP2()
		{
			return this->p2;
		}
	};

	//planned for itr2
	unordered_map<PROG_LINE, vector<Next*>> nextMap;
	unordered_map<PROG_LINE, vector<Next*>> nextStarMap;

public:
	Next*Table();
	

	void insertNext (PROG_LINE, PROG_LINE, bool);
	bool isNext (PROG_LINE, PROG_LINE);
	vector<PROG_LINE> getNextBy(PROG_LINE);
	vector<PROG_LINE> getNextFrom(PROG_LINE);

	void insertNextStar (PROG_LINE, PROG_LINE, bool);
	bool isNextStar (PROG_LINE, PROG_LINE);
	vector<PROG_LINE>  getNextStar(PROG_LINE);
	vector<PROG_LINE>  getPreviousStar(PROG_LINE);

};