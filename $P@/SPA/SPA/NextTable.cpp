#include "NextTable.h"
#include "PQLNextProcessor.h"

/**
* This method will be used as a constructor to create NextTable
*/
NextTable::NextTable() {
}

/**
* This method will be used to check Next(p1,p2)
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @return true if p2 is right after p1, otherwise return false
*/
bool NextTable::isNext(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;

	if (p1 == p2)
		return false;

	return PQLNextProcessor::isNext(p1, p2);
}

/**
* This method will be used to get prog_line after p1
* @param p1	The prog_line that is before p2
* @return list of prog_line after p1
*/
vector<PROG_LINE> NextTable::getNext(PROG_LINE p1)
{
	return PQLNextProcessor::getNext(p1);
}

/**
* This method will be used to get prog_line before p2
* @param p2	The prog_line that is after p1
* @return list of prog_line before p1
*/
vector<PROG_LINE> NextTable::getPrevious(PROG_LINE p2)
{
	return PQLNextProcessor::getPrevious(p2);
}

/**
* This method will be used to insert NextStar into the table
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @param next True or False
* @return list of prog_line before p1
*/
void NextTable::insertNextStar(PROG_LINE p1, PROG_LINE p2, bool next)
{
	const Next n(p1, p2, next);
	
	nextStarLock.lock();
	if (nextStarMap.count(p1) == 0) {
		vector<Next> nextLst;
		nextLst.push_back(n);
		pair<STMT, vector<Next>> newItem (p1, nextLst);
		this->nextStarMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(nextStarMap[p1], n)){
			nextStarMap[p1].push_back(n);
		}
	}
	nextStarLock.unlock();
}

/**
* This method will be used to check NextStar(p1,p2)
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @return true if p2 is reachable from p1, otherwise return false
*/
bool NextTable::isNextStar (PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;
	
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(nextStarLock);
		if (nextStarMap.count(p1) > 0) {

			const vector<Next>& nextStarLst = nextStarMap[p1];
	
			for (size_t i = 0; i < nextStarLst.size(); i++) {
				if (nextStarLst[i].getP2() == p2 )
					return nextStarLst[i].isNext();
			}
		}
	}

	if (PQLNextProcessor::isNextStar(p1,p2)){
		PKB::next.insertNextStar(p1, p2, true);
		return true;
	} else {
		PKB::next.insertNextStar(p1, p2, false);
		return false;
	}
}

/**
* This method will be used to check NextStar(p1,_)
* @param p1	The start prog_line to find all reachable prog_line
* @return a list of prog_line reachable from p1
*/
vector<PROG_LINE> NextTable::getNextStar(PROG_LINE p1)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(nextStarByLock);
		if (nextStarByMap.count(p1) > 0)
			return vector<PROG_LINE>(nextStarByMap[p1].begin(), nextStarByMap[p1].end());
	}
	
	const vector<PROG_LINE>& result = PQLNextProcessor::getNextStar(p1);
	const pair<STMT, vector<PROG_LINE>> newItem (p1, result);

	nextStarByLock.lock();
	this->nextStarByMap.insert(newItem);
	nextStarByLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertNextStar(p1, result.at(i), true);

	return result;
}

/**
* This method will be used to check NextStar(_,p2)
* @param p2	The end prog_line that can be reach from p1
* @return a list of prog_line that can reach p2
*/
vector<PROG_LINE> NextTable::getPreviousStar(PROG_LINE p2)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(previousStarLock);
		if (previousStarMap.count(p2) > 0)
			return vector<PROG_LINE>(previousStarMap[p2].begin(), previousStarMap[p2].end());
	}
	
	const vector<PROG_LINE>& result = PQLNextProcessor::getPreviousStar(p2);
	const pair<STMT, vector<PROG_LINE>> newItem (p2, result);

	previousStarLock.lock();
	this->previousStarMap.insert(newItem);
	previousStarLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertNextStar(result.at(i), p2, true);
	
	return result;
}

/**
* This method will be used to insert NextBip into the table
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @param next True or False
* @return list of prog_line before p1
*/
void NextTable::insertNextBip(PROG_LINE p1, PROG_LINE p2, bool next)
{
	const Next n(p1, p2, next);
	
	nextBipLock.lock();
	if (nextBipMap.count(p1) == 0) {
		vector<Next> nextLst;
		nextLst.push_back(n);
		pair<STMT, vector<Next>> newItem (p1, nextLst);
		this->nextBipMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(nextBipMap[p1], n)){
			nextBipMap[p1].push_back(n);
		}
	}
	nextBipLock.unlock();
}

/**
* This method will be used to check NextBip(p1,p2)
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @return true if p2 is reachable from p1, otherwise return false
*/
bool NextTable::isNextBip (PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;

	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(nextBipLock);
		if (nextBipMap.count(p1) > 0) {

			const vector<Next>& nextBipLst = nextBipMap[p1];
	
			for (size_t i = 0; i < nextBipLst.size(); i++) {
				if (nextBipLst[i].getP2() == p2 )
					return nextBipLst[i].isNext();
			}
		}
	}

	if (PQLNextProcessor::isNextBip(p1,p2)){
		PKB::next.insertNextBip(p1, p2, true);
		return true;
	} else {
		PKB::next.insertNextBip(p1, p2, false);
		return false;
	}
}

