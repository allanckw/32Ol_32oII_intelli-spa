#include "PQLNextProcessor.h"
#include "Helper.h"

bool PQLNextProcessor::isNextStar(PROG_LINE p1, PROG_LINE p2)
{
	vector<PROG_LINE> temp;
	stack<CFGNode*> nodesStack; 

	CFGNode* node = PKB::getCFGHead(0); //TODO Change to get CFGNode from the PROGLINE-STMT-ASTNode-CFGNode Table for p1
	
	for(int i=0;i<node->getNextNodes().size();i++) {
		nodesStack.push(node->getNextNodes().at(i));//maybe change here
	}
		
	while(nodesStack.size() > 0){

		CFGNode* tempnode = nodesStack.top();
		nodesStack.pop();
			
		vector<CFGNode*> next = tempnode->getNextNodes();

		if(tempnode->getProgramLines().size() > 0)//chk not a dummy node
		{
			PROG_LINE firstprogline = tempnode->getProgramLines().at(0);
				
			if(Helper::contains(temp,firstprogline)) {
				continue;//been here before//eg visited node
			} else {
				for(int i=0; i < tempnode->getProgramLines().size(); i++) {
					if(tempnode->getProgramLines().at(i) == p2) {

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
	
	CFGNode* node = PKB::getCFGHead(0); //TODO Change to get CFGNode from the PROGLINE-STMT-ASTNode-CFGNode Table
	
	for(int i=0;i<node->getProgramLines().size();i++) {
		PROG_LINE tpl = node->getProgramLines().at(i);
		
		if(tpl == p1) {
			if(i < node->getProgramLines().size()) {
				for(int j=i+1;j<node->getProgramLines().size();j++) {
						temp.push_back(node->getProgramLines().at(j));
				}
			}
			break;
		}
	}
	for(int i=0;i<node->getNextNodes().size();i++) {
			nodesStack.push(node->getNextNodes().at(i));//maybe change here
	}

	while(nodesStack.size() > 0){
		CFGNode* tempnode = nodesStack.top();
		nodesStack.pop();	 
			
		vector<CFGNode*> next = tempnode->getNextNodes(); 

		if(tempnode->getProgramLines().size() > 0)//chk not a dummy node
		{
			PROG_LINE firstprogline = tempnode->getProgramLines().at(0);
				
			if(Helper::contains(temp,firstprogline)) {
					continue;//been here before//eg visited node
			} else {
				for(int i=0;i<tempnode->getProgramLines().size();i++) {
					if(tempnode->getProgramLines().at(i) != -1)
						temp.push_back(tempnode->getProgramLines().at(i));
				}
			}
		}

		for(int i=0; i<next.size(); i++){
				nodesStack.push(next.at(i));//add right side in
		}
	}

	return temp;
}

//Next (_, p2)
vector<PROG_LINE> PQLNextProcessor::getPreviousStar(PROG_LINE p2)
{
	vector<PROG_LINE> temp;
	stack<CFGNode*> nodesStack; 

	CFGNode* node = PKB::getCFGHead(0); //TODO Change to get CFGNode from the PROGLINE-STMT-ASTNode-CFGNode Table
	
	for(int i=0;i<node->getProgramLines().size();i++){
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
	for(int i=0;i<node->getPreviousNodes().size();i++){
		nodesStack.push(node->getPreviousNodes().at(i));//maybe change here
		
	}
		
	while(nodesStack.size() > 0) {
		CFGNode* tempnode = nodesStack.top();
		nodesStack.pop();
		
		vector<CFGNode*> next = tempnode->getPreviousNodes();
		
		if(tempnode->getProgramLines().size() > 0)//chk not a dummy node
			{
				PROG_LINE firstprogline = tempnode->getProgramLines().at(0);
				
				if(Helper::contains(temp,firstprogline))
				{
					continue;//been here before//eg visited node
				}
				else
				{
					for(int i=0;i<tempnode->getProgramLines().size();i++)
					{
						if(tempnode->getProgramLines().at(i) != -1)
						temp.push_back(tempnode->getProgramLines().at(i));
					}
				}
			}

			for(int i=0; i<next.size(); i++) {
				nodesStack.push(next.at(i));//add right side in
			}
		}
		return temp;
	}
	