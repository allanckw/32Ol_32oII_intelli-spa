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

//next*(n, n) - special case only happens in while loops
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
	//vector<PROG_LINE> temp;
	//stack<CFGNode*> nodesStack; 
	//vector<PROG_LINE> visited;

	//for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
	//		CFGNode* currNode=PKB::getCFGHead(currentProc);
	//		while( currNode->last = PKB::TheBeginningAndTheEnd.at(currentProc).second || 
	//			currNode->type == CFGNode::DummyNode && currNode->children.oneChild == NULL) {
	//			if(currNode->type==CFGNode::WhileNode) { //Traverse all whiles nodes in the program 
	//				nodesStack.push(PKB::getCFGHead(currentProc));
	//				
	//				stack<int> whileStack;
	//				CFGNode* prevnode = 0;

	//				while(nodesStack.size() > 0){
	//					CFGNode* tempnode = nodesStack.top();
	//					nodesStack.pop();	 
	//				
	//					if(tempnode->type != CFGNode::DummyNode) {//chk not a dummy node
	//						
	//						PROG_LINE firstprogline = tempnode->first;
	//						
	//						if(Helper::contains(visited,firstprogline)) {
	//							if(tempnode->type == CFGNode::WhileNode) {
	//								if(whileStack.top() == firstprogline) {
	//									whileStack.pop();
	//								}
	//								else
	//									throw SPAException("impossible state reach in next*, while loop not linking back");
	//							}					
	//							continue;//been here before//eg visited node
	//						}else {
	//							visited.push_back(firstprogline);
	//							
	//							if(tempnode->type == CFGNode::WhileNode)
	//								whileStack.push(firstprogline);
	//							
	//							if(whileStack.size() > 0) {
	//								for(int i = tempnode->first; i <= tempnode->last; i++) {
	//								//for(int i=0;i<tempnode->getProgramLines().size();i++) {
	//									temp.push_back(i);
	//									//temp.push_back(tempnode->getProgramLines().at(i));
	//								}
	//							}
	//						}
	//					}else {//is a dummie node
	//						vector<CFGNode*> prev = tempnode->parents;
	//						if(prev.size() > 0 &&  prev.at(0) == prevnode)//come from left
	//							continue;
	//					}
	//					
	//					vector<CFGNode*> next = tempnode->getNextNodes();
	//					for(int i=next.size()-1; i>=0; i--){
	//						nodesStack.push(next.at(i));//add right side in
	//					}
	//					prevnode = tempnode;
	//				}
	//			} else {
	//				//cout <<	currNode->children.oneChild->first;
	//				cout << currNode->children.ifChildren.ifThen;
	//				//cout << currNode->children.whileChildren.whileIn;
	//			}
	//		}
	//}
	//return temp;
}
