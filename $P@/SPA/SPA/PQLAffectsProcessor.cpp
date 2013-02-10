#include "PQLAffectsProcessor.h"

//@NICK IF CANNOT FIND -> USE PQLNEXTPROCESSOR TO TRAVERSE DO NOT CODE HERE
//REMEMBER TO ADD BACK USING PKB::next::insertNextStar(p1, p2, true or false);
bool PQLAffectsProcessor::computeAffects(STMT a1, STMT a2) {

	if (a1 <= 0 || a2 <= 0) 
		return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	ASTStmtNode* a2ASTNode = PKB::stmtRefMap.at(a2).getASTStmtNode();

	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 
	CFGNode* a2CFGNode = PKB::stmtRefMap.at(a2).getCFGNode(); 

	// Check if they are in the same procedure
	if (a1CFGNode->getProcedure() != a2CFGNode->getProcedure())
		return false;

	// return false if either line number is not assign
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign)
		return false;

	// get the variable being modified
	VAR modifiedVar = a1ASTNode->getValue(); 

	//if nextStar(a1, a2) and uses(a2, v) does not hold 
	if (!(PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar))) 
		return false;

	//get list of cfgnextnodes of a1
	const vector<CFGNode*>& nextNodes = a1CFGNode->getNextNodes();

	//CFG Traversal and checking starts here

	return false;
}

bool PQLAffectsProcessor::computeAffectsStar(STMT a1, STMT a2) {
	
	if (a1 <= 0 || a2 <= 0) 
		return false;

	if (PKB::affects.isAffects(a1, a2)) //if it is already in affects, then affects* holds true~
		return true;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	ASTStmtNode* a2ASTNode = PKB::stmtRefMap.at(a2).getASTStmtNode();

	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 
	CFGNode* a2CFGNode = PKB::stmtRefMap.at(a2).getCFGNode(); 

	// Check if they are in the same procedure
	if (a1CFGNode->getProcedure() != a2CFGNode->getProcedure())
		return false;

	// return false if either line number is not assign
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign)
		return false;

	// get the variable being modified
	VAR modifiedVar = a1ASTNode->getValue(); 

	//if nextStar(a1, a2) and uses(a2, v) does not hold 
	if (!(PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar)))
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
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	ASTStmtNode* a2ASTNode = PKB::stmtRefMap.at(a2).getASTStmtNode();

	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 
	CFGNode* a2CFGNode = PKB::stmtRefMap.at(a2).getCFGNode(); 

	// return false if either line number is not assign
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign)
		return false;

	// Check if they are in the same procedure
	if (a1CFGNode->getProcedure() == a2CFGNode->getProcedure())
		return PKB::affects.isAffectsStar(a1, a2); //if they are in the same procedure, then AffectsBip = Affects*? TBC

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