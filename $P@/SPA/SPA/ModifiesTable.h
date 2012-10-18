#pragma once
#include "StdAfx.h"
typedef int PROCIndex;
typedef int STMTIndex;
typedef int VARIndex;

class ModifiesTable
{
private:
	vector<pair <STMTIndex, VARIndex>> modifiesStmtTable; //Table of (stmt, var modified by stmt)
	vector<pair <PROCIndex, VARIndex>> modifiesProcTable; //Table of (proc, var modified by proc)
	//Hash Tables
	vector<vector<VARIndex>> optimizedModifiesStmtTable;
	vector<vector<VARIndex>> optimizedModifiesProcTable;
	vector<vector<STMTIndex>> optimizedModifiedByStmtTable;
	vector<vector<PROCIndex>> optimizedModifiedByProcTable;

public:
	ModifiesTable();

	void insertStmtModifies(STMTIndex, VARIndex);
	void insertProcModifies(PROCIndex, VARIndex);
	void optimizeModifiesTables();
	vector<VARIndex> ModifiedByStmt(STMTIndex);
	vector<VARIndex> ModifiedByProc(PROCIndex);
	vector<STMTIndex> getModifiesStmt(VARIndex);
	vector<PROCIndex> getModifiesProc(VARIndex);
	bool isModifiedStmt(STMTIndex, VARIndex);
	bool isModifiedProc(PROCIndex, VARIndex);
}