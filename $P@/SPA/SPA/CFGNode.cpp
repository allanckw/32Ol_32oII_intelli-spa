#pragma once
#include "CFGNode.h"

/**
* This method will be used to create a new CFG Node
* @param type The type of Node which is while or if or standard or dummy
* @param start The starting prog line of the CFG Node
* @param end The ending prog line of the CFG Node
* @param p	The procedure of the CFG Node
*/
CFGNode::CFGNode(NodeType type, PROG_LINE start, PROG_LINE end, PROC p){
	this->starting = start;
	this->ending = end;
	this->belongingTo = p;
	this->type = type;
	this->isStart = false;
	this->isEnd = false;
}

/**
* This method will be used to link a CFG Node(Next Node)
* @param node The CFG Node that is to be link next
*/
void CFGNode::setLink(CFGNode* node){
	if (this->isEndNode())
		throw SPAException("Node1 cannot be an end node");
	else if (node->isStartNode())
		throw SPAException("Node2 cannot be a start node");
	else if(this->getProcedure() != node->getProcedure())
		throw SPAException("Nodes must be from the same procedure when linking them in a CFG!");
	else if(this->isStartNode() && node->isStartNode() || this->isEndNode() && node->isEndNode())
		throw SPAException("Invalid Node Types, please check your declarations!");

	else {
		this->addNextNode(node);
		node->addPreviousNode(this);
	}
}

/**
* This method will be used to set the CFGNode as starting CFG node
*/
void CFGNode::setStartNode(){
	this->isStart = true;
}

/**
* This method will be used to get whether the node is a starting CFG node
*/
bool CFGNode::isStartNode(){
	return this->isStart;
}

/**
* This method will be used to set CFGNode as ending CFG node
*/
void CFGNode::setEndNode(){
	this->isEnd = true;
}

/**
* This method will be used to get whether the node is a ending CFG node
*/
bool CFGNode::isEndNode(){
	return this->isEnd;
}

/**
* This method will be used to get whether the node is a dummy CFG Node
*/
bool CFGNode::isDummy(){
	return this->type == CFGNode::DummyNode;
}

/**
* This method will be used to return the Node Type
*/
CFGNode::NodeType CFGNode::getType(){
	return this->type;
}

/**
* This method will be used to add the next CFG Node
* @Param node the node that is next to the node
*/
void CFGNode::addNextNode(CFGNode* node)
{
	this->nextNodes.push_back(node);
}

/**
* This method will be used to add the previous CFG Node
* @Param node the node that is before the node
*/
void CFGNode::addPreviousNode(CFGNode* node)
{
	this->prevNodes.push_back(node);
}

/**
* This method will be used to return a list of node that is before the Node
*/
vector<CFGNode*> CFGNode::getPreviousNodes(){
	return this->prevNodes;
}

/**
* This method will be used to return all list of node that is before the Node
*/
vector<CFGNode*> CFGNode::getAllPreviousNodes(){
	vector<CFGNode*> prevs;
	//Recursively add previous nodes to the vector till the start?
	return prevs;
}

/**
* This method will be used to return a list of node that is after the Node
*/
vector<CFGNode*> CFGNode::getNextNodes(){
	return this->nextNodes;
}

/**
* This method will be used to return all list of node that is after the Node
*/
vector<CFGNode*> CFGNode::getAllNextNodes(){
	vector<CFGNode*> nexts;
	//Recursively add next nodes to the vector till the end?
	return nexts;
}

/**
* This method will be used to return the procedure these CFG node belong to
*/
PROC CFGNode::getProcedure(){
	return this->belongingTo;
}

/**
* This method will be used to return PROG Line that it contains
*/
vector<PROG_LINE> CFGNode::getProgramLines(){
	vector<PROG_LINE> progs;

	for (int i = this->starting; i <= this->ending; i++)
	{
		progs.push_back(i);
	}

	return progs;
}

/**
* This method will be used to return if PROG Line p1 is next to PROG Line p2
* @Param p1 The PROG Line that is before p2
* @Param p2 The PROG Line that is after p1
*/
bool CFGNode::isNext(PROG_LINE p1, PROG_LINE p2)
{
	if (this->isEndNode() && p2 > ending || //last node of a proc, and p2 > ending then no more 
		this->getType() == CFGNode::DummyNode && this->isEndNode()) //dummy and end -> exit node
		return false;

	int x = p1 + 1;
	if (p1 >= starting && p2 <= ending && x == p2){ //if in range and p1+1 = p2
		return true;

	} else if (p1 == ending) { 
		//if p1 is the last number of a node, then its next node could be in its children list
		for (unsigned int i = 0; i < getNextNodes().size(); i++){

			if (this->getType() != CFGNode::DummyNode){
				if (getNextNodes()[i]->starting == p2)
					return true;
			} else {
				return isNext(p1, p2, this);
			}
		}
		
		return false;
	}
}

/**
* This method will be used to return if PROG Line p1 is next to PROG Line p2
* @Param p1 The PROG Line that is before p2
* @Param p2 The PROG Line that is after p1
* @Param nextNode 
*/
bool CFGNode::isNext(PROG_LINE p1, PROG_LINE p2, CFGNode* nextNode)
{
	if (nextNode->isEndNode() && p2 > ending || //last node of a proc, and p2 > ending then no more 
		nextNode->getType() == CFGNode::DummyNode && nextNode->isEndNode()) //dummy and end -> exit node
		return false;

	for (unsigned int i = 0; i < getNextNodes().size(); i++){
		CFGNode* next = getNextNodes()[i];

		if (next->isEndNode() && p2 > ending){ //last node of a proc, and p2 > ending then no more 
			return false;

		} else if (next->getType() == CFGNode::DummyNode) { 

			bool result = isNext(p1, p2, this);
			if (result){ //true return, false need to carry on with loop
				//this execution only happens when there are many many nested if
				return true;
			}
		} else {
			if (next->starting == p2)
				return true;	
		}
	}

	return false;
}