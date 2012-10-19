#pragma once
typedef int PROCIndex;

class PROCTable
{
public:
	PROCTable(void);
	~PROCTable(void);
	PROCIndex insertPROC(string PROCName);
	
	string getPROCName(PROCIndex);
	PROCIndex getPROCIndex(string PROCName);

	int getSize();
	

private:
	vector<string> procs;
};

