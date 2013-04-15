#pragma once
#include "stdAfx.h"
#include "FormNode.h"
#include "Helper.h"
#include "Parser.h"
#include "PQLController.h"
#include "MultiQueryEval.h"
#include <sstream>

class Dnf
{
public:
	static FormNode* Convert(FormNode* c);
	static bool isOperator(std::string token);
	static FormNode* processAssignment(std::vector<std::string> expr);
	static std::string Trim(std::string str);
	static int find_closer(std::vector<std::string> l, int cur);
	static bool isQuery(std::string str);
	static bool iscon(std::string str);
	static std::vector<std::string> tokenizer(std::string line);
	static char easytolower(char in);

	int static compareOprPrecedence(string opr1, string opr2);  
	static int find_closer(std::string d);
	static bool isDNF(FormNode* n);
	int static getOperatorWeight(string token);
	static vector<pair<string,pair<string,vector<string>>>>*  CreateDNF(std::string str);

	static void Eval(std::string query, list<string>& results);
	static string getToken(const string& query, int& pos);

	static bool compare(string s1,string s2);

	static bool isVarType(string str);

	

};

