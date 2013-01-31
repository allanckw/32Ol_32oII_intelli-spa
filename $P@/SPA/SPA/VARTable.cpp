#pragma once
#include "StdAfx.h"
#include "VARTable.h"
#include "SPAException.h"


VARTable::VARTable(void)
{
}


VARTable::~VARTable(void)
{
}

/**
* This method will be use to add variable into the variable table
* @param VARName the variable being added
*/
VARIndex VARTable::insertVAR(string VARName)
{
	for(int i=0;i<vars.size();i++)
	{
		if(vars[i] == VARName)
			return i;
	}
	vars.push_back(VARName);
	return (vars.size() -1);
}

/**
* This method will be used to get the amount of variable in the variable table
*/
int VARTable::getSize()
{
	return vars.size();
}

/**
* This method will be used to return the string of the variable at index i in the variable table
* @param i the index of the variable being requested for
*/
string VARTable::getVARName(VARIndex i)
{
	if (i > vars.size() - 1){
		throw SPAException("Invalid Index, there is no variable at given location ");
	}
	else{
		return vars.at(i);
	}
}

/**
* This method will be used return the index of the VARName variable in the variable table
* @param VARName the name of the variable being requested for
*/
VARIndex VARTable::getVARIndex(string VARName)
{
	for(int i=0;i<vars.size();i++)
	{
		if(vars[i] == VARName)
			return i;
	}
	
	return -1;
}

/**
* This method will be used to check if a index is a valid variable table index
* @param i the index of the variable table which is being checked
*/
bool VARTable::isExists(VARIndex i)
{
	return (i >= 0 && i < vars.size());
}
