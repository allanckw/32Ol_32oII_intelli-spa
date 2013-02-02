#include "CFGBuilder.h"

/**
* This method will be used to build the CFGs from the PKB::Root of the AST
* At the end of a procedure, it will be added to a list that 
* will contains all the CFGs in a program to prepare for query evaluation
*/
void CFGBuilder::buildCFG(){

	CFGNode* rootCFG;
	CFGNode* currCFG;
	ASTStmtNode* currNode;
	int currChild=0;
	bool isRootSet=false;

	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
		isRootSet=false;
		ASTNode* procedureNode = (*PKB::rootNode).getChild(currentProc);

		ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) (*procedureNode).getChild(0);
		currNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(0);

		ASTStmtNode* firstNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(0);
		int start = firstNode->getStmtNumber();
		int end = start;
		int* currline=&start;

		for(int i=0; i<firstLevelStmtListNode->getSize(); i++)	{

			currNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(i);
			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call) 	{
				end=currNode->getStmtNumber();
				currline=&end;
				if(i==firstLevelStmtListNode->getSize()-1)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					if(isRootSet==true)
					{
						stmtNode->addPreviousNode(currCFG);
						currCFG->addNextNode(stmtNode);
					}
					else
					{
						rootCFG=stmtNode;
						currCFG=stmtNode;
						stmtNode->setStartNode();
						isRootSet=true;
					}
					currCFG=stmtNode;
				}
			}
			else if(currNode->getType()==ASTNode::While) {
				if(currNode->getStmtNumber() == firstNode->getStmtNumber()) {//if while is the first Node
				
					CFGNode* whileNode = processWhile(procedureNode,currline,currNode);
					whileNode->setStartNode();
					isRootSet=true;
					currCFG=whileNode;
					rootCFG=whileNode;
					start=(*currline)+1;
				} else {
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					if(isRootSet==true) { //if no root is set yet meaning first CFGNode(root)
					
						stmtNode->addPreviousNode(currCFG);
						currCFG->addNextNode(stmtNode);
					} else {
						rootCFG=stmtNode;
						stmtNode->setStartNode();
						isRootSet=true;
					}
					currCFG=stmtNode;
					CFGNode* whileNode = processWhile(procedureNode,currline,currNode);
					whileNode->addPreviousNode(currCFG);
					currCFG->addNextNode(whileNode);
					currCFG=whileNode;
					start=(*currline)+1;
				}
			}
			else if(currNode->getType()==ASTNode::If)
			{
				if(currNode->getStmtNumber() == firstNode->getStmtNumber()) {//if while is the first Node
				
					CFGNode* ifNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
					ifNode->setStartNode();
					currCFG=processIf(procedureNode,currline,currNode,ifNode);
					rootCFG=ifNode;
					start=(*currline)+1;
					isRootSet=true;
				} else {

					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					if(isRootSet)	{
						stmtNode->addPreviousNode(currCFG);
						currCFG->addNextNode(stmtNode);
					} else {
						rootCFG=stmtNode;
						stmtNode->setStartNode();
						isRootSet=true;
					}
					currCFG=stmtNode;
					
					start=currNode->getStmtNumber();
					CFGNode* ifNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
					currCFG->addNextNode(ifNode);
					ifNode->addPreviousNode(currCFG);
					currCFG=processIf(procedureNode,currline,currNode,ifNode);
					start=(*currline)+1;
				}
			}
		}

		if(currCFG->getType()==CFGNode::WhileNode) {
			CFGNode* dummyEndNode=new CFGNode(CFGNode::DummyNode,-1,-1,procedureNode->getValue());
			dummyEndNode->setEndNode();
			dummyEndNode->addPreviousNode(currCFG);
			currCFG->addNextNode(dummyEndNode);
			currCFG=dummyEndNode;
		} else {
			currCFG->setEndNode();
		}
		//At the end of the procedure, before moving on to the next procedure 
		//call PKB::addToCFGList(<the first node of the cfg>, procedureNode->getValue());
		//to add into the PKB cfg list
		PKB::addToCFGList(rootCFG,procedureNode->getValue());
	}
}

