#include "PQLNextProcessor.h"
#include "Helper.h"

bool PQLNextProcessor::isNextStar(PROG_LINE p1, PROG_LINE p2)
{

	vector<PROG_LINE> temp;
	stack<CFGNode*> nodesStack; 
	
	if (p1 == p2) {
		temp = PQLNextProcessor::getNextStar(p1);
		return Helper::contains(temp, p2);
	}


	CFGNode* node = PKB::stmtRefMap.at(p1).getCFGNode(); 
	
	for(int i=0;i<node->getProgramLines().size();i++) { 
		PROG_LINE tpl = node->getProgramLines().at(i);
		
		if(i < node->getProgramLines().size()) { 
			for(int j=i+1;j<node->getProgramLines().size();j++) {
				if( node->getProgramLines().at(j) == p2){
					PKB::next.insertNextStar(p1, p2, true);
					return true;
				}
			}
		}	
		break; //break from for loop
		
	}
	
	for(int i=0;i<node->getNextNodes().size();i++) {
		nodesStack.push(node->getNextNodes().at(i));
	}
		
	while(nodesStack.size() > 0) {

		CFGNode* tempnode = nodesStack.top();
		nodesStack.pop();
			
		vector<CFGNode*> next = tempnode->getNextNodes();

		if(tempnode->getType() != CFGNode::DummyNode)//chk not a dummy node
		{
			PROG_LINE firstprogline = tempnode->getProgramLines().at(0);
				
			if(Helper::contains(temp, firstprogline)) {
				continue;//been here before//eg visited node
			} else {
				for(int i=0; i < tempnode->getProgramLines().size(); i++) {
					if(tempnode->getProgramLines().at(i) == p2) {
						PKB::next.insertNextStar(p1, p2, true);
						return true;
					}
					temp.push_back(tempnode->getProgramLines().at(i));
				}
			}
		}

		for(int i=0; i<next.size(); i++) {
			nodesStack.push(next.at(i));//add right side in
		}
	}
	return false;
}

vector<PROG_LINE> PQLNextProcessor::getNextStar(PROG_LINE p1)
{

	vector<PROG_LINE> temp;
	stack<CFGNode*> nodesStack; 

	
	if (p1 < 0 ) {

	} else if (p1 == 0) { 
		//Next*(n, n) only applicable to while loops, only while loops can go back to themselves
		//beware of nested whiles, they should also be inside..
		
		for (PROC currentProc = 0; currentProc < PKB::procedures.getSize(); currentProc++) {
			//CFGNode* currNode=PKB::getCFGHead(currentProc);


			nodesStack.push(PKB::getCFGHead(currentProc));
			vector<PROG_LINE> visited;
			
			stack<int> whileStack;
			//int ifstack = 0;

			CFGNode* prevnode = 0;

			while(nodesStack.size() > 0){
			CFGNode* tempnode = nodesStack.top();
			nodesStack.pop();	 
			
			if(tempnode->getType() != CFGNode::DummyNode) 
			{//chk not a dummy node
			

				PROG_LINE firstprogline = tempnode->getProgramLines().at(0);
				
				if(Helper::contains(visited,firstprogline)) 
				{
					if(tempnode->getType() == CFGNode::WhileNode)
					{
						 if(whileStack.top() == firstprogline)
						 {
							 whileStack.pop();
						 }
						 else
							 throw SPAException("impossible state reach in next*, while loop not linking back");
					}
					
					continue;//been here before//eg visited node
				} 
				else 
				{
					visited.push_back(firstprogline);
					if(tempnode->getType() == CFGNode::WhileNode)
						whileStack.push(firstprogline);

					if(whileStack.size() > 0)
					{
						for(int i=0;i<tempnode->getProgramLines().size();i++) {
							if(tempnode->getProgramLines().at(i) != -1) {
								//PKB::next.insertNextStar(p1, tempnode->getProgramLines().at(i), true);
								temp.push_back(tempnode->getProgramLines().at(i));
							}
						}

					}
				}
			}
			else
			{//is a dummie node
				//if is a dummie node and come from left side means from IF, go back to by continuing
				vector<CFGNode*> prev = tempnode->getPreviousNodes(); 
				if(prev.size() > 0 &&  prev.at(0) == prevnode)//come from left
					continue;
			}
			vector<CFGNode*> next = tempnode->getNextNodes(); 
			for(int i=next.size()-1; i>=0; i--){
					nodesStack.push(next.at(i));//add right side in
			}

			prevnode = tempnode;
		
		}


		}
	} else {
		//Normal Case 
		CFGNode* node = PKB::stmtRefMap.at(p1).getCFGNode(); 
	
		for(int i=0;i<node->getProgramLines().size();i++) { 
			PROG_LINE tpl = node->getProgramLines().at(i);
		
			if(tpl == p1) { //if the next line contains itself (i.e. while loop)
				if(i < node->getProgramLines().size()) { // i < the the total no. of program lines
					for(int j=i+1;j<node->getProgramLines().size();j++) {
						//then all its lines after are nextstar bcos of the while loop effect
						PKB::next.insertNextStar(p1, node->getProgramLines().at(j), true);
						temp.push_back(node->getProgramLines().at(j));
					}
				}
				break; //break from for loop
			}
		}

		for(int i=0;i<node->getNextNodes().size();i++) {
				nodesStack.push(node->getNextNodes().at(i));
		}

		while(nodesStack.size() > 0){
			CFGNode* tempnode = nodesStack.top();
			nodesStack.pop();	 
			
			if(tempnode->getType() != CFGNode::DummyNode) {//chk not a dummy node
			
				PROG_LINE firstprogline = tempnode->getProgramLines().at(0);
				
				if(Helper::contains(temp,firstprogline)) {
						continue;//been here before//eg visited node
				} else {
					for(int i=0;i<tempnode->getProgramLines().size();i++) {
						if(tempnode->getProgramLines().at(i) != -1) {
							PKB::next.insertNextStar(p1, tempnode->getProgramLines().at(i), true);
							temp.push_back(tempnode->getProgramLines().at(i));
						}
					}
				}
			}
			vector<CFGNode*> next = tempnode->getNextNodes(); 
			for(int i=0; i<next.size(); i++){
					nodesStack.push(next.at(i));//add right side in
			}
		}
	}

	return temp;
}

