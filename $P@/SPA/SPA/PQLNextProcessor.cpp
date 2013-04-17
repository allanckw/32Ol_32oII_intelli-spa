#include "PQLNextProcessor.h"
#include "Helper.h"
#include "PKB.h"

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

	if (p1 < s1->last)
		return p2 == p1 + 1;

	CFGNode* child;
	queue<CFGNode*> children;
	children.push(s1);
	while (!children.empty()) {
		CFGNode* curr = children.front();
		children.pop();
		switch (curr->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode:
			child = curr->children.oneChild;
			if (child == NULL)
				break;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;
			break;

		case CFGNode::WhileNode:
			child = curr->children.whileChildren.whileIn;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;

			child = curr->children.whileChildren.whileOut;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;
			break;
		
		case CFGNode::IfNode:
			child = curr->children.ifChildren.ifThen;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;

			child = curr->children.ifChildren.ifElse;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;
			break;
		}
	}
	return false;
}

/**
* This method will be used to get all prog_line after p1
* @param p1	The prog_line
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
* This method will be used to get all prog_line before p2
* @param p2	The prog_line
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
			prevNode = prevNode->prev;
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
	unordered_set<int> seen;
	for (auto it = PKB::whileTable.begin(); it != PKB::whileTable.end(); it++)
		if (seen.count(*it) == 0) {
			CFGNode* cfg = PKB::stmtRefMap.at(*it).getCFGNode();
			CFGNode* cfg2 = cfg->children.whileChildren.whileOut;
			
			int last;
			if (cfg2->type == CFGNode::DummyNode) {
				const IntervalList* ilist = cfg2->prevList;
				while (ilist->next != NULL)
					ilist = ilist->next;
				last = ilist->last;
			} else
				last = cfg2->first - 1;
			
			for (int i = cfg->first; i <= last; i++)
				seen.insert(i);
		}
	return vector<int>(seen.begin(), seen.end());
}

/**
* This method will be used to check NextBip(p1,p2)
* @param p1	The prog_line that is before p2
* @param p2	The prog_line that is after p1
* @return true if NextBip(p1, p2) holds
*/
bool PQLNextProcessor::isNextBip(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 < 0 || p1 > PKB::maxProgLines || p2 < 0 || p2 > PKB::maxProgLines){
		return false;
	}

	CFGNode* s1 = PKB::stmtRefMap.at(p1).getCFGNode();

	if (PKB::callTable.count(p1) > 0)
		return p2 == PKB::TheBeginningAndTheEnd[PKB::calls.getProcCall(p1)].first;
	if (p1 < s1->last)
		return p2 == p1 + 1;
	
	CFGNode* child;
	queue<CFGNode*> children;
	children.push(s1);

	while (!children.empty()) {
		CFGNode* curr = children.front();
		children.pop();
		switch (curr->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode:
			child = curr->children.oneChild;
			if (child == NULL) {
				const vector<STMT>& caller = PKB::calls.getStmtCall(curr->proc);
				for (auto it = caller.begin(); it != caller.end(); it++) {
					CFGNode* node = PKB::stmtRefMap.at(*it).getCFGNode();
					if (*it < node->last) {
						if (p2 == *it + 1)
							return true;
					} else
						children.push(node);
				}
			} else if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else if (p2 == child->first)
				return true;
			break;

		case CFGNode::WhileNode:
			child = curr->children.whileChildren.whileIn;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;

			child = curr->children.whileChildren.whileOut;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;
			break;
		
		case CFGNode::IfNode:
			child = curr->children.ifChildren.ifThen;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;

			child = curr->children.ifChildren.ifElse;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else if (p2 == child->first)
				return true;
			break;
		}
	}
	
	return false;
}

