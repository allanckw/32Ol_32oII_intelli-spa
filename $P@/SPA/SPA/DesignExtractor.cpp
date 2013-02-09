#pragma once
#include "DesignExtractor.h"
#include "CFGBuilder.h"

int DesignExtractor::totalNumOfProcs;
unordered_map <PROC, unordered_set<PROC> > DesignExtractor::toProcAdjList;
unordered_map <PROC, unordered_set<PROC> > DesignExtractor::fromProcAdjList;
unordered_map <PROC, int> DesignExtractor::procCount;
unordered_map <PROC, vector< stack<ASTStmtNode*> > > DesignExtractor::savestate;

/**
* Extracts the design of the static root node in PKB Class and populate the respective tables
* (Modifies, Uses, Follows, Parents, Calls)
*/
void DesignExtractor::extractDesign()
{
	//Do two traversals

	//First traversal is to build up a call hierachy, so that we know
	//which procedures call which procedures.

	//Second traversal will traverse in an order such that before a
	//procedure is traversed, all procedures which call that procedure
	//will have been traversed already.

	//The reason for this complicated behaviour is to correctly set
	//the following relationships (Modifies and Uses)

	//4. For a procedure call statement s ‘call p’, Modifies (s, v) is
	//defined in the same way as Modifies (p, v).

	//3. For a procedure p, Modifies (p, v) holds if there is a statement
	//s in p or in a procedure called (directly or indirectly) from p
	//such that Modifies (s, v) holds.


	//Currently, not going to check nodes if it is of the correct node
	//type before typecasting. Not sure whether to do so at all or not

	totalNumOfProcs = PKB::procedures.getSize();
	PKB::statementNodes.push_back(0); //statements start from 1, so put dummy node in index 0
	PKB::stmtRefMap.push_back(StmtRef(-1, -1, 0)); //dummy node

	buildFirstRound(); //will build call table, statement and subtables (call, assign, while, if lists)
	
	//toposort
	vector<PROC> toposort;
	queue<PROC> insertableNodes;
	for (PROC proc = 0; proc < totalNumOfProcs; proc++)
		if (procCount.count(proc) == 0)
			insertableNodes.push(proc);
	while (!insertableNodes.empty()) {
		PROC proc = insertableNodes.front();
		insertableNodes.pop();
		toposort.push_back(proc);
		if (toProcAdjList.count(proc) > 0) {
			unordered_set<PROC> calledProcs = toProcAdjList[proc];
			for (auto it = calledProcs.begin(); it != calledProcs.end(); it++) {
				PROC calledProc = *it;
				int calledProcCount = procCount[calledProc];
				if (calledProcCount == 1) {
					insertableNodes.push(calledProc);
				} else {
					procCount[calledProc] = calledProcCount - 1;
				}
			}
		}
	}
	
	for (auto it = toposort.begin(); it != toposort.end(); it++)
		buildOtherTables(*it);

	DesignExtractor::CompleteExtraction();
}

/**
* Tables that require optimisation are called to do so here.
* Various other data in PKB are set.
*/
void DesignExtractor::CompleteExtraction()
{
	PKB::maxProgLines = PKB::statementTable.size();
	PKB::calls.optimizeCallsTable();
	PKB::modifies.optimizeModifiesTable();
	PKB::uses.optimizeUsesTable();

	CFGBuilder::traverseCFGToPopulateNext();
}

