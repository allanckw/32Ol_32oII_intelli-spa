#pragma once
#include "StdAfx.h"

class QueryTreeNode
{
public:
	typedef enum QTNodeType{
		Relationship,
		Condition,
		Pattern,
		Select,
		Project,
		LastSelect,
		Dummy
	};
	QueryTreeNode();
	void setParent(QueryTreeNode*);
	void setChild(QueryTreeNode*);
	QTNodeType getNodeType();
	QueryTreeNode* getParent();
	QueryTreeNode* getChild();

protected:
	QTNodeType nodeType;
	QueryTreeNode* parentNode;
	QueryTreeNode* childNode;

};