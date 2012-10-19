#include "StdAfx.h"
#include "PROCTable.h"


PROCTable::PROCTable(void)
{
	
}


PROCTable::~PROCTable(void)
{
}

PROCIndex PROCTable::insertPROC(string PROCName)
{
	for(int i=0;i<procs.size();i++)
	{
		if(procs[i] == PROCName)
			return i;
	}
	procs.push_back(PROCName);
	return (procs.size() -1);
}

int PROCTable::getSize()
{
	return procs.size();
}

string PROCTable::getPROCName(PROCIndex i)
{
	return procs.at(i);
}

PROCIndex PROCTable::getPROCIndex(string PROCName)
{
	for(int i=0;i<procs.size();i++)
	{
		if(procs[i] == PROCName)
			return i;
	}
	return -1;
}