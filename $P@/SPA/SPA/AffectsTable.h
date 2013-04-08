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
		{
			this->a1 = a1;
			this->a2 = a2;
			this->affected = affected;
		}

		bool isAffected()
		{
			return this->affected;
		}
		
		STMT getA1()
		{
			return this->a1;
		}

		STMT getA2()
		{
			return this->a2;
		}
	};


	unordered_map<STMT, vector<Affects*>> affectsMap; //Store the results of affects(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsByMap;	  //Store the results of affects(a1,_)
	unordered_map<STMT, vector<STMT>> affectsFromMap; //Store the results of affects(_,a2)
	
	unordered_map<STMT, vector<Affects*>> affectsStarMap; //Store the results of affects*(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsStarByMap;	  //Store the results of affects*(a1,_)
	unordered_map<STMT, vector<STMT>> affectsStarFromMap; //Store the results of affects*(_,a2)

	unordered_map<STMT, vector<Affects*>> affectsBipMap;  ////Store the results of affectsBip(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsBipByMap;	  //Store the results of affectsBip(a1,_)
	unordered_map<STMT, vector<STMT>> affectsBipFromMap; //Store the results of affectsBip(_,a2)

	unordered_map<STMT, vector<Affects*>> affectsBipStarMap;  //Store the results of affectsBip*(a1,a2)
	unordered_map<STMT, vector<STMT>> affectsBipStarByMap;	  //Store the results of affectsBip*(a1,_)
	unordered_map<STMT, vector<STMT>> affectsBipStarFromMap; //Store the results of affectsBip*(_,a2)

	static bool isDuplicate(vector<Affects*>& v, Affects* n);
public:
	AffectsTable();
	
	void insertAffects (STMT, STMT, bool);
	bool isAffects (STMT, STMT);
	vector<STMT> getAffectsBy(STMT);
	vector<STMT> getAffectsFrom(STMT);
	
	void insertAffectsStar (STMT, STMT, bool);
	bool isAffectsStar (STMT, STMT);
	vector<STMT>  getAffectsStarBy(STMT);
	vector<STMT>  getAffectsStarFrom(STMT);

	void insertAffectsBip(STMT, STMT, bool);
	bool isAffectsBip (STMT, STMT);
	vector<STMT>  getAffectsBipBy(STMT);
	vector<STMT>  getAffectsBipFrom(STMT);

	void insertAffectsBipStar(STMT, STMT, bool);
	bool isAffectsBipStar (STMT, STMT);
	vector<STMT>  getAffectsBipStarBy(STMT);
	vector<STMT>  getAffectsBipStarFrom(STMT);

	void tearDownCache();
};