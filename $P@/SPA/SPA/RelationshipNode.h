#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"
#include "PalletTown.h"

class RelationshipNode :
	public QueryTreeNode
{
private:
	PalletTown::garyOak relationshipType;
	PalletTown::ashKetchum firstVariableType, secondVariableType;
	string firstVariableName, secondVariableName;

public:
	RelationshipNode(PalletTown::garyOak, pair<pair<PalletTown::ashKetchum, string>, pair<PalletTown::ashKetchum, string>>);
	PalletTown::garyOak getRelationshipType();
	string getFirstVariableName();
	PalletTown::ashKetchum getFirstVariableType();
	string getSecondVariableName();
	PalletTown::ashKetchum getSecondVariableType();
};