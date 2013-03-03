#pragma once
#include "CFGNode.h"
#include "PKB.h"

/**
* This method will be used as a constructor to create CFGNode based on PROCEDURE
*/
CFGNode::CFGNode(PROC proc) : proc(proc), first(0), last(0), type(DummyNode), whileAncestor(NULL),
	nextList(), prevList(), modifySet(), useSet(), parents(), children()
{
}
/**
* This method will be used as a constructor to create CFGNode based on Parent
*/
CFGNode::CFGNode(CFGNode* parent) : proc(parent->proc), first(0), last(0), type(DummyNode), 
	whileAncestor(parent->whileAncestor), nextList(), modifySet(), useSet(), children(),
	parents(1, parent)
{
	if (parent->prevList == NULL)
		prevList = new IntervalList(parent->first, parent->last);
	else
		prevList = parent->prevList->Union(parent->first, parent->last);
}

///**
//* This method will be used to create a new CFG Node
//* @param type The type of Node which is while or if or standard or dummy
//* @param start The starting prog line of the CFG Node
//* @param end The ending prog line of the CFG Node
//* @param p	The procedure of the CFG Node
//*/
//CFGNode::CFGNode(NodeType type, PROG_LINE start, PROG_LINE end, PROC p){
//	this->starting = start;
//	this->ending = end;
//	this->belongingTo = p;
//	this->type = type;
//	this->isStart = false;
//	this->isEnd = false;
//}
//
///**
//* This method will be used to link a CFG Node(Next Node)
//* @param node The CFG Node that is to be link next
//*/
//void CFGNode::setLink(CFGNode* node){
//	if (this->isEndNode())
//		throw SPAException("Node1 cannot be an end node");
//	else if (node->isStartNode())
//		throw SPAException("Node2 cannot be a start node");
//	else if(this->getProcedure() != node->getProcedure())
//		throw SPAException("Nodes must be from the same procedure when linking them in a CFG!");
//	else if(this->isStartNode() && node->isStartNode() || this->isEndNode() && node->isEndNode())
//		throw SPAException("Invalid Node Types, please check your declarations!");
//
//	else {
//		this->addNextNode(node);
//		node->addPreviousNode(this);
//	}
//}
//
///**
//* This method will be used to set the CFGNode as starting CFG node
//*/
//void CFGNode::setStartNode(){
//	this->isStart = true;
//}
//
///**
//* This method will be used to get whether the node is a starting CFG node
//* @Return true if it is start node else false
//*/
//bool CFGNode::isStartNode(){
//	return this->isStart;
//}
//
///**
//* This method will be used to set CFGNode as ending CFG node
//*/
//void CFGNode::setEndNode(){
//	this->isEnd = true;
//}
//
///**
//* This method will be used to get whether the node is a ending CFG node
//* @Return true if it is end node else false
//*/
//bool CFGNode::isEndNode(){
//	return this->isEnd;
//}
//
///**
//* This method will be used to get whether the node is a dummy CFG Node
//* @Return true if it is dummy node else false
//*/
//bool CFGNode::isDummy(){
//	return this->type == CFGNode::DummyNode;
//}
//
///**
//* This method will be used to return the Node Type
//* @Return the node type
//*/
//CFGNode::NodeType CFGNode::getType(){
//	return this->type;
//}
//
///**
//* This method will be used to return starting PROG LINE
//* @Return the prog line
//*/
//PROG_LINE CFGNode::getStartLine(){
//	return this->starting;
//}
//
///**
//* This method will be used to return ending PROG LINE
//* @Return the prog line
//*/
//PROG_LINE CFGNode::getEndLine(){
//	return this->ending;
//}
///**
//* This method will be used to add the next CFG Node
//* @Param node the node that is next to the node
//*/
//
//void CFGNode::addNextNode(CFGNode* node)
//{
//	this->nextNodes.push_back(node);
//}
//
///**
//* This method will be used to add the previous CFG Node
//* @Param node the node that is before the node
//*/
//void CFGNode::addPreviousNode(CFGNode* node)
//{
//	this->prevNodes.push_back(node);
//}
//
///**
//* This method will be used to return a list of node that is before the Node
//* @Return List of CFGNode that is previous
//*/
//vector<CFGNode*> CFGNode::getPreviousNodes(){
//	return this->prevNodes;
//}
//
///**
//* This method will be used to return a list of node that is after the Node
//* @Return List of CFGNode that is next
//*/
//vector<CFGNode*> CFGNode::getNextNodes(){
//	return this->nextNodes;
//}
//
///**
//* This method will be used to return the procedure these CFG node belong to
//* @Return the PROC that CFG is in
//*/
//PROC CFGNode::getProcedure(){
//	return this->belongingTo;
//}
//
///**
//* This method is be use to get ALL PROG LINES associated with the CFGNode
//* @Return List of program lines
//*/
//vector<PROG_LINE> CFGNode::getProgramLines() {
//	vector<PROG_LINE> progs;
//
//	for (int i = this->starting; i <= this->ending; i++) {
//		progs.push_back(i);
//	}
//	return progs;
//}
//
//
//bool CFGNode::isProgLineBelongto(PROG_LINE p) {
//	for (int i = this->starting; i <= this->ending; i++) {
//		if (i == p)
//			return true;
//	}
//
//	return false;
//}
//
