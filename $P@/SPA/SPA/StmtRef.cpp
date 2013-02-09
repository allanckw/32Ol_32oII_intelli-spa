#include "StmtRef.h"
#include "ASTStmtNode.h"

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
	if (n->getType() != ASTNode::Assign || 	n->getType() != ASTNode::Call || 
		n->getType() != ASTNode::If || 	n->getType() != ASTNode::While)
	{
		throw SPAException("Invalid Node Type!");
	}
	else
	{
		ASTStmtNode* stmtNode = dynamic_cast<ASTStmtNode*>(n);
		if (stmtNode->getStmtNumber() != this->getStmtNo()) {
			throw SPAException("Stmt No. Does Not Match!");
		} else {
			this->astRef = n;
		}
	}
}

void StmtRef::setCFGNode(CFGNode* n)
{
	if (n->isProgLineBelongto(this->getProgLine()))
		this->cfgRef = n;
	else
		throw SPAException("Program line Does Not Match!");
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