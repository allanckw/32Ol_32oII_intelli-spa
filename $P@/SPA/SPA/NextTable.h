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

	unordered_map<PROG_LINE, vector<Next*>> nextStarMap;
	unordered_map<PROG_LINE, vector<STMT>> nextStarByMap;
	unordered_map<PROG_LINE, vector<STMT>> previousStarMap;

	unordered_map<PROG_LINE, vector<Next*>> nextBipMap;
	unordered_map<PROG_LINE, vector<STMT>> nextBipByMap;
	unordered_map<PROG_LINE, vector<STMT>> previousBipMap;

	unordered_map<PROG_LINE, vector<Next*>> nextBipStarMap;
	unordered_map<PROG_LINE, vector<STMT>> nextBipStarByMap;
	unordered_map<PROG_LINE, vector<STMT>> previousBipStarMap;


	static bool isDuplicate(vector<Next*> v, Next* n);

public:
	NextTable();
	bool isNext (PROG_LINE, PROG_LINE);
	vector<PROG_LINE> getNext(PROG_LINE);
	vector<PROG_LINE> getPrevious(PROG_LINE);

	void insertNextStar (PROG_LINE, PROG_LINE, bool);
	bool isNextStar (PROG_LINE, PROG_LINE);
	vector<PROG_LINE>  getNextStar(PROG_LINE);
	vector<PROG_LINE>  getPreviousStar(PROG_LINE);

	void insertNextBip (PROG_LINE, PROG_LINE, bool);
	bool isNextBip(PROG_LINE, PROG_LINE);
	vector<PROG_LINE> getNextBip(PROG_LINE);
	vector<PROG_LINE> getPreviousBip(PROG_LINE);

	void insertNextBipStar (PROG_LINE, PROG_LINE, bool);
	bool isNextBipStar(PROG_LINE, PROG_LINE);
	vector<PROG_LINE> getNextBipStar(PROG_LINE);
	vector<PROG_LINE> getPreviousBipStar(PROG_LINE);

	void tearDownCache();
};