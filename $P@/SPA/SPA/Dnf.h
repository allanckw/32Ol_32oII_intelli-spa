#pragma once
#include "stdAfx.h"
#include "FormNode.h"
#include "Helper.h"
#include "Parser.h"
#include "PQLController.h"
#include <sstream>

class Dnf
{
public:
	Dnf(void);
	~Dnf(void);

	static FormNode* Convert(FormNode* c);
	static bool isOperator(std::string token);
	static FormNode* processAssignment(std::vector<std::string> expr);
	static std::string Trim(std::string str);
	static int find_closer(std::vector<std::string> l, int cur);
	static bool isQuery(std::string str);
	static bool iscon(std::string str);
	static std::vector<std::string> tokenizer(std::string line);
	static char easytolower(char in);
	static int find_closer(std::string d);
	static bool isDNF(FormNode* n);

	static std::vector<pair<std::string,std::string>>*  CreateDNF(std::string str);

	static void Eval(std::string query, list<string>& results);
	static string getToken(const string& query, int& pos);

	static bool compare(string s1,string s2);

	

};

