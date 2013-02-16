#include "PQLAffectsProcessor.h"
#include "Helper.h"

bool PQLAffectsProcessor::isSatifyAffects(STMT a1, STMT a2)
{
	if (a1 <= 0 || a2 <= 0) return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	ASTStmtNode* a2ASTNode = PKB::stmtRefMap.at(a2).getASTStmtNode();
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 
	CFGNode* a2CFGNode = PKB::stmtRefMap.at(a2).getCFGNode(); 

	if (a1CFGNode->getProcedure() != a2CFGNode->getProcedure()) { //Check if same procedure
		PKB::affects.insertAffects(a1, a2, false);
		return false; 
	}
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign) { // return false if either stmt is not assign
		PKB::affects.insertAffects(a1, a2, false);
		return false; 
	}
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified

	if (!(PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar))) { //if nextStar(a1, a2) and uses(a2, v) does not hold 
		PKB::affects.insertAffects(a1, a2, false);
		return false;
	}

	return true;
}

bool PQLAffectsProcessor::isAffects(STMT a1, STMT a2) {

	if (PQLAffectsProcessor::isSatifyAffects(a1, a2) == false) 
		return false;

	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 

	stack<PROG_LINE> lines2visit;
	set<STMT> visited;

	vector<PROG_LINE> nexts = a1CFGNode->getNextProgramLines(a1);

	for (int i = 0; i < nexts.size(); i++) {
		lines2visit.push(nexts.at(i));
	}

	while(lines2visit.size() > 0) {
		
		STMT curr = lines2visit.top();
		lines2visit.pop();
		
		// visited?
		if (visited.find(curr) != visited.end()) 
			continue;

		visited.insert(curr);

		if (curr == a2) {
			PKB::affects.insertAffects(a1, a2, true);
			return true;
		}

		if (!PKB::next.isNextStar(a1, curr)) //if next* does not hold
			continue;

		ASTStmtNode* n = PKB::stmtRefMap.at(curr).getASTStmtNode();	
		if (n->getType() == ASTNode::Call) { // the variable is modified along the path
			if (PKB::modifies.isModifiedProc(n->getValue(), modifiedVar)){
				continue;
			}
		}

		if (n->getType() == ASTNode::Assign) {
			if (PKB::modifies.isModifiedStmt(n->getStmtNumber(), modifiedVar)){
				continue;
			}
		}

		vector<PROG_LINE> temp = PKB::next.getNext(curr);

		for (int k = 0; k < temp.size(); k++) {
			lines2visit.push(temp.at(k));
		}
	}
		
	return false;
}

bool PQLAffectsProcessor::isAffectsStar(STMT a1, STMT a2) {
	
	if (!isSatifyAffects(a1, a2)) 
		return false;

	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 

	stack<PROG_LINE> lines2visit;
	set<STMT> visited;

	vector<PROG_LINE> nexts = a1CFGNode->getNextProgramLines(a1);

	for (int i = 0; i < nexts.size(); i++) {
		lines2visit.push(nexts.at(i));
	}

	while(lines2visit.size() > 0) {
		STMT curr = lines2visit.top();
		lines2visit.pop();

		// visited?
		if (visited.find(curr) != visited.end()) 
			continue;
		visited.insert(curr);

		if (!(PKB::next.isNextStar(curr, a2))) //if next* does not hold for the 2nd portion of transitive closure
			continue;

		if (PKB::affects.isAffects(a1, curr) && PKB::affects.isAffects(curr, a2)) { //transitive closure check
			PKB::affects.insertAffectsStar(a1, a2, true);
			return true;
		}

		vector<PROG_LINE> temp = PKB::stmtRefMap.at(curr).getCFGNode()->getNextProgramLines(curr);

		for (int k = 0; k < temp.size(); k++) {
			lines2visit.push(temp.at(k));
		}
	}

	return false;
}

bool PQLAffectsProcessor::isAffectsBip(STMT a1, STMT a2) {
	if (a1 <= 0 || a2 <= 0) 
		return false;
	
	return false; 
		
}

