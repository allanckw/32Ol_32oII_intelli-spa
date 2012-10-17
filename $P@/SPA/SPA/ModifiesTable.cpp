#include "StdAfx.h"
#include "ModifiesTable.h"

ModifiesTable::ModifiesTable()
{
}

void ModifiesTable::insertStmtModifies(STMTIndex s, VARIndex v)
{
	auto newPair = make_pair(s, v);
	if (modifiesStmt.size() != 0)
	{
		for (int i = 0; i < modifiesStmt.size(); i++)
		{
			if (modifiesStmt.at(i) == newPair) //if pair already exists in table, do nothing
				return;
		}
	}

	modifiesStmt.push_back(newPair);
}

void ModifiesTable::insertProcModifies(PROCIndex p, VARIndex v)
{
	auto newPair = make_pair(p, v);
	if (modifiesProc.size() != 0)
	{
		for (int i = 0; i < modifiesProc.size(); i++)
		{
			if (modifiesProc.at(i) == newPair) //if pair already exists in table, do nothing
				return;
		}
	}

	modifiesProc.push_back(newPair);
}

vector<VARIndex> ModifiesTable::ModifiedByProc(PROCIndex p)
{
	vector<VARIndex> answer;
	if (modifiesProc.size() != 0)
	{
		for (int i = 0; i < modifiesProc.size(); i++)
		{
			if (modifiesProc.at(i).first == p)
				answer.push_back(modifiesProc.at(i).second);
		}
	}

	return answer;
}

vector<VARIndex> ModifiesTable::ModifiedByStmt(STMTIndex s)
{
	vector<VARIndex> answer;
	if (modifiesStmt.size() != 0)
	{
		for (int i = 0; i < modifiesStmt.size(); i++)
		{
			if (modifiesStmt.at(i).first == s)
				answer.push_back(modifiesStmt.at(i).second);
		}
	}

	return answer;
}

vector<STMTIndex> ModifiesTable::getModifiesStmt(VARIndex v)
{
	vector<STMTIndex> answer;
	if (modifiesStmt.size() != 0)
	{
		for (int i = 0; i < modifiesStmt.size(); i++)
		{
			if (modifiesStmt.at(i).second == v)
				answer.push_back(modifiesStmt.at(i).first);
		}
	}

	return answer;
}

vector<PROCIndex> ModifiesTable::getModifiesProc(VARIndex v)
{
	vector<PROCIndex> answer;
	if (modifiesProc.size() != 0)
	{
		for (int i = 0; i < modifiesProc.size(); i++)
		{
			if (modifiesProc.at(i).second == v)
				answer.push_back(modifiesProc.at(i).first);
		}
	}

	return answer;
}

bool ModifiesTable::isModifiedStmt(STMTIndex s, VARIndex v)
{
	if (modifiesStmt.size() != 0)
	{
		for (int i = 0; i < modifiesStmt.size(); i++)
		{
			if (modifiesStmt.at(i).first == s)
				if (modifiesStmt.at(i).second == v)
					return true;
		}
	}

	return false;
}

bool ModifiesTable::isModifiedProc(PROCIndex p, VARIndex v)
{
	if (modifiesProc.size() != 0)
	{
		for (int i = 0; i < modifiesProc.size(); i++)
		{
			if (modifiesProc.at(i).first == p)
				if (modifiesProc.at(i).second == v)
					return true;
		}
	}

	return false;
}
