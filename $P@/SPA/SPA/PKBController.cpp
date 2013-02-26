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


	DesignExtractor::extractDesign(); //Extract Design
		
	CFGBuilder::buildCFG(); //Build CFG

	//MyCFGBuilder::buildCFG(); //Build CFG

	RulesOfEngagement::initialise(); //Initialize ROE for Queries
}