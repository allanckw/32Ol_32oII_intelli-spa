#include "PQLAffectsProcessor.h"
#include "Helper.h"

bool PQLAffectsProcessor::isSatifyAffects(STMT a1, STMT a2)
{
	if (a1 <= 0 || a2 <= 0) return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	ASTStmtNode* a2ASTNode = PKB::stmtRefMap.at(a2).getASTStmtNode();
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 
	CFGNode* a2CFGNode = PKB::stmtRefMap.at(a2).getCFGNode(); 

	if (a1CFGNode->getProcedure() != a2CFGNode->getProcedure()) { //Check if same procedure
		PKB::affects.insertAffects(a1, a2, false);
		return false; 
	}
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign) { // return false if either stmt is not assign
		PKB::affects.insertAffects(a1, a2, false);
		return false; 
	}
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified

	if (!(PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar))) { //if nextStar(a1, a2) and uses(a2, v) does not hold 
		PKB::affects.insertAffects(a1, a2, false);
		return false;
	}

	return true;
}

bool PQLAffectsProcessor::isAffects(STMT a1, STMT a2) {

	if (PQLAffectsProcessor::isSatifyAffects(a1, a2) == false) 
		return false;

	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 

	stack<PROG_LINE> lines2visit;
	set<STMT> visited;

	bool selfAffects = (a1 == a2);

	vector<PROG_LINE> nexts = PKB::next.getNext(a1);

	for (int i = 0; i < nexts.size(); i++) {
		lines2visit.push(nexts.at(i));
	}

	while(lines2visit.size() > 0) {
		
		STMT curr = lines2visit.top();
		lines2visit.pop();
		
		// visited?
		if (visited.find(curr) != visited.end()) 
			continue;

		visited.insert(curr);

		if (curr == a2) {
			PKB::affects.insertAffects(a1, a2, true);
			return true;
		}

		if (!PKB::next.isNextStar(a1, curr)) //if next* does not hold (i.e wrong branch)
			continue;

		ASTStmtNode* n = PKB::stmtRefMap.at(curr).getASTStmtNode();	
		if (n->getType() == ASTNode::Call) { // the variable is modified along the path
			if (PKB::modifies.isModifiedProc(n->getValue(), modifiedVar)){
				continue;
			}
		}

		if (n->getType() == ASTNode::Assign) {
			if (PKB::modifies.isModifiedStmt(n->getStmtNumber(), modifiedVar)){
				continue;
			}
		}
		
		vector<PROG_LINE> temp = PKB::next.getNext(curr);

		for (int k = 0; k < temp.size(); k++) {
			lines2visit.push(temp.at(k));
		}
	}
		
	return false;
}

bool PQLAffectsProcessor::isAffectsStar(STMT a1, STMT a2) {
	
	if (!isSatifyAffects(a1, a2)) 
		return false;

	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 

	stack<PROG_LINE> lines2visit;
	set<STMT> visited;

	vector<PROG_LINE> nexts = PKB::next.getNext(a1);

	for (int i = 0; i < nexts.size(); i++) {
		lines2visit.push(nexts.at(i));
	}

	while(lines2visit.size() > 0) {
		STMT curr = lines2visit.top();
		lines2visit.pop();

		// visited?
		if (visited.find(curr) != visited.end()) 
			continue;
		visited.insert(curr);

		if (!(PKB::next.isNextStar(curr, a2))) //if next* does not hold for the 2nd portion of transitive closure
			continue;

		//@JK Compute Transitive Closure using pooling method
		//http://www8.cs.umu.se/kurser/TDBAfl/VT06/algorithms/BOOK/BOOK4/NODE163.HTM
		//The simplest algorithm just performs a breadth-first or depth-first search from each vertex and 
		//keeps track of all vertices encountered.   Doing n such traversals gives an O(n (n+m) ) algorithm, 
		//which degenerates to cubic time if the graph is dense. This algorithm is easily implemented, 
		//runs well on sparse graphs, and is likely the right answer for your application.

		//@JK DFS Completed, ur jod is to track of all vertices, the vertices are the variables being used in subsequent
		//program lines..
		vector<PROG_LINE> temp = PKB::next.getNext(curr);

		for (int k = 0; k < temp.size(); k++) {
			lines2visit.push(temp.at(k));
		}
	}
	return false;
}

bool PQLAffectsProcessor::isAffectsBip(STMT a1, STMT a2) {
	if (a1 <= 0 || a2 <= 0) 
		return false;
	
	return false; 
		
}


