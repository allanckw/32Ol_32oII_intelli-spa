#include "NextTable.h"
#include "PQLNextProcessor.h"

/**
* This method will be used as a constructor to create NextTable
*/
NextTable::NextTable() {
}

/**
* This method will be used to check Next(p1,p2)
* @Param p1	The prog_line that is before p2
* @Param p2	The prog_line that is after p1
* @Return true if p2 is right after p1, otherwise return false
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
* @Param p1	The prog_line that is before p2
* @Return list of prog_line after p1
*/
vector<PROG_LINE> NextTable::getNext(PROG_LINE p1)
{
	return PQLNextProcessor::getNext(p1);
}

/**
* This method will be used to get prog_line before p2
* @Param p2	The prog_line that is after p1
* @Return list of prog_line before p1
*/
vector<PROG_LINE> NextTable::getPrevious(PROG_LINE p2)
{
	return PQLNextProcessor::getPrevious(p2);
}

/**
* This method will be used to insert NEXTStar into the table
* @Param p1	The prog_line that is before p2
* @Param p2	The prog_line that is after p1
* @Param next True or False
* @Return list of prog_line before p1
*/
void NextTable::insertNextStar(PROG_LINE p1, PROG_LINE p2, bool next)
{
	Next* n = new Next(p1, p2, next);
	
	auto itr = this->nextStarMap.find(p1);
	
	if (itr == this->nextStarMap.end()) {
		vector<Next*> nextLst;
		nextLst.push_back(n);
		pair<PROG_LINE, vector<Next*>> newItem (p1, nextLst);
		this->nextStarMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(this->nextStarMap.at(p1), n)){
			this->nextStarMap.at(p1).push_back(n);
		}
	}
}

/**
* This method will be used to check NextStar(p1,p2)
* @Param p1	The prog_line that is before p2
* @Param p2	The prog_line that is after p1
* @Return true if p2 is reachable from p1, otherwise return false
*/
bool NextTable::isNextStar (PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;

	if (this->isNext(p1, p2))
		return true;
	
	auto itr = this->nextStarMap.find(p1);

	if (itr != this->nextStarMap.end()){

		vector<Next*> nxtLst =  itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getP2() == p2 )
				return nxtLst[i]->isNext();
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
* @Param p1	The start prog_line to find all reachable prog_line
* @Return a list of prog_line reachable from p1
*/
vector<PROG_LINE> NextTable::getNextStar(PROG_LINE p1)
{
	vector<PROG_LINE> progLines = PQLNextProcessor::getNextStar(p1);

	for (unsigned int i = 0; i < progLines.size(); i++){
		PKB::next.insertNextStar(p1, progLines.at(i), true);
	}

	return progLines;
}

/**
* This method will be used to check NextStar(_,p2)
* @Param p2	The end prog_line that can be reach from p1
* @Return a list of prog_line that can reach p2
*/
vector<PROG_LINE> NextTable::getPreviousStar(PROG_LINE p2)
{
	vector<PROG_LINE> progLines = PQLNextProcessor::getPreviousStar(p2);

	for (unsigned int i = 0; i < progLines.size(); i++){
		PKB::next.insertNextStar(progLines.at(i), p2, true);
	}

	return progLines;
}

/**
* This method will be used to check existing record
* @Param v	a list of nextstar
* @Param n	the nextstar to check for existing record
* @Return whether n exist in v
*/
bool NextTable::isDuplicate(vector<Next*> v, Next* n)
{
	for (unsigned int i = 0; i < v.size(); i++) {
		if (v[i]->getP1() == n->getP1() && v[i]->getP2() == n->getP2())
			return true;
	}

	return false;
}

/**
 * Tear down the cache after evaluation =( =( =( =( 
 * NEXT* ONLY!!!
 * CALL AFTER EVALUATION IS COMPLETE 
 */
void NextTable::tearDownCache()
{
	this->nextStarMap.clear();
}