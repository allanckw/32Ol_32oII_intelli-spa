#include "StdAfx.h"
#include "StmtNode.h"


StmtNode::StmtNode(void)
{
	StmtNumber = -1;
}


StmtNode::~StmtNode(void)
{
}

int StmtNode::GetStmtNumber(void)
{
	return StmtNumber;
}