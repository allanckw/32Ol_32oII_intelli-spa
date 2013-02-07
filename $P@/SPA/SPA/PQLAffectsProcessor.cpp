#include "PQLAffectsProcessor.h"

//@NICK IF CANNOT FIND -> USE PQLNEXTPROCESSOR TO TRAVERSE DO NOT CODE HERE
//REMEMBER TO ADD BACK USING PKB::next::insertNextStar(p1, p2, true or false);
bool PQLAffectsProcessor::computeAffects(STMT a1, STMT a2) {

	if (a1 <= 0 || a2 <= 0) 
		return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = NULL; 
	ASTStmtNode* a2ASTNode = NULL; 

	CFGNode* a1CFGNode = NULL;
	CFGNode* a2CFGNode = NULL;

	// Check if they are in the same procedure
	if (a1CFGNode->getProcedure() != a2CFGNode->getProcedure())
		return false;

	// return false if either line number is not assign
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign)
		return false;

	// get the variable being modified
	VAR modifiedVar = a1ASTNode->getValue(); 

	//if nextStar(a1, a2) and uses(a2, v) does not hold 
	if ((PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar)) ) 
		return false;

	//get list of cfgnextnodes of a1
	const vector<CFGNode*>& nextNodes = a1CFGNode->getNextNodes();

	//CFG Traversal and checking starts here

	return false;
}

bool PQLAffectsProcessor::computeAffectsStar(STMT a1, STMT a2) {
	
	if (a1 <= 0 || a2 <= 0) 
		return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = NULL; 
	ASTStmtNode* a2ASTNode = NULL; 

	CFGNode* a1CFGNode = NULL;
	CFGNode* a2CFGNode = NULL;

	// Check if they are in the same procedure
	if (a1CFGNode->getProcedure() != a2CFGNode->getProcedure())
		return false;

	// return false if either line number is not assign
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign)
		return false;

	// get the variable being modified
	VAR modifiedVar = a1ASTNode->getValue(); 

	//if nextStar(a1, a2) and uses(a2, v) does not hold 
	if ((PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar)) ) 
		return false;

	stack<PROG_LINE> visits;
	
	//get list of cfgnextnodes of a1
	const vector<CFGNode*>& nextNodes = a1CFGNode->getNextNodes();

	//CFG Traversal and checking starts here

	return false;
}

bool PQLAffectsProcessor::computeAffectsBip(STMT a1, STMT a2) {
	if (a1 <= 0 || a2 <= 0) 
		return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = NULL; 
	ASTStmtNode* a2ASTNode = NULL; 

	CFGNode* a1CFGNode = NULL;
	CFGNode* a2CFGNode = NULL;

	// Check if they are in the same procedure
	if (a1CFGNode->getProcedure() == a2CFGNode->getProcedure())
	//return affects star?

	// return false if either line number is not assign
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign)
		return false;

	// get the variable being modified
	VAR modifiedVar = a1ASTNode->getValue(); 

	//if nextStar(a1, a2) and uses(a2, v) does not hold 
	//if (!(PKB::nextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar)) ) 
		//return false;

	stack<PROG_LINE> visits;
	
	//get list of cfgnextnodes of a1
	const vector<CFGNode*>& nextNodes = a1CFGNode->getNextNodes();

	//CFG Traversal and checking starts here

	return false; 
		
}