#pragma once
#include "PKBController.h"

void PKBController::initializePKB(string filename)
{
	//Parse Source
	Parser* p = new Parser(filename);
	p->buildAST(); //build AST
	delete p;
	
	DesignExtractor::extractDesign(); //Extract Design

	CFGBuilder::buildCFG(); //Build CFG

	RulesOfEngagement::initialise(); //Initialize ROE for Queries
}