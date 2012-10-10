#include "stdafx.h"
#include "ASTEntity.h"

	
	Program::Program(string name)
	{
		this->name = name;
		vector<Procedure*> p;
		this->Procedures =p;
		//p.push_back(&Procedure("1",0));
	}

	string Program::GetName()
	{
		return name;
	}

	vector<Procedure*>* Program::GetProcedure()
	{
		return &Procedures;
	}

	
	Procedure::Procedure(string name,int line) : Stmt(line) 
	{
		this->name = name;
	}

	string Procedure::GetName()
	{
		return name;
	}

	vector<Stmt>* Procedure::GetStmt()
	{
		return &_stmtLst;;
	}


	Stmt::Stmt(int LineNumber)
	{
		this->LineNumber = LineNumber;
		//this->stmt = new list<Stmt>();
	}

	list<Stmt> Stmt::GetList()
	{
		return stmt;
	}

	int Stmt::GetLineNumber()
	{
		return LineNumber;
	}


	Call::Call(int LineNumber, Procedure pro) : Stmt(LineNumber), procedure(pro)
	{
		
	}


	/*










*/

//#include "stdafx.h"
//
//
//class Program
//{
//public:
//	Program(string name)
//	{
//		this->name = name;
//	}
//
//	string GetName()
//	{
//		return name;
//	}
//
//	list<Procedure> GetProcedure()
//	{
//		return Procedures;
//	}
//
//
//private:
//	string name;
//	list<Procedure> Procedures;
//};
//
//class Procedure
//{
//public:
//	Procedure(string name)
//	{
//		this->name = name;
//		
//	}
//
//	string GetName()
//	{
//		return name;
//	}
//
//	Stmt GetStmt()
//	{
//		return stmt;
//	}
//public:
//	string name;
//	Stmt stmt;
//};
//
//class Stmt
//{
//public:
//	Stmt(int LineNumber)
//	{
//		this->LineNumber = LineNumber;
//		//this->stmt = new list<Stmt>();
//
//	}
//
//	list<Stmt> GetList()
//	{
//		return stmt;
//	}
//
//	int GetLineNumber()
//	{
//		return LineNumber;
//	}
//private:
//	int LineNumber;
//	list<Stmt> stmt;
//};
//
//class Call : public Stmt
//{
//public:
//	Call(int LineNumber, Procedure *procedure) : Stmt(LineNumber)
//	{
//		this->procedure = procedure;
//	}
//private:
//	Procedure *procedure;
//};











//#include "stdafx.h"
//
//
//class Program
//{
//public:
//	Program(string name)
//	{
//		this->name = name;
//	}
//
//	string GetName()
//	{
//		return name;
//	}
//
//	list<Procedure> GetProcedure()
//	{
//		return Procedures;
//	}
//
//
//private:
//	string name;
//	list<Procedure> Procedures;
//};
//
//class Procedure
//{
//public:
//	Procedure(string name)
//	{
//		this->name = name;
//		
//	}
//
//	string GetName()
//	{
//		return name;
//	}
//
//	Stmt GetStmt()
//	{
//		return stmt;
//	}
//public:
//	string name;
//	Stmt stmt;
//};
//
//class Stmt
//{
//public:
//	Stmt(int LineNumber)
//	{
//		this->LineNumber = LineNumber;
//		//this->stmt = new list<Stmt>();
//
//	}
//
//	list<Stmt> GetList()
//	{
//		return stmt;
//	}
//
//	int GetLineNumber()
//	{
//		return LineNumber;
//	}
//private:
//	int LineNumber;
//	list<Stmt> stmt;
//};
//
//class Call : public Stmt
//{
//public:
//	Call(int LineNumber, Procedure *procedure) : Stmt(LineNumber)
//	{
//		this->procedure = procedure;
//	}
//private:
//	Procedure *procedure;
//};