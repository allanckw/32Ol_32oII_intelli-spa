#include "PQLContainPreprocessor.h"
#include "PKB.h"
#include "ASTNode.h"
#include "ASTStmtLstNode.h"

vector<ASTNode*> PQLContainPreprocessor::getNodes(ASTNode::NodeType type)
{
	vector<ASTNode*> results;
	ASTNode* root = PKB::rootNode;
	if (type == ASTNode::Assign || type == ASTNode::If || type == ASTNode::While || type == ASTNode::Call || type == ASTNode::Procedure) {
		return PQLContainPreprocessor::getPrecomputeNodes(type);
	} else {
		//traverse.. same type = add in result, else ignore
		results=getAllNodesByType(type);
	}
	return results;
}

vector<ASTNode*> PQLContainPreprocessor::getAllNodesByType(ASTNode::NodeType type)
{
	vector<ASTNode*> answer;
	ASTStmtNode* currNode;

	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
		ASTNode* procedureNode = (*PKB::rootNode).getChild(currentProc);
		ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) (*procedureNode).getChild(0);
		currNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(0);

		ASTStmtNode* firstNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(0);
		if(type==ASTNode::StmtLst)
			answer.push_back(firstLevelStmtListNode);
		

		for(int i=0; i<firstLevelStmtListNode->getSize(); i++)	{
			currNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(i);
			switch (currNode->getType()) {
			case ASTNode::While:
				processWhile(type,currNode,answer);
				break;
			case ASTNode::If:
				processIf(type, currNode, answer);
				break;
			case ASTNode::Assign:
				processAssign(type, currNode, answer);
				break;
			}
		}
	}

	return answer;
}

vector<ASTNode*> PQLContainPreprocessor::processWhile(ASTNode::NodeType type, ASTStmtNode *stmtNode, vector<ASTNode*>& answer)	
{
	ASTStmtNode* currNode;

	ASTNode* controlNode = (ASTNode*) (*stmtNode).getChild(0);
	ASTStmtLstNode* WhileStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(1);//While Statement List
	if(type==ASTNode::Variable)	{
		answer.push_back(controlNode);
	}
	else if(type==ASTNode::StmtLst)	{
		answer.push_back(WhileStatementListNode);
	}

	for(int i=0; i<WhileStatementListNode->getSize(); i++)
	{
		currNode=(ASTStmtNode*)WhileStatementListNode->getChild(i);
		switch(currNode->getType())
		{
			case ASTNode::While:
				processWhile(type,currNode,answer);
				break;
			case ASTNode::If:
				processIf(type,currNode,answer);
				break;
			case ASTNode::Assign:
				processAssign(type,currNode,answer);
				break;
		}
	}
	return answer;
}

vector<ASTNode*> PQLContainPreprocessor::processIf(ASTNode::NodeType type, ASTStmtNode *stmtNode, vector<ASTNode*>& answer)
{
	ASTStmtNode* currNode;
	ASTNode* controlNode=(ASTNode*)(*stmtNode).getChild(0);
	ASTStmtLstNode* ifThenStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(1);
	ASTStmtLstNode* ifElseStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(2);	
	if(type==ASTNode::Variable)
	{
		answer.push_back(controlNode);
	}
	else if(type==ASTNode::StmtLst)
	{
		answer.push_back(ifThenStatementListNode);
		answer.push_back(ifElseStatementListNode);
	}

	for(int i=0; i<ifThenStatementListNode->getSize(); i++)
	{
		currNode=(ASTStmtNode*)ifThenStatementListNode->getChild(i);
		switch(currNode->getType())
		{
			case ASTNode::While:
				processWhile(type,currNode,answer);
				break;
			case ASTNode::If:
				processIf(type,currNode,answer);
				break;
			case ASTNode::Assign:
				processAssign(type,currNode,answer);
				break;
		}
	}

	for(int i=0; i<ifElseStatementListNode->getSize(); i++)
	{
		currNode=(ASTStmtNode*)ifElseStatementListNode->getChild(i);
		switch(currNode->getType())
		{
			case ASTNode::While:
				processWhile(type,currNode,answer);
				break;
			case ASTNode::If:
				processIf(type,currNode,answer);
				break;
			case ASTNode::Assign:
				processAssign(type,currNode,answer);
				break;
		}
	}
	
	return answer;
}

vector<ASTNode*> PQLContainPreprocessor::processAssign(ASTNode::NodeType type, ASTStmtNode *stmtNode, vector<ASTNode*>& answer)
{
	ASTNode* current;
	stack<ASTNode*> assignNodeStack;
	assignNodeStack.push(stmtNode->getChild(1)); //Push right hand side
	assignNodeStack.push(stmtNode->getChild(0)); //Push left hand side

	while(assignNodeStack.size()>0)
	{
		current=assignNodeStack.top();
		assignNodeStack.pop();

		if(current->getType()==type){
			answer.push_back(current);
		}
		
		if(current->getChildren().size()>0){
			int lastChildPos=current->getChildren().size()-1;
			for(int i=lastChildPos; i>=0; i--){
				assignNodeStack.push(current->getChild(i));
			}
		}
	}

	return answer;
}

vector<ASTNode*> PQLContainPreprocessor::getNodes(ASTNode::NodeType type, int value)
{
	vector<ASTNode*> results;
	ASTNode* root = PKB::rootNode;

	//traverse.. same type and value = add in result else ignore
	vector<ASTNode*> resultByNodeType=getAllNodesByType(type);

	for(int i=0; i<resultByNodeType.size()-1; i++){
		ASTNode* CNode=resultByNodeType.at(i);
		if(CNode->getValue()==value) {
			results.push_back(CNode);
		}
	}
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
	 case ASTNode::Procedure:
		{
			return PKB::rootNode->getChildren();
		}

		return result;
	}
}