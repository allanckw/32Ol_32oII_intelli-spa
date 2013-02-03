#pragma once
#include "StdAfx.h"
#include "PROCTable.h"
#include "SPAException.h"

PROCTable::PROCTable(void)
{
}


/**
* This method will be used to insert procedure name into the procedure table
* @param PROCName the Procedure name being inserted
* @return The index of the procedure in the proc table
*/
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

/**
* This method will be used to get the number of procedure in the procedure table 
* @return The size of the  proc table
*/
int PROCTable::getSize()
{
	return procs.size();
}

/**
* This method will be used to get the procedure name via the index of the procedure table
* @param i the index of the procedure table being requested
* @return the actual string of the variable at index i
*/
string PROCTable::getPROCName(PROCIndex i)
{
	if (i > procs.size() -1 ){
		throw SPAException("Invalid Index, there is no procedure at given location ");
	}
	else{
		return procs.at(i);
	}
}

/**
* This method will be used to get the index of the procedure name in the procedure table
* @param PROCName the proecure name being requested for
* @return The index of the procedure in the proc table
*/
PROCIndex PROCTable::getPROCIndex(string PROCName)
{
	for(int i=0;i<procs.size();i++)
	{
		if(procs[i] == PROCName)
			return i;
	}
	return -1;
}

/**
* This method will be used to check wheather an index is a valid index for the procedure table
* @param i the index being tested for
* @return true if the procedure index exists, false otherwise
*/
bool PROCTable::isExists(PROCIndex i)
{
	return (i >= 0 && i < procs.size());
}