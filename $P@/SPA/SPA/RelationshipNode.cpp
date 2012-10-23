#pragma once
#include "StdAfx.h"
#include "RelationshipNode.h"
#include "QueryPreprocessor.h"
#include "PKB.h"

RelationshipNode::RelationshipNode(QueryPreprocessor::garyOak type, vector<string> contents)
{
	if (contents.size() != 2)
		throw SPAException("Weird relationship detected");

	this->nodeType = Relationship;
	this->relationshipType = type;
	this->firstVariable = contents.at(0);
	this->secondVariable = contents.at(1);
}

string RelationshipNode::getRelationshipType()
{
	return relationshipType;
}


string RelationshipNode::getFirstVariable()
{
	return firstVariable;
}

string RelationshipNode::getSecondVariable()
{
	return secondVariable;
}
