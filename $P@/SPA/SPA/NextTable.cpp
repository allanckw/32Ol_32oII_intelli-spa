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
	Next n = Next(p1, p2, next);
	
	//auto itr = this->nextStarMap.find(p1);
	
	if (this->nextStarMap.count(p1)>0) {
		vector<Next> nextLst;
		nextLst.push_back(n);
		pair<PROG_LINE, vector<Next>> newItem (p1, nextLst);
		this->nextStarMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(this->nextStarMap.at(p1), n)){
			this->nextStarMap.at(p1).push_back(n);
		}
	}
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

	if (this->isNext(p1, p2))
		return true;
	
	//auto itr = this->nextStarMap.find(p1);

	if (this->nextStarMap.count(p1) > 0){

		vector<Next> nxtLst = nextStarMap[p1];
	
		for (size_t i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i].getP2() == p2 )
				return nxtLst[i].isNext();
		}
	}

	if (PQLNextProcessor::isNextStar(p1,p2)){
		PKB::next.insertNextStar(p1,p2, true);
		return true;
	} else {
		PKB::next.insertNextStar(p1, p2, false);
		return false;
	}
}


/**
* This method will be used to check NextStar(p1,_)
* @param p1	The t prog_line to find all reachable prog_line
* @return a list of prog_line reachable from p1
*/
vector<PROG_LINE> NextTable::getNextStar(PROG_LINE p1)
{
	vector<PROG_LINE> result;
	//auto itr = this->nextStarByMap.find(p1);

	if (this->nextStarByMap.count(p1) > 0){
		if (nextStarByMap[p1].size() == 0){
			result =  PQLNextProcessor::getNextStar(p1);
			pair<PROG_LINE, vector<PROG_LINE>> newItem (p1, result);
			this->nextStarByMap.insert(newItem);

			for (int i = 0; i < result.size(); i++)	
				PKB::next.insertNextStar(p1, result.at(i), true);

		} else {
			return nextStarByMap[p1];
		}
	} else {
		result =  PQLNextProcessor::getNextStar(p1);
		pair<PROG_LINE, vector<PROG_LINE>> newItem (p1, result);
		this->nextStarByMap.insert(newItem);
		
		for (int i = 0; i < result.size(); i++)	
			PKB::next.insertNextStar(p1, result.at(i), true);
	}

	return result;
}

/**
* This method will be used to check NextStar(_,p2)
* @param p2	The end prog_line that can be reach from p1
* @return a list of prog_line that can reach p2
*/
vector<PROG_LINE> NextTable::getPreviousStar(PROG_LINE p2)
{
	vector<PROG_LINE> result;
	//auto itr = this->previousStarMap.find(p2);

	if (this->previousStarMap.count(p2) > 0){
		if (previousStarMap[p2].size() == 0){
			result =  PQLNextProcessor::getPreviousStar(p2);
			pair<PROG_LINE, vector<PROG_LINE>> newItem (p2, result);
			this->previousStarMap.insert(newItem);
			for (int i = 0; i < result.size(); i++)	
				PKB::next.insertNextStar(result.at(i), p2, true);
			
		} else {
			return previousStarMap[p2];
		}
	} else {
		result =  PQLNextProcessor::getPreviousStar(p2);
		pair<PROG_LINE, vector<PROG_LINE>> newItem (p2, result);
		this->previousStarMap.insert(newItem);
		for (int i = 0; i < result.size(); i++)	
			PKB::next.insertNextStar(result.at(i), p2, true);
	}

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
	Next n =  Next(p1, p2, next);
	
	//auto itr = this->nextBipMap.find(p1);
	
	if (this->nextBipMap.count(p1) > 0) {
		vector<Next> nextLst;
		nextLst.push_back(n);
		pair<PROG_LINE, vector<Next>> newItem (p1, nextLst);
		this->nextBipMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(this->nextBipMap.at(p1), n)){
			this->nextBipMap.at(p1).push_back(n);
		}
	}
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

	//auto itr = this->nextBipMap.find(p1);

	if (this->nextBipMap.count(p1) > 0){

		vector<Next> nxtLst = nextBipMap[p1];
	
		for (size_t i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i].getP2() == p2 )
				return nxtLst[i].isNext();
		}
	}

	if (PQLNextProcessor::isNextBip(p1,p2)){
		PKB::next.insertNextBip(p1,p2, true);
		return true;
	} else {
		PKB::next.insertNextBip(p1, p2, false);
		return false;
	}
}

/**
* This method will be used to check NextBip(p1,_)
* @param p1	The t prog_line to find all reachable prog_line
* @return a list of prog_line reachable from p1
*/
vector<PROG_LINE> NextTable::getNextBip(PROG_LINE p1)
{
	vector<PROG_LINE> result;
	//auto itr = this->nextBipByMap.find(p1);

	if (this->nextBipByMap.count(p1) > 0){
		if (nextBipByMap[p1].size() == 0){
			result =  PQLNextProcessor::getNextBip(p1);
			pair<PROG_LINE, vector<PROG_LINE>> newItem (p1, result);
			this->nextBipByMap.insert(newItem);

			for (int i = 0; i < result.size(); i++)	
				PKB::next.insertNextBip(p1, result.at(i), true);
			

		} else {
			return nextBipByMap[p1];
		}
	} else {
		result =  PQLNextProcessor::getNextBip(p1);
		pair<PROG_LINE, vector<PROG_LINE>> newItem (p1, result);
		this->nextBipByMap.insert(newItem);
		
		for (int i = 0; i < result.size(); i++)	
			PKB::next.insertNextBip(p1, result.at(i), true);
	}

	return result;
}

