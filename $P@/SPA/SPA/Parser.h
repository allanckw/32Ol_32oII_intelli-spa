#pragma once
#include "StmtNode.h"
#include "ASTNode.h"

class Parser
{
public:
	static vector<vector<string>> tokenized_codes;

	Parser(vector<string> codings);
	~Parser(void);

	//Build AST Methods - JK
	void buildAST();
private:
	bool IsEmpty(string str);
	
	bool is_number(const std::string& str);
	void AddTables(vector<string> list, string newtoken);
	vector<string> tokenizer(string line);
	
	//Build AST Methods - JK
	bool Parser::IsName(string s);
	ASTNode* processProcedure(int *i);
	StmtNode* processWhile(int *i, Index procIdx);
	StmtNode* processCall(int *i, Index procIdx);
};