/**
* This method will be used to get all prog_lines right after p1
* @param p1	The prog_line
* @return list of prog_line satisfying NextBip(p1, _)
*/
vector<PROG_LINE> PQLNextProcessor::getNextBip(PROG_LINE p1)
{
	if (p1 < 0 || p1 > PKB::maxProgLines)
		return vector<PROG_LINE>();

	CFGNode* s1 = PKB::stmtRefMap.at(p1).getCFGNode();

	if (PKB::callTable.count(p1) > 0)
		return vector<PROG_LINE>(1, PKB::TheBeginningAndTheEnd[PKB::calls.getProcCall(p1)].first);
	if (p1 < s1->last)
		return vector<PROG_LINE>(1, p1 + 1);

	vector<PROG_LINE> answer;

	CFGNode* child;
	queue<CFGNode*> children;
	children.push(s1);

	while (!children.empty()) {
		CFGNode* curr = children.front();
		children.pop();
		switch (curr->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode:
			child = curr->children.oneChild;
			if (child == NULL) {
				const vector<STMT>& caller = PKB::calls.getStmtCall(curr->proc);
				for (auto it = caller.begin(); it != caller.end(); it++) {
					CFGNode* node = PKB::stmtRefMap.at(*it).getCFGNode();
					if (*it < node->last) {
						answer.push_back(*it + 1);
					} else
						children.push(node);
				}
			} else if (child->type == CFGNode::DummyNode) {
				children.push(child);
			} else
				answer.push_back(child->first);
			break;

		case CFGNode::WhileNode:
			child = curr->children.whileChildren.whileIn;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else
				answer.push_back(child->first);

			child = curr->children.whileChildren.whileOut;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else
				answer.push_back(child->first);
			break;
		
		case CFGNode::IfNode:
			child = curr->children.ifChildren.ifThen;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else
				answer.push_back(child->first);

			child = curr->children.ifChildren.ifElse;
			if (child->type == CFGNode::DummyNode)
				children.push(child);
			else
				answer.push_back(child->first);
			break;
		}
	}
	return answer;

}

/**
* This method will be used to get all prog_lines right before p2
* @param p2	The prog_line
* @return list of prog_line satisfying NextBip(_, p2)
*/
vector<PROG_LINE> PQLNextProcessor::getPreviousBip(PROG_LINE p2)
{
	if (p2 < 0 || p2 > PKB::maxProgLines){
		return vector<PROG_LINE>();
	}

	const CFGNode * const s2 = PKB::stmtRefMap.at(p2).getCFGNode();
	vector<PROG_LINE> answer;
	queue<CFGNode*> parents;

	if (p2 > s2->first) {
		if (PKB::callTable.count(p2 - 1) == 0)
			return vector<PROG_LINE>(1, p2 - 1);

		parents.push(PKB::CFGTails[PKB::calls.getProcCall(p2 - 1)]);
	} else {
		const vector<CFGNode*>& s2parents = s2->parents;
		if (s2parents.empty())
			return PKB::calls.getStmtCall(s2->proc);
		
		for (auto it = s2parents.begin(); it != s2parents.end(); it++)
			parents.push(*it);
	}

	unordered_set<CFGNode*> seen;
	while (!parents.empty()) {
		CFGNode* curr = parents.front();
		parents.pop();
		if (seen.count(curr) > 0)
			continue;

		seen.insert(curr);
		if (curr->type == CFGNode::DummyNode) {
			vector<CFGNode*> s2parents = curr->parents;
			for (auto it = s2parents.begin(); it != s2parents.end(); it++)
				parents.push(*it);
		} else if (PKB::callTable.count(curr->last) == 0)
			answer.push_back(curr->last);
		else
			parents.push(PKB::CFGTails[PKB::calls.getProcCall(curr->last)]);
	}
	return answer;
}