vector<STMT> PQLAffectsProcessor::AffectsBy(STMT testaffectfirst)
{
	
	vector<PROG_LINE> t1 = PKB::next.getNext(testaffectfirst);
	vector<PROG_LINE> t2 = PKB::next.getPrevious(2);

	stack<PROG_LINE> progStack; 
	for(int i=0;i<t1.size();i++)
	{
		progStack.push(t1.at(i));
	}
	
	vector<PROG_LINE> test;
	vector<PROG_LINE> visited;

	stack<pair<pair<bool,bool>,int>> iflhs;//0 == left branch

	int prevProgLine=0;

	vector<VAR> varl =  PKB::modifies.getModifiedByStmt(testaffectfirst);

	int varindex = varl.at(0);

	while(progStack.size() > 0)
	{

		
		int currentLine = progStack.top();
		progStack.pop();

		if(Helper::contains(visited, currentLine))
		{
			//node visisted before
			continue;
		}

		vector<PROG_LINE> ne = PKB::next.getNext(currentLine);
		vector<PROG_LINE> pr = PKB::next.getPrevious(currentLine);
		
		bool isassing =true;
		if(ne.size() == 2 || pr.size() == 2)
		{
			
			CFGNode* node = PKB::stmtRefMap.at(currentLine).getCFGNode(); 
			

			if(node->getType() == CFGNode::IfNode)
			{
				isassing = false;
				//if start
				iflhs.push(pair<pair<bool,bool>,int>(pair<bool,bool>(false,false),0));
			}
			else if(node->getType() == CFGNode::WhileNode)
			{
				isassing = false;
			}


			if(pr.size() > 1 && pr.at(0) == prevProgLine && node->getType() != CFGNode::WhileNode)
			{
				//if left side
				pair<pair<bool,bool>,int> tempd = iflhs.top();
				tempd.second = 1;
				//continue;
			}

			if(pr.size() > 1 && pr.at(1) == prevProgLine && node->getType() != CFGNode::WhileNode)
			{
				//if right side
				continue;
				pair<pair<bool,bool>,int> tempd = iflhs.top();
				iflhs.pop();
			}
		}


		vector<VAR> proglinem = PKB::modifies.getModifiedByStmt(currentLine);
		vector<VAR> varu = PKB::uses.getUsedByStmt(currentLine);


		bool isused = false;
		for(int z=0;z<varu.size();z++)
		{
			if(varu.at(z) == varindex)
			{
				isused = true;
				break;
			}
		}

		bool ismod = false;
		for(int z=0;z<proglinem.size();z++)
		{
			if(proglinem.at(z) == varindex)
			{
				ismod = true;
				break;
			}
		}
		int lolt = currentLine;
		if(isused && isassing)
		{
			test.push_back(currentLine);
		}

		CFGNode::NodeType ll = PKB::stmtRefMap.at(currentLine).getCFGNode()->getType();
		if(ismod && PKB::stmtRefMap.at(currentLine).getCFGNode()->getType() != CFGNode::IfNode && PKB::stmtRefMap.at(currentLine).getCFGNode()->getType() != CFGNode::WhileNode)
		{
			
			continue;
		}
		

		visited.push_back(currentLine);
		prevProgLine = currentLine;
		//test.push_back(currentLine);

		for(int j=(ne.size() - 1);j >= 0;j--)
		{
			progStack.push(ne.at(j));
		}
	}
	return test;
}
vector<STMT> PQLAffectsProcessor::AffectsFrom(STMT testaffectfirst)
{
	
	vector<PROG_LINE> t1 = PKB::next.getPrevious(testaffectfirst);
	
	stack<PROG_LINE> progStack; 
	for(int i=0;i<t1.size();i++)
	{
		progStack.push(t1.at(i));
	}
	
	vector<PROG_LINE> test;
	vector<PROG_LINE> visited;

	stack<pair<pair<bool,bool>,int>> iflhs;//0 == left branch

	int prevProgLine=0;

	vector<VAR> varl =  PKB::uses.getUsedByStmt(testaffectfirst);

	while(progStack.size() > 0)
	{

		
		int currentLine = progStack.top();
		progStack.pop();

		if(Helper::contains(visited, currentLine))
		{
			//node visisted before
			continue;
		}

		vector<PROG_LINE> ne = PKB::next.getNext(currentLine);
		vector<PROG_LINE> pr = PKB::next.getPrevious(currentLine);

	
		bool isassing =true;
		if(ne.size() == 2 || pr.size() == 2)
		{
			
			CFGNode* node = PKB::stmtRefMap.at(currentLine).getCFGNode(); 
			

			if(node->getType() == CFGNode::WhileNode)
			{
				isassing = false;
				//if start
				
			}
			else
			{

				isassing = false;
			}



		}


		vector<VAR> proglinem = PKB::modifies.getModifiedByStmt(currentLine);//one only
		vector<VAR> varu = PKB::uses.getUsedByStmt(currentLine);//multi


		bool isused = false;
		for(int z=0;z<varu.size();z++)
		{
			//varl
			for(int y=0;y<varl.size();y++)
			{
				if(varu.at(z) == varl.at(y))
				{
					isused = true;
					goto cont1;

				}
			}
		}

		cont1:

		bool ismod = false;
		for(int z=0;z<proglinem.size();z++)
		{
			for(int y=0;y<varl.size();y++)
			{
				if(proglinem.at(z) == varl.at(y))
				{
					ismod = true;
					goto cont2;
				}
			}
		}
		cont2:
		int lolt = currentLine;
		if(ismod && isassing)
		{
			test.push_back(currentLine);
			visited.push_back(currentLine);
			continue;
		}

		CFGNode::NodeType ll = PKB::stmtRefMap.at(currentLine).getCFGNode()->getType();
		
		visited.push_back(currentLine);
		prevProgLine = currentLine;
		//test.push_back(currentLine);

		for(int j=(pr.size() - 1);j >= 0;j--)
		{
			progStack.push(pr.at(j));
		}
	}
	return test;
}