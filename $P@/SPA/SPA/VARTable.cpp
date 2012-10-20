#include "StdAfx.h"
#include "VARTable.h"
#include "SPAException.h"


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
	if (i > vars.size() - 1){
		throw SPAException("Invalid Index, there is no variable at given location ");
	}
	else{
		return vars.at(i);
	}
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
