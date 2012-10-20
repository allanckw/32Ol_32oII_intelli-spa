#include "DesignExtractor.h"

//TODO: For Simon, WX, HW Traverse After its built to fill in respective tables
DesignExtractor::DesignExtractor(CallsTable& ca, ModifiesTable& mo,
	UsesTable& us, ParentTable& pa, FollowsTable& fo)
{
	//c = ca;
	//m = mo;
	//u = us;
	//p = pa;
	//f = fo;

	//stack<StmtNode*> DFSstack;
	//stack<int> positionStack;
	//stack<StmtNode*> nextStmtStack;
	//stack<StmtNode*> tempStack;
	//ASTNode* tempNode;

	//ASTNode* programNode = PKB::rootNode; //program
	//if ((*programNode).getType() != ASTNode::NodeType::Program)
	//	throw SPAException("Expecting a Program Node");

	////just going to handle one procedure for now
	//ASTNode* procedureNode = (*programNode).getChild(0); //procedure
	//if ((*procedureNode).getType() != ASTNode::NodeType::Procedure)
	//	throw SPAException("Expecting a Procedure Node");
	//PROC currentProc = (*procedureNode).getValue();

	//ASTNode* stmtListNode = (*procedureNode).getChild(0); //stmtList
	//if ((*stmtListNode).getType() != ASTNode::NodeType::StmtLst)
	//	throw SPAException("Expecting a StmtLst Node");

	//if (!(*stmtListNode).isHasChildren())
	//	return;

	//tempNode = (*stmtListNode).getChild(0); //first child
	//if ((*tempNode).getType() != ASTNode::NodeType::Stmt)
	//	throw SPAException("Expecting a Stmt Node");
	//StmtNode* currentStmtNode = (StmtNode*) tempNode;
	//STMT currentStmtNumber = (*currentStmtNode).getStmtNumber();
	//int rootPosition = 0;
	//
	//while (true) {
	//	switch ((*currentStmtNode).getType()) {
	//	case ASTNode::NodeType::Assign: {
	//		//todo: check all nodes if its of correct node type before typecasting
	//		ExprNode* modifiesVarNode = (ExprNode*) (*currentStmtNode).getChild(0);
	//		VAR modifiesVar = (*modifiesVarNode).getValue();

	//		m.insertProcModifies(currentProc, modifiesVar);
	//		m.insertStmtModifies(currentStmtNumber, modifiesVar);
	//		while (!DFSstack.empty()) {
	//			StmtNode* tempNode = DFSstack.top();
	//			DFSstack.pop();
	//			STMT tempNumber = (*tempNode).getStmtNumber();
	//			m.insertStmtModifies(tempNumber, modifiesVar);
	//			tempStack.push(tempNode);
	//		}
	//		while (!tempStack.empty()) {
	//			StmtNode* tempNode = tempStack.top();
	//			tempStack.pop();
	//			DFSstack.push(tempNode);
	//		}

	//		//check rhs for all uses relationships
	//		stack<ExprNode*> exprStack;
	//		exprStack.push( (ExprNode*) (*currentStmtNode).getChild(1) );
	//		do {
	//			ExprNode* exprNode = exprStack.top();
	//			exprStack.pop();
	//			if ((*exprNode).getType() == ASTNode::NodeType::Variable) {
	//				VAR usesVar = (*exprNode).getValue(); 
	//				u.insertProcUses(currentProc, usesVar);
	//				u.insertStmtUses(currentStmtNumber, usesVar);
	//				while (!DFSstack.empty()) {
	//					StmtNode* tempNode = DFSstack.top();
	//					DFSstack.pop();
	//					STMT tempNumber = (*tempNode).getStmtNumber();
	//					u.insertStmtUses(tempNumber, usesVar);
	//					tempStack.push(tempNode);
	//				}
	//				while (!tempStack.empty()) {
	//					StmtNode* tempNode = tempStack.top();
	//					tempStack.pop();
	//					DFSstack.push(tempNode);
	//				}
	//			}
	//			else if ((*exprNode).isHasChildren()) {
	//				exprNode = (ExprNode*) (*exprNode).getChild(0);
	//				exprStack.push(exprNode);
	//				int index = 0;
	//				while ((*exprNode).isHasRightSibling()) {
	//					exprNode = (ExprNode*) (*exprNode).getChild(++index);
	//					exprStack.push(exprNode);
	//				}
	//			}
	//		} while (!exprStack.empty());
	//		break; }

	//	case ASTNode::NodeType::Call: {
	//		c.insertCalls(currentProc, (*currentStmtNode).getValue());
	//		/*
	//		not yet handling the case with multiple procedures
	//		but the idea is to save the current DFSstack contents into another stack/queue
	//		when ALL the procedures have been completed
	//		then slowly add back all the modifies and uses information in.
	//		Note: must wait till all the procedures have been checked to ensure total coverage.
	//		//*/
	//		break; }

	//	case ASTNode::NodeType::While:
	//	case ASTNode::NodeType::If: {
	//		if ((*currentStmtNode).getType() == ASTNode::NodeType::If)
	//			nextStmtStack.push((StmtNode*) (*currentStmtNode).getChild(2));
	//		nextStmtStack.push((StmtNode*) (*currentStmtNode).getChild(1));

	//		ExprNode* usesVarNode = (ExprNode*) (*currentStmtNode).getChild(0);
	//		VAR usesVar = (*usesVarNode).getValue(); 
	//		u.insertProcUses(currentProc, usesVar);
	//		u.insertStmtUses(currentStmtNumber, usesVar);
	//		while (!DFSstack.empty()) {
	//			StmtNode* tempNode = DFSstack.top();
	//			DFSstack.pop();
	//			STMT tempNumber = (*tempNode).getStmtNumber();
	//			u.insertStmtUses(tempNumber, usesVar);
	//			tempStack.push(tempNode);
	//		}
	//		while (!tempStack.empty()) {
	//			StmtNode* tempNode = tempStack.top();
	//			tempStack.pop();
	//			DFSstack.push(tempNode);
	//		}
	//		break; }
	//	default: throw SPAException("Unhandled Node type");
	//	}

	//	//add Parent relationship if current node is a child
	//	if (!DFSstack.empty()) {
	//		StmtNode* tempNode = DFSstack.top();
	//		STMT parentStmtNumber = (*tempNode).getStmtNumber();
	//		p.insertParent(parentStmtNumber, currentStmtNumber);
	//	}

	//	//add Follows relationship if current node has a right sibling
	//	if ((*currentStmtNode).isHasRightSibling()) {
	//		StmtNode* rightSibling;
	//		ASTNode* parentNode;
	//		int pointer;
	//		if (DFSstack.empty()) {
	//			parentNode = stmtListNode;
	//			pointer = rootPosition;
	//		} else {
	//			parentNode = DFSstack.top();
	//			pointer = positionStack.top();
	//		}
	//		rightSibling = (StmtNode*) (*parentNode).getChild(pointer + 1);
	//		STMT rightSiblingNumber = (*rightSibling).getStmtNumber();
	//		f.insertFollows(currentStmtNumber, rightSiblingNumber);
	//	}

	//	//go to the next stmt. choose from nextStmtStack if possible
	//	if (nextStmtStack.empty()) {
	//		ASTNode* parentNode;
	//		int pointer;
	//		if (DFSstack.empty()) {
	//			parentNode = stmtListNode;
	//			pointer = rootPosition;
	//		} else {
	//			parentNode = DFSstack.top();
	//			pointer = positionStack.top();
	//		}
	//		currentStmtNode = (StmtNode*) (*parentNode).getChild(pointer + 1);
	//	} else {
	//		DFSstack.push(currentStmtNode);
	//		positionStack.push(0);
	//		currentStmtNode = nextStmtStack.top();
	//		nextStmtStack.pop();
	//	}
	//}
}


DesignExtractor::~DesignExtractor(void)
{
}
