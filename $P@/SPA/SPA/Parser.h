#pragma once
#include "ASTStmtNode.h"
#include "ASTNode.h"

class Parser
{
public:
	static vector<vector<string>> tokenized_codes;

	Parser(string);


	//Build AST Methods - JK
	void buildAST();
private:
	bool isEmpty(string str);
	
	bool is_number(const std::string& str);
	void AddTables(vector<string> list, string newtoken);
	void tokenizer(string line);
	string Trim(string str);
	void AddToList(vector<string>& list, string str);
	
	//Build AST Methods - JK
	bool Parser::isName(string s);
	ASTNode* processProcedure(int *i, int *j);
	ASTStmtNode* processWhile(int *i, int *j, Index procIdx);
	ASTStmtNode* processIf(int *i, int *j, Index procIdx);
	ASTStmtNode* processCall(int *i, int *j, Index procIdx);
	ASTStmtNode* processAssignment(int *i, int *j);
	bool isStrCheckNoSpecialChar(string);
};