/**
* This method will be used to check NextBip*(p1,p2)
* @param p1	The prog_line that comes before p2
* @param p2	The prog_line that is after p1
* @return true if NextBip*(p1, p2) holds
*/
bool PQLNextProcessor::isNextBipStar(PROG_LINE p1, PROG_LINE p2)
{
	if (p1 < 0 || p1 > PKB::maxProgLines || p2 < 0 || p2 > PKB::maxProgLines){
		return false;
	}

	set<PROC> seenProc;
	set<STMT> seenStmt;
	queue<PROC> toSee;
	queue<PROG_LINE> starter;

	seenStmt.insert(p1);
	starter.push(p1);
	const CFGNode* s2 = PKB::stmtRefMap.at(p2).getCFGNode();

	while (!starter.empty()) {
		const PROG_LINE start = starter.front();
		starter.pop();
		const CFGNode* s1 = PKB::stmtRefMap.at(start).getCFGNode();

		if (p2 > start && p2 <= s1->last) {
			return true;
		}

		if (s1->proc == s2->proc) {
			const IntervalList * list = s1->nextList;
			if (list != NULL) {
				if (p2 < list->first) {
					list = list->prev;
					while (list != NULL && p2 < list->first)
						list = list->prev;
					if (list != NULL && p2 <= list->last)
						return true;
				} else if (p2 > list->last) {
					list = list->next;
					while (list != NULL && p2 > list->last)
						list = list->next;
					if (list != NULL && p2 >= list->first)
						return true;
				} else
					return true;
			}
		}
		
		const vector<PROC>& s1procs = PKB::calls.getCalledBy(s1->proc);
		for (auto it = s1procs.begin(); it != s1procs.end(); it++) {
			if (seenProc.count(*it) > 0)
				continue;
			bool found = false;
			const vector<STMT>& stmts = PKB::calls.getStmtCall(*it);
			for (auto it2 = stmts.begin(); it2 != stmts.end(); it2++) {
				if (*it2 >= start && *it2 <= s1->last) {
					found = true;
					break;
				}

				const IntervalList * list = s1->nextList;
				if (list != NULL) {
					if (*it2 < list->first) {
						list = list->prev;
						while (list != NULL && *it2 < list->first)
							list = list->prev;
						if (list != NULL && *it2 <= list->last) {
							found = true;
							break;
						}
					} else if (*it2 > list->last) {
						list = list->next;
						while (list != NULL && *it2 > list->last)
							list = list->next;
						if (list != NULL && *it2 >= list->first) {
							found = true;
							break;
						}
					} else {
						found = true;
						break;
					}
				}
			} //end for each statement that calls the procedure
			if (found) {
				toSee.push(*it);
				seenProc.insert(*it);
			}
		}
	
		while (!toSee.empty()) {
			PROC currProc = toSee.front();
			toSee.pop();
			/*if (seenProc.count(currProc) > 0)
				continue;
			seenProc.insert(currProc);*/

			const pair<STMT, STMT>& pair = PKB::TheBeginningAndTheEnd[currProc];
			STMT first = pair.first;
			STMT last = pair.second;
			if (p2 >= first && p2 <= last)
				return true;

			const vector<PROC>& currProcProcs = PKB::calls.getCalledBy(currProc);
			for (auto it = currProcProcs.begin(); it != currProcProcs.end(); it++) {
				if (seenProc.count(*it) == 0) {
					toSee.push(*it);
					seenProc.insert(*it);
				}
			}
		}

		const vector<STMT>& returnStmts = PKB::calls.getStmtCall(s1->proc);
		for (auto it = returnStmts.begin(); it != returnStmts.end(); it++) {
			if (seenStmt.count(*it) == 0) {
				PROC proc = PKB::stmtRefMap[*it].getCFGNode()->proc;
				if (seenProc.count(proc) == 0)
					starter.push(*it);
					seenStmt.insert(*it);
			}
		}
	}
	
	return false;
}

