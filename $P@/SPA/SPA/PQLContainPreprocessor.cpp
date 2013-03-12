#include "PQLContainPreprocessor.h"


vector<ASTNode*> PQLContainPreprocessor::getNodes(ASTNode::NodeType type)
{
	vector<ASTNode*> results;
	ASTNode* root = PKB::rootNode;
	if (type == ASTNode::Assign || type == ASTNode::If || type == ASTNode::While || type == ASTNode::Call) {
		return PQLContainPreprocessor::getPrecomputeNodes(type);
	} else {
		//traverse.. same type = add in result, else ignore
	}
	return results;
}


vector<ASTNode*> PQLContainPreprocessor::getNodes(ASTNode::NodeType type, int value)
{
	vector<ASTNode*> results;
	ASTNode* root = PKB::rootNode;

	//traverse.. same type and value = add in result else ignore

	return results;
}

vector<ASTNode*> PQLContainPreprocessor::getPrecomputeNodes(ASTNode::NodeType type)
{
	vector<ASTNode*> result;
	switch (type)
	{
	case ASTNode::Assign:
		{
			for (auto it = PKB::assignNodes.begin(); it != PKB::assignNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	case ASTNode::While:
		{
			for (auto it = PKB::whileNodes.begin(); it != PKB::whileNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	case ASTNode::If:
		{
			for (auto it = PKB::ifNodes.begin(); it != PKB::ifNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	case ASTNode::Call:
		{
			for (auto it = PKB::callNodes.begin(); it != PKB::callNodes.end(); it++){
				result.push_back((*it).second);
			}
		}
	}
}