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
	bool toAddStandardNode=false;

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
				toAddStandardNode=true;
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
					if(toAddStandardNode)
					{
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
						toAddStandardNode=false;
					}
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

					if(toAddStandardNode)	
					{
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
						toAddStandardNode=false;
					}
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
		//to add into the PKB cfg list
		PKB::addToCFGList(rootCFG,procedureNode->getValue());
	}

}

/**
* This method will be used to process While AST Node and build While CFG Node
* @Param procedureNode To 
* @Param s A pointer to keep 'Start' PROG_Line
* @Param stmtNode The while AST Node to be process
* @Return the reference of while node that is build
*/
CFGNode* CFGBuilder::processWhile(ASTNode* procedureNode,int* s, ASTStmtNode *stmtNode)	
{
	CFGNode* currCFG;
	ASTStmtNode* currNode;
	int start = stmtNode->getStmtNumber();
	int end = start;
	int* currline = &start;
	bool isFirstWhileCFGNodeSet=false;
	bool toAddStandardNode=false;

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
				toAddStandardNode=true;
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
					if(toAddStandardNode)
					{
						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
						stmtNode->addPreviousNode(currCFG);
						currCFG->addNextNode(stmtNode);
						currCFG=stmtNode;
						toAddStandardNode=false;
					}
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
					if(toAddStandardNode)
					{
						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
						stmtNode->addPreviousNode(currCFG);
						currCFG->addNextNode(stmtNode);
						currCFG=stmtNode;
						toAddStandardNode=false;
					}
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
	//currCFG->addNextNode(whileNode);
	//currCFG->setLink(whileNode);
	currCFG->addNextNode(whileNode);
	whileNode->addPreviousNode(currCFG);
	(*s)=(*currline);
	return whileNode;
}

