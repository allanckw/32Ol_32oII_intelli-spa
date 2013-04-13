#pragma once
#include "StdAfx.h"

class AffectsTable
{
private:
    class Affects 
	{
	private:
		STMT a1, a2;
		bool affected;

	public:
		Affects(STMT a1, STMT a2, bool affected)
			: a1(a1), a2(a2), affected(affected) {}

		bool isAffected() const
		{
			return this->affected;
		}
		
		STMT getA1() const
		{
			return this->a1;
		}

		STMT getA2() const
		{
			return this->a2;
		}
	};

	Concurrency::reader_writer_lock affectsLock;
	Concurrency::reader_writer_lock affectsByLock;
	Concurrency::reader_writer_lock affectsFromLock;
	unordered_map<STMT, vector<Affects>> affectsMap; //Store the results of affects(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsByMap;	  //Store the results of affects(a1,_)
	unordered_map<STMT, vector<STMT>> affectsFromMap; //Store the results of affects(_,a2)
	
	Concurrency::reader_writer_lock affectsStarLock;
	Concurrency::reader_writer_lock affectsStarByLock;
	Concurrency::reader_writer_lock affectsStarFromLock;
	unordered_map<STMT, vector<Affects>> affectsStarMap; //Store the results of affects*(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsStarByMap;	  //Store the results of affects*(a1,_)
	unordered_map<STMT, vector<STMT>> affectsStarFromMap; //Store the results of affects*(_,a2)

	Concurrency::reader_writer_lock affectsBipLock;
	Concurrency::reader_writer_lock affectsBipByLock;
	Concurrency::reader_writer_lock affectsBipFromLock;
	unordered_map<STMT, vector<Affects>> affectsBipMap;  ////Store the results of affectsBip(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsBipByMap;	  //Store the results of affectsBip(a1,_)
	unordered_map<STMT, vector<STMT>> affectsBipFromMap; //Store the results of affectsBip(_,a2)

	Concurrency::reader_writer_lock affectsBipStarLock;
	Concurrency::reader_writer_lock affectsBipStarByLock;
	Concurrency::reader_writer_lock affectsBipStarFromLock;
	unordered_map<STMT, vector<Affects>> affectsBipStarMap;  //Store the results of affectsBip*(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsBipStarByMap;	  //Store the results of affectsBip*(a1,_)
	unordered_map<STMT, vector<STMT>> affectsBipStarFromMap; //Store the results of affectsBip*(_,a2)

	void insertAffects (STMT, STMT, bool);
	void insertAffectsStar (STMT, STMT, bool);
	void insertAffectsBip(STMT, STMT, bool);
	void insertAffectsBipStar(STMT, STMT, bool);
	static bool isDuplicate(const vector<Affects>& v, const Affects& n);
public:
	AffectsTable();
	
	bool isAffects (STMT, STMT);
	vector<STMT> getAffectsBy(STMT);
	vector<STMT> getAffectsFrom(STMT);
	
	bool isAffectsStar (STMT, STMT);
	vector<STMT>  getAffectsStarBy(STMT);
	vector<STMT>  getAffectsStarFrom(STMT);

	bool isAffectsBip (STMT, STMT);
	vector<STMT>  getAffectsBipBy(STMT);
	vector<STMT>  getAffectsBipFrom(STMT);

	bool isAffectsBipStar (STMT, STMT);
	vector<STMT>  getAffectsBipStarBy(STMT);
	vector<STMT>  getAffectsBipStarFrom(STMT);

	void tearDownCache();
};