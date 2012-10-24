#include "DesignExtractor.h"

 void DesignExtractor::extractDesign()
{
	//currently, not going to check nodes if it is of
	//the correct node type before typecasting
	//not sure whether to do so at all or not

	stack<StmtNode*> DFSstack;
	stack<StmtLstNode*> DFSstmtLstStack;
	stack<int> positionStack;
	stack<bool> traversingThenPartOfIfStack;
	stack<StmtNode*> tempStack;
	stack<ExprNode*> exprStack;
	ASTNode* tempASTNode;
	StmtNode* tempStmtNode;

	ASTNode* programNode = PKB::rootNode; //program
	/*if ((*programNode).getType() != ASTNode::NodeType::Program)
		throw SPAException("Expecting a Program Node");*/

	//just going to handle one procedure for now
	ASTNode* procedureNode = (*programNode).getChild(0); //procedure
	/*if ((*procedureNode).getType() != ASTNode::NodeType::Procedure)
		throw SPAException("Expecting a Procedure Node");*/
	PROC currentProc = (*procedureNode).getValue();

	tempASTNode = (*procedureNode).getChild(0); //stmtList
	/*if ((*tempNode).getType() != ASTNode::NodeType::StmtLst)
		throw SPAException("Expecting a StmtLst Node");*/
	StmtLstNode* firstLevelStmtListNode = (StmtLstNode*) tempASTNode;
	/*if (!(*firstLevelStmtListNode).isHasChildren())
		return;*/

	tempASTNode = (*firstLevelStmtListNode).getChild(0); //first child
	/*if ((*tempNode).getType() != ASTNode::NodeType::Stmt)
		throw SPAException("Expecting a Stmt Node");*/
	StmtNode* firstLevelStmtNode = (StmtNode*) tempASTNode;
	int firstLevelPosition = 0;

	StmtLstNode* currentStmtListNode = firstLevelStmtListNode;
	StmtNode* currentStmtNode = firstLevelStmtNode;
	STMT currentStmtNumber = (*currentStmtNode).getStmtNumber();
	int currentPosition = 0;
	
	while (true) {
		switch ((*currentStmtNode).getType()) {
		case ASTNode::NodeType::Assign: {
			ExprNode* modifiesVarNode = (ExprNode*) (*currentStmtNode).getChild(0);
			VAR modifiesVar = (*modifiesVarNode).getValue();

			PKB::modifies.insertProcModifies(currentProc, modifiesVar);
			PKB::modifies.insertStmtModifies(currentStmtNumber, modifiesVar);
			while (!DFSstack.empty()) {
				tempStmtNode = DFSstack.top();
				DFSstack.pop();
				STMT tempNumber = (*tempStmtNode).getStmtNumber();
				PKB::modifies.insertStmtModifies(tempNumber, modifiesVar);
				tempStack.push(tempStmtNode);
			}
			while (!tempStack.empty()) {
				tempStmtNode = tempStack.top();
				tempStack.pop();
				DFSstack.push(tempStmtNode);
			}

			//check rhs for all uses relationships
			if (!exprStack.empty())
				throw new SPAException("Huh, how come stack not empty?"); 
			exprStack.push( (ExprNode*) (*currentStmtNode).getChild(1) ); //rhs of assign
			do {
				ExprNode* exprNode = exprStack.top();
				exprStack.pop();
				if ((*exprNode).getType() == ASTNode::NodeType::Variable) {
					VAR usesVar = (*exprNode).getValue(); 
					PKB::uses.insertProcUses(currentProc, usesVar);
					PKB::uses.insertStmtUses(currentStmtNumber, usesVar);
					while (!DFSstack.empty()) {
						tempStmtNode = DFSstack.top();
						DFSstack.pop();
						STMT tempNumber = (*tempStmtNode).getStmtNumber();
						PKB::uses.insertStmtUses(tempNumber, usesVar);
						tempStack.push(tempStmtNode);
					}
					while (!tempStack.empty()) {
						tempStmtNode = tempStack.top();
						tempStack.pop();
						DFSstack.push(tempStmtNode);
					}
				} //a variable would not have any children
				else if ((*exprNode).isHasChildren()) {
					exprStack.push((ExprNode*) (*exprNode).getChild(0));
					exprStack.push((ExprNode*) (*exprNode).getChild(1));
				}
			} while (!exprStack.empty());
			break; }

		case ASTNode::NodeType::Call: {
			PKB::calls.insertCalls(currentProc, (*currentStmtNode).getValue());
			/*
			not yet handling the case with multiple procedures
			but the idea is to save the current DFSstack contents into another stack/queue
			when ALL the procedures have been completed
			then slowly add back all the modifies and uses information in.
			Note: must wait till all the procedures have been checked to ensure total coverage.
			//*/
			break; }

		case ASTNode::NodeType::While:
		case ASTNode::NodeType::If: {
			ExprNode* usesVarNode = (ExprNode*) (*currentStmtNode).getChild(0);
			VAR usesVar = (*usesVarNode).getValue(); 
			PKB::uses.insertProcUses(currentProc, usesVar);
			PKB::uses.insertStmtUses(currentStmtNumber, usesVar);
			while (!DFSstack.empty()) {
				tempStmtNode = DFSstack.top();
				DFSstack.pop();
				STMT tempNumber = (*tempStmtNode).getStmtNumber();
				PKB::uses.insertStmtUses(tempNumber, usesVar);
				tempStack.push(tempStmtNode);
			}
			while (!tempStack.empty()) {
				tempStmtNode = tempStack.top();
				tempStack.pop();
				DFSstack.push(tempStmtNode);
			}
			break; }
		default: throw SPAException("Unhandled Node type");
		}

		//go to the next stmt.
		//go down if can go down (container statement)
		if ((*currentStmtNode).getType() == ASTNode::NodeType::While
			|| (*currentStmtNode).getType() == ASTNode::NodeType::If) {
				DFSstack.push(currentStmtNode);
				DFSstmtLstStack.push(currentStmtListNode);
				positionStack.push(currentPosition);

				if ((*currentStmtNode).getType() == ASTNode::NodeType::If)
					traversingThenPartOfIfStack.push(true);
				//indicates is in the 'then' part of the if statement

				currentStmtListNode = (StmtLstNode*) (*currentStmtNode).getChild(1);
				currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(0);
				STMT newStmtNumber = (*currentStmtNode).getStmtNumber();
				currentPosition = 0;
				
				//add Parent and Follows relationship for all the children
				StmtNode* olderChild = currentStmtNode;
				STMT olderChildNumber = newStmtNumber;
				StmtNode* youngerChild;
				STMT youngerChildNumber;
				PKB::parent.insertParent(currentStmtNumber, olderChildNumber);
				for (int i = 1; i < (*currentStmtListNode).getSize(); i++) {
					youngerChild = (StmtNode*) (*currentStmtListNode).getChild(i);
					youngerChildNumber = (*youngerChild).getStmtNumber();
					PKB::parent.insertParent(currentStmtNumber, youngerChildNumber);
					PKB::follows.insertFollows(olderChildNumber, youngerChildNumber);
					olderChild = youngerChild;
					olderChildNumber = youngerChildNumber;
				}

				currentStmtNumber = newStmtNumber;
		} else {
			//go right if can go right, if cannot, go up until can go right
			bool notYetGotNextChild = true;
			while (notYetGotNextChild) {
				if (DFSstack.empty()) {
					if (firstLevelPosition + 1 < (*firstLevelStmtListNode).getSize()) {
						currentStmtNode = (StmtNode*) (*firstLevelStmtNode).getChild(++firstLevelPosition);
						//currentPosition++; //not relevant for first level traversing
						notYetGotNextChild = false;
					} else
						break; //the end!
				} else {
					if (currentPosition + 1 < (*currentStmtListNode).getSize()) { //try right
						currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(++currentPosition);
						currentStmtNumber = (*currentStmtNode).getStmtNumber();

						notYetGotNextChild = false;
					} else { //must go up
						StmtNode* parentNode = DFSstack.top();
					
						if ((*parentNode).getType() == ASTNode::NodeType::If &&
							traversingThenPartOfIfStack.top()) { //go to 'else' part of if
								traversingThenPartOfIfStack.pop();
								traversingThenPartOfIfStack.push(false);

								currentStmtListNode = (StmtLstNode*) (*parentNode).getChild(2);
								currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(0);
								currentStmtNumber = (*currentStmtNode).getStmtNumber();
								currentPosition = 0;

								notYetGotNextChild = false;
						} else {
							if ((*parentNode).getType() == ASTNode::NodeType::If)
								traversingThenPartOfIfStack.pop(); //was at 'else' part of if

							currentStmtNode = parentNode;
							currentStmtListNode = DFSstmtLstStack.top();
							currentPosition = positionStack.top();
							
							DFSstack.pop();
							DFSstmtLstStack.pop();
						}
					}
				}
			}
		}

	}
}

