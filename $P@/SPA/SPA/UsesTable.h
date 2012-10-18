#pragma once
#include "StdAfx.h"
typedef int PROCIndex;
typedef int STMTIndex;
typedef int VARIndex;

class UsesTable
{
private:
	vector<pair <STMTIndex, VARIndex>> usesStmtTable; //Table of (stmt, var used by stmt)
	vector<pair <PROCIndex, VARIndex>> usesProcTable; //Table of (proc, var used by proc)
	//Hash Tables
	vector<vector<VARIndex>> optimizedUsedByStmtTable;
	vector<vector<VARIndex>> optimizedUsedByProcTable;
	vector<vector<STMTIndex>> optimizedUsedInStmtTable;
	vector<vector<PROCIndex>> optimizedUsedInProcTable;

public:
	UsesTable();

	void insertStmtUses(STMTIndex, VARIndex);
	void insertProcUses(PROCIndex, VARIndex);
	void optimizeUsesTables();
	vector<VARIndex> getUsedByStmt(STMTIndex);
	vector<VARIndex> getUsedByProc(PROCIndex);
	vector<STMTIndex> getUsedInStmt(VARIndex);
	vector<PROCIndex> getUsedInProc(VARIndex);
	bool isUsedStmt(STMTIndex, VARIndex);
	bool isUsedProc(PROCIndex, VARIndex);
}