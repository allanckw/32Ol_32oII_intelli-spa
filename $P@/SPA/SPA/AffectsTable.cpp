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
	const Affects n(a1, a2, isAffected);
	
	affectsLock.lock();
	if (affectsMap.count(a1) == 0) {
		vector<Affects> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects>> newItem (a1, affectsLst);
		this->affectsMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(affectsMap[a1], n)){
			affectsMap[a1].push_back(n);
		}
	}
	affectsLock.unlock();
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

	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsLock);
		if (affectsMap.count(a1) > 0) {

			const vector<Affects>& affectsLst = affectsMap[a1];
	
			for (size_t i = 0; i < affectsLst.size(); i++) {
				if (affectsLst[i].getA2() == a2 )
					return affectsLst[i].isAffected();
			}
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
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsByLock);
		if (affectsByMap.count(a1) > 0)
			return vector<STMT>(affectsByMap[a1].begin(), affectsByMap[a1].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsBy(a1);
	const pair<STMT, vector<STMT>> newItem (a1, result);

	affectsByLock.lock();
	this->affectsByMap.insert(newItem);
	affectsByLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffects(a1, result.at(i), true);

	return result;	
}

/**
* This method will be used to get a list of a2 that is affects(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affect a2
*/
vector<STMT> AffectsTable::getAffectsFrom(STMT a2)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsFromLock);
		if (affectsFromMap.count(a2) > 0)
			return vector<STMT>(affectsFromMap[a2].begin(), affectsFromMap[a2].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsFrom(a2);
	const pair<STMT, vector<STMT>> newItem (a2, result);

	affectsFromLock.lock();
	this->affectsFromMap.insert(newItem);
	affectsFromLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffects(result.at(i), a2, true);
	
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
	const Affects n(a1, a2, isAffected);
	
	affectsStarLock.lock();
	if (affectsStarMap.count(a1) == 0) {
		vector<Affects> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects>> newItem (a1, affectsLst);
		this->affectsStarMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(affectsStarMap[a1], n)){
			affectsStarMap[a1].push_back(n);
		}
	}
	affectsStarLock.unlock();
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

	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsStarLock);
		if (affectsStarMap.count(a1) > 0) {

			const vector<Affects>& affectsStarLst = affectsStarMap[a1];
	
			for (size_t i = 0; i < affectsStarLst.size(); i++) {
				if (affectsStarLst[i].getA2() == a2 )
					return affectsStarLst[i].isAffected();
			}
		}
	}

	if (PQLAffectsProcessor::isAffectsStar(a1,a2)){
		PKB::affects.insertAffectsStar(a1, a2, true);
		return true;
	} else {
		PKB::affects.insertAffectsStar(a1, a2, false);
		return false;
	}
}

/**
* This method will be used to get a list of a2 that is Affects(a1,_)
* @param a1	The statement that is going to affect a2
* @return a list of statement that is affected by a1
*/
vector<STMT> AffectsTable::getAffectsStarBy(STMT a1)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsStarByLock);
		if (affectsStarByMap.count(a1) > 0)
			return vector<STMT>(affectsStarByMap[a1].begin(), affectsStarByMap[a1].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsStarBy(a1);
	const pair<STMT, vector<STMT>> newItem (a1, result);

	affectsStarByLock.lock();
	this->affectsStarByMap.insert(newItem);
	affectsStarByLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffectsStar(a1, result.at(i), true);

	return result;
}

