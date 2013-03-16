#include "PQLContainPreprocessor.h"
#include "PKB.h"
#include "ASTNode.h"
#include "ASTStmtLstNode.h"

vector<ASTNode*> PQLContainPreprocessor::getNodes(ASTNode::NodeType type)
{
	vector<ASTNode*> results;
	ASTNode* root = PKB::rootNode;
	results = getAllNodesByType(type);
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
