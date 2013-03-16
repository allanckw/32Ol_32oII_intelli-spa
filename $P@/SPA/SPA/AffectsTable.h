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

	//planned for itr2
	unordered_map<STMT, vector<Affects*>> affectsMap;
	unordered_map<STMT, vector<Affects*>> affectsStarMap;

	unordered_map<STMT, vector<Affects*>> affectsBipMap;
	unordered_map<STMT, vector<Affects*>> affectsBipStarMap;

	static bool isDuplicate(vector<Affects*> v, Affects* n);
public:
	AffectsTable();
	
	//planned for itr2
	void insertAffects (STMT, STMT, bool);
	bool isAffects (STMT, STMT);
	vector<STMT> getAffectsBy(STMT);
	vector<STMT> getAffectsFrom(STMT);

	//planned for itr2 + 3
	void insertAffectsStar (STMT, STMT, bool);
	bool isAffectsStar (STMT, STMT);
	vector<STMT>  getAffectsByStar(STMT);
	vector<STMT>  getAffectsFromStar(STMT);

	//planned for itr3 + 4
	void insertAffectsBip(STMT, STMT, bool);
	bool isAffectsBip (STMT, STMT);
	vector<STMT>  getAffectsByBip(STMT);
	vector<STMT>  getAffectsFromBip(STMT);

	//planned for itr3 + 4
	void insertAffectsBipStar(STMT, STMT, bool);
	bool isAffectsBipStar (STMT, STMT);
	vector<STMT>  getAffectsByBipStar(STMT);
	vector<STMT>  getAffectsFromBipStar(STMT);

	void tearDownCache();
};