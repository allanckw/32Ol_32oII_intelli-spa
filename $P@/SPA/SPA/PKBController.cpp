#pragma once
#include "PKBController.h"

/**
* Method to initalize the PKB, builds the AST from a SIMPLE source file, builds the CFG and initialization for queries
* @param	filename	the file name of the SIMPLE source file
*/
void PKBController::initializePKB(string filename)
{
	//Parse Source
	Parser* p = new Parser(filename);
	p->buildAST(); //build AST
	delete p;
	
	CFGBuilder::buildCFG(); //Build CFG

	DesignExtractor::extractDesign(); //Extract Design
	MyCFGBuilder::buildCFG(); //Build CFG

	RulesOfEngagement::initialise(); //Initialize ROE for Queries
}