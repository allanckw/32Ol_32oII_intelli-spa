#include "PQLAffectsProcessor.h"
#include "Helper.h"

//Affects
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

	stack<STMT> lines2visit;
	set<STMT> visited;
	vector<PROG_LINE> nexts = PKB::next.getNext(a1);

	for (int i = 0; i < nexts.size(); i++) 
		lines2visit.push(nexts.at(i));
	
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
		
		vector<STMT> temp = PKB::next.getNext(curr);

		for (int k = 0; k < temp.size(); k++) {
			lines2visit.push(temp.at(k));
		}
	}
		
	return false;
}

vector<STMT> PQLAffectsProcessor::getAffectsBy(STMT a1)
{
	vector<PROG_LINE> nexts = PKB::next.getNext(a1);
	stack<STMT> lines2visit; 

	for (int i = 0; i < nexts.size(); i++) 
		lines2visit.push(nexts.at(i));
	
	vector<STMT> result;
	set<STMT> visited;

	stack<pair<pair<bool,bool>,int>> ifLHS;//0 == left branch

	PROG_LINE prevProgLine = 0;
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified

	while(lines2visit.size() > 0) {
		STMT curr = lines2visit.top();
		lines2visit.pop();

		// visited?
		if (visited.find(curr) != visited.end()) 
			continue;
		
		visited.insert(curr);
		ASTNode* currASTNode = PKB::stmtRefMap.at(curr).getASTStmtNode();

		vector<STMT> nextLine = PKB::next.getNext(curr);
		vector<STMT> prevLine = PKB::next.getPrevious(curr);
		
		bool isPassingBy = true;
		if(nextLine.size() == 2 || prevLine.size() == 2) {
			
			CFGNode* node = PKB::stmtRefMap.at(curr).getCFGNode(); 
			
			if(node->getType() == CFGNode::IfNode) 	{
				isPassingBy = false;
				//if start
				ifLHS.push(pair<pair<bool,bool>,int>(pair<bool,bool>(false,false), 0));

			} else if(node->getType() == CFGNode::WhileNode) {
				isPassingBy = false;
			}

			if(prevLine.size() > 1 && prevLine.at(0) == prevProgLine && node->getType() != CFGNode::WhileNode) 	{
				//if left side
				pair<pair<bool,bool>,int> tempd = ifLHS.top();
				tempd.second = 1;
			}

			if(prevLine.size() > 1 && prevLine.at(1) == prevProgLine && node->getType() != CFGNode::WhileNode) {
				//if right side
				continue;
				pair<pair<bool,bool>,int> tempd = ifLHS.top();
				ifLHS.pop();
			}
		}

		vector<VAR> proglinem = PKB::modifies.getModifiedByStmt(curr);
		vector<VAR> varu = PKB::uses.getUsedByStmt(curr);
		
		bool isused = false;
		for(int z=0; z<varu.size(); z++) {
			if(varu.at(z) == modifiedVar)	{
				isused = true;
				break;
			}
		}

		bool ismod = false;
		for(int z=0; z<proglinem.size(); z++) {
			if(proglinem.at(z) == modifiedVar) {
				ismod = true;
				break;
			}
		}

		if(isused && isPassingBy && currASTNode->getType() == ASTNode::Assign) 	{
			result.push_back(curr);
		}

		if(ismod && currASTNode->getType() != ASTNode::If && currASTNode->getType() != ASTNode::While) 
			continue;

		prevProgLine = curr;

		for(int j=(nextLine.size() - 1); j >= 0; j--)
			lines2visit.push(nextLine.at(j));
	}

	//CACHE
	for (int i = 0; i < result.size(); i++)	{
		PKB::affects.insertAffects(a1, result.at(i), true);
	}

	return result;
}

vector<STMT> PQLAffectsProcessor::getAffectsFrom(STMT a2)
{
	vector<STMT> prevs = PKB::next.getPrevious(a2);
	
	stack<STMT> lines2visit;

	for(int i=0; i<prevs.size(); i++) 
		lines2visit.push(prevs.at(i));
	
	vector<STMT> result;
	set<STMT> visited;

	stack<pair<pair<bool,bool>,int>> ifLHS;//0 == left branch

	PROG_LINE prevProgLine=0;

	vector<VAR> varl =  PKB::uses.getUsedByStmt(a2);

	while(lines2visit.size() > 0) {
		int curr = lines2visit.top();
		lines2visit.pop();
		
		
		// visited?
		if (visited.find(curr) != visited.end()) 
			continue;

		visited.insert(curr);
		ASTNode* currASTNode = PKB::stmtRefMap.at(curr).getASTStmtNode();
		
		vector<STMT> nextLine = PKB::next.getNext(curr);
		vector<STMT> prevLine = PKB::next.getPrevious(curr);
	
		bool isPassingBy =true;
		if(nextLine.size() == 2 || prevLine.size() == 2) {
			
			if(currASTNode->getType() == ASTNode::While){
					isPassingBy = false;
					//if start
			} else {
				isPassingBy = false;
			}
		}

		vector<VAR> proglinem = PKB::modifies.getModifiedByStmt(curr);//one only
		vector<VAR> varu = PKB::uses.getUsedByStmt(curr);//multi
	
		bool isused = false;
		for(int z=0; z<varu.size(); z++) {
			for(int y=0;y<varl.size();y++) {
				if(varu.at(z) == varl.at(y)) {
					isused = true;
					goto cont1;
				}
			}
		}
cont1:
		bool ismod = false;
		for(int z=0; z<proglinem.size(); z++)	{
			for(int y=0; y<varl.size(); y++) 	{
				if(proglinem.at(z) == varl.at(y)) {
					ismod = true;
					goto cont2;
				}
			}
		}
			
cont2:
		if(ismod && isPassingBy && currASTNode->getType() == ASTNode::Assign) {
			result.push_back(curr);
			continue;
		}
	
		prevProgLine = curr;

		for(int j=(prevLine.size() - 1);j >= 0;j--) {
			lines2visit.push(prevLine.at(j));
		}
		
	}
	
	//CACHE
	for (int i = 0; i < result.size(); i++)	{
		PKB::affects.insertAffects(result.at(i), a2 , true);
	}

	return result;
}

//Affects*
bool PQLAffectsProcessor::isSatifyAffectsStar(STMT a1, STMT a2)
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

	if (!(PKB::next.isNextStar(a1, a2))) { //if nextStar(a1, a2) 
		PKB::affects.insertAffects(a1, a2, false);
		return false;
	}

	return true;
}

bool PQLAffectsProcessor::isAffectsStar(STMT a1, STMT a2) {
	
	if (!isSatifyAffects(a1, a2)) 
		return false;

	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 

	stack<STMT> lines2visit;
	set<STMT> visited;

	vector<STMT> nexts = PKB::next.getNext(a1);

	for (int i = 0; i < nexts.size(); i++) 
		lines2visit.push(nexts.at(i));
	

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
		vector<STMT> temp = PKB::next.getNext(curr);

		for (int k = 0; k < temp.size(); k++) {
			lines2visit.push(temp.at(k));
		}
	}
	return false;
}


//AffectsBip.
bool PQLAffectsProcessor::isAffectsBip(STMT a1, STMT a2) {
	if (a1 <= 0 || a2 <= 0) 
		return false;
	
	return false; 
		
}
