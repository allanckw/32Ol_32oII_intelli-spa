#include "DesignExtractor.h"

static int totalNumOfProcs;
static unordered_map <PROC, unordered_set<PROC> > toProcAdjList;
static unordered_map <PROC, unordered_set<PROC> > fromProcAdjList;
static unordered_map <PROC, int> procCount;
static unordered_map <PROC, vector< stack<StmtNode*> > > savestate;

vector<ASTNode::NodeType> PKB::statementTable;
unordered_set<STMT> assignTable;
unordered_set<STMT> callTable;
unordered_set<STMT> whileTable;
unordered_set<STMT> ifTable;

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

	buildCallsTable(); //will build statement table and subtables as well
	PKB::calls.optimizeCallsTable();

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

}

void DesignExtractor::buildCallsTable() {

	stack<StmtNode*> DFSstack;
	stack<StmtLstNode*> DFSstmtLstStack;
	stack<int> positionStack;
	stack<bool> traversingThenPartOfIfStack;

	ASTNode* programNode = PKB::rootNode; //program
	
	for (PROC currentProc = 0; currentProc < totalNumOfProcs; currentProc++) {
		ASTNode* procedureNode = (*programNode).getChild(currentProc); //procedure
		StmtLstNode* firstLevelStmtListNode = (StmtLstNode*) (*procedureNode).getChild(0); //stmtList
		StmtNode* firstLevelStmtNode = (StmtNode*) (*firstLevelStmtListNode).getChild(0); //first statement
		int firstLevelPosition = 0;

		StmtLstNode* currentStmtListNode = firstLevelStmtListNode;
		StmtNode* currentStmtNode = firstLevelStmtNode;
		int currentPosition = 0;
		bool haveNextChildren = true;

		while (haveNextChildren) {
			switch ((*currentStmtNode).getType()) {

			case ASTNode::Call: {
				PROC calledProc = (*currentStmtNode).getValue();
				PKB::calls.insertCalls(currentProc, calledProc);

				toProcAdjList[currentProc].insert(calledProc);
				fromProcAdjList[calledProc].insert(currentProc);

				if (procCount.count(calledProc) == 0)
					procCount[calledProc] = 1;
				else
					procCount[calledProc]++;

				savestate[(*currentStmtNode).getValue()].push_back(
					DFSstack);
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

					currentStmtListNode = (StmtLstNode*) (*currentStmtNode).getChild(1);
					currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(0);
					currentPosition = 0;
			} else {
				//go right if can go right, if cannot, go up until can go right
				bool notYetGotNextChild = true;
				while (notYetGotNextChild) {
					if (DFSstack.empty()) {
						if (firstLevelPosition + 1 < (*firstLevelStmtListNode).getSize()) {
							currentStmtNode = (StmtNode*) (*firstLevelStmtListNode).getChild(++firstLevelPosition);
							currentPosition = firstLevelPosition;
							notYetGotNextChild = false;
						} else { //end
							haveNextChildren = false;
							notYetGotNextChild = false;
						}
					} else {
						if (currentPosition + 1 < (*currentStmtListNode).getSize()) { //try right
							currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(++currentPosition);
							notYetGotNextChild = false;
						} else { //must go up
							StmtNode* parentNode = DFSstack.top();

							if ((*parentNode).getType() == ASTNode::If &&
								traversingThenPartOfIfStack.top()) { //go to 'else' part of if
									traversingThenPartOfIfStack.pop();
									traversingThenPartOfIfStack.push(false);

									currentStmtListNode = (StmtLstNode*) (*parentNode).getChild(2);
									currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(0);
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

	stack<StmtNode*> DFSstack;
	stack<StmtLstNode*> DFSstmtLstStack;
	stack<int> positionStack;
	stack<bool> traversingThenPartOfIfStack;
	stack<StmtNode*> tempStack;
	stack<ExprNode*> exprStack;
	ASTNode* tempASTNode;
	StmtNode* tempStmtNode;

	ASTNode* programNode = PKB::rootNode; //program
	
	ASTNode* procedureNode = (*programNode).getChild(currentProc); //procedure

	tempASTNode = (*procedureNode).getChild(0); //stmtList
	StmtLstNode* firstLevelStmtListNode = (StmtLstNode*) tempASTNode;

	tempASTNode = (*firstLevelStmtListNode).getChild(0); //first child
	StmtNode* firstLevelStmtNode = (StmtNode*) tempASTNode;
	int firstLevelPosition = 0;

	StmtLstNode* currentStmtListNode = firstLevelStmtListNode;
	StmtNode* currentStmtNode = firstLevelStmtNode;
	STMT currentStmtNumber = (*currentStmtNode).getStmtNumber();
	int currentPosition = 0;
	bool haveNextChildren = true;

	while (haveNextChildren) {
		switch ((*currentStmtNode).getType()) {
		case ASTNode::Assign: {
			PKB::assignTable.insert(currentStmtNumber);
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
			//adding all the parent procs. JOY!
			for (auto it = ancestors.begin(); it != ancestors.end(); it++) {
				PROC ancestor = *it;
				PKB::modifies.insertProcModifies(ancestor, modifiesVar);
				vector< stack<StmtNode*> > savestates = savestate[ancestor];
				for (auto it2 = savestates.begin(); it2 != savestates.end(); it2++) {
					stack<StmtNode*> state = *it2;
					while (!state.empty()) {
						tempStmtNode = DFSstack.top();
						DFSstack.pop();
						STMT tempNumber = (*tempStmtNode).getStmtNumber();
						PKB::modifies.insertStmtModifies(tempNumber, modifiesVar);
					}
				}
			}

			//check rhs for all uses relationships
			if (!exprStack.empty())
				throw new SPAException("Huh, how come stack not empty?"); 
			exprStack.push( (ExprNode*) (*currentStmtNode).getChild(1) ); //rhs of assign
			do {
				ExprNode* exprNode = exprStack.top();
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
						PKB::uses.insertStmtUses(ancestor, usesVar);
						vector< stack<StmtNode*> > savestates = savestate[ancestor];
						for (auto it2 = savestates.begin(); it2 != savestates.end(); it2++) {
							stack<StmtNode*> state = *it2;
							while (!state.empty()) {
								tempStmtNode = DFSstack.top();
								DFSstack.pop();
								STMT tempNumber = (*tempStmtNode).getStmtNumber();
								PKB::uses.insertStmtUses(tempNumber, usesVar);
							}
						}
					}
				} //a variable would not have any children
				else if ((*exprNode).isHasChildren()) {
					exprStack.push((ExprNode*) (*exprNode).getChild(1));
					exprStack.push((ExprNode*) (*exprNode).getChild(0));
				}
			} while (!exprStack.empty());
			break; }

		case ASTNode::While:
		case ASTNode::If: {
			if ((*currentStmtNode).getType() == ASTNode::While)
				PKB::whileTable.insert(currentStmtNumber);
			else
				PKB::ifTable.insert(currentStmtNumber);

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
			//adding all the parent procs. JOY!
			for (auto it = ancestors.begin(); it != ancestors.end(); it++) {
				PROC ancestor = *it;
				PKB::uses.insertStmtUses(ancestor, usesVar);
				vector< stack<StmtNode*> > savestates = savestate[ancestor];
				for (auto it2 = savestates.begin(); it2 != savestates.end(); it2++) {
					stack<StmtNode*> state = *it2;
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

				if ((*currentStmtNode).getType() == ASTNode::If)
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
						currentStmtNode = (StmtNode*) (*firstLevelStmtListNode).getChild(++firstLevelPosition);
						currentPosition = firstLevelPosition;
						currentStmtNumber = (*currentStmtNode).getStmtNumber();
						notYetGotNextChild = false;
					} else { //end
						haveNextChildren = false;
						notYetGotNextChild = false;
					}
				} else {
					if (currentPosition + 1 < (*currentStmtListNode).getSize()) { //try right
						currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(++currentPosition);
						currentStmtNumber = (*currentStmtNode).getStmtNumber();
						notYetGotNextChild = false;
					} else { //must go up
						StmtNode* parentNode = DFSstack.top();

						if ((*parentNode).getType() == ASTNode::If &&
							traversingThenPartOfIfStack.top()) { //go to 'else' part of if
								traversingThenPartOfIfStack.pop();
								traversingThenPartOfIfStack.push(false);

								currentStmtListNode = (StmtLstNode*) (*parentNode).getChild(2);
								currentStmtNode = (StmtNode*) (*currentStmtListNode).getChild(0);
								currentStmtNumber = (*currentStmtNode).getStmtNumber();
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

