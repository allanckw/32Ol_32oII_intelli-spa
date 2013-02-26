#pragma once
#include "MyCFGBuilder.h"
#include "MyCFG.h"
#include "PKB.h"
#include "ASTNode.h"
#include "ASTStmtLstNode.h"

void MyCFGBuilder::buildCFG()
{
	stack<ASTStmtNode*> DFSstack;
	stack<ASTStmtLstNode*> DFSstmtLstStack;
	stack<int> positionStack;
	stack<bool> traversingThenPartOfIfStack;

	unordered_map<ASTNode*, MyCFG*> map;
	stack<MyCFG*> stack;
	PKB::bigTable.push_back(NULL);

	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
		ASTNode* procedureNode = PKB::rootNode->getChild(currentProc); //procedure
		ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) procedureNode->getChild(0); //stmtList
		int firstLevelPosition = 0;

		ASTStmtLstNode* currentStmtListNode = firstLevelStmtListNode;
		ASTStmtNode* currentStmtNode = (ASTStmtNode*) firstLevelStmtListNode->getChild(0); //first statement
		int currentPosition = 0;
		int currentStatementNumber = currentStmtNode->getStmtNumber();
		bool haveNextChildren = true;
		
		MyCFG* newCFG;
		MyCFG* currCFG = new MyCFG(currentProc);
		PKB::addToCFGList(currCFG);

		while (haveNextChildren) {
			switch (currentStmtNode->getType()) {
			case ASTNode::While:
				if (currCFG->first != 0) {
					currCFG->type = MyCFG::Normal;//used to be WhilePrev;
					stack.push(currCFG);
					newCFG = new MyCFG(currCFG); //while
					currCFG->children.oneChild = newCFG;
					currCFG = newCFG;
				}
				map.insert(pair<ASTNode*, MyCFG*>(currentStmtNode, currCFG));
				currCFG->first = currCFG->last = currentStatementNumber;
				currCFG->type = MyCFG::While;

				newCFG = new MyCFG(currCFG); //while out
				currCFG->children.whileChildren.whileOut = newCFG;

				if (currCFG->whileAncestor == NULL)
					currCFG->whileAncestor = currCFG;

				newCFG = new MyCFG(currCFG); //while in
				currCFG->children.whileChildren.whileIn = newCFG;
				
				PKB::bigTable.push_back(currCFG);
				stack.push(currCFG);
				currCFG = currCFG->children.whileChildren.whileIn;
				break;

			case ASTNode::If:
				if (currCFG->first == 0)
					currCFG->first = currentStatementNumber;
				currCFG->last = currentStatementNumber;
				currCFG->type = MyCFG::If;
				map.insert(pair<ASTNode*, MyCFG*>(currentStmtNode, currCFG));

				newCFG = new MyCFG(currCFG); //if then
				currCFG->children.ifChildren.ifThen = newCFG;

				newCFG = new MyCFG(currCFG); //if else
				currCFG->children.ifChildren.ifElse = newCFG;

				newCFG = new MyCFG(currCFG); //if later
				currCFG->children.ifChildren.ifLater = newCFG;
				
				PKB::bigTable.push_back(currCFG);
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
				PKB::bigTable.push_back(currCFG);
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
								currCFG->type = MyCFG::Normal;
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
								currCFG->type = MyCFG::Normal;
							stack.push(currCFG);
							ASTStmtNode* parentNode = DFSstack.top();
							MyCFG* parentCFG = map[parentNode];
							
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
									MyCFG* temp = parentCFG->children.ifChildren.ifLater;
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

		MyCFG* temp;
		while (!stack.empty()) {
			currCFG = stack.top();
			stack.pop();

			if (currCFG->whileAncestor == NULL)
				switch (currCFG->type) {
				case MyCFG::Normal:
					temp = currCFG->children.oneChild;
					if (temp->first != 0) {
						if (temp->nextList == NULL)
							currCFG->nextList = new IntervalList(temp->first, temp->first);
						else
							currCFG->nextList = temp->nextList->Union(temp->first, temp->first);
					}
					break;
				case MyCFG::If:
					MyCFG* temp = currCFG->children.ifChildren.ifLater;
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
				MyCFG* whileAncestor = currCFG->whileAncestor;
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