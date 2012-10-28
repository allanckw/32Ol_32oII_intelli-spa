#include "StdAfx.h"
#include "CallsTable.h"
#include "PKB.h"

CallsTable::CallsTable()
{
	noProcs = PKB::procedures.getSize();
}

void CallsTable::insertCalls(PROC p1, PROC p2)
{
	if (p1 == p2)
		throw SPAException("Procedure cannot perform direct call on itself.");

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
	vector<PROC> immediateChildren, procsChecked;
	noProcs = PKB::procedures.getSize();

	optimizedCalledByTable = new vector<PROC>[noProcs];
	optimizedCalledByStarTable = new vector<PROC>[noProcs];
	optimizedCalledFromTable = new vector<PROC>[noProcs];
	optimizedCalledFromStarTable = new vector<PROC>[noProcs];

	if (!callsTable.empty())
	{
		for (int i = 0; i < callsTable.size(); i++)
		{
			optimizedCalledByTable[callsTable.at(i).first].push_back(callsTable.at(i).second);
			optimizedCalledFromTable[callsTable.at(i).second].push_back(callsTable.at(i).first);
		}
	}

	for (int x = 0; x < noProcs; x++)
	{
		immediateChildren = optimizedCalledByTable[x];
		optimizeCalledByStarTable(x, immediateChildren, procsChecked);

		immediateChildren = optimizedCalledFromTable[x];
		optimizeCalledFromStarTable(x, immediateChildren, procsChecked);
	}

	return;
}

void CallsTable::optimizeCalledByStarTable(PROC p, vector<PROC> currentChildren, vector<PROC> procsChecked)
{
	vector<PROC> newChildren;
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
			{
				procsChecked.push_back(currentChildren.at(k));
				newChildren.push_back(currentChildren.at(k));
			}

			isProcChecked = false;
		}
	}
	else if (procsChecked.empty() && !currentChildren.empty())
	{
		for (int a = 0; a < currentChildren.size(); a++)
		{
			procsChecked.push_back(currentChildren.at(a));
			newChildren.push_back(currentChildren.at(a));
		}
	}

	//Base case: No new children left to explore
	if (newChildren.empty())
		return;

	//To insert new grandchildren for next recursive call
	currentChildren.clear();
	
	for (int i = 0; i < newChildren.size(); i++) //for every child procedure
	{
		//add each new child to optimized table
		optimizedCalledByStarTable[p].push_back(newChildren.at(i)); 
		if (!optimizedCalledByTable[newChildren.at(i)].empty())
		{
			for (int j = 0; j < optimizedCalledByTable[newChildren.at(i)].size(); j++) //for every child of each child procedure
			{
				//add each grandchild to list of children to interrogate next(WHOS UR CHILD?!
				currentChildren.push_back(optimizedCalledByTable[newChildren.at(i)].at(j));
			}
		}
	}

	optimizeCalledByStarTable(p, currentChildren, procsChecked);
}

void CallsTable::optimizeCalledFromStarTable(PROC p, vector<PROC> currentChildren, vector<PROC> procsChecked)
{
	vector<PROC> newChildren;
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
			{
				procsChecked.push_back(currentChildren.at(k));
				newChildren.push_back(currentChildren.at(k));
			}

			isProcChecked = false;
		}
	}
	else if (procsChecked.empty() && !currentChildren.empty())
	{
		for (int a = 0; a < currentChildren.size(); a++)
		{
			procsChecked.push_back(currentChildren.at(a));
			newChildren.push_back(currentChildren.at(a));
		}
	}

	//Base case: No new children left to explore
	if (newChildren.empty())
		return;

	//To insert new grandchildren for next recursive call
	currentChildren.clear();
	
	for (int i = 0; i < newChildren.size(); i++) //for every child procedure
	{
		//add each new child to optimized table
		optimizedCalledFromStarTable[p].push_back(newChildren.at(i)); 
		if (!optimizedCalledFromTable[newChildren.at(i)].empty())
		{
			for (int j = 0; j < optimizedCalledFromTable[newChildren.at(i)].size(); j++) //for every child of each child procedure
			{
				//add each grandchild to list of children to interrogate next(WHOS UR CHILD?!
				currentChildren.push_back(optimizedCalledFromTable[newChildren.at(i)].at(j));
			}
		}
	}

	optimizeCalledFromStarTable(p, currentChildren, procsChecked);
}

vector<PROC> CallsTable::getCalledBy(PROC p)
{
	vector<PROC> answer;

	if (noProcs - 1 >= p)
		answer = optimizedCalledByTable[p];

	return answer;
}

vector<PROC> CallsTable::getCalledFrom(PROC p)
{
	vector<PROC> answer;

	if (noProcs - 1 >= p)
		answer = optimizedCalledFromTable[p];

	return answer;
}

vector<PROC> CallsTable::getCalledByStar(PROC p)
{
	vector<PROC> answer;

	if (noProcs - 1 >= p)
		answer = optimizedCalledByStarTable[p];

	return answer; 
}

vector<PROC> CallsTable::getCalledFromStar(PROC p)
{
	vector<PROC> answer;

	if (noProcs - 1 >= p)
		answer = optimizedCalledFromStarTable[p];

	return answer;
}

bool CallsTable::isCalled(PROC p1, PROC p2)
{
	if (noProcs - 1 >= p1 && noProcs - 1 >= p2)
	{
		for (int i = 0; i < optimizedCalledByTable[p1].size(); i++)
		{
			if(optimizedCalledByTable[p1].at(i) == p2)
				return true;
		}
	}

	return false;
}

bool CallsTable::isCalledStar(PROC p1, PROC p2)
{
	if (noProcs - 1 >= p1 && noProcs - 1 >= p2)
	{
		for (int i = 0; i < optimizedCalledByStarTable[p1].size(); i++)
		{
			if(optimizedCalledByStarTable[p1].at(i) == p2)
				return true;
		}
	}

	return false;
}

//////////////////////////////////
//Functions for Testing purposes//
//////////////////////////////////
void CallsTable::displayCallsTable()
{
	cout<<"ORIGINAL CALLSTABLE:"<<endl;
	cout<<"p1 p2"<<endl;
	for (int i = 0; i < callsTable.size(); i++)
		cout<<callsTable.at(i).first<<"   "<<callsTable.at(i).second<<endl;
}

void CallsTable::displayCalledByTable()
{
	cout<<"PROC that calls PROC"<<endl;
	for (int i = 0; i < noProcs; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedCalledByTable[i].size(); j++)
			cout<<optimizedCalledByTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void CallsTable::displayCalledByStarTable()
{
	cout<<"PROC that calls* PROC*"<<endl;
	for (int i = 0; i < noProcs; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedCalledByStarTable[i].size(); j++)
			cout<<optimizedCalledByStarTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void CallsTable::displayCalledFromTable()
{
	cout<<"PROC called by PROC"<<endl;
	for (int i = 0; i < noProcs; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedCalledFromTable[i].size(); j++)
			cout<<optimizedCalledFromTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void CallsTable::displayCalledFromStarTable()
{
	cout<<"PROC called* by PROC"<<endl;
	for (int i = 0; i < noProcs; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedCalledFromStarTable[i].size(); j++)
			cout<<optimizedCalledFromStarTable[i].at(j)<<" ";

		cout<<endl;
	}
}