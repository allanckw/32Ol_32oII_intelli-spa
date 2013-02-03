#pragma once
typedef int PROCIndex;

class PROCTable
{
public:
	PROCTable(void);

	PROCIndex insertPROC(string PROCName);
	
	string getPROCName(PROCIndex);
	PROCIndex getPROCIndex(string PROCName);

	bool isExists(PROCIndex i);

	int getSize();
	

private:
	vector<string> procs;
};

