#pragma once
#include "AstNode.h"

typedef string Expr;

class ExprNode :
	public ASTNode
{
public:
	ExprNode(NodeType, int);
	ExprNode(NodeType, Expr);

	~ExprNode(void);
	bool isMatched(Expr);

	virtual ASTNode* AddChild(ASTNode* c, int childLoc);

private:
	bool isVARMatched(string);
	bool isCONSTMatched(int);
	bool isOPRMatched(string);

	string getOperator();
	void setOperator(string);
};

