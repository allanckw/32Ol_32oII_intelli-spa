#pragma once
#include "stdAfx.h"
#include "ASTNode.h"
#include "PKB.h"

class PQLContainPreprocessor
{
public:
	static vector<ASTNode*> getNodes(ASTNode::NodeType);

private:
	static vector<ASTNode*> PQLContainPreprocessor::getAllNodesByType(ASTNode::NodeType);
	static vector<ASTNode*> PQLContainPreprocessor::processWhile(ASTNode::NodeType type, ASTStmtNode *stmtNode, vector<ASTNode*>& answer);
	static vector<ASTNode*> PQLContainPreprocessor::processIf(ASTNode::NodeType type, ASTStmtNode *stmtNode, vector<ASTNode*>& answer);
	static vector<ASTNode*> PQLContainPreprocessor::processAssign(ASTNode::NodeType type, ASTStmtNode *stmtNode, vector<ASTNode*>& answer);
};

