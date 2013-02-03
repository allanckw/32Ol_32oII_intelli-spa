#pragma once

#include "stdafx.h"
typedef int VARIndex;

class VARTable
{
public:
	VARTable(void);


	VARIndex insertVAR(string VARName);
	
	string getVARName(VARIndex);
	VARIndex getVARIndex(string VARName);

	int getSize();

	bool isExists(VARIndex);

	
private:
	vector<string> vars;
};
