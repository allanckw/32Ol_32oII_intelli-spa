#pragma once
#include "AstNode.h"

typedef string Expr;

class ASTExprNode :
	public ASTNode
{
public:
	ASTExprNode(NodeType, int);
	ASTExprNode(NodeType, Expr);

	bool isMatched(Expr);

	virtual ASTNode* addChild(ASTNode* c, int childLoc);

private:
	bool isVARMatched(string);
	bool isCONSTMatched(int);
	bool isOPRMatched(string);

	string getOperator();
	void setOperator(string);
};

