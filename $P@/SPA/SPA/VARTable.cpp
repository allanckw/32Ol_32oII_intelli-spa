#include "StdAfx.h"
#include "VARTable.h"



VARTable::VARTable(void)
{
}


VARTable::~VARTable(void)
{
}

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

int VARTable::getSize()
{
	return vars.size();
}

string VARTable::getVARName(VARIndex i)
{
	return vars.at(i);
}

VARIndex VARTable::getVARIndex(string VARName)
{
	for(int i=0;i<vars.size();i++)
	{
		if(vars[i] == VARName)
			return i;
	}
	
	return -1;
}
