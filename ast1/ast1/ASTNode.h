#pragma once
#include "StdAfx.h"

enum NodeType { 
	Program, 
	Procedure, 
	StmtLst,
	Assign,
	Call,
	Operator,
	Variable,
	Constant,
	While,
	If
};

class ASTNode
{
private:
	NodeType nType;
	string name;

	ASTNode* parent;
	vector<ASTNode*> children;
	ASTNode* rightSibling;
	bool isRoot;

public:
	ASTNode(void);
	ASTNode(NodeType type, string name);
	ASTNode* SetParent(ASTNode * p);
	ASTNode* SetRightSibling(ASTNode * s);
	ASTNode* AddChild(ASTNode* c);
	void SetRoot();
	~ASTNode(void);

};

