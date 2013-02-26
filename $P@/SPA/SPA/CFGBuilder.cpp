#include "CFGBuilder.h"
#include "PKB.h"
#include "ASTNode.h"
#include "ASTStmtLstNode.h"

void CFGBuilder::buildCFG()
{
	stack<ASTStmtNode*> DFSstack;
	stack<ASTStmtLstNode*> DFSstmtLstStack;
	stack<int> positionStack;
	stack<bool> traversingThenPartOfIfStack;

	unordered_map<ASTNode*, CFGNode*> map;
	stack<CFGNode*> stack;

	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
		ASTNode* procedureNode = PKB::rootNode->getChild(currentProc); //procedure
		ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) procedureNode->getChild(0); //stmtList
		int firstLevelPosition = 0;

		ASTStmtLstNode* currentStmtListNode = firstLevelStmtListNode;
		ASTStmtNode* currentStmtNode = (ASTStmtNode*) firstLevelStmtListNode->getChild(0); //first statement
		int currentPosition = 0;
		int currentStatementNumber = currentStmtNode->getStmtNumber();
		bool haveNextChildren = true;
		
		CFGNode* newCFG;
		CFGNode* currCFG = new CFGNode(currentProc);
		PKB::addToCFGList(currCFG);

		while (haveNextChildren) {
			switch (currentStmtNode->getType()) {
			case ASTNode::While:
				if (currCFG->first != 0) {
					currCFG->type = CFGNode::StandardNode;
					stack.push(currCFG);
					newCFG = new CFGNode(currCFG); //while
					currCFG->children.oneChild = newCFG;
					currCFG = newCFG;
				}
				map.insert(pair<ASTNode*, CFGNode*>(currentStmtNode, currCFG));
				currCFG->first = currCFG->last = currentStatementNumber;
				currCFG->type = CFGNode::WhileNode;

				newCFG = new CFGNode(currCFG); //while out
				currCFG->children.whileChildren.whileOut = newCFG;

				if (currCFG->whileAncestor == NULL)
					currCFG->whileAncestor = currCFG;

				newCFG = new CFGNode(currCFG); //while in
				currCFG->children.whileChildren.whileIn = newCFG;
				
				//PKB::bigTable.push_back(currCFG);
				PKB::stmtRefMap.at(currentStatementNumber).setCFGNode(currCFG);

				stack.push(currCFG);
				currCFG = currCFG->children.whileChildren.whileIn;
				break;

			case ASTNode::If:
				if (currCFG->first == 0)
					currCFG->first = currentStatementNumber;
				currCFG->last = currentStatementNumber;
				currCFG->type = CFGNode::IfNode;
				map.insert(pair<ASTNode*, CFGNode*>(currentStmtNode, currCFG));

				newCFG = new CFGNode(currCFG); //if then
				currCFG->children.ifChildren.ifThen = newCFG;

				newCFG = new CFGNode(currCFG); //if else
				currCFG->children.ifChildren.ifElse = newCFG;

				newCFG = new CFGNode(currCFG); //if later
				currCFG->children.ifChildren.ifLater = newCFG;
				
				PKB::stmtRefMap.at(currentStatementNumber).setCFGNode(currCFG);
				stack.push(currCFG);
				currCFG = currCFG->children.ifChildren.ifThen;
				traversingThenPartOfIfStack.push(true);
				break;
			}

			//go to the next stmt.
			//go down if can go down (container statement)
			switch (currentStmtNode->getType()) {
			case ASTNode::While:
			case ASTNode::If:
				DFSstack.push(currentStmtNode);
				DFSstmtLstStack.push(currentStmtListNode);
				positionStack.push(currentPosition);
				
				currentStmtListNode = (ASTStmtLstNode*) (*currentStmtNode).getChild(1);
				currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(0);
				currentStatementNumber = currentStmtNode->getStmtNumber();
				currentPosition = 0;
				break;
			default: {
				PKB::stmtRefMap.at(currentStatementNumber).setCFGNode(currCFG);
				if (currCFG->first == 0)
					currCFG->first = currentStatementNumber;
				currCFG->last = currentStatementNumber;

				const vector<VAR>& varsM = PKB::modifies.getModifiedByStmt(currentStatementNumber);
				currCFG->modifySet.insert(varsM.begin(), varsM.end());
				const vector<VAR>& varsU = PKB::uses.getUsedByStmt(currentStatementNumber);
				currCFG->useSet.insert(varsU.begin(), varsU.end());

				//go right if can go right, if cannot, go up until can go right
				bool notYetGotNextChild = true;
				while (notYetGotNextChild) {
					if (DFSstack.empty()) {
						if (firstLevelPosition + 1 < firstLevelStmtListNode->getSize()) {
							currentStmtNode = (ASTStmtNode*) firstLevelStmtListNode->getChild(++firstLevelPosition);
							currentStatementNumber = currentStmtNode->getStmtNumber();
							currentPosition = firstLevelPosition;
						} else { //end
							if (currCFG->first != 0)
								currCFG->type = CFGNode::StandardNode;
							haveNextChildren = false;
						}
						notYetGotNextChild = false;
					} else {
						if (currentPosition + 1 < currentStmtListNode->getSize()) { //try right
							currentStmtNode = (ASTStmtNode*) currentStmtListNode->getChild(++currentPosition);
							currentStatementNumber = currentStmtNode->getStmtNumber();
							notYetGotNextChild = false;
						} else { //must go up
							if (currCFG->first != 0)
								currCFG->type = CFGNode::StandardNode;
							stack.push(currCFG);
							ASTStmtNode* parentNode = DFSstack.top();
							CFGNode* parentCFG = map[parentNode];
							
							if ((*parentNode).getType() == ASTNode::If &&
								traversingThenPartOfIfStack.top()) { //go to 'else' part of if
									traversingThenPartOfIfStack.pop();
									traversingThenPartOfIfStack.push(false);
									
									currCFG->children.oneChild = parentCFG->children.ifChildren.ifLater;
									parentCFG->children.ifChildren.ifLater->parents[0] = currCFG;
									currCFG = parentCFG->children.ifChildren.ifElse;

									currentStmtListNode = (ASTStmtLstNode*) (*parentNode).getChild(2);
									currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(0);
									currentStatementNumber = currentStmtNode->getStmtNumber();
									currentPosition = 0;

									notYetGotNextChild = false;
							} else { //'else' part of if OR in part of while
								if ((*parentNode).getType() == ASTNode::If) {
									traversingThenPartOfIfStack.pop(); //was at 'else' part of if
									CFGNode* temp = parentCFG->children.ifChildren.ifLater;
									if (currCFG->last == NULL) {
										const IntervalList* tempList = currCFG->prevList;
										while (tempList->next != NULL)
											tempList = tempList->next;
										temp->prevList = temp->prevList->Union(
											parentCFG->children.ifChildren.ifThen->first,
											tempList->last);
									} else
										temp->prevList = temp->prevList->Union(
											parentCFG->children.ifChildren.ifThen->first,
											currCFG->last);
									currCFG->children.oneChild = temp;
									temp->parents.push_back(currCFG);
									currCFG = temp;
								} else { //While
									if (currCFG->first == 0)
										parentCFG->prevList = currCFG->prevList;
									else
										parentCFG->prevList = currCFG->prevList =
											currCFG->prevList->Union(currCFG->first, currCFG->last);
									currCFG->children.oneChild = parentCFG;
									parentCFG->parents.push_back(currCFG);
									currCFG = parentCFG->children.whileChildren.whileOut;
									currCFG->prevList = parentCFG->prevList;
								}

								currentStmtNode = parentNode;
								currentStmtListNode = DFSstmtLstStack.top();
								currentPosition = positionStack.top();

								DFSstack.pop();
								DFSstmtLstStack.pop();
								positionStack.pop();
							}
						}
					}
				}
					 }
			}
		}
		map.clear();

		CFGNode* temp;
		while (!stack.empty()) {
			currCFG = stack.top();
			stack.pop();

			if (currCFG->whileAncestor == NULL)
				switch (currCFG->type) {
				case CFGNode::StandardNode:
					temp = currCFG->children.oneChild;
					if (temp->first != 0) {
						if (temp->nextList == NULL)
							currCFG->nextList = new IntervalList(temp->first, temp->first);
						else
							currCFG->nextList = temp->nextList->Union(temp->first, temp->first);
					}
					break;
				case CFGNode::IfNode:
					CFGNode* temp = currCFG->children.ifChildren.ifLater;
					if (temp->first == 0) {
						temp = temp->parents[1];
						if (temp->first == 0) {
							if (temp->nextList == NULL) {
								const IntervalList* tempList = temp->prevList;
								while (tempList->next != NULL)
									tempList = tempList->next;
								currCFG->nextList = new IntervalList(
								currCFG->children.ifChildren.ifThen->first, tempList->last);
							} else {
								currCFG->nextList = temp->nextList->Union(
									currCFG->children.ifChildren.ifThen->first, temp->last);
							}
						} else {
							if (temp->nextList == NULL) {
								currCFG->nextList = new IntervalList(
									currCFG->children.ifChildren.ifThen->first, temp->last);
							} else {
								currCFG->nextList = temp->nextList->Union(
									currCFG->children.ifChildren.ifThen->first, temp->last);
							}
						}
					} else {
						if (temp->nextList == NULL)
							currCFG->nextList = new IntervalList(
							currCFG->children.ifChildren.ifThen->first, temp->last);
						else
							currCFG->nextList = temp->nextList->Union(
							currCFG->children.ifChildren.ifThen->first, temp->last);
					}
					break;
				}
			else { //inside a while loop or is a while itself
				CFGNode* whileAncestor = currCFG->whileAncestor;
				if (currCFG != whileAncestor) {
					currCFG->prevList = whileAncestor->prevList;
					if (whileAncestor->nextList == NULL) {
						temp = whileAncestor->children.whileChildren.whileOut;
						if (temp->nextList == NULL) {
							if (temp->first == 0) {
								const IntervalList* tempList = temp->prevList;
								while (tempList->next != NULL)
									tempList = tempList->next;
								whileAncestor->nextList = new IntervalList(
									whileAncestor->first, tempList->last);
							} else
								whileAncestor->nextList = new IntervalList(
								whileAncestor->first, temp->last);
						} else {
							if (temp->first == 0)
								whileAncestor->nextList = temp->nextList;
							else
								whileAncestor->nextList = temp->nextList->Union(
								whileAncestor->first, temp->last);
						}
					}
					currCFG->nextList = currCFG->whileAncestor->nextList;
				}
			}
		}
	}
}



