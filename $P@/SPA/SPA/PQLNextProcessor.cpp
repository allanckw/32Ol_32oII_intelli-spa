#include "PQLNextProcessor.h"
#include "Helper.h"

/**
* This method will be used to check Next(p1,p2)
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @return true if p2 is right after p1, otherwise return false
*/
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

/**
* This method will be used to get prog_line after p1
* @param p1	The prog_line that is before p2
* @return list of prog_line after p1
*/
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

/**
* This method will be used to get prog_line before p2
* @param p2	The prog_line that is after p1
* @return list of prog_line before p1
*/
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

/**
* This method will be used to check NextStar(p1,p2)
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @return true if p2 is reachable from p1, otherwise return false
*/
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

/**
* This method will be used to check NextStar(p1,_)
* @param p1	The start prog_line to find all reachable prog_line
* @return a list of prog_line reachable from p1
*/
vector<PROG_LINE> PQLNextProcessor::getNextStar(PROG_LINE p1)
{
	if (p1 < 0 || p1 > PKB::maxProgLines){
		return vector<PROG_LINE>();
	}
	
	if (p1 == 0)
		return PQLNextProcessor::getSelfNextStar();

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

/**
* This method will be used to check NextStar(_,p2)
* @param p2	The end prog_line that can be reach from p1
* @return a list of prog_line that can reach p2
*/
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

/**
* This method is only for special case which happens for while loops
*/
vector<PROG_LINE> PQLNextProcessor::getSelfNextStar(){ 
	vector<int> ans; 
	unordered_set<int> seen; 
	for (auto it =PKB::whileTable.begin(); it != PKB::whileTable.end(); it++) 
		if (seen.count(*it) == 0) { 
			seen.insert(*it); 
			CFGNode* cfg = PKB::stmtRefMap.at(*it).getCFGNode(); 
			CFGNode* cfg2 = cfg->children.whileChildren.whileOut; 

			for (int i = cfg->first; i < cfg2->first; i++) {
					ans.push_back(i ); 
			}
			
			while (cfg2 != NULL && cfg2->first != 0) 
				cfg2 = cfg2->children.oneChild; 
				
			if (cfg2 == NULL) {
				for (int i = cfg->first; i < PKB::TheBeginningAndTheEnd[cfg->proc].second; i++) 
					ans.push_back(i); 
			} else if (cfg2->type == CFGNode::DummyNode) {
				//cfg2 = cfg->children.whileChildren.whileIn; 
				vector<STMT> x = PKB::next.getNextStar(cfg->first);
				for (int i = 0; i < x.size(); i++)
					ans.push_back(x[i]);
			}else { 
				for (int i = cfg->first; i < cfg2->first; i++){ 
					ans.push_back(i); 
				}
			} 
		}
		return ans;
}

bool PQLNextProcessor::isNextBip(PROG_LINE p1, PROG_LINE p2){
	return false;
}
vector<PROG_LINE> PQLNextProcessor::getPreviousBip(PROG_LINE p2){
	vector<PROG_LINE> ans;

	return ans;
}
vector<PROG_LINE> PQLNextProcessor::getNextBip(PROG_LINE p1){
	vector<PROG_LINE> ans;

	return ans;
}
vector<PROG_LINE> PQLNextProcessor::getSelfNextBip(){
	vector<PROG_LINE> ans;

	return ans;
}

bool PQLNextProcessor::isNextBipStar(PROG_LINE p1, PROG_LINE p2){
	return false;
}
vector<PROG_LINE> PQLNextProcessor::getPreviousBipStar(PROG_LINE p2){
	vector<PROG_LINE> ans;

	return ans;
}
vector<PROG_LINE> PQLNextProcessor::getNextBipStar(PROG_LINE p1){
	vector<PROG_LINE> ans;

	return ans;
}
vector<PROG_LINE> PQLNextProcessor::getSelfNextBipStar(){
	vector<PROG_LINE> ans;

	return ans;
}