/**
* This method will be used to process While AST Node and build While CFG Node
* @param procedureNode To 
* @param s A pointer to keep 'Start' PROG_Line
* @param stmtNode The while AST Node to be process
*/
CFGNode* CFGBuilder::processWhile(ASTNode* procedureNode,int* s, ASTStmtNode *stmtNode)	
{
	CFGNode* currCFG;
	ASTStmtNode* currNode;
	int start = stmtNode->getStmtNumber();
	int end = start;
	int* currline = &start;
	bool isFirstWhileCFGNodeSet=false;

	CFGNode* whileNode=new CFGNode(CFGNode::WhileNode, start, start, procedureNode->getValue());
	currCFG=whileNode;
	start++;
	ASTStmtLstNode* WhileStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(1);//While Statement List
	ASTStmtNode* firstNode=(ASTStmtNode*)WhileStatementListNode->getChild(0);
	for(int i=0; i<WhileStatementListNode->getSize(); i++)
	{
			currNode=(ASTStmtNode*) (*WhileStatementListNode).getChild(i);
			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call)
			{
				end=currNode->getStmtNumber();
				currline=&end;
				isFirstWhileCFGNodeSet=true;
				if(i==WhileStatementListNode->getSize()-1)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(currCFG);
					currCFG->addNextNode(stmtNode);
					currCFG=stmtNode;
				}
			}
			else if(currNode->getType()==ASTNode::While)
			{
				if (isFirstWhileCFGNodeSet==true)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(currCFG);
					currCFG->addNextNode(stmtNode);
					currCFG=stmtNode;
				}
				
				CFGNode* whileInternalNode = processWhile(procedureNode,currline,currNode);			
				whileInternalNode->addPreviousNode(currCFG);
				currCFG->addNextNode(whileInternalNode);
				currCFG=whileInternalNode;
				start=(*currline)+1;
				isFirstWhileCFGNodeSet=true;	
			}
			else if(currNode->getType()==ASTNode::If)
			{
				if (isFirstWhileCFGNodeSet==true)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(currCFG);
					currCFG->addNextNode(stmtNode);
					currCFG=stmtNode;
				}

				start=currNode->getStmtNumber();
				CFGNode* ifInternalNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
				//processIf(procedureNode,currline,currNode, NULL);
				currCFG->addNextNode(ifInternalNode);
				ifInternalNode->addPreviousNode(currCFG);
				currCFG=processIf(procedureNode,currline,currNode,ifInternalNode);
				isFirstWhileCFGNodeSet=true;
				start=(*currline)+1;
			}
		}
	currCFG->addNextNode(whileNode);
	(*s)=(*currline);
	return whileNode;
}

