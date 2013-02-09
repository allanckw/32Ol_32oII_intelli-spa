#include "NextTable.h"
#include "PQLNextProcessor.h"

//DO NOT TOUCH NEXT, NEXT CAN BE PRE-COMPUTED BUT NOT NEXT* 
void NextTable::insertNext (PROG_LINE p1, PROG_LINE p2, bool next)
{
	Next* n = new Next(p1, p2, next);
	
	auto itr = this->nextMap.find(p1);
	
	if (itr == this->nextMap.end()) {
		vector<Next*> nextLst;
		nextLst.push_back(n);
		pair<PROG_LINE, vector<Next*>> newItem (p1, nextLst);
		this->nextMap.insert(newItem);
	} else {
		this->nextMap.at(p1).push_back(n);
	}
}

bool NextTable::isNext(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
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
			progLines.push_back(nxtLst[i]->getP2());
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

//USE PQLNEXTPROCESSOR TO COMPUTE
//QUERYEVALUATOR WILL ONLY USE NEXT TABLE
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
		this->nextStarMap.at(p1).push_back(n);
	}
}

bool NextTable::isNextStar (PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;

	auto itr = this->nextStarMap.find(p1);
	if (itr != this->nextStarMap.end()){

		vector<Next*> nxtLst =  itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getP2() == p2 )
				return nxtLst[i]->isNext();
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