/**
* Traverse the entire AST and extracts the call relationship between procedures.
*/
void DesignExtractor::buildFirstRound() {

	stack<ASTStmtNode*> DFSstack;
	stack<ASTStmtLstNode*> DFSstmtLstStack;
	stack<int> positionStack;
	stack<bool> traversingThenPartOfIfStack;

	ASTNode* programNode = PKB::rootNode; //program
	
	for (PROC currentProc = 0; currentProc < totalNumOfProcs; currentProc++) {
		ASTNode* procedureNode = (*programNode).getChild(currentProc); //procedure
		ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) (*procedureNode).getChild(0); //stmtList
		int firstLevelPosition = 0;

		ASTStmtLstNode* currentStmtListNode = firstLevelStmtListNode;
		ASTStmtNode* currentStmtNode = (ASTStmtNode*) (*firstLevelStmtListNode).getChild(0); //first statement
		int currentPosition = 0;
		int firstStatementNumber = (*currentStmtNode).getStmtNumber();
		int lastStatementNumber;
		bool haveNextChildren = true;

		while (haveNextChildren) {
			lastStatementNumber = (*currentStmtNode).getStmtNumber();
			switch ((*currentStmtNode).getType()) {

			case ASTNode::Call: {
				PROC calledProc = (*currentStmtNode).getValue();
				PKB::calls.insertCalls(currentProc, calledProc);
				PKB::uses.linkCallStmtToProcUses((*currentStmtNode).getStmtNumber(), calledProc);
				PKB::modifies.linkCallStmtToProcModifies((*currentStmtNode).getStmtNumber(), calledProc);

				if (toProcAdjList[currentProc].count(calledProc) == 0) {
					toProcAdjList[currentProc].insert(calledProc);
					fromProcAdjList[calledProc].insert(currentProc);

					if (procCount.count(calledProc) == 0)
						procCount[calledProc] = 1;
					else
						procCount[calledProc]++;
				}

				savestate[calledProc].push_back(DFSstack);
				break; }
										  
			case ASTNode::Assign:
			case ASTNode::While:
			case ASTNode::If:
				break;

			default: throw SPAException("Unhandled Node type");
			}

			//build statement table as well
			//vector<ASTNode::NodeType> statementTable = PKB::statementTable;
			PKB::statementTable.push_back((*currentStmtNode).getType());

			//go to the next stmt.
			//go down if can go down (container statement)
			if ((*currentStmtNode).getType() == ASTNode::While
				|| (*currentStmtNode).getType() == ASTNode::If) {
					DFSstack.push(currentStmtNode);
					DFSstmtLstStack.push(currentStmtListNode);
					positionStack.push(currentPosition);

					if ((*currentStmtNode).getType() == ASTNode::If)
						traversingThenPartOfIfStack.push(true);
					//indicates is in the 'then' part of the if statement

					currentStmtListNode = (ASTStmtLstNode*) (*currentStmtNode).getChild(1);
					currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(0);
					currentPosition = 0;
			} else {
				//go right if can go right, if cannot, go up until can go right
				bool notYetGotNextChild = true;
				while (notYetGotNextChild) {
					if (DFSstack.empty()) {
						if (firstLevelPosition + 1 < (*firstLevelStmtListNode).getSize()) {
							currentStmtNode = (ASTStmtNode*) (*firstLevelStmtListNode).getChild(++firstLevelPosition);
							currentPosition = firstLevelPosition;
							notYetGotNextChild = false;
						} else { //end
							PKB::TheBeginningAndTheEnd.push_back(make_pair(
								firstStatementNumber, lastStatementNumber));

							haveNextChildren = false;
							notYetGotNextChild = false;
						}
					} else {
						if (currentPosition + 1 < (*currentStmtListNode).getSize()) { //try right
							currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(++currentPosition);
							notYetGotNextChild = false;
						} else { //must go up
							ASTStmtNode* parentNode = DFSstack.top();

							if ((*parentNode).getType() == ASTNode::If &&
								traversingThenPartOfIfStack.top()) { //go to 'else' part of if
									traversingThenPartOfIfStack.pop();
									traversingThenPartOfIfStack.push(false);

									currentStmtListNode = (ASTStmtLstNode*) (*parentNode).getChild(2);
									currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(0);
									currentPosition = 0;

									notYetGotNextChild = false;
							} else {
								if ((*parentNode).getType() == ASTNode::If)
									traversingThenPartOfIfStack.pop(); //was at 'else' part of if

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
}

/**
* Traverse the section of the AST concerning a single procedure.
* All other tables are filled during this traversal.
* @param currentProc procedure to be analysed
*/
void DesignExtractor::buildOtherTables(PROC currentProc) {

	//currently, not going to check nodes if it is of
	//the correct node type before typecasting
	//not sure whether to do so at all or not

	//build list of ancestors
	//(to be optimised in the future - do a set union of all the parents)
	bool *isNotAnAncestor = new bool[totalNumOfProcs];
	for (int i = 0; i < totalNumOfProcs; i++)
		isNotAnAncestor[i] = true;
	unordered_set<PROC> ancestors;
	stack<PROC> tempStackForProc;
	tempStackForProc.push(currentProc);
	isNotAnAncestor[currentProc] = false;
	do {
		PROC proc = tempStackForProc.top();
		tempStackForProc.pop();
		if (fromProcAdjList.count(proc) > 0) {
			unordered_set<PROC> parents = fromProcAdjList[proc];
			for (auto it = parents.begin(); it != parents.end(); it++) {
				PROC parent = *it;
				if (isNotAnAncestor[parent]) {
					tempStackForProc.push(parent);
					isNotAnAncestor[parent] = false;
					ancestors.insert(parent);
				}
			}
		}
	} while (!tempStackForProc.empty());
	delete [] isNotAnAncestor;

	stack<ASTStmtNode*> DFSstack;
	stack<ASTStmtLstNode*> DFSstmtLstStack;
	stack<int> positionStack;
	stack<bool> traversingThenPartOfIfStack;
	stack<ASTStmtNode*> tempStack;
	stack<ASTExprNode*> exprStack;
	ASTNode* tempASTNode;
	ASTStmtNode* tempStmtNode;

	ASTNode* programNode = PKB::rootNode; //program
	
	ASTNode* procedureNode = (*programNode).getChild(currentProc); //procedure

	tempASTNode = (*procedureNode).getChild(0); //stmtList
	ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) tempASTNode;

	tempASTNode = (*firstLevelStmtListNode).getChild(0); //first child
	int firstLevelPosition = 0;

	ASTStmtLstNode* currentStmtListNode = firstLevelStmtListNode;
	ASTStmtNode* currentStmtNode = (ASTStmtNode*) (*firstLevelStmtListNode).getChild(0); //first statement
	STMT currentStmtNumber = (*currentStmtNode).getStmtNumber();
	int currentPosition = 0;
	bool haveNextChildren = true;

	while (haveNextChildren) {
		PKB::statementNodes.push_back(currentStmtNode);
		PKB::stmtRefMap.push_back(StmtRef(currentStmtNumber, currentStmtNumber, currentStmtNode));
		switch ((*currentStmtNode).getType()) {
		case ASTNode::Assign: {
			PKB::assignTable.insert(currentStmtNumber);
			PKB::assignNodes.insert(pair<STMT, ASTNode*>(currentStmtNumber, currentStmtNode));

			ASTExprNode* modifiesVarNode = (ASTExprNode*) (*currentStmtNode).getChild(0);
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
			//adding all the parent procs. JOY!
			for (auto it = ancestors.begin(); it != ancestors.end(); it++) {
				PROC ancestor = *it;
				PKB::modifies.insertProcModifies(ancestor, modifiesVar);
			}
			vector< stack<ASTStmtNode*> >& savestates = savestate[currentProc];
			for (auto it2 = savestates.begin(); it2 != savestates.end(); it2++) {
				stack<ASTStmtNode*> state = *it2;
				while (!state.empty()) {
					tempStmtNode = state.top();
					state.pop();
					STMT tempNumber = (*tempStmtNode).getStmtNumber();
					if (PKB::modifies.isModifiedStmt(tempNumber, modifiesVar))
						break;
					PKB::modifies.insertStmtModifies(tempNumber, modifiesVar);
				}
			}

			//check rhs for all uses relationships
			if (!exprStack.empty())
				throw new SPAException("Huh, how come stack not empty?"); 
			exprStack.push( (ASTExprNode*) (*currentStmtNode).getChild(1) ); //rhs of assign
			do {
				ASTExprNode* exprNode = exprStack.top();
				exprStack.pop();
				if ((*exprNode).getType() == ASTNode::Variable) {
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
					//adding all the parent procs. JOY!
					for (auto it = ancestors.begin(); it != ancestors.end(); it++) {
						PROC ancestor = *it;
						PKB::uses.insertProcUses(ancestor, usesVar);
					}
					vector< stack<ASTStmtNode*> >& savestates = savestate[currentProc];
					for (auto it2 = savestates.begin(); it2 != savestates.end(); it2++) {
						stack<ASTStmtNode*> state = *it2;
						while (!state.empty()) {
							tempStmtNode = state.top();
							state.pop();
							STMT tempNumber = (*tempStmtNode).getStmtNumber();
							PKB::uses.insertStmtUses(tempNumber, usesVar);
						}
					}
				} else if ((*exprNode).getType() == ASTNode::Constant) {
					PKB::constantsTable[(*exprNode).getValue()].push_back(currentStmtNumber);
				} else {
					exprStack.push((ASTExprNode*) (*exprNode).getChild(1));
					exprStack.push((ASTExprNode*) (*exprNode).getChild(0));
				}
			} while (!exprStack.empty());
			break; }

		case ASTNode::While:
		case ASTNode::If: {
			if ((*currentStmtNode).getType() == ASTNode::While) {
				PKB::whileTable.insert(currentStmtNumber);
				PKB::whileNodes.insert(pair<STMT, ASTNode*>(currentStmtNumber, currentStmtNode));
			} else {
				PKB::ifTable.insert(currentStmtNumber);
				PKB::ifNodes.insert(pair<STMT, ASTNode*>(currentStmtNumber, currentStmtNode));
			}

			ASTExprNode* usesVarNode = (ASTExprNode*) (*currentStmtNode).getChild(0);
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
			//adding all the parent procs. JOY!
			for (auto it = ancestors.begin(); it != ancestors.end(); it++) {
				PROC ancestor = *it;
				PKB::uses.insertProcUses(ancestor, usesVar);
				vector< stack<ASTStmtNode*> > savestates = savestate[ancestor];
				for (auto it2 = savestates.begin(); it2 != savestates.end(); it2++) {
					stack<ASTStmtNode*> state = *it2;
					while (!state.empty()) {
						tempStmtNode = DFSstack.top();
						DFSstack.pop();
						STMT tempNumber = (*tempStmtNode).getStmtNumber();
						PKB::uses.insertStmtUses(tempNumber, usesVar);
					}
				}
			}
			break; }

		case ASTNode::Call: {
			PKB::callTable.insert(currentStmtNumber);
			PKB::callNodes.insert(pair<STMT, ASTNode*>(currentStmtNumber, currentStmtNode));
			PKB::calls.insertStmtCall(currentStmtNumber, (*currentStmtNode).getValue());
			break; }

		default: throw SPAException("Unhandled Node type");
		}

		//go to the next stmt.
		//go down if can go down (container statement)
		if ((*currentStmtNode).getType() == ASTNode::While
			|| (*currentStmtNode).getType() == ASTNode::If) {
				DFSstack.push(currentStmtNode);
				DFSstmtLstStack.push(currentStmtListNode);
				positionStack.push(currentPosition);

				if ((*currentStmtNode).getType() == ASTNode::If) {
					//indicates is in the 'then' part of the if statement
					traversingThenPartOfIfStack.push(true);

					//add Parent and Follows relationship for children in 'else' part
					ASTStmtLstNode* tempStmtListNode =
						(ASTStmtLstNode*) (*currentStmtNode).getChild(2);
					ASTStmtNode* olderChild = (ASTStmtNode*) (*tempStmtListNode).getChild(0);
					STMT olderChildNumber = (*olderChild).getStmtNumber();
					ASTStmtNode* youngerChild;
					STMT youngerChildNumber;
					PKB::parent.insertParent(currentStmtNumber, olderChildNumber);
					for (int i = 1; i < (*tempStmtListNode).getSize(); i++) {
						youngerChild = (ASTStmtNode*) (*tempStmtListNode).getChild(i);
						youngerChildNumber = (*youngerChild).getStmtNumber();
						PKB::parent.insertParent(currentStmtNumber, youngerChildNumber);
						PKB::follows.insertFollows(olderChildNumber, youngerChildNumber);
						olderChild = youngerChild;
						olderChildNumber = youngerChildNumber;
					}
				}

				currentStmtListNode = (ASTStmtLstNode*) (*currentStmtNode).getChild(1);
				currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(0);
				STMT newStmtNumber = (*currentStmtNode).getStmtNumber();
				currentPosition = 0;

				//add Parent and Follows relationship for all the children
				ASTStmtNode* olderChild = currentStmtNode;
				STMT olderChildNumber = newStmtNumber;
				ASTStmtNode* youngerChild;
				STMT youngerChildNumber;
				PKB::parent.insertParent(currentStmtNumber, olderChildNumber);
				for (int i = 1; i < (*currentStmtListNode).getSize(); i++) {
					youngerChild = (ASTStmtNode*) (*currentStmtListNode).getChild(i);
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
						currentStmtNode = (ASTStmtNode*) (*firstLevelStmtListNode).getChild(++firstLevelPosition);
						currentPosition = firstLevelPosition;
						STMT oldStmtNumber = currentStmtNumber;
						currentStmtNumber = (*currentStmtNode).getStmtNumber();

						PKB::follows.insertFollows(oldStmtNumber, currentStmtNumber);

						notYetGotNextChild = false;
					} else { //end
						haveNextChildren = false;
						notYetGotNextChild = false;
					}
				} else {
					if (currentPosition + 1 < (*currentStmtListNode).getSize()) { //try right
						currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(++currentPosition);
						currentStmtNumber = (*currentStmtNode).getStmtNumber();
						notYetGotNextChild = false;
					} else { //must go up
						ASTStmtNode* parentNode = DFSstack.top();

						if ((*parentNode).getType() == ASTNode::If &&
							traversingThenPartOfIfStack.top()) { //go to 'else' part of if
								traversingThenPartOfIfStack.pop();
								traversingThenPartOfIfStack.push(false);

								currentStmtListNode = (ASTStmtLstNode*) (*parentNode).getChild(2);
								currentStmtNode = (ASTStmtNode*) (*currentStmtListNode).getChild(0);
								currentStmtNumber = (*currentStmtNode).getStmtNumber();
								currentPosition = 0;

								notYetGotNextChild = false;
						} else {
							if ((*parentNode).getType() == ASTNode::If)
								traversingThenPartOfIfStack.pop(); //was at 'else' part of if

							currentStmtNode = parentNode;
							currentStmtNumber = (*currentStmtNode).getStmtNumber();
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