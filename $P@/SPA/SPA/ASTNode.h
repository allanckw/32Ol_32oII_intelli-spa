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

	virtual ASTNode* addChild(ASTNode*);
	virtual ASTNode* setParent(ASTNode * p);

	ASTNode* getAncestor() const;
	void setAncestor(ASTNode*);

	void setRoot(PROC);

	ASTNode::NodeType getType() const;
	ASTNode* getChild(unsigned int) const;
	ASTNode* getParent() const; 

	int getValue() const;
 
	bool isHasChildren();
	bool isRoot();

	vector<ASTNode*> getChildren() const;

protected:
	int value;
	ASTNode* parent;
	ASTNode* ancestorNode;

	vector<ASTNode*> children;
	NodeType nodeType;

private:
	
	bool root;
};


