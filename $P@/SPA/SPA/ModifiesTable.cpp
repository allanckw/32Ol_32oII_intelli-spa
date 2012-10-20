#include "StdAfx.h"
#include "PKB.h"
#include "ModifiesTable.h"

ModifiesTable::ModifiesTable()
{
	noProgLines = 5;
	noProcs = 5;
	noVars = 5;

	/*
	noProgLines = PKB::maxProgLines;
	noProcs = PKB::procedures.getSize();
	noVars = PKB::variables.getSize();
	*/
}

void ModifiesTable::insertStmtModifies(STMT s, VAR v)
{
	if (s == 0)	{
		throw new SPAException("Invalid Reference: There is no statement zero!");
	}
	auto newPair = make_pair(s, v);
	if (!modifiesStmtTable.empty())
	{
		for (int i = 0; i < modifiesStmtTable.size(); i++)
		{
			if (modifiesStmtTable.at(i) == newPair) //if pair already exists in table, do nothing
				return;
		}
	}

	modifiesStmtTable.push_back(newPair);
}

void ModifiesTable::insertProcModifies(PROC p, VAR v)
{
	auto newPair = make_pair(p, v);
	if (!modifiesProcTable.empty())
	{
		for (int i = 0; i < modifiesProcTable.size(); i++)
		{
			if (modifiesProcTable.at(i) == newPair) //if pair already exists in table, do nothing
				return;
		}
	}

	modifiesProcTable.push_back(newPair);
}

//This function should be invoked once modifiestable has been fully populated by whoever is populating it
void ModifiesTable::optimizeModifiesTable()
{
	optimizedModifiesStmtTable = new vector<VAR>[noProgLines];
	optimizedModifiedByStmtTable = new vector<STMT>[noVars];
	optimizedModifiesProcTable = new vector<VAR>[noProcs];
	optimizedModifiedByProcTable = new vector<PROC>[noVars];

	if (!modifiesStmtTable.empty())
	{
		for (int i = 0; i < modifiesStmtTable.size(); i++)
		{
			optimizedModifiedByStmtTable[modifiesStmtTable.at(i).first].push_back(modifiesStmtTable.at(i).second);
			optimizedModifiesStmtTable[modifiesStmtTable.at(i).second].push_back(modifiesStmtTable.at(i).first);
		}
	}
	if (!modifiesProcTable.empty())
	{
		for (int j = 0; j < modifiesProcTable.size(); j++)
		{
			optimizedModifiedByProcTable[modifiesProcTable.at(j).first].push_back(modifiesProcTable.at(j).second);
			optimizedModifiesProcTable[modifiesProcTable.at(j).second].push_back(modifiesProcTable.at(j).first);
		}
	}

	return;
}

vector<VAR> ModifiesTable::getModifiedByStmt(STMT s)
{
	vector<VAR> answer;

	if (noProgLines - 1 >= s)
		answer = optimizedModifiedByStmtTable[s];

	return answer;
}

vector<VAR> ModifiesTable::getModifiedByProc(PROC p)
{
	vector<VAR> answer;

	if (noProcs - 1 >= p)
		answer = optimizedModifiedByProcTable[p];

	return answer;
}

vector<STMT> ModifiesTable::getModifiesStmt(VAR v)
{
	vector<STMT> answer;

	if (noVars - 1 >= v)
		answer = optimizedModifiesStmtTable[v];

	return answer;
}

vector<PROC> ModifiesTable::getModifiesProc(VAR v)
{
	vector<PROC> answer;

	if (noVars - 1 >= v)
		answer = optimizedModifiesProcTable[v];

	return answer;
}

bool ModifiesTable::isModifiedStmt(STMT s, VAR v)
{
	if (noProgLines - 1 >= s && noVars - 1 > v)
	{
		for (int i = 0; i < optimizedModifiesStmtTable[s].size(); i++)
		{
			if(optimizedModifiesStmtTable[s].at(i) == v)
				return true;
		}
	}

	return false;
}

bool ModifiesTable::isModifiedProc(PROC p, VAR v)
{
	if (noProcs - 1 >= p && noVars - 1 >= v)
	{
		for (int i = 0; i < optimizedModifiesProcTable[p].size(); i++)
		{
			if(optimizedModifiesProcTable[p].at(i) == v)
				return true;
		}
	}

	return false;
}

//////////////////////////////////
//Functions for testing purposes//
//////////////////////////////////
void ModifiesTable::displayModifiedByProcTable()
{
	cout<<"PROC that modify VAR"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedModifiedByProcTable[i].size(); j++)
			cout<<optimizedModifiedByProcTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void ModifiesTable::displayModifiedByStmtTable()
{
	cout<<"STMT that modify VAR"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedModifiedByStmtTable[i].size(); j++)
			cout<<optimizedModifiedByStmtTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void ModifiesTable::displayModifiesProcTable()
{
	cout<<"VAR modified by PROC"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedModifiesProcTable[i].size(); j++)
			cout<<optimizedModifiesProcTable[i].at(j)<<" ";

		cout<<endl;
	}
}

void ModifiesTable::displayModifiesStmtTable()
{
	cout<<"VAR modified by STMT"<<endl;
	for (int i = 0; i < noVars; i++)
	{
		cout<<i<<": ";
		for (int j = 0; j < optimizedModifiesStmtTable[i].size(); j++)
			cout<<optimizedModifiesStmtTable[i].at(j)<<" ";

		cout<<endl;
	}
}