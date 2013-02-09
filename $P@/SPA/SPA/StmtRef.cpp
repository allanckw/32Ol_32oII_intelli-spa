#include "StmtRef.h"


StmtRef::StmtRef(PROG_LINE p, STMT s)
{
	this->p = p;
	this->s = s;
}

StmtRef::StmtRef(PROG_LINE p, STMT s, ASTStmtNode* a, CFGNode* c)
{
	this->p = p;
	this->s = s;
	if (a->getStmtNumber() != this->getStmtNo()) {
		throw SPAException("Stmt No. Does Not Match!");
	} else {
		this->astRef = a;
	}

	if (c->isProgLineBelongto(this->getProgLine()))
		this->cfgRef = c;
	else
		throw SPAException("Program line Does Not Match!");
}

StmtRef::StmtRef(PROG_LINE p, STMT s, ASTStmtNode* a)
{
	this->p = p;
	this->s = s;
	if (a->getStmtNumber() != this->getStmtNo()) {
		throw SPAException("Stmt No. Does Not Match!");
	} else {
		this->astRef = a;
	}
}

void StmtRef::setASTStmtNode(ASTStmtNode* a)
{

	if (a->getStmtNumber() != this->getStmtNo()) {
		throw SPAException("Stmt No. Does Not Match!");
	} else {
		this->astRef = a;
	}
	
}

void StmtRef::setCFGNode(CFGNode* c)
{
	if (c->isProgLineBelongto(this->getProgLine()))
		this->cfgRef = c;
	else
		throw SPAException("Program line Does Not Match!");
}

ASTStmtNode* StmtRef::getASTStmtNode()
{
	return this->astRef;
}

CFGNode* StmtRef::getCFGNode()
{
	return this->cfgRef;
}

PROG_LINE StmtRef::getProgLine()
{
	return this->p;
}

STMT StmtRef::getStmtNo(){
	return this->s;
}