/**
* This method will be used to process If AST Node and build If CFG Node
* @param procedureNode To 
* @param s A pointer to keep 'Start' PROG_Line
* @param stmtNode The while AST Node to be process
* @param ifNode The 'if' CFGNode to link
*/
CFGNode* CFGBuilder::processIf(ASTNode* procedureNode, int *s, ASTStmtNode *stmtNode, CFGNode *ifNode)
{
	CFGNode* currCFG;
	CFGNode* thencurrCFG;
	CFGNode* elsecurrCFG;
	ASTStmtNode* currNode;
	int start = stmtNode->getStmtNumber();
	int end = start;
	int* currline = &start;
	bool isFirstIfCFGNodeSet=false;
	//CFGNode* ifNode=new CFGNode(CFGNode::WhileNode, start, start, procedureNode->getValue());
	currCFG=ifNode;


	start++;
	thencurrCFG=currCFG;
	elsecurrCFG=currCFG;

	ASTStmtLstNode* ifThenStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(1);//While Statement List
	for(int i=0; i<ifThenStatementListNode->getSize(); i++)
	{
		currNode=(ASTStmtNode*) (*ifThenStatementListNode).getChild(i);
			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call)
			{
				end=currNode->getStmtNumber();
				currline=&end;
				isFirstIfCFGNodeSet=true;
				if(i==ifThenStatementListNode->getSize()-1)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(thencurrCFG);
					thencurrCFG->addNextNode(stmtNode);
					thencurrCFG=stmtNode;
				}
			}
			else if(currNode->getType()==ASTNode::While)
			{
				if (isFirstIfCFGNodeSet==true)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(thencurrCFG);
					thencurrCFG->addNextNode(stmtNode);
					thencurrCFG=stmtNode;
				}
				
				CFGNode* whileInternalNode = processWhile(procedureNode,currline,currNode);			
				whileInternalNode->addPreviousNode(thencurrCFG);
				thencurrCFG->addNextNode(whileInternalNode);
				thencurrCFG=whileInternalNode;
				start=(*currline)+1;
				isFirstIfCFGNodeSet=true;	
			}
			else if(currNode->getType()==ASTNode::If)
			{
				if (isFirstIfCFGNodeSet==true)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(thencurrCFG);
					thencurrCFG->addNextNode(stmtNode);
					thencurrCFG=stmtNode;
				}

				CFGNode* ifInternalNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
				//processIf(procedureNode,currline,currNode, NULL);
				thencurrCFG->addNextNode(ifInternalNode);
				ifInternalNode->addPreviousNode(thencurrCFG);
				thencurrCFG=processIf(procedureNode,currline,currNode,ifInternalNode);
				start=(*currline)+1;
				isFirstIfCFGNodeSet=true;
			}
	}

	isFirstIfCFGNodeSet=false;
	start=end+1;
	ASTStmtLstNode* ifElseStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(2);//While Statement List
	for(int i=0; i<ifElseStatementListNode->getSize(); i++)
	{
		currNode=(ASTStmtNode*) (*ifElseStatementListNode).getChild(i);
			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call)
			{
				end=currNode->getStmtNumber();
				currline=&end;
				isFirstIfCFGNodeSet=true;
				if(i==ifElseStatementListNode->getSize()-1)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(elsecurrCFG);
					elsecurrCFG->addNextNode(stmtNode);
					elsecurrCFG=stmtNode;
				}
			}
			else if(currNode->getType()==ASTNode::While)
			{
				if (isFirstIfCFGNodeSet==true)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(elsecurrCFG);
					elsecurrCFG->addNextNode(stmtNode);
					elsecurrCFG=stmtNode;
				}
				
				CFGNode* whileInternalNode = processWhile(procedureNode,currline,currNode);			
				whileInternalNode->addPreviousNode(elsecurrCFG);
				elsecurrCFG->addNextNode(whileInternalNode);
				elsecurrCFG=whileInternalNode;
				start=(*currline)+1;
				isFirstIfCFGNodeSet=true;	
			}
			else if(currNode->getType()==ASTNode::If)
			{
				if (isFirstIfCFGNodeSet==true)
				{
					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
					stmtNode->addPreviousNode(elsecurrCFG);
					elsecurrCFG->addNextNode(stmtNode);
					elsecurrCFG=stmtNode;
				}

				CFGNode* ifInternalNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
				//processIf(procedureNode,currline,currNode, NULL);
				elsecurrCFG->addNextNode(ifInternalNode);
				ifInternalNode->addPreviousNode(elsecurrCFG);
				elsecurrCFG=processIf(procedureNode,currline,currNode,ifInternalNode);
				start=(*currline)+1;
				isFirstIfCFGNodeSet=true;
			}
	}

	//Combine If else Node into one ending Node before continuing
	CFGNode* endIfDummyNode=new CFGNode(CFGNode::DummyNode,-1,-1,procedureNode->getValue());
	thencurrCFG->addNextNode(endIfDummyNode);
	elsecurrCFG->addNextNode(endIfDummyNode);
	endIfDummyNode->addPreviousNode(thencurrCFG);
	endIfDummyNode->addPreviousNode(elsecurrCFG);
	
	(*s)=(*currline);
	return endIfDummyNode;
}
