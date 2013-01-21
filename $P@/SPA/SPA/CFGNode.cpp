#include "CFGNode.h"

//ToDo: Fill in the blank for CFG.

CFGNode::CFGNode(PROG_LINE start, PROG_LINE end, PROC p){
	this->starting = start;
	this->ending = end;
	this->belongingTo = p;
}

//link node1->node2 in the cfg
void setLink(CFGNode* node1, CFGNode* node2){
	if (node1->isEndNode())
		throw SPAException("Node1 cannot be an end node");
	else if (node2->isStartNode())
		throw SPAException("Node2 cannot be a start node");
	else if(node1->getProcedure() != node2->getProcedure())
		throw SPAException("Nodes must be from the same procedure when linking them in a CFG!");
	else {
		node1->setNextNode(node2);
		node2->setPreviousNode(node1);
	}
}

void CFGNode::setAsStartNode(){
	this->isStart = true;
}
	
void CFGNode::setAsEndNode(){
	this->isEnd = true;
}
	
bool CFGNode::isStartNode(){
	return this->isStart;
}

bool CFGNode::isEndNode(){
	return this->isEnd;
}

void CFGNode::setNextNode(CFGNode* node)
{
	this->nextNode = node;
}

void CFGNode::setPreviousNode(CFGNode* node)
{
	this->prevNode = node;
}

CFGNode* CFGNode::getPreviousNode(){
	return this->prevNode;
}

vector<CFGNode*> CFGNode::getAllPreviousNodes(){
	vector<CFGNode*> prevs;
	//Recursively add previous nodes to the vector till the start?

	return prevs;
}

CFGNode* CFGNode::getNextNode(){
	return this->nextNode;
}

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

CFGNode::~CFGNode(void){

}