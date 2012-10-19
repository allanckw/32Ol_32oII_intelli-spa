#include "StdAfx.h"
#include "CallsTable.h"
#include "PROCTable.h"

CallsTable::CallsTable()
{
}

void CallsTable::insertCalls(PROCIndex p1, PROCIndex p2)
{
	auto newPair = make_pair(p1, p2);
	if (callsTable.size() != 0)
	{
		for (int i = 0; i < callsTable.size(); i++)
		{
			if (callsTable.at(i) == newPair)
				return;
		}
	}

	callsTable.push_back(newPair);
}

//This function should be called by whoever after CallsTable is fully populated
//Creates hash tables callsTable and callsTable star for fast access during queries
void CallsTable::optimizeCallsTable()
{
	vector<PROCIndex> immediateChildren, procsChecked;

	if (callsTable.size() != 0)
	{
		for (int i = 0; i < callsTable.size(); i++)
		{
			optimizedCalledByTable.at(callsTable.at(i).first).push_back(callsTable.at(i).second);
			optimizedCalledFromTable.at(callsTable.at(i).second).push_back(callsTable.at(i).first);
		}
	}

	for (int x = 0; x < PROCTable->size(); x++) //Need actual size of PROCTable during runtime
	{
		immediateChildren = optimizedCalledByTable.at(x);
		optimizeCalledByStarTable(x, immediateChildren, procsChecked);

		immediateChildren = optimizedCalledFromTable.at(x);
		optimizeCalledFromStarTable(x, immediateChildren, procsChecked);
	}

	return;
}

void CallsTable::optimizeCalledByStarTable(PROCIndex p, vector<PROCIndex> currentChildren, vector<PROCIndex> procsChecked)
{
	vector<PROCIndex> newChildren;
	bool isProcChecked = false;

	//Keeping track of which procedures have already been interrogated and picking those not explored yet
	newChildren.clear();
	if (!procsChecked.empty() && !currentChildren.empty())
	{
		for (int k = 0; k < currentChildren.size(); k++)
		{
			for (int x = 0; x < procsChecked.size(); x++)
			{
				if (currentChildren.at(k) == procsChecked.at(x))
				{
					isProcChecked = true;
					break;
				}
			}

			if(!isProcChecked)
				procsChecked.push_back(currentChildren.at(k));

			isProcChecked = false;
		}
	}

	//Base case: either all procedures exhausted, or no new children left to explore
	//Need to know actual size of PROCTable during runtime
	if (procsChecked.size() == "max size of PROCTable" || newChildren.empty())
		return;

	//To insert new grandchildren for next recursive call
	currentChildren.clear();
	
	for (int i = 0; i < newChildren.size(); i++) //for every child procedure
	{
		//add each new child to optimized table
		optimizedCalledByStarTable.at(p).push_back(newChildren.at(i)); 
		if (!optimizedCalledByTable.at(newChildren.at(i)).empty())
		{
			for (int j = 0; j < optimizedCalledByTable.at(newChildren.at(i)).size(); j++) //for every child of each child procedure
			{
				//add each grandchild to list of children to interrogate next(WHOS UR CHILD?!
				currentChildren.push_back(optimizedCalledByTable.at(newChildren.at(i)).at(j));
			}
		}
	}

	optimizeCalledByStarTable(p, currentChildren, procsChecked);
}

void CallsTable::optimizeCalledFromStarTable(PROCIndex p, vector<PROCIndex> currentChildren, vector<PROCIndex> procsChecked)
{
	vector<PROCIndex> newChildren;
	bool isProcChecked = false;

	//Keeping track of which procedures have already been interrogated and picking those not explored yet
	newChildren.clear();
	if (!procsChecked.empty() && !currentChildren.empty())
	{
		for (int k = 0; k < currentChildren.size(); k++)
		{
			for (int x = 0; x < procsChecked.size(); x++)
			{
				if (currentChildren.at(k) == procsChecked.at(x))
				{
					isProcChecked = true;
					break;
				}
			}

			if(!isProcChecked)
				procsChecked.push_back(currentChildren.at(k));

			isProcChecked = false;
		}
	}

	//Base case: either all procedures exhausted, or no new children left to explore
	//Need to know actual size of PROCTable during runtime
	if (procsChecked.size() == "max size of PROCTable" || newChildren.empty())
		return;

	//To insert new grandchildren for next recursive call
	currentChildren.clear();
	
	for (int i = 0; i < newChildren.size(); i++) //for every child procedure
	{
		//add each new child to optimized table
		optimizedCalledFromStarTable.at(p).push_back(newChildren.at(i));
		if (!optimizedCalledFromTable.at(newChildren.at(i)).empty())
		{
			for (int j = 0; j < optimizedCalledFromTable.at(newChildren.at(i)).size(); j++) //for every child of each child procedure
			{
				//add each grandchild to list of children to interrogate next(WHOS UR CHILD?!
				currentChildren.push_back(optimizedCalledFromTable.at(newChildren.at(i)).at(j));
			}
		}
	}

	optimizeCalledFromStarTable(p, currentChildren, procsChecked);
}

vector<PROCIndex> CallsTable::getCalledBy(PROCIndex p)
{
	vector<PROCIndex> answer;

	if (optimizedCalledByTable.size() >= p)
		answer = optimizedCalledByTable.at(p);

	return answer;
}

vector<PROCIndex> CallsTable::getCalledFrom(PROCIndex p)
{
	vector<PROCIndex> answer;

	if (optimizedCalledFromTable.size() >= p)
		answer = optimizedCalledFromTable.at(p);

	return answer;
}

vector<PROCIndex> CallsTable::getCalledByStar(PROCIndex p)
{
	vector<PROCIndex> answer;

	if (optimizedCalledByStarTable.size() >= p)
		answer = optimizedCalledByStarTable.at(p);

	return answer; 
}

vector<PROCIndex> CallsTable::getCalledFromStar(PROCIndex p)
{
	vector<PROCIndex> answer;

	if (optimizedCalledFromStarTable.size() >= p)
		answer = optimizedCalledFromStarTable.at(p);

	return answer;
}

bool CallsTable::isCalled(PROCIndex p1, PROCIndex p2)
{
	if (optimizedCalledByTable.size() >= p1)
	{
		for (int i = 0; i < optimizedCalledByTable.at(p1).size(); i++)
		{
			if(optimizedCalledByTable.at(p1).at(i) == p2)
				return true;
		}
	}

	return false;
}

bool CallsTable::isCalledStar(PROCIndex p1, PROCIndex p2)
{
	if (optimizedCalledByStarTable.size() >= p1)
	{
		for (int i = 0; i < optimizedCalledByStarTable.at(p1).size(); i++)
		{
			if(optimizedCalledByStarTable.at(p1).at(i) == p2)
				return true;
		}
	}

	return false;
}