//Next (_, p2)
vector<PROG_LINE> PQLNextProcessor::getPreviousStar(PROG_LINE p2)
{
	vector<PROG_LINE> temp;
	stack<CFGNode*> nodesStack; 

	CFGNode* node = PKB::stmtRefMap.at(p2).getCFGNode(); 
	
	for(int i=0;i<node->getProgramLines().size();i++) {
		
		PROG_LINE tpl = node->getProgramLines().at(i);

		if(tpl == p2){
			if(i > 0) {
				for(int j=i-1; j>=0; j--) {
						temp.push_back(node->getProgramLines().at(j));
				}
			}
			break;
		}
	}
	for(int i=0;i<node->getPreviousNodes().size();i++) {
	
		nodesStack.push(node->getPreviousNodes().at(i));
	}
		
	while(nodesStack.size() > 0) {
		CFGNode* tempnode = nodesStack.top();
		nodesStack.pop();
		
		vector<CFGNode*> next = tempnode->getPreviousNodes();
		
		if(tempnode->getType() != CFGNode::DummyNode) {//chk not a dummy node
			PROG_LINE firstprogline = tempnode->getProgramLines().at(0);

			if(Helper::contains(temp,firstprogline)) {
				continue;//been here before//eg visited node
			} else {
				for(int i=0;i<tempnode->getProgramLines().size();i++) {
					if(tempnode->getProgramLines().at(i) != -1){
						PKB::next.insertNextStar(tempnode->getProgramLines().at(i), p2, true);
						temp.push_back(tempnode->getProgramLines().at(i));
					}
				}
			}
		}
		
		for(int i=0; i<next.size(); i++) {
			nodesStack.push(next.at(i));//add right side in
		}
	}
	return temp;
}
	
//vector<PROG_LINE> zzz (PROG_LINE p1)
//{
//	//nicky nicky happy cny 
//	//p1++;
//	
//	CFGNode* node = PKB::stmtRefMap.at(p1).getCFGNode(); 
//		
//	vector<PROG_LINE> temp;
//	stack<CFGNode*> nodesStack; 
//
//	nodesStack.push(node);
//	vector<PROG_LINE> visited;
//
//			stack<int> whileStack;
//			stack<pair<int,int>> ifstack; //progline,0=left,1=right
//			//int ifstack = 0;
//
//			CFGNode* prevnode = 0;
//
//			while(nodesStack.size() > 0){
//			CFGNode* tempnode = nodesStack.top();
//			nodesStack.pop();	 
//			
//			if(tempnode->getType() != CFGNode::DummyNode) 
//			{//chk not a dummy node
//			
//
//				PROG_LINE firstprogline = tempnode->getProgramLines().at(0);
//				
//				if(Helper::contains(visited,firstprogline)) 
//				{
//					if(tempnode->getType() == CFGNode::WhileNode)
//					{
//						 if(whileStack.top() == firstprogline)
//						 {
//							 whileStack.pop();
//						 }
//						 else
//							 throw SPAException("impossible state reach in next*, while loop not linking back");
//					}
//					
//					
//					continue;//been here before//eg visited node
//				} 
//				else 
//				{
//					visited.push_back(firstprogline);
//					if(tempnode->getType() == CFGNode::WhileNode)
//						whileStack.push(firstprogline);
//					else if(tempnode->getType() == CFGNode::IfNode)
//					{
//						
//						ifstack.push(pair<int,int>(firstprogline,0));
//					}
//
//					if(whileStack.size() > 0)
//					{
//						//current node is inside a while loop 
//					}
//					else if(ifstack.size() >0)
//					{
//						if(ifstack.top().second == 0)
//						{
//							//if left branch
//						}
//						else
//						{
//							//if right branch
//						}
//					}
//					
//						for(int i=0;i<tempnode->getProgramLines().size();i++) {
//							if(tempnode->getProgramLines().at(i) != -1) {
//								//PKB::next.insertNextStar(p1, tempnode->getProgramLines().at(i), true);
//								temp.push_back(tempnode->getProgramLines().at(i));
//							}
//						}
//
//					
//				}
//			}
//			else
//			{//is a dummie node
//				//if is a dummie node and come from left side means from IF, go back to by continuing
//				vector<CFGNode*> prev = tempnode->getPreviousNodes(); 
//				if(prev.size() > 0 &&  prev.at(0) == prevnode)//come from left
//				{
//					ifstack.top().second++;
//					continue;
//				}
//				else if(prev.size() > 1 &&  prev.at(1) == prevnode)
//				{
//					ifstack.pop();
//				}
//			}
//			vector<CFGNode*> next = tempnode->getNextNodes(); 
//			for(int i=next.size()-1; i>=0; i--){
//					nodesStack.push(next.at(i));//add right side in
//			}
//
//			prevnode = tempnode;
//		
//		}
//
//		return temp;
//}

