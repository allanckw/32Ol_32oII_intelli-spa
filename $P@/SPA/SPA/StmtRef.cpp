#include "StmtRef.h"

/**
* This method will be used as a constructor to create a statement reference
* @Param p the program line
* @Param s the statement no.
*/
StmtRef::StmtRef(PROG_LINE p, STMT s)
{
	this->p = p;
	this->s = s;
}

/**
* This method will be used as a constructor to create a statement reference
* @Param p the program line
* @Param s the statement no.
* @Param a the ASTStmtNode
* @Param c the CFGNode
*/
StmtRef::StmtRef(PROG_LINE p, STMT s, ASTStmtNode* a, CFGNode* c)
{
	this->p = p;
	this->s = s;
	this->setASTStmtNode(a);
	this->setCFGNode(c);
}


/**
* This method will be used as a constructor to create a statement reference
* @Param p the program line
* @Param s the statement no.
* @Param a the ASTStmtNode
*/
StmtRef::StmtRef(PROG_LINE p, STMT s, ASTStmtNode* a)
{
	this->p = p;
	this->s = s;
	this->setASTStmtNode(a);
}

/**
* This method will be set the ASTStmtNode of the statement reference
* @Param a the ASTStmtNode
*/
void StmtRef::setASTStmtNode(ASTStmtNode* a)
{

	if (a->getStmtNumber() != this->getStmtNo()) {
		throw SPAException("Stmt No. Does Not Match!");
	} else {
		this->astRef = a;
	}
	
}

/**
* This method will be set the CFGNode of the statement reference
* @Param c the CFGNode
*/
void StmtRef::setCFGNode(CFGNode* c)
{
	this->cfgRef = c;
}


/**
* This method will return the ASTStmtNode of the statement reference
* @Return the ASTStmtNode
*/
ASTStmtNode* StmtRef::getASTStmtNode()
{
	return this->astRef;
}

/**
* This method will return the CFGNode of the statement reference
* @Return the CFGNode
*/
CFGNode* StmtRef::getCFGNode()
{
	return this->cfgRef;
}

/**
* This method will return the program line of the statement reference
* @Return the program line
*/
PROG_LINE StmtRef::getProgLine()
{
	return this->p;
}

/**
* This method will return the statement number of the statement reference
* @Return the statement number
*/
STMT StmtRef::getStmtNo(){
	return this->s;
}