#include "AffectsTable.h"
#include "PQLAffectsProcessor.h"

AffectsTable::AffectsTable()
{
}

//planned for itr2
void AffectsTable::insertAffects (STMT s1, STMT s2, bool isAffected)
{
	Affects* n = new Affects(s1, s2, isAffected);
	
	auto itr = this->affectsMap.find(s1);
	
	if (itr == this->affectsMap.end()) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (s1, affectsLst);
		this->affectsMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(this->affectsMap.at(s1), n)){
			this->affectsMap.at(s1).push_back(n);
		}
	}
}


bool AffectsTable::isAffects (STMT s1, STMT s2)
{
	if (s1 <= 0 || s2 <= 0)
		return false;

	auto itr = this->affectsMap.find(s1);

	if (itr != this->affectsMap.end()){

		vector<Affects*> nxtLst =  itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getA2() == s2 )
				return nxtLst[i]->isAffected();
		}
	}

	if (PQLAffectsProcessor::isAffects(s1,s2)){
		return true;
	} else {
		PKB::affects.insertAffects(s1, s2, false);
		return false;
	}
	return false;
}


//planned for itr2 + 3
void AffectsTable::insertAffectsStar (STMT s1, STMT s2, bool isAffected)
{
	Affects* n = new Affects(s1, s2, isAffected);
	
	auto itr = this->affectsStarMap.find(s1);
	
	if (itr == this->affectsStarMap.end()) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (s1, affectsLst);
		this->affectsStarMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(this->affectsStarMap.at(s1), n)){
			this->affectsStarMap.at(s1).push_back(n);
		}
	}
}

bool AffectsTable::isAffectsStar (STMT s1, STMT s2)
{
	if (s1 <= 0 || s2 <= 0)
		return false;

	if (PKB::affects.isAffects(s1, s2))
		return true;

	auto itr = this->affectsStarMap.find(s1);

	if (itr != this->affectsStarMap.end()){

		vector<Affects*> nxtLst =  itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getA2() == s2 )
				return nxtLst[i]->isAffected();
		}
	}

	if (PQLAffectsProcessor::isAffectsStar(s1,s2)){
		return true;
	} else {
		PKB::affects.insertAffectsStar(s1, s2, false);
		return false;
	}

	return false;
}

//planned for itr3 + 4
void AffectsTable::insertAffectsBip(STMT s1, STMT s2, bool isAffected)
{
	Affects* n = new Affects(s1, s2, isAffected);
	
	auto itr = this->affectsBipMap.find(s1);
	
	if (itr == this->affectsBipMap.end()) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (s1, affectsLst);
		this->affectsBipMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(this->affectsBipMap.at(s1), n)){
			this->affectsBipMap.at(s1).push_back(n);
		}
	}
}

bool AffectsTable::isAffectsBip (STMT s1, STMT s2)
{
	if (s1 <= 0 || s2 <= 0)
		return false;

	auto itr = this->affectsBipMap.find(s1);

	if (itr != this->affectsBipMap.end()){

		vector<Affects*> nxtLst =  itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getA2() == s2 )
				return nxtLst[i]->isAffected();
		}
	}
	//TODO: Implement
	//if (PQLAffectsProcessor::isAffectsBip(s1,s2)){
	//	return true;
	//} else {
	//	PKB::affects.insertAffectsBip(s1, s2, false);
	//	return false;
	//}
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