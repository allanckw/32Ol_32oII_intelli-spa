#include "AffectsTable.h"
#include "PQLAffectsProcessor.h"

AffectsTable::AffectsTable()
{
}

//planned for itr2
void AffectsTable::insertAffects (STMT a1, STMT a2, bool isAffected)
{
	Affects* n = new Affects(a1, a2, isAffected);
	
	auto itr = this->affectsMap.find(a1);
	
	if (itr == this->affectsMap.end()) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (a1, affectsLst);
		this->affectsMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(this->affectsMap.at(a1), n)){
			this->affectsMap.at(a1).push_back(n);
		}
	}
}


//QE should use PKB Abstractions
//store as false if cannot be found
bool AffectsTable::isAffects (STMT a1, STMT a2)
{
	if (a1 <= 0 || a2 <= 0)
		return false;

	auto itr = this->affectsMap.find(a1);
	if (itr == this->affectsMap.end()) {
		vector<Affects*> nxtLst = itr->second;

		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getA2() == a2 )
				return nxtLst[i]->isAffected();
		}
	}

	//if not found, compute affects using PQLAffectsProcessor
	//Fill in the blanks accordingly

	return false;
}

//For Affects(a1, _)
vector<STMT> AffectsTable::getAffectsBy(STMT a1)
{
	vector<STMT> affects;

	return affects;
}

//For Affects (_, a2)
vector<STMT> AffectsTable::getAffectsFrom(STMT a2)
{
	vector<STMT> affects;

	return affects;
}

//planned for itr2 + 3
void AffectsTable::insertAffectsStar (STMT a1, STMT a2, bool isAffected)
{
	Affects* n = new Affects(a1, a2, isAffected);
	
	auto itr = this->affectsStarMap.find(a1);
	
	if (itr == this->affectsStarMap.end()) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (a1, affectsLst);
		this->affectsStarMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(this->affectsStarMap.at(a1), n)){
			this->affectsStarMap.at(a1).push_back(n);
		}
	}
}

bool AffectsTable::isAffectsStar (STMT a1, STMT a2)
{
	return false;
}

vector<STMT>  AffectsTable::getAffectsByStar(STMT a1)
{
	vector<STMT> affects;

	return affects;
}

vector<STMT>  AffectsTable::getAffectsFromStar(STMT a2)
{
	vector<STMT> affects;

	return affects;
}

//planned for itr3 + 4
void AffectsTable::insertAffectsBip(STMT a1, STMT a2, bool isAffected)
{
	Affects* n = new Affects(a1, a2, isAffected);
	
	auto itr = this->affectsBipMap.find(a1);
	
	if (itr == this->affectsBipMap.end()) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (a1, affectsLst);
		this->affectsBipMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(this->affectsBipMap.at(a1), n)){
			this->affectsBipMap.at(a1).push_back(n);
		}
	}
}

bool isAffectsBip (STMT a1, STMT a2)
{
	return false;
}

vector<STMT>  AffectsTable::getAffectsByBip(STMT a1)
{
	vector<STMT> affects;

	return affects;
}

vector<STMT>  AffectsTable::getAffectsFromBip(STMT a2)
{
	vector<STMT> affects;

	return affects;
}

bool AffectsTable::isAffectsEmpty()
{
	if (this->affectsMap.size() > 0)
		return true;
	else
	{
		//traverse until find smth and return
	}
	return false; //totally cannot find
}

//for Affects*(_, _) only
bool AffectsTable::isAffectsStarEmpty()
{
	if (this->affectsMap.size() > 0)
		return true;
	else if (this->affectsStarMap.size() > 0)
		return true;
	else
	{
		//traverse until find smth and return
	}
	return false; //totally cannot find
}

bool AffectsTable::isAffectsBipEmpty()
{
	if (this->affectsMap.size() > 0)
		return true;
	else if (this->affectsStarMap.size() > 0)
		return true;
	else if (this->affectsBipMap.size() > 0)
		return true;
	else
	{
		//traverse until find smth and return
	}
	return false; //totally cannot find
}

bool AffectsTable::isDuplicate(vector<Affects*> v, Affects* a)
{
	for (unsigned int i = 0; i < v.size(); i++) {
		if (v[i]->getA1() == a->getA1() && v[i]->getA2() == a->getA2())
			return true;
	}

	return false;
}