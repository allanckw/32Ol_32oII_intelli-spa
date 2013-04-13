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
			: p1(p1), p2(p2), next(next) {}

		bool isNext() const
		{
			return this->next;
		}
		
		PROG_LINE getP1() const
		{
			return this->p1;
		}

		PROG_LINE getP2() const
		{
			return this->p2;
		}
	};

	Concurrency::reader_writer_lock nextStarLock;
	Concurrency::reader_writer_lock nextStarByLock;
	Concurrency::reader_writer_lock previousStarLock;
	unordered_map<PROG_LINE, vector<Next>> nextStarMap;
	unordered_map<PROG_LINE, vector<STMT>> nextStarByMap;
	unordered_map<PROG_LINE, vector<STMT>> previousStarMap;

	Concurrency::reader_writer_lock nextBipLock;
	Concurrency::reader_writer_lock nextBipByLock;
	Concurrency::reader_writer_lock previousBipLock;
	unordered_map<PROG_LINE, vector<Next>> nextBipMap;
	unordered_map<PROG_LINE, vector<STMT>> nextBipByMap;
	unordered_map<PROG_LINE, vector<STMT>> previousBipMap;

	Concurrency::reader_writer_lock nextBipStarLock;
	Concurrency::reader_writer_lock nextBipStarByLock;
	Concurrency::reader_writer_lock previousBipStarLock;
	unordered_map<PROG_LINE, vector<Next>> nextBipStarMap;
	unordered_map<PROG_LINE, vector<STMT>> nextBipStarByMap;
	unordered_map<PROG_LINE, vector<STMT>> previousBipStarMap;

	
	void insertNextStar (PROG_LINE, PROG_LINE, bool);
	void insertNextBip (PROG_LINE, PROG_LINE, bool);
	void insertNextBipStar (PROG_LINE, PROG_LINE, bool);
	static bool isDuplicate(const vector<Next>& v, const Next& n);

public:
	NextTable();
	bool isNext (PROG_LINE, PROG_LINE);
	vector<PROG_LINE> getNext(PROG_LINE);
	vector<PROG_LINE> getPrevious(PROG_LINE);

	bool isNextStar (PROG_LINE, PROG_LINE);
	vector<PROG_LINE>  getNextStar(PROG_LINE);
	vector<PROG_LINE>  getPreviousStar(PROG_LINE);

	bool isNextBip(PROG_LINE, PROG_LINE);
	vector<PROG_LINE> getNextBip(PROG_LINE);
	vector<PROG_LINE> getPreviousBip(PROG_LINE);

	bool isNextBipStar(PROG_LINE, PROG_LINE);
	vector<PROG_LINE> getNextBipStar(PROG_LINE);
	vector<PROG_LINE> getPreviousBipStar(PROG_LINE);

	void tearDownCache();
};