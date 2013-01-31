#pragma once
#include "CFGNode.h"

CFGNode::CFGNode(NodeType type, PROG_LINE start, PROG_LINE end, PROC p){
	this->starting = start;
	this->ending = end;
	this->belongingTo = p;
	this->type = type;
	this->isStart = false;
	this->isEnd = false;
}

//link node1->node2 in the cfg
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

void CFGNode::setStartNode(){
	this->isStart = true;
}

bool CFGNode::isStartNode(){
	return this->isStart;
}

void CFGNode::setEndNode(){
	this->isEnd = true;
}

bool CFGNode::isEndNode(){
	return this->isEnd;
}

bool CFGNode::isDummy(){
	return this->type == CFGNode::DummyNode;
}

CFGNode::NodeType CFGNode::getType(){
	return this->type;
}

void CFGNode::addNextNode(CFGNode* node)
{
	this->nextNodes.push_back(node);
}

void CFGNode::addPreviousNode(CFGNode* node)
{
	this->prevNodes.push_back(node);
}

vector<CFGNode*> CFGNode::getPreviousNodes(){
	return this->prevNodes;
}

//To be traversed later
vector<CFGNode*> CFGNode::getAllPreviousNodes(){
	vector<CFGNode*> prevs;
	//Recursively add previous nodes to the vector till the start?
	return prevs;
}

vector<CFGNode*> CFGNode::getNextNodes(){
	return this->nextNodes;
}

//To be traversed later
vector<CFGNode*> CFGNode::getAllNextNodes(){
	vector<CFGNode*> nexts;
	//Recursively add next nodes to the vector till the end?
	return nexts;
}

PROC CFGNode::getProcedure(){
	return this->belongingTo;
}

vector<PROG_LINE> CFGNode::getProgramLines(){
	vector<PROG_LINE> progs;

	for (int i = this->starting; i <= this->ending; i++)
	{
		progs.push_back(i);
	}

	return progs;
}

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