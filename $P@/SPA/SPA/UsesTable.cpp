#include "StdAfx.h"
#include "UsesTable.h"

UsesTable::UsesTable()
{
}

void UsesTable::insertStmtUses(STMT s, VAR v)
{
	auto newPair = make_pair(s, v);
	if (usesStmtTable.size() != 0)
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
	if (usesProcTable.size() != 0)
	{
		for (int i = 0; i < usesProcTable.size(); i++)
		{
			if (usesProcTable.at(i) == newPair) //if pair already exists in table, do nothing
				return;
		}
	}

	usesProcTable.push_back(newPair);
}

//This function should be invoked once modifiestable has been fully populated by whoever is populating it
void UsesTable::optimizeUsesTables()
{
	if (!usesStmtTable.empty())
	{
		for (int i = 0; i < usesStmtTable.size(); i++)
		{
			optimizedUsedByStmtTable.at(usesStmtTable.at(i).first).push_back(usesStmtTable.at(i).second);
			optimizedUsedInStmtTable.at(usesStmtTable.at(i).second).push_back(usesStmtTable.at(i).first);
		}
	}
	if (!usesProcTable.empty())
	{
		for (int j = 0; j < usesProcTable.size(); j++)
		{
			optimizedUsedByProcTable.at(usesProcTable.at(j).first).push_back(usesProcTable.at(j).second);
			optimizedUsedInProcTable.at(usesProcTable.at(j).second).push_back(usesProcTable.at(j).first);
		}
	}

	return;
}

vector<VAR> UsesTable::getUsedByStmt(STMT s)
{
	vector<VAR> answer;

	if (optimizedUsedByStmtTable.size() >= s)
		answer = optimizedUsedByStmtTable.at(s);

	return answer;
}

vector<VAR> UsesTable::getUsedByProc(PROC p)
{
	vector<VAR> answer;

	if (optimizedUsedByProcTable.size() >= p)
		answer = optimizedUsedByProcTable.at(p);

	return answer;
}

vector<STMT> UsesTable::getUsedInStmt(VAR v)
{
	vector<VAR> answer;

	if (optimizedUsedInStmtTable.size() >= v)
		answer = optimizedUsedInStmtTable.at(v);

	return answer;
}

vector<PROC> UsesTable::getUsedInProc(VAR v)
{
	vector<VAR> answer;

	if (optimizedUsedInProcTable.size() >= v)
		answer = optimizedUsedInProcTable.at(v);

	return answer;
}

bool UsesTable::isUsedStmt(STMT s, VAR v)
{
	if (optimizedUsedByStmtTable.size() >= s)
	{
		for (int i = 0; i < optimizedUsedByStmtTable.at(s).size(); i++)
		{
			if(optimizedUsedByStmtTable.at(s).at(i) == v)
				return true;
		}
	}

	return false;
}

bool UsesTable::isUsedProc(PROC p, VAR v)
{
	if (optimizedUsedInStmtTable.size() >= p)
	{
		for (int i = 0; i < optimizedUsedInStmtTable.at(p).size(); i++)
		{
			if(optimizedUsedInStmtTable.at(p).at(i) == v)
				return true;
		}
	}

	return false;
}