///**
//* This method will be used to build the CFGs from the PKB::Root of the AST
//* At the end of a procedure, it will be added to a list that 
//* will contains all the CFGs in a program to prepare for query evaluation
//*/
//
//set<CFGNode*> CFGBuilder::visited;
//
//void CFGBuilder::buildCFG(){
//
//	CFGNode* rootCFG;
//	CFGNode* currCFG;
//	ASTStmtNode* currNode;
//	int currChild=0;
//	bool isRootSet=false;
//	bool toAddStandardNode=false;
//
//	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
//		isRootSet=false;
//		ASTNode* procedureNode = (*PKB::rootNode).getChild(currentProc);
//
//		ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) (*procedureNode).getChild(0);
//		currNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(0);
//
//		ASTStmtNode* firstNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(0);
//		int start = firstNode->getStmtNumber();
//		int end = start;
//		int* currline=&start;
//
//		for(int i=0; i<firstLevelStmtListNode->getSize(); i++)	{
//
//			currNode=(ASTStmtNode*) (*firstLevelStmtListNode).getChild(i);
//			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call) 	{
//				toAddStandardNode=true;
//				end=currNode->getStmtNumber();
//				currline=&end;
//				if(i==firstLevelStmtListNode->getSize()-1)
//				{
//					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//					if(isRootSet==true)
//					{
//						stmtNode->addPreviousNode(currCFG);
//						currCFG->addNextNode(stmtNode);
//					}
//					else
//					{
//						rootCFG=stmtNode;
//						currCFG=stmtNode;
//						stmtNode->setStartNode();
//						isRootSet=true;
//					}
//					currCFG=stmtNode;
//				}
//			}
//			else if(currNode->getType()==ASTNode::While) {
//				if(currNode->getStmtNumber() == firstNode->getStmtNumber()) {//if while is the first Node
//					CFGNode* whileNode = processWhile(procedureNode,currline,currNode);
//					whileNode->setStartNode();
//					isRootSet=true;
//					currCFG=whileNode;
//					rootCFG=whileNode;
//					start=(*currline)+1;
//				} else {
//					if(toAddStandardNode)
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						if(isRootSet==true) { //if no root is set yet meaning first CFGNode(root)
//							stmtNode->addPreviousNode(currCFG);
//							currCFG->addNextNode(stmtNode);		
//						} else {
//							rootCFG=stmtNode;
//							stmtNode->setStartNode();
//							isRootSet=true;
//						}
//						currCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//					CFGNode* whileNode = processWhile(procedureNode,currline,currNode);
//					whileNode->addPreviousNode(currCFG);
//					currCFG->addNextNode(whileNode);
//					currCFG=whileNode;
//					if((i+1)<firstLevelStmtListNode->getSize())
//						start=(*currline)+1;
//				}
//			}
//			else if(currNode->getType()==ASTNode::If)
//			{
//				if(currNode->getStmtNumber() == firstNode->getStmtNumber()) {//if while is the first Node
//				
//					CFGNode* ifNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
//					ifNode->setStartNode();
//					currCFG=processIf(procedureNode,currline,currNode,ifNode);
//					rootCFG=ifNode;
//					start=(*currline)+1;
//					isRootSet=true;
//				} else {
//
//					if(toAddStandardNode)	
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						if(isRootSet)	{
//							stmtNode->addPreviousNode(currCFG);
//							currCFG->addNextNode(stmtNode);
//						} else {
//							rootCFG=stmtNode;
//							stmtNode->setStartNode();
//							isRootSet=true;
//						}
//						currCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//					start=currNode->getStmtNumber();
//					CFGNode* ifNode = new CFGNode(CFGNode::IfNode,currNode->getStmtNumber(),currNode->getStmtNumber(),procedureNode->getValue());
//					currCFG->addNextNode(ifNode);
//					ifNode->addPreviousNode(currCFG);
//					currCFG=processIf(procedureNode,currline,currNode,ifNode);
//					if((i+1)<firstLevelStmtListNode->getSize())
//						start=(*currline)+1;
//				}
//			}
//		}
//
//		if(currCFG->getType()==CFGNode::WhileNode) {
//			CFGNode* dummyEndNode=new CFGNode(CFGNode::DummyNode,-1,-1,procedureNode->getValue());
//			dummyEndNode->setEndNode();
//			dummyEndNode->addPreviousNode(currCFG);
//			currCFG->addNextNode(dummyEndNode);
//			currCFG=dummyEndNode;
//		} else {
//			currCFG->setEndNode();
//		}
//		//At the end of the procedure, before moving on to the next procedure 
//		//to add into the PKB cfg list
//		PKB::addToCFGList(rootCFG,procedureNode->getValue());
//	}
//
//}
//
///**
//* This method will be used to process While AST Node and build While CFG Node
//* @Param procedureNode To 
//* @Param s A pointer to keep 'Start' PROG_Line
//* @Param stmtNode The while AST Node to be process
//* @Return the reference of while node that is build
//*/
//CFGNode* CFGBuilder::processWhile(ASTNode* procedureNode,int* s, ASTStmtNode *stmtNode)	
//{
//	CFGNode* currCFG;
//	ASTStmtNode* currNode;
//	int start = stmtNode->getStmtNumber();
//	int end = start;
//	int* currline = &start;
//	bool isFirstWhileCFGNodeSet=false;
//	bool toAddStandardNode=false;
//
//	CFGNode* whileNode=new CFGNode(CFGNode::WhileNode, start, start, procedureNode->getValue());
//	currCFG=whileNode;
//	start++;
//	ASTStmtLstNode* WhileStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(1);//While Statement List
//	ASTStmtNode* firstNode=(ASTStmtNode*)WhileStatementListNode->getChild(0);
//	for(int i=0; i<WhileStatementListNode->getSize(); i++)
//	{
//
//		currNode=(ASTStmtNode*) (*WhileStatementListNode).getChild(i);
//
//		if(currNode->getStmtNumber()==28)
//		{
//			cout<<""<<endl;
//		}
//		start;
//			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call)
//			{
//				end=currNode->getStmtNumber();
//				currline=&end;
//				isFirstWhileCFGNodeSet=true;
//				toAddStandardNode=true;
//				if(i==WhileStatementListNode->getSize()-1)
//				{
//					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//					stmtNode->addPreviousNode(currCFG);
//					currCFG->addNextNode(stmtNode);
//					currCFG=stmtNode;
//				}
//			}
//			else if(currNode->getType()==ASTNode::While)
//			{
//				if (isFirstWhileCFGNodeSet==true)
//				{
//					if(toAddStandardNode)
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						stmtNode->addPreviousNode(currCFG);
//						currCFG->addNextNode(stmtNode);
//						currCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//				}
//				
//				CFGNode* whileInternalNode = processWhile(procedureNode,currline,currNode);			
//				whileInternalNode->addPreviousNode(currCFG);
//				currCFG->addNextNode(whileInternalNode);
//				currCFG=whileInternalNode;
//				if((i+1)<WhileStatementListNode->getSize())
//					start=(*currline)+1;
//				isFirstWhileCFGNodeSet=true;	
//			}
//			else if(currNode->getType()==ASTNode::If)
//			{
//				if (isFirstWhileCFGNodeSet==true)
//				{
//					if(toAddStandardNode)
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						stmtNode->addPreviousNode(currCFG);
//						currCFG->addNextNode(stmtNode);
//						currCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//				}
//
//				start=currNode->getStmtNumber();
//				CFGNode* ifInternalNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
//				//processIf(procedureNode,currline,currNode, NULL);
//				currCFG->addNextNode(ifInternalNode);
//				ifInternalNode->addPreviousNode(currCFG);
//				currCFG=processIf(procedureNode,currline,currNode,ifInternalNode);
//				isFirstWhileCFGNodeSet=true;
//				if((i+1)<WhileStatementListNode->getSize())
//					start=(*currline)+1;
//			}
//		}
//	//currCFG->addNextNode(whileNode);
//	//currCFG->setLink(whileNode);
//	currCFG->addNextNode(whileNode);
//	whileNode->addPreviousNode(currCFG);
//	(*s)=(*currline);
//	return whileNode;
//}
//
///**
//* This method will be used to process If AST Node and build If CFG Node
//* @Param procedureNode To 
//* @Param s A pointer to keep 'Start' PROG_Line
//* @Param stmtNode The while AST Node to be process
//* @Param ifNode The 'if' CFGNode to link
//* @Return the reference of the last node of If
//*/
//CFGNode* CFGBuilder::processIf(ASTNode* procedureNode, int *s, ASTStmtNode *stmtNode, CFGNode *ifNode)
//{
//	CFGNode* currCFG;
//	CFGNode* thencurrCFG;
//	CFGNode* elsecurrCFG;
//	ASTStmtNode* currNode;
//	int start = stmtNode->getStmtNumber();
//	int end = start;
//	int* currline = &start;
//	bool isFirstIfCFGNodeSet=false;
//	bool toAddStandardNode=false;
//
//	//CFGNode* ifNode=new CFGNode(CFGNode::WhileNode, start, start, procedureNode->getValue());
//	currCFG=ifNode;
//
//
//	start++;
//	thencurrCFG=currCFG;
//	elsecurrCFG=currCFG;
//
//	ASTStmtLstNode* ifThenStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(1);//While Statement List
//	for(int i=0; i<ifThenStatementListNode->getSize(); i++)
//	{
//		currNode=(ASTStmtNode*) (*ifThenStatementListNode).getChild(i);
//			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call)
//			{
//				end=currNode->getStmtNumber();
//				currline=&end;
//				isFirstIfCFGNodeSet=true;
//				toAddStandardNode=true;
//				if(i==ifThenStatementListNode->getSize()-1)
//				{
//					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//					stmtNode->addPreviousNode(thencurrCFG);
//					thencurrCFG->addNextNode(stmtNode);
//					thencurrCFG=stmtNode;
//				}
//			}
//			else if(currNode->getType()==ASTNode::While)
//			{
//				if (isFirstIfCFGNodeSet==true)
//				{
//					if(toAddStandardNode)
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						stmtNode->addPreviousNode(thencurrCFG);
//						thencurrCFG->addNextNode(stmtNode);
//						thencurrCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//				}
//				
//				CFGNode* whileInternalNode = processWhile(procedureNode,currline,currNode);			
//				whileInternalNode->addPreviousNode(thencurrCFG);
//				thencurrCFG->addNextNode(whileInternalNode);
//				thencurrCFG=whileInternalNode;
//				if((i+1)<ifThenStatementListNode->getSize())
//					start=(*currline)+1;
//				isFirstIfCFGNodeSet=true;	
//			}
//			else if(currNode->getType()==ASTNode::If)
//			{
//				if (isFirstIfCFGNodeSet==true)
//				{
//					if(toAddStandardNode)
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						stmtNode->addPreviousNode(thencurrCFG);
//						thencurrCFG->addNextNode(stmtNode);
//						thencurrCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//				}
//				CFGNode* ifInternalNode = new CFGNode(CFGNode::IfNode,start,start,procedureNode->getValue());
//				//processIf(procedureNode,currline,currNode, NULL);
//				thencurrCFG->addNextNode(ifInternalNode);
//				ifInternalNode->addPreviousNode(thencurrCFG);
//				thencurrCFG=processIf(procedureNode,currline,currNode,ifInternalNode);
//				if((i+1)<ifThenStatementListNode->getSize())
//					start=(*currline)+1;
//				isFirstIfCFGNodeSet=true;
//			}
//	}
//
//	isFirstIfCFGNodeSet=false;
//	start=(*currline)+1;
//	ASTStmtLstNode* ifElseStatementListNode = (ASTStmtLstNode*)(*stmtNode).getChild(2);//While Statement 
//	for(int i=0; i<ifElseStatementListNode->getSize(); i++)
//	{
//		currNode=(ASTStmtNode*) (*ifElseStatementListNode).getChild(i);
//		if(currNode->getStmtNumber()==23)
//			int b=222222;
//			if(currNode->getType()==ASTNode::Assign || currNode->getType()==ASTNode::Call)
//			{
//				end=currNode->getStmtNumber();
//				currline=&end;
//				isFirstIfCFGNodeSet=true;
//				toAddStandardNode=true;
//				if(i==ifElseStatementListNode->getSize()-1)
//				{
//					CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//					stmtNode->addPreviousNode(elsecurrCFG);
//					elsecurrCFG->addNextNode(stmtNode);
//					elsecurrCFG=stmtNode;
//				}
//			}
//			else if(currNode->getType()==ASTNode::While)
//			{
//				if (isFirstIfCFGNodeSet==true)
//				{
//					if(toAddStandardNode)
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						stmtNode->addPreviousNode(elsecurrCFG);
//						elsecurrCFG->addNextNode(stmtNode);
//						elsecurrCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//				}
//				CFGNode* whileInternalNode = processWhile(procedureNode,currline,currNode);			
//				whileInternalNode->addPreviousNode(elsecurrCFG);
//				elsecurrCFG->addNextNode(whileInternalNode);
//				elsecurrCFG=whileInternalNode;
//				if((i+1)<ifElseStatementListNode->getSize())
//					start=(*currline)+1;
//				isFirstIfCFGNodeSet=true;	
//			}
//			else if(currNode->getType()==ASTNode::If)
//			{
//				if (isFirstIfCFGNodeSet==true)
//				{
//					if(toAddStandardNode)
//					{
//						CFGNode* stmtNode=new CFGNode(CFGNode::StandardNode,start,end,procedureNode->getValue());
//						stmtNode->addPreviousNode(elsecurrCFG);
//						elsecurrCFG->addNextNode(stmtNode);
//						elsecurrCFG=stmtNode;
//						toAddStandardNode=false;
//					}
//				}
//				CFGNode* ifInternalNode = new CFGNode(CFGNode::IfNode,currNode->getStmtNumber(),currNode->getStmtNumber(),procedureNode->getValue());
//				//processIf(procedureNode,currline,currNode, NULL);
//				elsecurrCFG->addNextNode(ifInternalNode);
//				ifInternalNode->addPreviousNode(elsecurrCFG);
//				elsecurrCFG=processIf(procedureNode,currline,currNode,ifInternalNode);
//				if((i+1)<ifElseStatementListNode->getSize())
//					start=(*currline)+1;
//				isFirstIfCFGNodeSet=true;
//			}
//	}
//
//	//Combine If else Node into one ending Node before continuing
//	CFGNode* endIfDummyNode=new CFGNode(CFGNode::DummyNode,-1,-1,procedureNode->getValue());
//	thencurrCFG->addNextNode(endIfDummyNode);
//	elsecurrCFG->addNextNode(endIfDummyNode);
//	endIfDummyNode->addPreviousNode(thencurrCFG);
//	endIfDummyNode->addPreviousNode(elsecurrCFG);
//	
//	(*s)=(*currline);
//	return endIfDummyNode;
//}
//
//
//void CFGBuilder::traverseCFGToPopulateNext()
//{
//	//PKB::stmtRefMap[<prog_line>].setCFGNode(); <- use this to put the reference
//	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++)
//	{
//		CFGNode* currNode=PKB::getCFGHead(currentProc);
//		visited.clear();
//		PROG_LINE plStart=currNode->getStartLine();
//
//		while(currNode->isEndNode()==false)
//		{
//			if(visited.find(currNode)!=visited.end())
//				break;
//
//			if(currNode->getType()==CFGNode::StandardNode)
//			{
//				visited.insert(currNode);
//				for(PROG_LINE pl=currNode->getStartLine();pl<=currNode->getEndLine();pl++)
//				{
//					PKB::stmtRefMap[pl].setCFGNode(currNode);
//					if(plStart!=pl)
//					{
//						PKB::next.insertNext(plStart,pl,true);
//						plStart=pl;
//					}
//				}
//				plStart=currNode->getEndLine();
//				currNode=currNode->getNextNodes().at(0);
//			}
//			else if(currNode->getType()==CFGNode::WhileNode)
//			{
//				visited.insert(currNode);
//				if(currNode->isStartNode()==false)
//				{
//					PKB::next.insertNext(plStart,currNode->getStartLine(),true);
//				}
//				PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
//				plStart=currNode->getStartLine();
//				PKB::next.insertNextStar(plStart,plStart,true);
//				traverseCFGWhile(currNode);
//				vector<PROG_LINE> prevLine=getPrevFromWhile(currNode);
//				for(int i=0;i<prevLine.size();i++)
//					PKB::next.insertNext(prevLine.at(i),currNode->getStartLine(),true);
//				currNode=currNode->getNextNodes().at(1);
//			}
//			else if(currNode->getType()==CFGNode::IfNode)
//			{
//				visited.insert(currNode);
//				if(currNode->isStartNode()==false)
//				{
//					PKB::next.insertNext(plStart,currNode->getStartLine(),true);
//				}
//				PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
//				currNode=traverseCFGIf(currNode);
//			}
//			else if(currNode->getType()==CFGNode::DummyNode)
//			{
//				currNode->getNextNodes().at(0);
//			}
//		}
//
//		if(currNode->isEndNode()==true && currNode->isDummy()==false)
//		{
//			visited.insert(currNode);
//			for(PROG_LINE pl=currNode->getStartLine();pl<=currNode->getEndLine();pl++)
//			{
//				PKB::stmtRefMap[pl].setCFGNode(currNode);
//				if(plStart!=pl)
//				{
//					PKB::next.insertNext(plStart,pl,true);
//					plStart=pl;
//				}
//			}
//		}
//	}
//}
//
//void CFGBuilder::traverseCFGWhile(CFGNode* whileNode)
//{
//	CFGNode* currNode=whileNode->getNextNodes().at(0);
//	//PKB::next.insertNext(whileNode->getStartLine(),currNode->getStartLine(),true);
//	PROG_LINE plStart=whileNode->getStartLine();
//
//	while(currNode!=whileNode)
//	{
//		if(visited.find(currNode)!=visited.end())
//			break;
//		if(currNode->getType()==CFGNode::StandardNode)
//		{
//			visited.insert(currNode);
//			for(PROG_LINE pl=currNode->getStartLine();pl<=currNode->getEndLine();pl++)
//			{
//				PKB::stmtRefMap[pl].setCFGNode(currNode);
//				if(plStart!=pl)
//				{
//					PKB::next.insertNext(plStart,pl,true);
//					plStart=pl;
//				}
//			}
//			plStart=currNode->getEndLine();
//			currNode=currNode->getNextNodes().at(0);
//		}
//
//		else if(currNode->getType()==CFGNode::WhileNode)
//		{
//			visited.insert(currNode);
//			if(currNode->isStartNode()==false)
//			{
//				PKB::next.insertNext(plStart,currNode->getStartLine(),true);
//			}
//			PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
//			plStart=currNode->getStartLine();
//			PKB::next.insertNextStar(plStart,plStart,true);
//			traverseCFGWhile(currNode);
//			vector<PROG_LINE> prevLine=getPrevFromWhile(currNode);
//			for(int i=0;i<prevLine.size();i++)
//				PKB::next.insertNext(prevLine.at(i),currNode->getStartLine(),true);
//			currNode=currNode->getNextNodes().at(1);
//			//CFGNode* prevNode=currNode->getPreviousNodes().at(1);
//			//stack<CFGNode> prevNodeStack;
//			//prevNodeStack.push(prevNode);
//		}
//
//		else if(currNode->getType()==CFGNode::IfNode)
//		{
//			visited.insert(currNode);
//			if(currNode->isStartNode()==false)
//			{
//				PKB::next.insertNext(plStart,currNode->getStartLine(),true);
//			}
//			PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
//			currNode=traverseCFGIf(currNode);
//		}
//
//		else if(currNode->getType()==CFGNode::DummyNode)
//		{
//			currNode->getNextNodes().at(0);
//		}
//	}
//}
//
//vector<PROG_LINE> CFGBuilder::getPrevFromWhile(CFGNode* whileNode)
//{
//	vector<PROG_LINE> result;
//	stack<CFGNode*> prevNodesStack; 
//	vector<CFGNode*> prev = whileNode->getPreviousNodes();
//	for(int i=0; i<prev.size(); i++){
//		if(prev.at(i)->isDummy())
//			prevNodesStack.push(prev.at(i));
//		else
//			result.push_back(prev.at(i)->getEndLine());
//	}
//
//	while(prevNodesStack.size()>0)
//	{
//		CFGNode* dummy=prevNodesStack.top();
//		prevNodesStack.pop();
//
//		prev=dummy->getPreviousNodes();
//		for(int i=0; i<prev.size(); i++){
//			if(prev.at(i)->isDummy())
//				prevNodesStack.push(prev.at(i));
//			else
//				result.push_back(prev.at(i)->getEndLine());
//		}
//	}
//
//	return result;
//}
//
//CFGNode* CFGBuilder::traverseCFGIf(CFGNode* ifNode)
//{
//	PROG_LINE plStartL=ifNode->getStartLine();
//	PROG_LINE plStartR=ifNode->getStartLine();
//	
//	CFGNode* leftCurrNode=ifNode->getNextNodes().at(0);
//	CFGNode* rightCurrNode=ifNode->getNextNodes().at(1);
//
//
//	while(leftCurrNode->isDummy()==false && leftCurrNode->isEndNode()==false)
//	{
//		if(visited.find(leftCurrNode)!=visited.end())
//			break;
//
//		if(leftCurrNode->getType()==CFGNode::StandardNode)
//		{
//			visited.insert(leftCurrNode);
//			for(PROG_LINE pl=leftCurrNode->getStartLine();pl<=leftCurrNode->getEndLine();pl++)
//			{
//				PKB::stmtRefMap[pl].setCFGNode(leftCurrNode);
//				if(plStartL!=pl)
//				{
//					PKB::next.insertNext(plStartL,pl,true);	
//					plStartL=pl;
//				}
//			}
//			plStartL=leftCurrNode->getEndLine();
//			leftCurrNode=leftCurrNode->getNextNodes().at(0);
//		}
//		else if(leftCurrNode->getType()==CFGNode::WhileNode)
//		{
//			visited.insert(leftCurrNode);
//			PKB::next.insertNext(plStartL,leftCurrNode->getStartLine(),true);
//			PKB::stmtRefMap[leftCurrNode->getStartLine()].setCFGNode(leftCurrNode);
//			plStartL=leftCurrNode->getStartLine();
//			PKB::next.insertNextStar(plStartL,plStartL,true);
//			traverseCFGWhile(leftCurrNode);
//			vector<PROG_LINE> prevLine=getPrevFromWhile(leftCurrNode);
//			for(int i=0;i<prevLine.size();i++)
//				PKB::next.insertNext(prevLine.at(i),leftCurrNode->getStartLine(),true);
//			leftCurrNode=leftCurrNode->getNextNodes().at(1);
//		}
//
//		else if(leftCurrNode->getType()==CFGNode::IfNode)
//		{
//			visited.insert(leftCurrNode);
//			PKB::next.insertNext(plStartL,leftCurrNode->getStartLine(),true);
//			PKB::stmtRefMap[leftCurrNode->getStartLine()].setCFGNode(leftCurrNode);
//			leftCurrNode=traverseCFGIf(leftCurrNode);
//			plStartL=leftCurrNode->getStartLine();
//		}
//	}
//	
//	while(rightCurrNode->isDummy()==false && rightCurrNode->isEndNode()==false)
//	{
//		if(visited.find(rightCurrNode)!=visited.end())
//			break;
//		
//		if(rightCurrNode->getType()==CFGNode::StandardNode)
//		{
//			visited.insert(rightCurrNode);
//			for(PROG_LINE pl=rightCurrNode->getStartLine();pl<=rightCurrNode->getEndLine();pl++)
//			{
//				PKB::stmtRefMap[pl].setCFGNode(rightCurrNode);
//				if(plStartR!=pl)
//				{
//					PKB::next.insertNext(plStartR,pl,true);
//					plStartR=pl;
//				}
//			}
//
//			plStartR=rightCurrNode->getEndLine();
//			rightCurrNode=rightCurrNode->getNextNodes().at(0);
//		}
//		else if(rightCurrNode->getType()==CFGNode::WhileNode)
//		{
//			visited.insert(rightCurrNode);
//			if(rightCurrNode->isStartNode()==false)
//			{
//				PKB::next.insertNext(plStartR,rightCurrNode->getStartLine(),true);
//			}
//			PKB::stmtRefMap[rightCurrNode->getStartLine()].setCFGNode(rightCurrNode);
//			plStartR=rightCurrNode->getStartLine();
//			PKB::next.insertNextStar(plStartR,plStartR,true);
//			traverseCFGWhile(rightCurrNode);
//			vector<PROG_LINE> prevLine=getPrevFromWhile(rightCurrNode);
//			for(int i=0;i<prevLine.size();i++)
//				PKB::next.insertNext(prevLine.at(i),rightCurrNode->getStartLine(),true);
//			rightCurrNode=rightCurrNode->getNextNodes().at(1);
//		}
//		else if(rightCurrNode->getType()==CFGNode::IfNode)
//		{
//			visited.insert(rightCurrNode);
//			if(rightCurrNode->isStartNode()==false)
//			{
//				PKB::next.insertNext(plStartR,rightCurrNode->getStartLine(),true);
//			}
//			PKB::stmtRefMap[rightCurrNode->getStartLine()].setCFGNode(rightCurrNode);
//			rightCurrNode=traverseCFGIf(rightCurrNode);
//			plStartR=rightCurrNode->getStartLine();
//		}
//	}
//
//	CFGNode* currNode=leftCurrNode;
//	while(currNode->isDummy() && currNode->isEndNode()==false)
//	{
//		currNode=currNode->getNextNodes().at(0);
//	}
//
//	if(currNode->isDummy()==false)
//	{
//		PKB::stmtRefMap[currNode->getStartLine()].setCFGNode(currNode);
//		PKB::next.insertNext(plStartL,currNode->getStartLine(),true);
//		PKB::next.insertNext(plStartR,currNode->getStartLine(),true);
//	}
//	return currNode;
//}