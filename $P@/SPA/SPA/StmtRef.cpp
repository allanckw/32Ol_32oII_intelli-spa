#include "StmtRef.h"


StmtRef::StmtRef(PROG_LINE p, STMT s)
{
	this->p = p;
	this->s = s;
}

StmtRef::StmtRef(PROG_LINE p, STMT s, ASTNode* a, CFGNode* c)
{
	this->p = p;
	this->s = s;
	this->astRef = a;
	this->cfgRef = c;
}

StmtRef::StmtRef(PROG_LINE p, STMT s, ASTNode* a)
{
	this->p = p;
	this->s = s;
	this->astRef = a;
}

void StmtRef::setASTNode(ASTNode* n)
{
	 this->astRef = n;
}

void StmtRef::setCFGNode(CFGNode* n)
{
	this->cfgRef = n;
}

ASTNode* StmtRef::getASTNode()
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

