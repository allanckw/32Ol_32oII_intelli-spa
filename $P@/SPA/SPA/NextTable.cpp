#include "NextTable.h"
#include "PQLNextProcessor.h"


void NextTable::insertNext (PROG_LINE p1, PROG_LINE p2, bool next)
{
	if (p1 == p2) //impossibru case
		return;

	Next* n = new Next(p1, p2, next);
	
	auto itr = this->nextMap.find(p1);
	
	if (itr == this->nextMap.end()) {
		vector<Next*> nextLst;
		nextLst.push_back(n);
		pair<PROG_LINE, vector<Next*>> newItem (p1, nextLst);
		this->nextMap.insert(newItem);
	} else {
		if (!NextTable::isDuplicate(this->nextMap.at(p1), n)){
			this->nextMap.at(p1).push_back(n);
		}
	}
}

bool NextTable::isNext(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;

	if (p1 == p2)
		return false;

	auto itr = this->nextMap.find(p1);
	if (itr != this->nextMap.end()){

		vector<Next*> nxtLst = itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getP2() == p2 )
				return nxtLst[i]->isNext();
		}
	}

	return false;
}

vector<PROG_LINE> NextTable::getNext(PROG_LINE p1)
{
	vector<PROG_LINE> progLines;

	auto itr = this->nextMap.find(p1);

	if (itr != this->nextMap.end()){
		vector<Next*> nxtLst = itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if( nxtLst[i]->isNext()){
				progLines.push_back(nxtLst[i]->getP2());
			}
		}
	}

	return progLines;
}

vector<PROG_LINE> NextTable::getPrevious(PROG_LINE p2)
{
	vector<PROG_LINE> progLines;

	for (auto itr = nextMap.begin(); itr != nextMap.end(); itr++) {
		vector<Next*> nxtLst =  itr->second;

		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getP2() == p2 && nxtLst[i]->isNext())
			{
				progLines.push_back((PROG_LINE)itr->first);
				break;
			}
		}
	}
	return progLines;
}


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
		return true;
	} else {
		PKB::next.insertNextStar(p1, p2, false);
		return false;
	}
}

//USE FOR NEXT*(p1, _)
vector<PROG_LINE> NextTable::getNextStar(PROG_LINE p1)
{
	return PQLNextProcessor::getNextStar(p1);
}

//USE FOR NEXT*(_, p2)
vector<PROG_LINE> NextTable::getPreviousStar(PROG_LINE p2)
{
	return PQLNextProcessor::getPreviousStar(p2);
}

bool NextTable::isNextEmpty()
{
	return (this->nextMap.size() == 0);
}

bool NextTable::isDuplicate(vector<Next*> v, Next* n)
{
	for (unsigned int i = 0; i < v.size(); i++) {
		if (v[i]->getP1() == n->getP1() && v[i]->getP2() == n->getP2())
			return true;
	}

	return false;
}


int NextTable::getSize()
{
	return this->nextMap.size();
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