#pragma once
#include "StdAfx.h"

class QueryTreeNode
{
public:
	QueryTreeNode();
	void setParent(QueryTreeNode*);
	void setChild(QueryTreeNode*);
	QueryTreeNode* getParent();
	QueryTreeNode* getChild();

protected:
	typedef enum QTNodeType{
		Relationship,
		Condition,
		Select,
		Project
	};
	QTNodeType nodeType;
	QueryTreeNode* parentNode;
	QueryTreeNode* childNode;
};