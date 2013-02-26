#include "NextTable.h"
#include "PQLNextProcessor.h"

NextTable::NextTable() {
}

bool NextTable::isNext(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;

	if (p1 == p2)
		return false;

	return PQLNextProcessor::isNext(p1, p2);
}

vector<PROG_LINE> NextTable::getNext(PROG_LINE p1)
{
	return PQLNextProcessor::getNext(p1);
}

vector<PROG_LINE> NextTable::getPrevious(PROG_LINE p2)
{
	return PQLNextProcessor::getPrevious(p2);
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
		PKB::next.insertNextStar(p1,p2, true);
		return true;
	} else {
		PKB::next.insertNextStar(p1, p2, false);
		return false;
	}
}

//USE FOR NEXT*(p1, _)
vector<PROG_LINE> NextTable::getNextStar(PROG_LINE p1)
{
	vector<PROG_LINE> progLines = PQLNextProcessor::getNextStar(p1);

	for (unsigned int i = 0; i < progLines.size(); i++){
		PKB::next.insertNextStar(p1, progLines.at(i), true);
	}

	return progLines;
}

//USE FOR NEXT*(_, p2)
vector<PROG_LINE> NextTable::getPreviousStar(PROG_LINE p2)
{
	vector<PROG_LINE> progLines = PQLNextProcessor::getPreviousStar(p2);

	for (unsigned int i = 0; i < progLines.size(); i++){
		PKB::next.insertNextStar(progLines.at(i), p2, true);
	}

	return progLines;
}

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