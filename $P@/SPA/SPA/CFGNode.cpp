#pragma once
#include "CFGNode.h"

CFGNode::CFGNode(NodeType type, PROG_LINE start, PROG_LINE end, PROC p){
	this->starting = start;
	this->ending = end;
	this->belongingTo = p;
	this->type = type;
}

//link node1->node2 in the cfg
void setLink(CFGNode* node1, CFGNode* node2){
	if (node1->isEndNode())
		throw SPAException("Node1 cannot be an end node");
	else if (node2->isStartNode())
		throw SPAException("Node2 cannot be a start node");
	else if(node1->getProcedure() != node2->getProcedure())
		throw SPAException("Nodes must be from the same procedure when linking them in a CFG!");
	else if(node1->isStartNode() && node2->isStartNode() || node1->isEndNode() && node2->isEndNode())
		throw SPAException("Invalid Node Types, please check your declarations!");

	else {
		node1->addNextNode(node2);
		node2->addPreviousNode(node1);
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