/**
* This method will be used to check NextBip(p1,_)
* @param p1	The start prog_line to find all reachable prog_line
* @return a list of prog_line reachable from p1
*/
vector<PROG_LINE> NextTable::getNextBip(PROG_LINE p1)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(nextBipByLock);
		if (nextBipByMap.count(p1) > 0)
			return vector<PROG_LINE>(nextBipByMap[p1].begin(), nextBipByMap[p1].end());
	}
	
	const vector<PROG_LINE>& result = PQLNextProcessor::getNextBip(p1);
	const pair<STMT, vector<PROG_LINE>> newItem (p1, result);

	nextBipByLock.lock();
	this->nextBipByMap.insert(newItem);
	nextBipByLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertNextBip(p1, result.at(i), true);

	return result;
}

/**
* This method will be used to check NextBip(_,p2)
* @param p2	The end prog_line that can be reach from p1
* @return a list of prog_line that can reach p2
*/
vector<PROG_LINE> NextTable::getPreviousBip(PROG_LINE p2)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(previousBipLock);
		if (previousBipMap.count(p2) > 0)
			return vector<PROG_LINE>(previousBipMap[p2].begin(), previousBipMap[p2].end());
	}
	
	const vector<PROG_LINE>& result = PQLNextProcessor::getPreviousBip(p2);
	const pair<STMT, vector<PROG_LINE>> newItem (p2, result);

	previousBipLock.lock();
	this->previousBipMap.insert(newItem);
	previousBipLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertNextBip(result.at(i), p2, true);
	
	return result;
}


/**
* This method will be used to insert NextBipStar into the table
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @param next True or False
* @return list of prog_line before p1
*/
void NextTable::insertNextBipStar(PROG_LINE p1, PROG_LINE p2, bool next)
{
	const Next n(p1, p2, next);
	
	nextBipStarLock.lock();
	if (nextBipStarMap.count(p1) == 0) {
		vector<Next> nextLst;
		nextLst.push_back(n);
		pair<STMT, vector<Next>> newItem (p1, nextLst);
		this->nextBipStarMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(nextBipStarMap[p1], n)){
			nextBipStarMap[p1].push_back(n);
		}
	}
	nextBipStarLock.unlock();
}

/**
* This method will be used to check NextBipStar(p1,p2)
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @return true if p2 is reachable from p1, otherwise return false
*/
bool NextTable::isNextBipStar (PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;
	
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(nextBipStarLock);
		if (nextBipStarMap.count(p1) > 0) {

			const vector<Next>& nextBipStarLst = nextBipStarMap[p1];
	
			for (size_t i = 0; i < nextBipStarLst.size(); i++) {
				if (nextBipStarLst[i].getP2() == p2 )
					return nextBipStarLst[i].isNext();
			}
		}
	}

	if (PQLNextProcessor::isNextBipStar(p1,p2)){
		PKB::next.insertNextBipStar(p1, p2, true);
		return true;
	} else {
		PKB::next.insertNextBipStar(p1, p2, false);
		return false;
	}
}

/**
* This method will be used to check NextBipStar(p1,_)
* @param p1	The start prog_line to find all reachable prog_line
* @return a list of prog_line reachable from p1
*/
vector<PROG_LINE> NextTable::getNextBipStar(PROG_LINE p1)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(nextBipStarByLock);
		if (nextBipStarByMap.count(p1) > 0)
			return vector<PROG_LINE>(nextBipStarByMap[p1].begin(), nextBipStarByMap[p1].end());
	}
	
	const vector<PROG_LINE>& result = PQLNextProcessor::getNextBipStar(p1);
	const pair<STMT, vector<PROG_LINE>> newItem (p1, result);

	nextBipStarByLock.lock();
	this->nextBipStarByMap.insert(newItem);
	nextBipStarByLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertNextBipStar(p1, result.at(i), true);

	return result;
}

/**
* This method will be used to check NextBipStar(_,p2)
* @param p2	The end prog_line that can be reach from p1
* @return a list of prog_line that can reach p2
*/
vector<PROG_LINE> NextTable::getPreviousBipStar(PROG_LINE p2)
{
	{
		Concurrency::reader_writer_lock::scoped_lock_read hi(previousBipStarLock);
		if (previousBipStarMap.count(p2) > 0)
			return vector<PROG_LINE>(previousBipStarMap[p2].begin(), previousBipStarMap[p2].end());
	}
	
	const vector<PROG_LINE>& result = PQLNextProcessor::getPreviousBipStar(p2);
	const pair<STMT, vector<PROG_LINE>> newItem (p2, result);

	previousBipStarLock.lock();
	this->previousBipStarMap.insert(newItem);
	previousBipStarLock.unlock();
		
	for (size_t i = 0; i < result.size(); i++)	
		insertNextBipStar(result.at(i), p2, true);
	
	return result;
}


/**
* This method will be used to check existing record
* @param v	a list of nextstar
* @param n	the nextstar to check for existing record
* @return whether n exist in v
*/
bool NextTable::isDuplicate(const vector<Next>& v, const Next& n)
{
	for (size_t i = 0; i < v.size(); i++) {
		if (v[i].getP1() == n.getP1() && v[i].getP2() == n.getP2())
			return true;
	}

	return false;
}

/**
 * Tear down the cache after evaluation
 * CALL AFTER EVALUATION IS COMPLETE 
 */
void NextTable::tearDownCache()
{
	this->nextStarMap.clear();
	nextStarByMap.clear();
	previousStarMap.clear();

	this->nextBipMap.clear();
	nextBipByMap.clear();
	previousBipMap.clear();

	this->nextBipStarMap.clear();
	nextBipStarByMap.clear();
	previousBipStarMap.clear();
}