/**
* This method will be used to process If AST Node and build If CFG Node
* @Param procedureNode To 
* @Param s A pointer to keep 'Start' PROG_Line
* @Param stmtNode The while AST Node to be process
* @Param ifNode The 'if' CFGNode to link
* @Return the reference of the last node of If
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
	bool toAddStandardNode=false;

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
				toAddStandardNode=true;
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
					if(toAddStandardNode)
					{
						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
						stmtNode->addPreviousNode(thencurrCFG);
						thencurrCFG->addNextNode(stmtNode);
						thencurrCFG=stmtNode;
						toAddStandardNode=false;
					}
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
					if(toAddStandardNode)
					{
						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
						stmtNode->addPreviousNode(thencurrCFG);
						thencurrCFG->addNextNode(stmtNode);
						thencurrCFG=stmtNode;
						toAddStandardNode=false;
					}
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
				toAddStandardNode=true;
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
					if(toAddStandardNode)
					{
						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
						stmtNode->addPreviousNode(elsecurrCFG);
						elsecurrCFG->addNextNode(stmtNode);
						elsecurrCFG=stmtNode;
						toAddStandardNode=false;
					}
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
					if(toAddStandardNode)
					{
						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
						stmtNode->addPreviousNode(elsecurrCFG);
						elsecurrCFG->addNextNode(stmtNode);
						elsecurrCFG=stmtNode;
						toAddStandardNode=false;
					}
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

void CFGBuilder::traverseCFGToPopulateNext()
{
	//PKB::stmtRefMap[<prog_line>].setCFGNode(); <- use this to put the reference

	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++)
	{
		CFGNode* currNode=PKB::getCFGHead(currentProc);
		PROG_LINE plStart=currNode->getStartLine();
		while(currNode->isEndNode()==false)
		{
			if(currNode->getType()==CFGNode::StandardNode)
			{
				for(PROG_LINE pl=currNode->getStartLine();pl<=currNode->getEndLine();pl++)
				{
					PKB::stmtRefMap[pl].setCFGNode(currNode);
					if(plStart!=pl)
					{
						PKB::next.insertNext(plStart,pl,true);
						plStart=pl;
					}
				}
				plStart=currNode->getEndLine();
				currNode=currNode->getNextNodes().at(0);
			}
			else if(currNode->getType()==CFGNode::WhileNode)
			{
				if(currNode->isStartNode()==false)
				{
					PKB::next.insertNext(plStart,currNode->getStartLine(),true);
				}
				PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
				plStart=currNode->getStartLine();
				PKB::next.insertNextStar(plStart,plStart,true);
				traverseCFGWhile(currNode);
				currNode=currNode->getNextNodes().at(1);
			}
			else if(currNode->getType()==CFGNode::IfNode)
			{
				if(currNode->isStartNode()==false)
				{
					PKB::next.insertNext(plStart,currNode->getStartLine(),true);
				}
				PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
				currNode=traverseCFGIf(currNode);
				plStart=currNode->getStartLine();
			}
		}
		if(currNode->isEndNode()==true && currNode->isDummy()==false)
		{
			for(PROG_LINE pl=currNode->getStartLine();pl<=currNode->getEndLine();pl++)
			{
				PKB::stmtRefMap[pl].setCFGNode(currNode);
				if(plStart!=pl)
				{
					PKB::next.insertNext(plStart,pl,true);
					plStart=pl;
				}
			}
		}
	}
}

void CFGBuilder::traverseCFGWhile(CFGNode* whileNode)
{
	CFGNode* currNode=whileNode->getNextNodes().at(0);
	//PKB::next.insertNext(whileNode->getStartLine(),currNode->getStartLine());
	PROG_LINE plStart=whileNode->getStartLine();
	while(currNode!=whileNode)
	{
		if(currNode->getType()==CFGNode::StandardNode)
		{

			for(PROG_LINE pl=currNode->getStartLine();pl<=currNode->getEndLine();pl++)
			{	
				PKB::stmtRefMap[pl].setCFGNode(currNode);
				if(plStart!=pl)
				{
					PKB::next.insertNext(plStart,pl,true);
					plStart=pl;
				}
			}
			plStart=currNode->getEndLine();
			currNode=currNode->getNextNodes().at(0);
		}
		else if(currNode->getType()==CFGNode::WhileNode)
		{
			if(currNode->isStartNode()==false)
			{
				PKB::next.insertNext(plStart,currNode->getStartLine(),true);
			}
			PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
			plStart=currNode->getStartLine();
			PKB::next.insertNextStar(plStart,plStart,true);
			traverseCFGWhile(currNode);
			currNode=currNode->getNextNodes().at(1);
		}
		else if(currNode->getType()==CFGNode::IfNode)
		{
			if(currNode->isStartNode()==false)
			{
				PKB::next.insertNext(plStart,currNode->getStartLine(),true);
			}
			PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
			currNode=traverseCFGIf(currNode);
			plStart=currNode->getStartLine();
		}
	}
	PKB::next.insertNext(plStart,whileNode->getStartLine(),true);
}

CFGNode* CFGBuilder::traverseCFGIf(CFGNode* ifNode)
{
	PROG_LINE plStartL=ifNode->getStartLine();
	PROG_LINE plStartR=ifNode->getStartLine();
	
	CFGNode* leftCurrNode=ifNode->getNextNodes().at(0);
	CFGNode* rightCurrNode=ifNode->getNextNodes().at(1);


	while(leftCurrNode->isDummy()==false)
	{
		if(leftCurrNode->getType()==CFGNode::StandardNode)
		{
			for(PROG_LINE pl=leftCurrNode->getStartLine();pl<=leftCurrNode->getEndLine();pl++)
			{
				PKB::stmtRefMap[pl].setCFGNode(leftCurrNode);
				if(plStartL!=pl)
				{
					PKB::next.insertNext(plStartL,pl,true);	
					plStartL=pl;
				}
			}
			plStartL=leftCurrNode->getEndLine();
			leftCurrNode=leftCurrNode->getNextNodes().at(0);
		}
		else if(leftCurrNode->getType()==CFGNode::WhileNode)
		{
			if(leftCurrNode->isStartNode()==false)
			{
				PKB::next.insertNext(plStartL,leftCurrNode->getStartLine(),true);
			}
			PKB::stmtRefMap[leftCurrNode->getStartLine()].setCFGNode(leftCurrNode);
			plStartL=leftCurrNode->getStartLine();
			PKB::next.insertNextStar(plStartL,plStartL,true);
			traverseCFGWhile(leftCurrNode);
			leftCurrNode=leftCurrNode->getNextNodes().at(1);
		}
		else if(leftCurrNode->getType()==CFGNode::IfNode)
		{
			if(leftCurrNode->isStartNode()==false)
			{
				PKB::next.insertNext(plStartL,leftCurrNode->getStartLine(),true);
			}
			PKB::stmtRefMap[leftCurrNode->getStartLine()].setCFGNode(leftCurrNode);
			leftCurrNode=traverseCFGIf(leftCurrNode);
		}
	}

	while(rightCurrNode->isDummy()==false)
	{
		if(rightCurrNode->getType()==CFGNode::StandardNode)
		{
			for(PROG_LINE pl=rightCurrNode->getStartLine();pl<=rightCurrNode->getEndLine();pl++)
			{
				PKB::stmtRefMap[pl].setCFGNode(rightCurrNode);
				if(plStartL!=pl)
				{
					PKB::next.insertNext(plStartR,pl,true);
					plStartR=pl;
				}
			}

			plStartR=rightCurrNode->getEndLine();
			rightCurrNode=rightCurrNode->getNextNodes().at(0);
		}
		else if(rightCurrNode->getType()==CFGNode::WhileNode)
		{
			if(rightCurrNode->isStartNode()==false)
			{
				PKB::next.insertNext(plStartR,rightCurrNode->getStartLine(),true);
			}
			PKB::stmtRefMap[rightCurrNode->getStartLine()].setCFGNode(rightCurrNode);
			plStartR=rightCurrNode->getStartLine();
			PKB::next.insertNextStar(plStartR,plStartR,true);
			traverseCFGWhile(rightCurrNode);
			rightCurrNode=rightCurrNode->getNextNodes().at(1);
		}
		else if(rightCurrNode->getType()==CFGNode::IfNode)
		{
			if(rightCurrNode->isStartNode()==false)
			{
				PKB::next.insertNext(plStartR,rightCurrNode->getStartLine(),true);
			}
			PKB::stmtRefMap[rightCurrNode->getStartLine()].setCFGNode(rightCurrNode);
			rightCurrNode=traverseCFGIf(rightCurrNode);
			plStartR=rightCurrNode->getStartLine();
		}
	}
	
	CFGNode* currNode=leftCurrNode;
	while(currNode->isDummy() && currNode->isEndNode()==false)
	{
		currNode=currNode->getNextNodes().at(0);
	}

	if(currNode->isDummy()==false)
	{
		PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
		PKB::next.insertNext(plStartL,currNode->getStartLine(),true);
		PKB::next.insertNext(plStartR,currNode->getStartLine(),true);
	}
	return currNode;
}