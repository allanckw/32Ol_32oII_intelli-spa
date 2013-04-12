#include "AffectsTable.h"
#include "PQLAffectsProcessor.h"

/**
* This method will be used as a constructor to create AffectsTable
*/
AffectsTable::AffectsTable()
{
}

//planned for itr2
/**
* This method will be used to insert Affects(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @param isAffected	whether a1 affect a2
*/
void AffectsTable::insertAffects (STMT a1, STMT a2, bool isAffected)
{
	Affects* n = new Affects(a1, a2, isAffected);

	if (this->affectsMap.count(a1) > 0) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (a1, affectsLst);
		this->affectsMap.insert(newItem);

	} else {
		if (!AffectsTable::isDuplicate(affectsMap[a1], n)){
			affectsMap[a1].push_back(n);
		}
	}
}

/**
* This method will be used to insert Affects(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether a1 affect a2
*/
bool AffectsTable::isAffects (STMT a1, STMT a2)
{
	if (a1 <= 0 || a2 <= 0)
		return false;

	if (this->affectsMap.count(a1) > 0){

		vector<Affects*>& affectsLst =  affectsMap[a1];

		for (size_t i = 0; i < affectsLst.size(); i++) {
			if (affectsLst[i]->getA2() == a2 )
				return affectsLst[i]->isAffected();
		}
	}

	if (PQLAffectsProcessor::isAffects(a1,a2)){
		insertAffects(a1, a2, true);
		return true;
	} else {
		insertAffects(a1, a2, false);
		return false;
	}
	return false;
}

/**
* This method will be used to get a list of a2 that is affects(a1,_)
* @param a1	The statement that is going to affect a2
* @return a list of statement that is affected by a1
*/
vector<STMT> AffectsTable::getAffectsBy(STMT a1)
{
	vector<STMT> result;
	////auto itr = this->affectsByMap.find(a1);

	if (this->affectsByMap.count(a1) > 0){
		return  affectsByMap[a1];
	} else {
		result =  PQLAffectsProcessor::getAffectsBy(a1);
		pair<STMT, vector<STMT>> newItem (a1, result);
		this->affectsByMap.insert(newItem);

		for (size_t i = 0; i < result.size(); i++)	
			insertAffects(a1, result.at(i), true);
	}

	return result;
}

/**
* This method will be used to get a list of a2 that is affects(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affect a2
*/
vector<STMT> AffectsTable::getAffectsFrom(STMT a2)
{
	vector<STMT> result;
	////auto itr = this->affectsFromMap.find(a2);

	if (this->affectsFromMap.count(a2) > 0){
		return affectsFromMap[a2];
	} else {
		result =  PQLAffectsProcessor::getAffectsFrom(a2);
		pair<STMT, vector<STMT>> newItem (a2, result);
		this->affectsFromMap.insert(newItem);
		for (size_t i = 0; i < result.size(); i++)	
			insertAffects(result.at(i), a2, true);
	}

	return result;
}

/**
* This method will be used to insert AffectsStar(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @param isAffected	whether a1 affectStar a2
*/
void AffectsTable::insertAffectsStar (STMT a1, STMT a2, bool isAffected)
{
	Affects* n = new Affects(a1, a2, isAffected);

	//auto itr = this->affectsStarMap.find(a1);

	if (this->affectsStarMap.count(a1) > 0) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (a1, affectsLst);
		this->affectsStarMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(affectsStarMap[a1], n)){
			affectsStarMap[a1].push_back(n);
		}
	}
}

/**
* This method will be used to check AffectsStar(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether a1 affectStar a2
*/
bool AffectsTable::isAffectsStar (STMT a1, STMT a2)
{
	if (a1 <= 0 || a2 <= 0)
		return false;

	if (PKB::affects.isAffects(a1, a2))
		return true;

	//auto itr = this->affectsStarMap.find(a1);

	if (this->affectsStarMap.count(a1) > 0){

		vector<Affects*>& affectsLst =  affectsStarMap[a1];

		for (size_t i = 0; i < affectsLst.size(); i++) {
			if (affectsLst[i]->getA2() == a2 )
				return affectsLst[i]->isAffected();
		}
	}

	if (PQLAffectsProcessor::isAffectsStar(a1,a2)){
		PKB::affects.insertAffectsStar(a1, a2, true);
		return true;
	} else {
		PKB::affects.insertAffectsStar(a1, a2, false);
		return false;
	}

	return false;
}

