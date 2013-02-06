#include "NextTable.h"
#include "CFGBuilder.h"
#include "PKB.h"
#include "CFGNode.h"

NextTable::NextTable(void)
{
}


NextTable::~NextTable(void)
{
}

	bool NextTable::isNextStar(STMT s1, STMT s2)
	{
		vector<STMT> temp;
		stack<CFGNode*> nodesStack; 

		CFGNode* node = PKB::getCFGHead(0);

		if(contains(node->getProgramLines(),s2) && s1 < s2)
			return true;

		
		for(int i=0;i<node->getNextNodes().size();i++)
		{
			nodesStack.push(node->getNextNodes().at(i));//maybe change here
		}
		while(nodesStack.size() > 0)
		{

			CFGNode* tempnode = nodesStack.top();
			nodesStack.pop();
							

			//if (MatcherTree(tempnode,pattern))//,isSub))
			//{
			//	return true; 
			//}
			
			
			vector<CFGNode*> next = tempnode->getNextNodes();

			if(tempnode->getProgramLines().size() > 0)//chk not a dummy node
			{
				STMT firstprogline = tempnode->getProgramLines().at(0);
				
				if(contains(temp,firstprogline))
				{
					continue;//been here before//eg visited node
				}
				else
				{
					for(int i=0;i<tempnode->getProgramLines().size();i++)
					{
						if(tempnode->getProgramLines().at(i) == s2)
						{
							return true;
						}
						temp.push_back(tempnode->getProgramLines().at(i));
					}
				}
			}

			for(int i=0;i<next.size();i++)
			{
				nodesStack.push(next.at(i));//add right side in
			}

				
			
			
		}

		return false;
	}
	bool NextTable::isNext(STMT s1, STMT s2)
	{
		CFGNode* node = PKB::getCFGHead(0);
		vector<CFGNode*> nextnodes = node->getNextNodes();

		if(contains(node->getProgramLines(),s2) && s1+1 == s2)
			return true;

		for(int i = 0;i<nextnodes.size();i++)
		{
			CFGNode* node = nextnodes.at(i);
			if(node->isDummy())
			{
				for(int j=0;j<node->getNextNodes().size();j++)
				{
					nextnodes.push_back(node->getNextNodes().at(j));
				}

				continue;
				
			}
			vector<PROG_LINE> ProgLinesnextnodes = node->getProgramLines();
			//vector<PROG_LINE> ProgLinesnextnodes = nextnodes.at(i)->getProgramLines();

			if(ProgLinesnextnodes.size() > 0 && ProgLinesnextnodes.at(0) < s2 && (ProgLinesnextnodes.at(0)+ProgLinesnextnodes.size() - 1)>s2)
				return true;
			/*for(int j=0;j<ProgLinesnextnodes.size();j++)
			{
				if(ProgLinesnextnodes.at(j) == s2)
					return true;
			}*/
		}
		return false;
	}

	bool NextTable::contains(vector<STMT> list, STMT s1)
	{
		if(std::find(list.begin(),list.end(), s1) != list.end())
		{
			return true;
		}
		return false;
	}

	vector<STMT> NextTable::getNextStar(STMT s1)
	{
		vector<STMT> temp;
		stack<CFGNode*> nodesStack; 

		CFGNode* node = PKB::getCFGHead(0);

		for(int i=0;i<node->getProgramLines().size();i++)
		{
			PROG_LINE tpl = node->getProgramLines().at(i);
			if(tpl == s1)
			{
				if(i < node->getProgramLines().size())
				{
					for(int j=i+1;j<node->getProgramLines().size();j++)
					{
						temp.push_back(node->getProgramLines().at(j));
					}
				}
				break;
			}
		}

		for(int i=0;i<node->getNextNodes().size();i++)
		{
			nodesStack.push(node->getNextNodes().at(i));//maybe change here
		}

		while(nodesStack.size() > 0)
		{

			CFGNode* tempnode = nodesStack.top();
			nodesStack.pop();
							

			//if (MatcherTree(tempnode,pattern))//,isSub))
			//{
			//	return true; 
			//}
			
			
			vector<CFGNode*> next = tempnode->getNextNodes();

			if(tempnode->getProgramLines().size() > 0)//chk not a dummy node
			{
				STMT firstprogline = tempnode->getProgramLines().at(0);
				
				if(contains(temp,firstprogline))
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

			for(int i=0;i<next.size();i++)
			{
				nodesStack.push(next.at(i));//add right side in
			}

				
			
			
		}

		return temp;
	}
	vector<STMT> NextTable::getPreviousStar(STMT s2)
	{
		vector<STMT> temp;
		stack<CFGNode*> nodesStack; 

		CFGNode* node = PKB::getCFGHead(0);

		for(int i=0;i<node->getProgramLines().size();i++)
		{
			PROG_LINE tpl = node->getProgramLines().at(i);
			if(tpl == s2)
			{
				if(i > 0)
				{
					for(int j=i-1;j>=0;j--)
					{
						temp.push_back(node->getProgramLines().at(j));
					}
				}
				break;
			}
		}
		
		for(int i=0;i<node->getPreviousNodes().size();i++)
		{
			nodesStack.push(node->getPreviousNodes().at(i));//maybe change here
		}
		while(nodesStack.size() > 0)
		{

			CFGNode* tempnode = nodesStack.top();
			nodesStack.pop();
							

			//if (MatcherTree(tempnode,pattern))//,isSub))
			//{
			//	return true; 
			//}
			
			//tempnode->getAllNextNodes()
			vector<CFGNode*> next = tempnode->getPreviousNodes();



			if(tempnode->getProgramLines().size() > 0)//chk not a dummy node
			{
				STMT firstprogline = tempnode->getProgramLines().at(0);
				
				if(contains(temp,firstprogline))
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

			for(int i=0;i<next.size();i++)
			{
				nodesStack.push(next.at(i));//add right side in
			}

				
			
			
		}

		return temp;
	}
	vector<STMT> NextTable::getNext(STMT s1)
	{
		vector<STMT> temp;

		CFGNode* node = PKB::getCFGHead(0);
		vector<CFGNode*> nextnodes = node->getNextNodes();

		

		for(int i=0;i<node->getProgramLines().size();i++)
		{
			PROG_LINE tpl = node->getProgramLines().at(i);
			if(tpl == s1)
			{
				if(i < node->getProgramLines().size())
				{
					for(int j=i+1;j<node->getProgramLines().size();j++)
					{
						temp.push_back(node->getProgramLines().at(j));
					}
				}
				break;
			}
		}


		if(temp.size() > 0)
			return temp;



		for(int i = 0;i<nextnodes.size();i++)
		{
			CFGNode* node = nextnodes.at(i);
			if(node->isDummy())
			{
				for(int j=0;j<node->getNextNodes().size();j++)
				{
					nextnodes.push_back(node->getNextNodes().at(j));
				}

				continue;
				
			}
			vector<PROG_LINE> ProgLinesnextnodes = node->getProgramLines();



			for(int j=0;j<ProgLinesnextnodes.size();j++)
			{
				temp.push_back(ProgLinesnextnodes.at(j));
			}
			
		}
		

		return temp;
	}
	vector<STMT> NextTable::getPrevious(STMT s2)
	{
		vector<STMT> temp;

		CFGNode* node = PKB::getCFGHead(0);
		vector<CFGNode*> prevnodes = node->getPreviousNodes();

		for(int i=0;i<node->getProgramLines().size();i++)
		{
			PROG_LINE tpl = node->getProgramLines().at(i);
			if(tpl == s2)
			{
				if(i > 0)
				{
					for(int j=i-1;j>=0;j--)
					{
						temp.push_back(node->getProgramLines().at(j));
					}
				}
				break;
			}
		}
		
		if(temp.size() > 0)
			return temp;
		for(int i = 0;i<prevnodes.size();i++)
		{

			CFGNode* node = prevnodes.at(i);
			if(node->isDummy())
			{
				for(int j=0;j<node->getPreviousNodes().size();j++)
				{
					prevnodes.push_back(node->getPreviousNodes().at(j));
				}

				continue;
				
			}
			vector<PROG_LINE> ProgLinesprevnodes = node->getProgramLines();
			//vector<PROG_LINE> ProgLinesprevnodes = prevnodes.at(i)->getProgramLines();

			for(int j=0;j<ProgLinesprevnodes.size();j++)
			{
				temp.push_back(ProgLinesprevnodes.at(j));
			}
			
		}
		

		return temp;
	}