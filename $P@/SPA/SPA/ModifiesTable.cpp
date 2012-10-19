#include "StdAfx.h"
#include "ModifiesTable.h"

ModifiesTable::ModifiesTable()
{
}

void ModifiesTable::insertStmtModifies(STMTIndex s, VARIndex v)
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

void ModifiesTable::insertProcModifies(PROCIndex p, VARIndex v)
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

vector<VARIndex> ModifiesTable::ModifiedByStmt(STMTIndex s)
{
	vector<VARIndex> answer;

	if (optimizedModifiesStmtTable.size() >= s)
		answer = optimizedModifiesStmtTable.at(s);

	return answer;
}

vector<VARIndex> ModifiesTable::ModifiedByProc(PROCIndex p)
{
	vector<VARIndex> answer;

	if (optimizedModifiesProcTable.size() >= p)
		answer = optimizedModifiesProcTable.at(p);

	return answer;
}

vector<STMTIndex> ModifiesTable::getModifiesStmt(VARIndex v)
{
	vector<VARIndex> answer;

	if (optimizedModifiedByProcTable.size() >= v)
		answer = optimizedModifiedByProcTable.at(v);

	return answer;
}

vector<PROCIndex> ModifiesTable::getModifiesProc(VARIndex v)
{
	vector<VARIndex> answer;

	if (optimizedModifiedByStmtTable.size() >= v)
		answer = optimizedModifiedByStmtTable.at(v);

	return answer;
}

bool ModifiesTable::isModifiedStmt(STMTIndex s, VARIndex v)
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

bool ModifiesTable::isModifiedProc(PROCIndex p, VARIndex v)
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
