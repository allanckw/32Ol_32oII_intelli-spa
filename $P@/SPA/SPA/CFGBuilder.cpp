#include "CFGBuilder.h"


void CFGBuilder::buildCFG(){
	//Use PKB::TheBeginningAndTheEnd to find starting program line, and ending program line of a procedure
	//pair<STMT, STMT> startAndEndProgLine = PKB::TheBeginningAndTheEnd.at(currentProc);
	//startAndEndProgLine.first = 1st line in a proc, 
	//startAndEndProgLine.second = last line in a proc.

	//In the case that the end of the while loop is the end of the procedure, create a dummy node and call setEndNode(),
	//it will denote the end of the procedure
	
	for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
		ASTNode* procedureNode = (*PKB::rootNode).getChild(currentProc); //procedure
		pair<STMT, STMT> startAndEndProgLine = PKB::TheBeginningAndTheEnd.at(currentProc);

		int start = startAndEndProgLine.first;
		int end = start;

		ASTStmtLstNode* firstLevelStmtListNode = (ASTStmtLstNode*) (*procedureNode).getChild(0); //stmtList

		for (int i = 0; i < firstLevelStmtListNode->getSize(); i++)
		{
			ASTNode *childnode = firstLevelStmtListNode->getChild(i);

			if (childnode->getType() == ASTNode::Assign || childnode->getType() == ASTNode::Call) {

				//Tracking the ending program line of the CFG Node, call and assign can just carry on until a while
				//or if statement if found then create a new node
				end++; 

			} else if (childnode->getType() == ASTNode::While) { 

				//The start and end variables will have to be set to the new starting point after processing While
				
				//Building Block Testing
				if (i == startAndEndProgLine.first) {
				
					//First program line is a while loop, create a new node..
					CFGNode* whileNode = new CFGNode(CFGNode::WhileNode, start, start, procedureNode->getValue());
					//link the childrens
				
				} else {
					//Create standard node here before processing while and linking it 
					CFGNode* stdNode = new CFGNode(CFGNode::StandardNode, start, end, procedureNode->getValue());
					//process the while and link stdNode to while later
					start = end + 1; //After creating std node, a new start
					end = start;
				}
			
			} else if (childnode ->getType() == ASTNode::If){
				//The start and end variables will have to be set to the new starting point after processing if
				if (i == startAndEndProgLine.first) {
					
					//First program line is a if else, create a new node..
					CFGNode* ifNode = new CFGNode(CFGNode::IfNode, start, start, procedureNode->getValue());
					//link the childrens

				} else {
					//Create standard node here before processing if and linking it 
					CFGNode* stdNode = new CFGNode(CFGNode::StandardNode, start, end, procedureNode->getValue());
					//process the if and link stdNode to while later
					start = end + 1; //After creating std node, a new start
					end = start;
				}
			}
		}
	}

	//At the end of the procedure, before moving on to the next procedure 
	//call PKB::addToCFGList(<the first node of the cfg>, procedureNode->getValue());
	//to add into the PKB cfg list
}