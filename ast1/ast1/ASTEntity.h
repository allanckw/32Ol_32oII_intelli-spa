
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <list>

using namespace std;

class Stmt
{
public:
	Stmt(int LineNumber);
	list<Stmt> GetList();
	int GetLineNumber();
private:
	int LineNumber;
	list<Stmt> stmt;
};

class Procedure: public Stmt
{
public:
	Procedure(string name,int line);
	string GetName();
	vector<Stmt>* GetStmt();
	
private:
	string name;
	vector<Stmt> _stmtLst;
};

class Program
{
public:
	Program(string name);	
	string GetName();
	vector<Procedure*>* GetProcedure();
	

private:
	string name;
	vector<Procedure*> Procedures;
};





class Call : public Stmt
{
public:
	Call(int LineNumber, Procedure pro);
private:
	Procedure& procedure;
};