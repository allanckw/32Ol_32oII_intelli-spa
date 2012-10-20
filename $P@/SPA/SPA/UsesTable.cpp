#include "StdAfx.h"
#include "PKB.h"
#include "UsesTable.h"

UsesTable::UsesTable()
{
	noProgLines = PKB::maxProgLines;
	noProcs = PKB::procedures.getSize();
	noVars = PKB::variables.getSize();
}

void UsesTable::insertStmtUses(STMT s, VAR v)
{
	auto newPair = make_pair(s, v);
	if (!usesStmtTable.empty())
	{
		for (int i = 0; i < usesStmtTable.size(); i++)
		{
			if (usesStmtTable.at(i) == newPair) //if pair already exists in table, do nothing
				return;
		}
	}

	usesStmtTable.push_back(newPair);
}

void UsesTable::insertProcUses(PROC p, VAR v)
{
	auto newPair = make_pair(p, v);
	if (!usesProcTable.empty())
	{
		for (int i = 0; i < usesProcTable.size(); i++)
		{
			if (usesProcTable.at(i) == newPair) //if pair already exists in table, do nothing
				return;
		}
	}

	usesProcTable.push_back(newPair);
}

//This function should be invoked once usestable has been fully populated by whoever is populating it
void UsesTable::optimizeUsesTables()
{
	optimizedUsedByStmtTable = new vector<VAR>[noProgLines];
	optimizedUsedInStmtTable = new vector<STMT>[noVars];
	optimizedUsedByProcTable = new vector<VAR>[noProcs];
	optimizedUsedInProcTable = new vector<PROC>[noVars];

	if (!usesStmtTable.empty())
	{
		for (int i = 0; i < usesStmtTable.size(); i++)
		{
			optimizedUsedInStmtTable[usesStmtTable.at(i).first].push_back(usesStmtTable.at(i).second);
			optimizedUsedByStmtTable[usesStmtTable.at(i).second].push_back(usesStmtTable.at(i).first);
		}
	}
	if (!usesProcTable.empty())
	{
		for (int j = 0; j < usesProcTable.size(); j++)
		{
			optimizedUsedInProcTable[usesProcTable.at(j).first].push_back(usesProcTable.at(j).second);
			optimizedUsedByProcTable[usesProcTable.at(j).second].push_back(usesProcTable.at(j).first);
		}
	}

	return;
}

vector<VAR> UsesTable::getUsedInStmt(STMT s)
{
	vector<VAR> answer;

	if (noProgLines - 1 >= s)
		answer = optimizedUsedInStmtTable[s];

	return answer;
}

vector<VAR> UsesTable::getUsedInProc(PROC p)
{
	vector<VAR> answer;

	if (noProcs - 1 >= p)
		answer = optimizedUsedInProcTable[p];

	return answer;
}

vector<STMT> UsesTable::getUsedByStmt(VAR v)
{
	vector<STMT> answer;

	if (noVars - 1 >= v)
		answer = optimizedUsedByStmtTable[v];

	return answer;
}

vector<PROC> UsesTable::getUsedByProc(VAR v)
{
	vector<PROC> answer;

	if (noVars - 1 >= v)
		answer = optimizedUsedByProcTable[v];

	return answer;
}

bool UsesTable::isUsedStmt(STMT s, VAR v)
{
	if (noProgLines - 1 >= s && noVars - 1 > v)
	{
		for (int i = 0; i < optimizedUsedByStmtTable[s].size(); i++)
		{
			if(optimizedUsedByStmtTable[s].at(i) == v)
				return true;
		}
	}

	return false;
}

bool UsesTable::isUsedProc(PROC p, VAR v)
{
	if (noProcs - 1 >= p && noVars - 1 >= v)
	{
		for (int i = 0; i < optimizedUsedByProcTable[p].size(); i++)
		{
			if(optimizedUsedByProcTable[p].at(i) == v)
				return true;
		}
	}

	return false;
}

//////////////////////////////////
//Functions for testing purposes//
//////////////////////////////////
void UsesTable::displayUsedInProcTable()
{
	cout<<"PROC VAR"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		for (int j = 0; j < optimizedUsedInProcTable[i].size(); j++)
			cout<<optimizedUsedInProcTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void UsesTable::displayUsedInStmtTable()
{
	cout<<"STMT VAR"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		for (int j = 0; j < optimizedUsedInStmtTable[i].size(); j++)
			cout<<optimizedUsedInStmtTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void UsesTable::displayUsedByProcTable()
{
	cout<<"VAR PROC"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		for (int j = 0; j < optimizedUsedByProcTable[i].size(); j++)
			cout<<optimizedUsedByProcTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void UsesTable::displayUsedByStmtTable()
{
	cout<<"VAR STMT"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		for (int j = 0; j < optimizedUsedByProcTable[i].size(); j++)
			cout<<optimizedUsedByProcTable[i].at(j)<<" ";

		cout<<endl;
	}
}