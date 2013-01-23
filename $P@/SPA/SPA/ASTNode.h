#pragma once
#include "StdAfx.h"
#include "SPAException.h"

class ASTNode
{
public:
		
	typedef enum NodeType { 
		Program, 
		Procedure,
		StmtLst,
		//Stmts
		Assign,
		Call,
		While,
		If,
		//EXPR
		Operator,
		Variable,
		Constant
	};

	
	
	/*	In C++, pure virtual functions are declared using the pure specifier[1] (= 0), as demonstrated below.
	class Abstract {
	public:
		virtual void pure_virtual() = 0;
	};
	The pure virtual function declaration provides only the prototype of the method. */

	ASTNode();
	ASTNode(NodeType);
	ASTNode(NodeType, PROC);

	~ASTNode(void);

	virtual ASTNode* addChild(ASTNode*);
	virtual ASTNode* setParent(ASTNode * p);

	void setRoot(int);

	ASTNode::NodeType getType();
	ASTNode* getChild(int);
	ASTNode* getParent();

	int getValue();
 
	bool isHasChildren();
	bool isRoot();

protected:
	int value;
	ASTNode* parent;
	vector<ASTNode*> children;
	NodeType nodeType;

private:
	
	bool root;
};


