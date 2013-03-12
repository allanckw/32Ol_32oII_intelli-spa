#pragma once
#include "stdAfx.h"
#include "ASTNode.h"
#include "PKB.h"

class PQLContainPreprocessor
{
public:
	static vector<ASTNode*> getNodes(ASTNode::NodeType);

	static vector<ASTNode*> getNodes(ASTNode::NodeType, int value);

private:
	static vector<ASTNode*> getPrecomputeNodes(ASTNode::NodeType);

};