/**
* This method will be used to get a list of a2 that is affects*(a1,_)
* @param a1	The statement that is going to affect a2
* @return a list of statement that is affected by a1
*/
vector<STMT> AffectsTable::getAffectsStarBy(STMT a1)
{
	vector<STMT> result;
	//auto itr = this->affectsStarByMap.find(a1);

	if (this->affectsStarByMap.count(a1) > 0){
		return affectsStarByMap[a1];
	} else {
		result =  PQLAffectsProcessor::getAffectsStarBy(a1);
		pair<STMT, vector<STMT>> newItem (a1, result);
		this->affectsStarByMap.insert(newItem);

		for (size_t i = 0; i < result.size(); i++)	
			insertAffectsStar(a1, result.at(i), true);
	}

	return result;
}

/**
* This method will be used to get a list of a2 that is affects*(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affect a2
*/
vector<STMT> AffectsTable::getAffectsStarFrom(STMT a2)
{
	vector<STMT> result;
	//auto itr = this->affectsStarFromMap.find(a2);

	if (this->affectsStarFromMap.count(a2) > 0){
		return affectsStarFromMap[a2];
	} else {
		result =  PQLAffectsProcessor::getAffectsStarFrom(a2);
		pair<STMT, vector<STMT>> newItem (a2, result);
		this->affectsStarFromMap.insert(newItem);
		for (size_t i = 0; i < result.size(); i++)	
			PKB::affects.insertAffectsStar(result.at(i), a2, true);
	}

	return result;
}

/**
* This method will be used to insert AffectsBip(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether a1 affect a2
*/
void AffectsTable::insertAffectsBip(STMT a1, STMT a2, bool isAffected)
{
	Affects* n = new Affects(a1, a2, isAffected);

	//auto itr = this->affectsBipMap.find(a1);

	if (this->affectsBipMap.count(a1) > 0) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (a1, affectsLst);
		this->affectsBipMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(affectsBipMap[a1], n)){
			affectsBipMap[a1].push_back(n);
		}
	}
}

/**
* This method will be used to insert AffectsBip(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether a1 affect a2
*/
bool AffectsTable::isAffectsBip (STMT a1, STMT a2)
{
	if (a1 <= 0 || a2 <= 0)
		return false;

	//auto itr = this->affectsBipMap.find(a1);

	if (this->affectsBipMap.count(a1) > 0){

		vector<Affects*>& affectsLst =  affectsBipMap[a1];

		for (size_t i = 0; i < affectsLst.size(); i++) {
			if (affectsLst[i]->getA2() == a2 )
				return affectsLst[i]->isAffected();
		}
	}

	if (PQLAffectsProcessor::isAffectsBip(a1,a2)){
		PKB::affects.insertAffectsBip(a1, a2, true);
		return true;
	} else {
		PKB::affects.insertAffectsBip(a1, a2, false);
		return false;
	}
}

/**
* This method will be used to get a list of a2 that is affectsBip(a1,_)
* @param a1	The statement that is going to affect a2
* @return a list of statement that is affected by a1
*/
vector<STMT> AffectsTable::getAffectsBipBy(STMT a1)
{
	vector<STMT> result;
	//auto itr = this->affectsBipByMap.find(a1);

	if (this->affectsBipByMap.count(a1) > 0){
		return affectsBipByMap[a1];

	} else {
		result =  PQLAffectsProcessor::getAffectsBipBy(a1);
		pair<STMT, vector<STMT>> newItem (a1, result);
		this->affectsBipByMap.insert(newItem);

		for (size_t i = 0; i < result.size(); i++)	
			PKB::affects.insertAffectsBip(a1, result.at(i), true);
	}

	return result;
}

/**
* This method will be used to get a list of a2 that is affectsBip(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affect a2
*/
vector<STMT> AffectsTable::getAffectsBipFrom(STMT a2)
{
	vector<STMT> result;
	//auto itr = this->affectsBipFromMap.find(a2);

	if (this->affectsBipFromMap.count(a2) > 0){
		return affectsBipFromMap[a2];
	} else {
		result =  PQLAffectsProcessor::getAffectsBipFrom(a2);
		pair<STMT, vector<STMT>> newItem (a2, result);
		this->affectsBipFromMap.insert(newItem);
		for (size_t i = 0; i < result.size(); i++)	
			PKB::affects.insertAffectsBip(result.at(i), a2, true);
	}

	return result;
}

