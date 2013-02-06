#include "NextTable.h"

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

bool NextTable::isNext (PROG_LINE p1, PROG_LINE p2)
{
	if (p1 <= 0 || p2 <= 0)
		return false;

	auto itr = this->nextMap.find(p1);
	if (itr != this->nextMap.end()){

		vector<Next*> nxtLst = (vector<Next*>)itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getP2() == p2 )
				return nxtLst[i]->isNext();
		}
	}

	return false;
}

vector<PROG_LINE> NextTable::getNextBy(PROG_LINE p1)
{
	vector<PROG_LINE> progLines;

	auto itr = this->nextMap.find(p1);

	if (itr != this->nextMap.end()){
		vector<Next*> nxtLst = (vector<Next*>)itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			progLines.push_back(nxtLst[i]->getP2());
		}
	}

	return progLines;

}

vector<PROG_LINE> NextTable::getNextFrom(PROG_LINE p2)
{
	vector<PROG_LINE> progLines;

	for (auto itr = nextMap.begin(); itr != nextMap.end(); itr++) {
		vector<Next*> nxtLst = (vector<Next*>)itr->second;

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

		vector<Next*> nxtLst = (vector<Next*>)itr->second;
	
		for (unsigned int i = 0; i < nxtLst.size(); i++) {
			if (nxtLst[i]->getP2() == p2 )
				return nxtLst[i]->isNext();
		}
	}

	//@NICK IF CANNOT FIND -> USE PQLNEXTPROCESSOR TO TRAVERSE DO NOT CODE HERE
	//REMEMBER TO ADD BACK USING PKB::next::insertNextStar(p1, p2, true or false);

	return false;
}

//USE FOR NEXT*(p1, _)
vector<PROG_LINE> NextTable::getNextByStar(PROG_LINE p1)
{
	vector<PROG_LINE> progLines;
	//UP TO U TO IMPLEMENT, BUT DO USE PQLNEXTPROCESSOR


	return progLines;
}

//USE FOR NEXT*(_, p2)
vector<PROG_LINE> NextTable::getNextFromStar(PROG_LINE p2)
{
	vector<PROG_LINE> progLines;
	//UP TO U TO IMPLEMENT, BUT DO USE PQLNEXTPROCESSOR
	return progLines;
}
