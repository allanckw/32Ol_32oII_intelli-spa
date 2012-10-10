#pragma once
#include "stmtnode.h"
class CallNode :
	public StmtNode
{
public:
	CallNode(void);
	~CallNode(void);
	virtual int GetStmtNo();

};