/**
* This method will be used to insert AffectsBipStar(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether a1 affect a2
*/
void AffectsTable::insertAffectsBipStar(STMT a1, STMT a2, bool isAffected)
{
	Affects* n = new Affects(a1, a2, isAffected);

	//auto itr = this->affectsBipStarMap.find(a1);

	if (this->affectsBipStarMap.count(a1) > 0) {
		vector<Affects*> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects*>> newItem (a1, affectsLst);
		this->affectsBipStarMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(affectsBipStarMap[a1], n)){
			affectsBipStarMap[a1].push_back(n);
		}
	}
}

/**
* This method will be used to insert AffectsBipStar(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether a1 affect a2
*/
bool AffectsTable::isAffectsBipStar (STMT a1, STMT a2)
{
	if (a1 <= 0 || a2 <= 0)
		return false;

	//auto itr = this->affectsBipStarMap.find(a1);

	if (this->affectsBipStarMap.count(a1) > 0){

		vector<Affects*>& affectsLst = affectsBipStarMap[a1];

		for (size_t i = 0; i < affectsLst.size(); i++) {
			if (affectsLst[i]->getA2() == a2 )
				return affectsLst[i]->isAffected();
		}
	}

	if (PQLAffectsProcessor::isAffectsBipStar(a1,a2)){
		PKB::affects.insertAffectsBipStar(a1, a2, true);
		return true;
	} else {
		PKB::affects.insertAffectsBipStar(a1, a2, false);
		return false;
	}
}

/**
* This method will be used to get a list of a2 that is affectsBipStar(a1,_)
* @param a1	The statement that is going to affect a2
* @return a list of statement that is affected by a1
*/
vector<STMT> AffectsTable::getAffectsBipStarBy(STMT a1)
{
	vector<STMT> result;
	//auto itr = this->affectsBipStarByMap.find(a1);

	if (this->affectsBipStarByMap.count(a1) > 0){
		return affectsBipStarByMap[a1];
	} else {
		result =  PQLAffectsProcessor::getAffectsBipStarBy(a1);
		pair<STMT, vector<STMT>> newItem (a1, result);
		this->affectsBipStarByMap.insert(newItem);

		for (size_t i = 0; i < result.size(); i++)	
			PKB::affects.insertAffectsBipStar(a1, result.at(i), true);
	}

	return result;
}

/**
* This method will be used to get a list of a2 that is affectsBipStar(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affectStar a2
*/
vector<STMT> AffectsTable::getAffectsBipStarFrom(STMT a2)
{
	vector<STMT> result;
	//auto itr = this->affectsBipStarFromMap.find(a2);

	if (this->affectsBipStarFromMap.count(a2) > 0){
		return affectsBipStarFromMap[a2];
	} else {
		result =  PQLAffectsProcessor::getAffectsBipStarFrom(a2);
		pair<STMT, vector<STMT>> newItem (a2, result);
		this->affectsBipStarFromMap.insert(newItem);
		for (size_t i = 0; i < result.size(); i++)	
			PKB::affects.insertAffectsBipStar(result.at(i), a2, true);
	}

	return result;
}


/**
* This method will be used to check existing record
* @param v	a list of affects
* @param a	the affectstar to check for existing record
* @return whether a exist in v
*/
bool AffectsTable::isDuplicate(vector<Affects*>& v, Affects* a)
{
	for (size_t i = 0; i < v.size(); i++) {
		if (v[i]->getA1() == a->getA1() && v[i]->getA2() == a->getA2())
			return true;
	}

	return false;
}

/**
* Tear down the cache after evaluation 
* CALL AFTER EVALUATION IS COMPLETE 
*/
void AffectsTable::tearDownCache() {
	this->affectsByMap.clear();
	this->affectsFromMap.clear();
	this->affectsMap.clear();

	this->affectsStarByMap.clear();
	this->affectsStarFromMap.clear();
	this->affectsStarMap.clear();

	this->affectsBipByMap.clear();
	this->affectsBipFromMap.clear();
	this->affectsBipMap.clear();

	this->affectsBipStarByMap.clear();
	this->affectsBipStarFromMap.clear();
	this->affectsBipStarMap.clear();
}