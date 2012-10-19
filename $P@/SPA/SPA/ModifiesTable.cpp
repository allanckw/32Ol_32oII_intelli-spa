#include "StdAfx.h"
#include "ModifiesTable.h"

ModifiesTable::ModifiesTable()
{
}

void ModifiesTable::insertStmtModifies(STMT s, VAR v)
{
	auto newPair = make_pair(s, v);
	if (modifiesStmtTable.size() != 0)
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
	if (modifiesProcTable.size() != 0)
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
void ModifiesTable::optimizeModifiesTables()
{
	if (!modifiesStmtTable.empty())
	{
		for (int i = 0; i < modifiesStmtTable.size(); i++)
		{
			optimizedModifiesStmtTable.at(modifiesStmtTable.at(i).first).push_back(modifiesStmtTable.at(i).second);
			optimizedModifiedByStmtTable.at(modifiesStmtTable.at(i).second).push_back(modifiesStmtTable.at(i).first);
		}
	}
	if (!modifiesProcTable.empty())
	{
		for (int j = 0; j < modifiesProcTable.size(); j++)
		{
			optimizedModifiesProcTable.at(modifiesProcTable.at(j).first).push_back(modifiesProcTable.at(j).second);
			optimizedModifiedByProcTable.at(modifiesProcTable.at(j).second).push_back(modifiesProcTable.at(j).first);
		}
	}

	return;
}

vector<VAR> ModifiesTable::ModifiedByStmt(STMT s)
{
	vector<VAR> answer;

	if (optimizedModifiesStmtTable.size() >= s)
		answer = optimizedModifiesStmtTable.at(s);

	return answer;
}

vector<VAR> ModifiesTable::ModifiedByProc(PROC p)
{
	vector<VAR> answer;

	if (optimizedModifiesProcTable.size() >= p)
		answer = optimizedModifiesProcTable.at(p);

	return answer;
}

vector<STMT> ModifiesTable::getModifiesStmt(VAR v)
{
	vector<VAR> answer;

	if (optimizedModifiedByProcTable.size() >= v)
		answer = optimizedModifiedByProcTable.at(v);

	return answer;
}

vector<PROC> ModifiesTable::getModifiesProc(VAR v)
{
	vector<VAR> answer;

	if (optimizedModifiedByStmtTable.size() >= v)
		answer = optimizedModifiedByStmtTable.at(v);

	return answer;
}

bool ModifiesTable::isModifiedStmt(STMT s, VAR v)
{
	if (optimizedModifiesStmtTable.size() >= s)
	{
		for (int i = 0; i < optimizedModifiesStmtTable.at(s).size(); i++)
		{
			if(optimizedModifiesStmtTable.at(s).at(i) == v)
				return true;
		}
	}

	return false;
}

bool ModifiesTable::isModifiedProc(PROC p, VAR v)
{
	if (optimizedModifiesProcTable.size() >= p)
	{
		for (int i = 0; i < optimizedModifiesProcTable.at(p).size(); i++)
		{
			if(optimizedModifiesProcTable.at(p).at(i) == v)
				return true;
		}
	}

	return false;
}