/**
* This method will be used to get a list of a2 that is Affects(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affect a2
*/
vector<STMT> AffectsTable::getAffectsStarFrom(STMT a2)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsStarFromLock);
		if (affectsStarFromMap.count(a2) > 0)
			return vector<STMT>(affectsStarFromMap[a2].begin(), affectsStarFromMap[a2].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsStarFrom(a2);
	const pair<STMT, vector<STMT>> newItem (a2, result);

	affectsStarFromLock.lock();
	this->affectsStarFromMap.insert(newItem);
	affectsStarFromLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffectsStar(result.at(i), a2, true);
	
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
	const Affects n(a1, a2, isAffected);
	
	affectsBipLock.lock();
	if (affectsBipMap.count(a1) == 0) {
		vector<Affects> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects>> newItem (a1, affectsLst);
		this->affectsBipMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(affectsBipMap[a1], n)){
			affectsBipMap[a1].push_back(n);
		}
	}
	affectsBipLock.unlock();
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

	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsBipLock);
		if (affectsBipMap.count(a1) > 0) {

			const vector<Affects>& affectsBipLst = affectsBipMap[a1];
	
			for (size_t i = 0; i < affectsBipLst.size(); i++) {
				if (affectsBipLst[i].getA2() == a2 )
					return affectsBipLst[i].isAffected();
			}
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
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsBipByLock);
		if (affectsBipByMap.count(a1) > 0)
			return vector<STMT>(affectsBipByMap[a1].begin(), affectsBipByMap[a1].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsBipBy(a1);
	const pair<STMT, vector<STMT>> newItem (a1, result);

	affectsBipByLock.lock();
	this->affectsBipByMap.insert(newItem);
	affectsBipByLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffectsBip(a1, result.at(i), true);

	return result;
}

/**
* This method will be used to get a list of a2 that is affectsBip(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affect a2
*/
vector<STMT> AffectsTable::getAffectsBipFrom(STMT a2)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsBipFromLock);
		if (affectsBipFromMap.count(a2) > 0)
			return vector<STMT>(affectsBipFromMap[a2].begin(), affectsBipFromMap[a2].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsBipFrom(a2);
	const pair<STMT, vector<STMT>> newItem (a2, result);

	affectsBipFromLock.lock();
	this->affectsBipFromMap.insert(newItem);
	affectsBipFromLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffectsBip(result.at(i), a2, true);
	
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
	const Affects n(a1, a2, isAffected);
	
	affectsBipStarLock.lock();
	if (affectsBipStarMap.count(a1) == 0) {
		vector<Affects> affectsLst;
		affectsLst.push_back(n);
		pair<STMT, vector<Affects>> newItem (a1, affectsLst);
		this->affectsBipStarMap.insert(newItem);
	} else {
		if (!AffectsTable::isDuplicate(affectsBipStarMap[a1], n)){
			affectsBipStarMap[a1].push_back(n);
		}
	}
	affectsBipStarLock.unlock();
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

	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsBipStarLock);
		if (affectsBipStarMap.count(a1) > 0) {

			const vector<Affects>& affectsBipStarLst = affectsBipStarMap[a1];
	
			for (size_t i = 0; i < affectsBipStarLst.size(); i++) {
				if (affectsBipStarLst[i].getA2() == a2 )
					return affectsBipStarLst[i].isAffected();
			}
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
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsBipStarByLock);
		if (affectsBipStarByMap.count(a1) > 0)
			return vector<STMT>(affectsBipStarByMap[a1].begin(), affectsBipStarByMap[a1].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsBipStarBy(a1);
	const pair<STMT, vector<STMT>> newItem (a1, result);

	affectsBipStarByLock.lock();
	this->affectsBipStarByMap.insert(newItem);
	affectsBipStarByLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffectsBipStar(a1, result.at(i), true);

	return result;
}

/**
* This method will be used to get a list of a2 that is affectsBipStar(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affectStar a2
*/
vector<STMT> AffectsTable::getAffectsBipStarFrom(STMT a2)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(affectsBipStarFromLock);
		if (affectsBipStarFromMap.count(a2) > 0)
			return vector<STMT>(affectsBipStarFromMap[a2].begin(), affectsBipStarFromMap[a2].end());
	}
	
	const vector<STMT>& result = PQLAffectsProcessor::getAffectsBipStarFrom(a2);
	const pair<STMT, vector<STMT>> newItem (a2, result);

	affectsBipStarFromLock.lock();
	this->affectsBipStarFromMap.insert(newItem);
	affectsBipStarFromLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertAffectsBipStar(result.at(i), a2, true);
	
	return result;
}


/**
* This method will be used to check existing record
* @param v	a list of affects
* @param a	the affectstar to check for existing record
* @return whether a exist in v
*/
bool AffectsTable::isDuplicate(const vector<Affects>& v, const Affects& a)
{
	for (size_t i = 0; i < v.size(); i++) {
		if (v[i].getA1() == a.getA1() && v[i].getA2() == a.getA2())
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