/**
* This method will be used to check NextBip(_,p2)
* @param p2	The end prog_line that can be reach from p1
* @return a list of prog_line that can reach p2
*/
vector<PROG_LINE> NextTable::getPreviousBip(PROG_LINE p2)
{
	vector<PROG_LINE> result;
	//auto itr = this->previousBipMap.find(p2);

	if (this->previousBipMap.count(p2) > 0){
		if (previousBipMap[p2].size() == 0){
			result =  PQLNextProcessor::getPreviousBip(p2);
			pair<PROG_LINE, vector<PROG_LINE>> newItem (p2, result);
			this->previousBipMap.insert(newItem);
			for (int i = 0; i < result.size(); i++)	
				PKB::next.insertNextBip(result.at(i), p2, true);
			
		} else {
			return previousBipMap[p2];
		}
	} else {
		result =  PQLNextProcessor::getPreviousBip(p2);
		pair<PROG_LINE, vector<PROG_LINE>> newItem (p2, result);
		this->previousBipMap.insert(newItem);
		for (int i = 0; i < result.size(); i++)	
			PKB::next.insertNextBip(result.at(i), p2, true);
	}

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
	Next n =  Next(p1, p2, next);
	
	//auto itr = this->nextBipStarMap.find(p1);
	
	if (this->nextBipStarMap.count(p1) > 0) {
		vector<Next> nextLst;
		nextLst.push_back(n);
		pair<PROG_LINE, vector<Next>> newItem (p1, nextLst);
		this->nextBipStarMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(this->nextBipStarMap.at(p1), n)){
			this->nextBipStarMap.at(p1).push_back(n);
		}
	}
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
	
	//auto itr = this->nextBipStarMap.find(p1);

	if (this->nextBipStarMap.count(p1) > 0){

		vector<Next> nxtLst =  nextBipStarMap[p1];
	
		for (size_t i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i].getP2() == p2 )
				return nxtLst[i].isNext();
		}
	}

	if (PQLNextProcessor::isNextBipStar(p1,p2)){
		PKB::next.insertNextBipStar(p1,p2, true);
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
	vector<PROG_LINE> result;
	//auto itr = this->nextBipStarByMap.find(p1);

	if (this->nextBipStarByMap.count(p1) > 0){
		if (nextBipStarByMap[p1].size() == 0){
			result =  PQLNextProcessor::getNextBipStar(p1);
			pair<PROG_LINE, vector<PROG_LINE>> newItem (p1, result);
			this->nextBipStarByMap.insert(newItem);

			for (int i = 0; i < result.size(); i++)	
				PKB::next.insertNextBipStar(p1, result.at(i), true);
			

		} else {
			return nextBipStarByMap[p1];
		}
	} else {
		result =  PQLNextProcessor::getNextBipStar(p1);
		pair<PROG_LINE, vector<PROG_LINE>> newItem (p1, result);
		this->nextBipStarByMap.insert(newItem);
		
		for (int i = 0; i < result.size(); i++)	
			PKB::next.insertNextBipStar(p1, result.at(i), true);
	}

	return result;
}

/**
* This method will be used to check NextBipStar(_,p2)
* @param p2	The end prog_line that can be reach from p1
* @return a list of prog_line that can reach p2
*/
vector<PROG_LINE> NextTable::getPreviousBipStar(PROG_LINE p2)
{
	vector<PROG_LINE> result;
	//auto itr = this->previousBipStarMap.find(p2);

	if (this->previousBipStarMap.count(p2) > 0){
		if (previousBipStarMap[p2].size() == 0){
			result =  PQLNextProcessor::getPreviousBipStar(p2);
			pair<PROG_LINE, vector<PROG_LINE>> newItem (p2, result);
			this->previousBipStarMap.insert(newItem);
			for (int i = 0; i < result.size(); i++)	
				PKB::next.insertNextBipStar(result.at(i), p2, true);
			
		} else {
			return previousBipStarMap[p2];
		}
	} else {
		result =  PQLNextProcessor::getPreviousBipStar(p2);
		pair<PROG_LINE, vector<PROG_LINE>> newItem (p2, result);
		this->previousBipStarMap.insert(newItem);
		for (int i = 0; i < result.size(); i++)	
			PKB::next.insertNextBipStar(result.at(i), p2, true);
	}

	return result;
}


/**
* This method will be used to check existing record
* @param v	a list of nextstar
* @param n	the nextstar to check for existing record
* @return whether n exist in v
*/
bool NextTable::isDuplicate(vector<Next> v, Next n)
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
	this->nextBipMap.clear();
	this->nextBipStarMap.clear();
}
