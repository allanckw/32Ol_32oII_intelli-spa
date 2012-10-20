#pragma once
#include "stdAfx.h"
#include "PROCTable.h"
#include "VARTable.h"
#include "ASTNode.h"

class PKB
{
public:
	static int maxProgLines;

	static PROCTable procedures;
	static VARTable variables;
	static ASTNode* rootNode;



	PKB(void);
	~PKB(void);
};

