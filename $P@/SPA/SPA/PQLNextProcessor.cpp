#include "PQLNextProcessor.h"
#include "Helper.h"

bool PQLNextProcessor::isNext(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 < 0 || p1 > PKB::maxProgLines || p2 < 0 || p2 > PKB::maxProgLines){
		return false;
	}

	CFGNode* s1 = PKB::stmtRefMap.at(p1).getCFGNode();

	if (p1 < s1->last){
		return p2 == p1 + 1;
	}

	CFGNode* child;
	queue<CFGNode*> children;
	children.push(s1);
	while (children.size() > 0) {
		CFGNode* curr = children.front();
		children.pop();
		switch (curr->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode:
			child = curr->children.oneChild;
			if (child == NULL)
				break;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				if (p2 == child->first)
					return true;
			}
			break;

		case CFGNode::WhileNode:
			child = curr->children.whileChildren.whileIn;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				if (p2 == child->first)
					return true;
			}

			child = curr->children.whileChildren.whileOut;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				if (p2 == child->first)
					return true;
			}
			break;
		
		case CFGNode::IfNode:
			child = curr->children.ifChildren.ifThen;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				if (p2 == child->first){
					return true;
				}
			}
			child = curr->children.ifChildren.ifElse;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				if (p2 == child->first){
					return true;
				}
			}
			break;
		}
	}
	return false;
}

vector<PROG_LINE> PQLNextProcessor::getNext(PROG_LINE p1)
{
	if (p1 < 0 || p1 > PKB::maxProgLines)
		return vector<PROG_LINE>();

	CFGNode* s1 = PKB::stmtRefMap.at(p1).getCFGNode();

	if (p1 < s1->last)
		return vector<PROG_LINE>(1, p1 + 1);

	vector<PROG_LINE> answer;

	CFGNode* child;
	queue<CFGNode*> children;
	children.push(s1);

	while (children.size() > 0) {
		CFGNode* curr = children.front();
		children.pop();
		switch (curr->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode:
			child = curr->children.oneChild;
			if (child == NULL)
				break;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			break;

		case CFGNode::WhileNode:
			child = curr->children.whileChildren.whileIn;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			child = curr->children.whileChildren.whileOut;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			break;

		case CFGNode::IfNode:
			child = curr->children.ifChildren.ifThen;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			child = curr->children.ifChildren.ifElse;
			if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else {
				answer.push_back(child->first);
			}
			break;
		}
	}
	return answer;

}

vector<PROG_LINE> PQLNextProcessor::getPrevious(PROG_LINE p2)
{
	if (p2 < 0 || p2 > PKB::maxProgLines){
		return vector<PROG_LINE>();
	}

	CFGNode* s2 = PKB::stmtRefMap.at(p2).getCFGNode();
	if (p2 > s2->first){
		return vector<PROG_LINE>(1, p2 - 1);
	}

	vector<PROG_LINE> answer;
	CFGNode* parent;
	queue<CFGNode*> parents;
	parents.push(s2);

	while (!parents.empty()) {
		CFGNode* curr = parents.front();
		parents.pop();
		for (auto it = curr->parents.begin(); it != curr->parents.end(); it++) {
			parent = *it;
			switch (parent->type) {
			case CFGNode::DummyNode:
				parents.push(parent);
				break;
			default:
				answer.push_back(parent->last);
			}
		}
	}
	return answer;
}


bool PQLNextProcessor::isNextStar(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 < 0 || p1 > PKB::maxProgLines || p2 < 0 || p2 > PKB::maxProgLines){
		return false;
	}

	CFGNode* s1 = PKB::stmtRefMap.at(p1).getCFGNode();
		
	if (p2 > p1 && p2 <= s1->last){
		return true;
	}
	CFGNode* s2 = PKB::stmtRefMap.at(p2).getCFGNode();

	if (s1->proc != s2->proc){
			return false;
	}

	if (s1->whileAncestor != NULL && s1->whileAncestor == s2->whileAncestor) {
			return true;
	}

	const IntervalList * list = s1->nextList;
	
	if (list == NULL){
		return false;
	}

	if (p2 < list->first) {
		list = list->prev;
		while (list != NULL && p2 < list->first){
			list = list->prev;
		}
		if (list == NULL || p2 > list->last){
			return false;
		}
			
		return true;
		
	} else if (p2 > list->last) {
		list = list->next;
		while (list != NULL && p2 > list->last){
			list = list->next;
		}
		if (list == NULL || p2 < list->first) {
			return false;
		}		
		return true;
	}
		
	return true;
}

vector<PROG_LINE> PQLNextProcessor::getNextStar(PROG_LINE p1)
{
	if (p1 < 0 || p1 > PKB::maxProgLines){
		return vector<PROG_LINE>();
	}
	
	vector<PROG_LINE> answer;
	
	CFGNode* s1 = PKB::stmtRefMap.at(p1).getCFGNode();

	for (int i = p1 + 1; i <= s1->last; i++){
		answer.push_back(i);
	}

	const IntervalList * node = s1->nextList;

	if (node != NULL) {
		for (int i = node->first; i <= node->last; i++){
			answer.push_back(i);
		}

		const IntervalList * prevNode = node->prev;

		while (prevNode != NULL) {
			for (int i = prevNode->first; i <= prevNode->last; i++){
				answer.push_back(i);
			}
		}

		node = node->next;
		
		while (node != NULL) {
			for (int i = node->first; i <= node->last; i++){
				answer.push_back(i);
			}
			node = node->next;
		}
	}
	return answer;
}

vector<PROG_LINE> PQLNextProcessor::getPreviousStar(PROG_LINE p2) {
	if (p2 < 0 || p2 > PKB::maxProgLines)
		return vector<PROG_LINE>();

	vector<PROG_LINE> answer;

	CFGNode* s2 = PKB::stmtRefMap.at(p2).getCFGNode();

	for (int i = s2->first; i < p2; i++){
		answer.push_back(i);
	}

	const IntervalList * node = s2->prevList;

	if (node != NULL) {
		for (int i = node->first; i <= node->last; i++){
			answer.push_back(i);
		}

		const IntervalList * prevNode = node->prev;

		while (prevNode != NULL) {
			for (int i = prevNode->first; i <= prevNode->last; i++)
				answer.push_back(i);
			prevNode = prevNode->prev;
		}

		node = node->next;
		while (node != NULL) {
			for (int i = node->first; i <= node->last; i++)
				answer.push_back(i);
			node = node->next;
		}
	}
	return answer;
}
