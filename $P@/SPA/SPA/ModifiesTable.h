#pragma once
#include "StdAfx.h"
typedef int PROCIndex;
typedef int STMTIndex;
typedef int VARIndex;

class ModifiesTable
{
private:
	vector<pair <int, int>> modifiesStmt; //Table of (stmt, var modified by stmt)
	vector<pair <int, int>> modifiesProc; //Table of (proc, var modified by proc)

public:
	ModifiesTable();

	void insertStmtModifies(STMTIndex, VARIndex);
	void insertProcModifies(PROCIndex, VARIndex);
	vector<VARIndex> ModifiedByProc(PROCIndex);
	vector<VARIndex> ModifiedByStmt(STMTIndex);
	vector<STMTIndex> getModifiesStmt(VARIndex);
	vector<PROCIndex> getModifiesProc(VARIndex);
	bool isModifiedStmt(STMTIndex, VARIndex);
	bool isModifiedProc(PROCIndex, VARIndex);
}