/**
* This method will be used to get all prog_lines after p1
* @param p1	The prog_line
* @return list of prog_line satisfying NextBip*(p1, _)
*/
vector<PROG_LINE> PQLNextProcessor::getNextBipStar(PROG_LINE p1)
{
	if (p1 < 0 || p1 > PKB::maxProgLines)
		return vector<PROG_LINE>();
	
	vector<PROG_LINE> answer;
	set<PROC> seenProc;
	set<STMT> seenStmt;
	queue<PROC> toSee;
	queue<PROG_LINE> starter;

	seenStmt.insert(p1);
	starter.push(p1);

	while (!starter.empty()) {
		const PROG_LINE start = starter.front();
		starter.pop();
		const CFGNode* s1 = PKB::stmtRefMap.at(start).getCFGNode();

		for (int i = p1 + 1; i <= s1->last; ++i)
			answer.push_back(i);

		const IntervalList * node = s1->nextList;

		if (node != NULL) {
			for (int i = node->first; i <= node->last; i++)
				answer.push_back(i);

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
		
		const vector<PROC>& s1procs = PKB::calls.getCalledBy(s1->proc);
		for (auto it = s1procs.begin(); it != s1procs.end(); it++) {
			if (seenProc.count(*it) > 0)
				continue;
			bool found = false;
			const vector<STMT>& stmts = PKB::calls.getStmtCall(*it);
			for (auto it2 = stmts.begin(); it2 != stmts.end(); it2++) {
				if (*it2 >= start && *it2 <= s1->last) {
					found = true;
					break;
				}

				const IntervalList * list = s1->nextList;
				if (list != NULL) {
					if (*it2 < list->first) {
						list = list->prev;
						while (list != NULL && *it2 < list->first)
							list = list->prev;
						if (list != NULL && *it2 <= list->last) {
							found = true;
							break;
						}
					} else if (*it2 > list->last) {
						list = list->next;
						while (list != NULL && *it2 > list->last)
							list = list->next;
						if (list != NULL && *it2 >= list->first) {
							found = true;
							break;
						}
					} else {
						found = true;
						break;
					}
				}
			} //end for each statement that calls the procedure
			if (found) {
				toSee.push(*it);
				seenProc.insert(*it);
			}
		}
	
		while (!toSee.empty()) {
			const PROC currProc = toSee.front();
			toSee.pop();

			const pair<STMT, STMT>& pair = PKB::TheBeginningAndTheEnd[currProc];
			for (int i = pair.first; i <= pair.second; ++i)
				answer.push_back(i);

			const vector<PROC>& currProcProcs = PKB::calls.getCalledBy(currProc);
			for (auto it = currProcProcs.begin(); it != currProcProcs.end(); it++) {
				if (seenProc.count(*it) == 0) {
					toSee.push(*it);
					seenProc.insert(*it);
				}
			}
		}

		const vector<STMT>& returnStmts = PKB::calls.getStmtCall(s1->proc);
		for (auto it = returnStmts.begin(); it != returnStmts.end(); it++) {
			if (seenStmt.count(*it) == 0) {
				PROC proc = PKB::stmtRefMap[*it].getCFGNode()->proc;
				if (seenProc.count(proc) == 0)
					starter.push(*it);
					seenStmt.insert(*it);
			}
		}
	}
	
	return answer;
}

/**
* This method will be used to get all prog_lines before p2
* @param p2	The prog_line
* @return list of prog_line satisfying NextBip*(_, p2)
*/
vector<PROG_LINE> PQLNextProcessor::getPreviousBipStar(PROG_LINE p2)
{
	if (p2 < 0 || p2 > PKB::maxProgLines){
		return vector<PROG_LINE>();
	}

	CFGNode* s2 = PKB::stmtRefMap.at(p2).getCFGNode();
	vector<PROG_LINE> answer;
	queue<CFGNode*> parents;

	if (p2 > s2->first) {
		if (PKB::callTable.count(p2 - 1) == 0)
			return vector<PROG_LINE>(1, p2 - 1);

		parents.push(PKB::CFGTails[PKB::calls.getProcCall(p2 - 1)]);
	} else {
		vector<CFGNode*> s2parents = s2->parents;
		if (s2parents.empty())
			return PKB::calls.getStmtCall(p2);
		
		for (auto it = s2parents.begin(); it != s2parents.end(); it++)
			parents.push(*it);
	}

	while (!parents.empty()) {
		CFGNode* curr = parents.front();
		parents.pop();
		if (curr->type == CFGNode::DummyNode) {
			vector<CFGNode*> s2parents = s2->parents;
			for (auto it = s2parents.begin(); it != s2parents.end(); it++)
				parents.push(*it);
		} else
			answer.push_back(curr->last);
	}
	return answer;
}

/**
* This method is only for special case which happens for while loops
*/
vector<PROG_LINE> PQLNextProcessor::getSelfNextBipStar(){
	vector<PROG_LINE> ans;

	return ans;
}