#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"

class RelationshipNode :
	public QueryTreeNode
{
private:
	string relationshipType;
	string firstVariable;
	string secondVariable;

public:
	RelationshipNode(QueryPreprocessor::garyOak, vector<string>);
	string getRelationshipType();
	string getFirstVariable();
	string getSecondVariable();
};