#pragma once
#include "StdAfx.h"
#include "QueryTreeNode.h"
#include "QueryPreprocessor.h"
#include "PalletTown.h"

class ConditionNode :
	public QueryTreeNode
{
public:
	ConditionNode(PalletTown::mistyWaterflower, pair<pair<PalletTown::ashKetchum, string>, string>);
	string getConditionVariableName();
	PalletTown::ashKetchum getConditionVariableType();
	PalletTown::mistyWaterflower getConditionAttribute();
	string getConditionBoundary();
private:
	string conditionVariableName;
	PalletTown::ashKetchum conditionVariableType;
	PalletTown::mistyWaterflower conditionAttribute;
	string conditionBoundary;
};