#pragma once
#include "AstNode.h"


typedef int Index;

class StmtNode : 
	public ASTNode
{
private:
	int stmtNumber;
	void setValue(Index value);
	void addChildToWhile(ASTNode* c, int);
	void addChildToIF(ASTNode* c, int);
	void addChildToAssign(ASTNode* c, int);

public:
	StmtNode(int stmtNo, NodeType nodeType, Index value);
	~StmtNode();
	int getStmtNumber();

	virtual ASTNode* addChild(ASTNode* c);
	virtual ASTNode* addChild(ASTNode* c, int childLoc);
};

