#pragma once
#include "StdAfx.h"
#include "RelationshipNode.h"
#include "QueryPreprocessor.h"
#include "PalletTown.h"
#include "PKB.h"

RelationshipNode::RelationshipNode(PalletTown::garyOak type, 
	pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>> contents)
{
	this->nodeType = Relationship;
	this->relationshipType = type;
	this->firstVariableType = contents.first.first;
	this->firstVariableName = contents.first.second;
	this->secondVariableType = contents.second.first;
	this->secondVariableName = contents.second.second;
}

PalletTown::garyOak RelationshipNode::getRelationshipType()
{
	return relationshipType;
}

string RelationshipNode::getFirstVariableName()
{
	return firstVariableName;
}

PalletTown::ashKetchum RelationshipNode::getFirstVariableType()
{
	return firstVariableType;
}

string RelationshipNode::getSecondVariableName()
{
	return secondVariableName;
}

PalletTown::ashKetchum RelationshipNode::getSecondVariableType()
{
	return secondVariableType;
}
