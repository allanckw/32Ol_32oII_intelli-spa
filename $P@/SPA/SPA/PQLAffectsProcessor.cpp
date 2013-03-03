#include "PQLAffectsProcessor.h"
#include "Helper.h"

//Affects
/**
* This method will be used to check whether a1 and a2 satisfy Affect Condition
* @Param a1	The statement that is going to affect a2
* @Param a2	The statement that is affected by a1
* @Return whether Affect(a1,a2) satisfy the condition of affect
*/
bool PQLAffectsProcessor::isSatifyAffects(STMT a1, STMT a2)
{

	if (a1 <= 0 || a1 > PKB::maxProgLines || a2 <= 0 || a2 > PKB::maxProgLines)
		return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	ASTStmtNode* a2ASTNode = PKB::stmtRefMap.at(a2).getASTStmtNode();
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 
	CFGNode* a2CFGNode = PKB::stmtRefMap.at(a2).getCFGNode(); 

	if (a1CFGNode->proc!= a2CFGNode->proc) { //Check if same procedure
		return false; 
	}
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign) { // return false if either stmt is not assign
		return false; 
	}
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified

	if (!(PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar))) { //if nextStar(a1, a2) and uses(a2, v) does not hold 
		return false;
	}

	return true;
}

/**
* This method will be used to check Affect(a1,a2)
* @Param a1	The statement that is going to affect a2
* @Param a2	The statement that is affected by a1
* @Return whether Affect(a1,a2) holds
*/
bool PQLAffectsProcessor::isAffects(STMT a1, STMT a2) {

	if (!PQLAffectsProcessor::isSatifyAffects(a1, a2)) 
		return false;

	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 

	stack<STMT> lines2visit;
	set<PROG_LINE> visited;
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

/**
* This method will be used to get a list of statement that is affected by a1
* @Param a1	The statement that is going to affect a2
* @Return a list of statement that is affected by a1
*/
vector<STMT> PQLAffectsProcessor::getAffectsFrom(STMT a2)
{
	if ((PKB::stmtRefMap.at(a2).getASTStmtNode()->getType() != ASTNode::Assign) ||
		(a2 < 0 || a2 > PKB::maxProgLines) || (PKB::assignTable.count(a2) == 0))
		return vector<STMT>();

	const vector<VAR>& usesVarVector = PKB::uses.getUsedByStmt(a2);
	unordered_set<VAR> usesVarSet(usesVarVector.begin(), usesVarVector.end());
	CFGNode* s2 = PKB::stmtRefMap.at(a2).getCFGNode();

	unordered_set<STMT> answer;

	bool toStep = false;
	for (auto it = s2->modifySet.begin(); it != s2->modifySet.end(); it++)
		if (usesVarSet.count(*it) > 0) {
			toStep = true;
			break;
		}
	if (toStep)
		for (int i = a2 - 1; i >= s2->first; i--)
			if (PKB::assignTable.count(i) > 0) {
				const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (usesVarSet.count(stmtModifiesVar) > 0) {
					answer.insert(i);
					usesVarSet.erase(stmtModifiesVar);
					if (usesVarSet.size() == 0)
						return vector<STMT>(answer.begin(), answer.end());
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (usesVarSet.count(*it) > 0) {
						usesVarSet.erase(*it);
						if (usesVarSet.size() == 0)
							return vector<STMT>(answer.begin(), answer.end());
					}
			}
	
	queue<pair<CFGNode*, unordered_set<int>>> search;
	list<pair<CFGNode*, unordered_set<int>>> ifQueue;
	unordered_set<CFGNode*> seen;
	for (auto it = s2->parents.begin(); it != s2->parents.end(); it++)
		search.push(pair<CFGNode*, unordered_set<int>>(*it, usesVarSet));

	while (!(search.empty() && ifQueue.empty())) {
		pair<CFGNode*, unordered_set<int>> currPair;
		CFGNode* currCFG;
		unordered_set<int> usesVar;
		if (search.empty()) {
			currPair = ifQueue.front();
			ifQueue.erase(ifQueue.begin());
			currCFG = currPair.first;
			usesVar = currPair.second;
		} else {
			currPair = search.front();
			search.pop();
			currCFG = currPair.first;
			if (currCFG->type == CFGNode::IfNode) {
				bool match = false;
				for (auto it = ifQueue.begin(); it != ifQueue.end(); it++)
					if (currCFG = it->first) {
						match = true;
						usesVar = currPair.second;
						usesVar.insert(it->second.begin(), it->second.end());
						break;
					}
				if (!match) {
					ifQueue.push_back(currPair);
					continue;
				}
			} else
				usesVar = currPair.second;
		}
		if (seen.count(currCFG) > 0)
			continue;
		seen.insert(currCFG);
		
		bool toStep = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (usesVar.count(*it) > 0) {
				toStep = true;
				break;
			}
		if (toStep) { //some statement will modify some of the used variables
			for (int i = currCFG->last; i >= currCFG->first; i--)
				if (PKB::assignTable.count(i) > 0) {
					const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (usesVar.count(stmtModifiesVar) > 0) {
						answer.insert(i);
						usesVar.erase(stmtModifiesVar);
						if (usesVar.size() == 0)
							break;
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++) {
						if (usesVar.count(*it) > 0) {
							usesVar.erase(*it);
							if (usesVar.size() == 0)
								break;
						}
					}
					if (usesVar.size() == 0)
						break;
				}
		}

		if (usesVar.size() != 0)
			for (auto it = currCFG->parents.begin(); it != currCFG->parents.end(); it++)
				search.push(pair<CFGNode*, unordered_set<int>>(*it, usesVar));
	}
	return vector<STMT>(answer.begin(), answer.end());

}

/**
* This method will be used to get a list of statement that is going to affect a2
* @Param a2	The statement that is affected by a1
* @Return a list of statement that is going to affect a2
*/
vector<STMT> PQLAffectsProcessor::getAffectsBy(STMT a1)
{
	if ((a1 < 0 || a1 > PKB::maxProgLines) || (PKB::assignTable.count(a1) == 0))
		return vector<STMT>(); //TODO: double check with cristina

	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(a1)[0];
	CFGNode* s1 = PKB::stmtRefMap.at(a1).getCFGNode();
	unordered_set<STMT> answer;

	for (int i = a1 + 1; i <= s1->last; i++)
		if (PKB::assignTable.count(i) > 0) {
			const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
			for (auto it = stmtUsesVar.begin(); it != stmtUsesVar.end(); it++)
				if (modifiesVar == *it) {
					answer.insert(i);
					break;
				}
			if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
				return vector<STMT>(answer.begin(), answer.end());
		} else if (PKB::callTable.count(i) > 0) {
			const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
			for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
				if (modifiesVar == *it)
					return vector<STMT>(answer.begin(), answer.end());
		}
	
	queue<CFGNode*> search;
	unordered_set<CFGNode*> seen;
	
	switch (s1->type) {
	case CFGNode::StandardNode:
		if (s1->children.oneChild != NULL)
			search.push(s1->children.oneChild);
		break;

	case CFGNode::WhileNode:
		search.push(s1->children.whileChildren.whileIn);
		search.push(s1->children.whileChildren.whileOut);
		break;

	case CFGNode::IfNode:
		search.push(s1->children.ifChildren.ifThen);
		search.push(s1->children.ifChildren.ifElse);
		break;
	}

	while (!search.empty()) {
		s1 = search.front();
		search.pop();
		if (seen.count(s1) > 0)
			continue;
		seen.insert(s1);

		bool continueCFG;

		if (s1->modifySet.count(modifiesVar) == 0) {
			if (s1->useSet.count(modifiesVar) > 0)
				for (int i = s1->first; i <= s1->last; i++)
					if (PKB::assignTable.count(i) > 0) {
						const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
						for (auto it = stmtUsesVar.begin(); it != stmtUsesVar.end(); it++)
							if (modifiesVar == *it) {
								answer.insert(i);
								break;
							}
					
					}
			continueCFG = true;
		} else { //some statement will modify the variable
			if (s1->useSet.count(modifiesVar) > 0)
				for (int i = s1->first; i <= s1->last; i++)
					if (PKB::assignTable.count(i) > 0) {
						const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
						for (auto it = stmtUsesVar.begin(); it != stmtUsesVar.end(); it++)
							if (modifiesVar == *it) {
								answer.insert(i);
								break;
							}
						if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
							break;
					} else if (PKB::callTable.count(i) > 0) {
						bool ok = true;
						const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
						for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
							if (modifiesVar == *it) {
								ok = false;
								break;
							}
						if (!ok)
							break;
					}
			continueCFG = false;
		}

		if (continueCFG)
			switch (s1->type) {
			case CFGNode::StandardNode:
			case CFGNode::DummyNode:
				if (s1->children.oneChild != NULL)
					search.push(s1->children.oneChild);
				break;

			case CFGNode::WhileNode:
				search.push(s1->children.whileChildren.whileIn);
				search.push(s1->children.whileChildren.whileOut);
				break;

			case CFGNode::IfNode:
				search.push(s1->children.ifChildren.ifThen);
				search.push(s1->children.ifChildren.ifElse);
				break;
			}
	}
	return vector<STMT>(answer.begin(), answer.end());


}

//Affects*
/**
* This method will be used to check whether a1 and a2 satisfy AffectStar Condition
* @Param a1	The statement that is going to affect a2
* @Param a2	The statement that is affected by a1
* @Return whether Affect(a1,a2) satisfy the condition of affectStar
*/
bool PQLAffectsProcessor::isSatifyAffectsStar(STMT a1, STMT a2)
{
	if (a1 <= 0 || a1 > PKB::maxProgLines || a2 <= 0 || a2 > PKB::maxProgLines)
		return false;
	
	//Get Corresponding ASTNodes / CFGNodes from table..
	ASTStmtNode* a1ASTNode = PKB::stmtRefMap.at(a1).getASTStmtNode();
	ASTStmtNode* a2ASTNode = PKB::stmtRefMap.at(a2).getASTStmtNode();
	CFGNode* a1CFGNode = PKB::stmtRefMap.at(a1).getCFGNode(); 
	CFGNode* a2CFGNode = PKB::stmtRefMap.at(a2).getCFGNode(); 

	if (a1CFGNode->proc != a2CFGNode->proc) { //Check if same procedure
		return false; 
	}
	if (a1ASTNode->getType() != ASTNode::Assign || a2ASTNode->getType() != ASTNode::Assign) { // return false if either stmt is not assign
		return false; 
	}
	VAR modifiedVar = a1ASTNode->getValue(); // get the variable being modified

	if (!(PKB::next.isNextStar(a1, a2))) { //if nextStar(a1, a2) 
		return false;
	}

	return true;
}

/**
* This method will be used to check AffectStar(a1,a2)
* @Param a1	The statement that is going to affect a2
* @Param a2	The statement that is affected by a1
* @Return whether AffectStar(a1,a2) holds
*/
bool PQLAffectsProcessor::isAffectsStar(STMT a1, STMT a2)
{
	if (!PQLAffectsProcessor::isSatifyAffectsStar(a1, a2)) 
		return false;

	
	const vector<VAR>& modifiesVarVector = PKB::modifies.getModifiedByStmt(a1);
	unordered_set<VAR> modifiesVarSet(modifiesVarVector.begin(), modifiesVarVector.end());
	const CFGNode* s1 = PKB::stmtRefMap.at(a1).getCFGNode();
	const CFGNode* s2 = PKB::stmtRefMap.at(a2).getCFGNode();

	if (a2 <= s1->last && s1->whileAncestor == NULL) {
		if (a1 >= a2)
			return false;
		for (int i = a1 + 1; i < a2; i++)
			if (PKB::assignTable.count(i) > 0) {
				const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
				const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (modifiesVarSet.count(modifiesVar)) { //this statement kills value of some prev
					bool match = false;
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							if (modifiesVar == *it) { //overall effect -> do nothing
								match = true;
								break;
							}
						}
					if (!match) { //kill previous value
						modifiesVarSet.erase(modifiesVar);
						if (modifiesVarSet.size() == 0)
							return false;
					}
				} else { //this statement does not kill anything
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							modifiesVarSet.insert(modifiesVar); //overall effect -> add it in
							break;
						}
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0) {
						modifiesVarSet.erase(*it);
						if (modifiesVarSet.size() == 0)
							return false;
					}
			}
		const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(a2);
		for (auto it = usesVar.begin(); it != usesVar.end(); it++)
			if (modifiesVarSet.count(*it) > 0)
				return true;
		return false;
	} else {
		const IntervalList* list = s1->nextList;
		if (list == NULL)
			return false;
		if (a2 < list->first) {
			list = list->prev;
			while (list != NULL && a2 < list->first)
				list = list->prev;
			if (list == NULL || a2 > list->last)
				return false;
		} else if (a2 > list->last) {
			list = list->next;
			while (list != NULL && a2 > list->last)
				list = list->next;
			if (list == NULL || a2 < list->first)
				return false;
		}

		for (int i = a1 + 1; i <= s1->last; i++)
			if (PKB::assignTable.count(i) > 0) {
				const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
				const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (modifiesVarSet.count(modifiesVar)) { //this statement kills value of some prev
					bool match = false;
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							if (modifiesVar == *it) { //overall effect -> do nothing
								match = true;
								break;
							}
						}
					if (!match) //kill previous value
						modifiesVarSet.erase(modifiesVar);
				} else { //this statement does not kill anything
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
							modifiesVarSet.insert(modifiesVar); //overall effect -> add it in
							break;
						}
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0)
						modifiesVarSet.erase(*it);
			}
	}

	queue<pair<CFGNode*, unordered_set<int>>> search;
	unordered_map<CFGNode*, unordered_set<int>> seen;
	
	switch (s1->type) {
	case CFGNode::StandardNode:
	case CFGNode::DummyNode:
		if (s1->children.oneChild != NULL)
			search.push(pair<CFGNode*, unordered_set<int>>(s1->children.oneChild, modifiesVarSet));
		break;

	case CFGNode::WhileNode:
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.whileChildren.whileIn, modifiesVarSet));
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.whileChildren.whileOut, modifiesVarSet));
		break;

	case CFGNode::IfNode:
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.ifChildren.ifThen, modifiesVarSet));
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.ifChildren.ifElse, modifiesVarSet));
		break;
	}

	while (!search.empty()) {
		pair<CFGNode*, unordered_set<int>> currPair = search.front();
		search.pop();
		CFGNode* currCFG = currPair.first;
		unordered_set<int>& currVar = currPair.second;
		if (seen.count(currCFG) > 0) {
			unordered_set<int>& seenVar = seen[currCFG];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				currVar.erase(*it);
			if (currVar.size() == 0)
				continue;
			for (auto it = currVar.begin(); it != currVar.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<CFGNode*, unordered_set<int>>(currCFG, currVar));

		bool step = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (currVar.count(*it) > 0) {
				step = true;
				break;
			}
		if (!step)
			for (auto it = currCFG->useSet.begin(); it != currCFG->useSet.end(); it++)
				if (currVar.count(*it) > 0) {
					step = true;
					break;
				}

		if (step)
			for (int i = currCFG->first; i <= currCFG->last; i++)
				if (PKB::assignTable.count(i) > 0) {
					const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
					const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (currVar.count(modifiesVar)) { //this statement kills value of some prev
						bool match = false;
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								if (modifiesVar == *it) { //overall effect -> do nothing
									if (i == a2)
										return true;
									match = true;
									break;
								}
							}
						if (!match) //kill previous value
							currVar.erase(modifiesVar);
					} else { //this statement does not kill anything
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								if (i == a2)
									return true;
								currVar.insert(modifiesVar); //overall effect -> add it in
								break;
							}
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
						if (currVar.count(*it) > 0)
							currVar.erase(*it);
				}
		
		switch (currCFG->type) {
		case CFGNode::StandardNode:
			if (currCFG->children.oneChild != NULL)
				search.push(pair<CFGNode*, unordered_set<int>>(currCFG->children.oneChild, currVar));
			break;

		case CFGNode::WhileNode:
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.whileChildren.whileIn, currVar));
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.whileChildren.whileOut, currVar));
			break;

		case CFGNode::IfNode:
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.ifChildren.ifThen, currVar));
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.ifChildren.ifElse, currVar));
			break;
		}
	}
	return false; //shouldn't reach this point
}

/**
* This method will be used to get a list of a2 that is affectStar(a1,_)
* @Param a1	The statement that is going to affect a2
* @Return a list of statement that is affectStar by a1
*/
vector<STMT> PQLAffectsProcessor::getAffectsByStar(STMT a1)
{
	if (a1 < 0 || a1 > PKB::maxProgLines)
		return vector<STMT>();
	if (PKB::assignTable.count(a1) == 0)
		//throw SPAException("Both arguments to Affects must be assignments");
		return vector<STMT>(); //TODO: double check with cristina

	vector<VAR>& modifiesVarVector = PKB::modifies.getModifiedByStmt(a1);
	unordered_set<VAR> modifiesVarSet(modifiesVarVector.begin(), modifiesVarVector.end());
	const CFGNode* s1 = PKB::stmtRefMap.at(a1).getCFGNode();
	unordered_set<STMT> answer;

	for (int i = a1 + 1; i <= s1->last; i++)
		if (PKB::assignTable.count(i) > 0) {
			const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
			const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
			if (modifiesVarSet.count(modifiesVar)) { //this statement kills value of some prev
				bool match = false;
				for (auto it = usesVar.begin(); it != usesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
						answer.insert(i);
						if (modifiesVar == *it) { //overall effect -> do nothing
							match = true;
							break;
						}
					}
				if (!match) { //kill previous value
					modifiesVarSet.erase(modifiesVar);
					if (modifiesVarSet.size() == 0)
						return vector<STMT>(answer.begin(), answer.end());
				}
			} else { //this statement does not kill anything
				for (auto it = usesVar.begin(); it != usesVar.end(); it++)
					if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
						answer.insert(i); //overall effect -> add it in
						modifiesVarSet.insert(modifiesVar);
						break;
					}
			}
		} else if (PKB::callTable.count(i) > 0) {
			const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
			for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
				if (modifiesVarSet.count(*it) > 0) {
					modifiesVarSet.erase(*it);
					if (modifiesVarSet.size() == 0)
						return vector<STMT>(answer.begin(), answer.end());
				}
		}

	queue<pair<CFGNode*, unordered_set<int>>> search;
	unordered_map<CFGNode*, unordered_set<int>> seen;
	
	switch (s1->type) {
	case CFGNode::StandardNode:
		if (s1->children.oneChild != NULL)
			search.push(pair<CFGNode*, unordered_set<int>>(s1->children.oneChild, modifiesVarSet));
		break;

	case CFGNode::WhileNode:
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.whileChildren.whileIn, modifiesVarSet));
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.whileChildren.whileOut, modifiesVarSet));
		break;

	case CFGNode::IfNode:
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.ifChildren.ifThen, modifiesVarSet));
		search.push(pair<CFGNode*, unordered_set<int>>(
			s1->children.ifChildren.ifElse, modifiesVarSet));
		break;
	}

	while (!search.empty()) {
		pair<CFGNode*, unordered_set<int>> currPair = search.front();
		search.pop();
		CFGNode* currCFG = currPair.first;
		unordered_set<int>& currVar = currPair.second;
		if (seen.count(currCFG) > 0) {
			unordered_set<int>& seenVar = seen[currCFG];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				currVar.erase(*it);
			if (currVar.size() == 0)
				continue;
			for (auto it = currVar.begin(); it != currVar.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<CFGNode*, unordered_set<int>>(currCFG, currVar));

		bool step = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (currVar.count(*it) > 0) {
				step = true;
				break;
			}
		if (!step)
			for (auto it = currCFG->useSet.begin(); it != currCFG->useSet.end(); it++)
				if (currVar.count(*it) > 0) {
					step = true;
					break;
				}

		if (step)
			for (int i = currCFG->first; i <= currCFG->last; i++)
				if (PKB::assignTable.count(i) > 0) {
					const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
					const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (currVar.count(modifiesVar)) { //this statement kills value of some prev
						bool match = false;
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								answer.insert(i);
								if (modifiesVar == *it) { //overall effect -> do nothing
									match = true;
									break;
								}
							}
						if (!match) { //kill previous value
							currVar.erase(modifiesVar);
							if (currVar.size() == 0)
								break;
						}
					} else { //this statement does not kill anything
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (currVar.count(*it) > 0) { //this statement uses a variable
								answer.insert(i);
								currVar.insert(modifiesVar); //overall effect -> add it in
								break;
							}
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
						if (currVar.count(*it) > 0) {
							currVar.erase(*it);
							if (currVar.size() == 0)
								break;
						}
				}
		
		switch (currCFG->type) {
		case CFGNode::StandardNode:
		case CFGNode::DummyNode:
			if (currCFG->children.oneChild != NULL)
				search.push(pair<CFGNode*, unordered_set<int>>(currCFG->children.oneChild, currVar));
			break;

		case CFGNode::WhileNode:
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.whileChildren.whileIn, currVar));
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.whileChildren.whileOut, currVar));
			break;

		case CFGNode::IfNode:
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.ifChildren.ifThen, currVar));
			search.push(pair<CFGNode*, unordered_set<int>>(
				currCFG->children.ifChildren.ifElse, currVar));
			break;
		}
	}
	return vector<STMT>(answer.begin(), answer.end());
}

/**
* This method will be used to get a list of a2 that is affectStar(_,a2)
* @Param a2	The statement that is going to affect by a1
* @Return a list of statement that is affectStar a2
*/
vector<STMT>  PQLAffectsProcessor::getAffectsFromStar(STMT a2)
{
	if (a2 < 0 || a2 > PKB::maxProgLines)
		return vector<STMT>();
	if (PKB::assignTable.count(a2) == 0)
		return vector<STMT>(); //TODO: double check with cristina

	const vector<VAR>& usesVarVector = PKB::uses.getUsedByStmt(a2);
	unordered_set<VAR> usesVarSet(usesVarVector.begin(), usesVarVector.end());
	const CFGNode* s2 = PKB::stmtRefMap.at(a2).getCFGNode();
	unordered_set<STMT> answer;

	bool toStep = false;
		for (auto it = s2->modifySet.begin(); it != s2->modifySet.end(); it++)
			if (usesVarSet.count(*it) > 0) {
				toStep = true;
				break;
			}
	if (toStep)
		for (int i = a2 - 1; i >= s2->first; i--)
			if (PKB::assignTable.count(i) > 0) {
				const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
				if (usesVarSet.count(stmtModifiesVar) > 0) {
					answer.insert(i);
					usesVarSet.erase(stmtModifiesVar);
					const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
					usesVarSet.insert(stmtUsesVar.begin(), stmtUsesVar.end());
					if (usesVarSet.size() == 0)
						return vector<STMT>(answer.begin(), answer.end());
				}
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (usesVarSet.count(*it) > 0) {
						usesVarSet.erase(*it);
						if (usesVarSet.size() == 0)
							return vector<STMT>(answer.begin(), answer.end());
					}
			}

	queue<pair<CFGNode*, unordered_set<int>>> search;
	unordered_map<CFGNode*, unordered_set<int>> seen;
	for (auto it = s2->parents.begin(); it != s2->parents.end(); it++)
		search.push(pair<CFGNode*, unordered_set<int>>(*it, usesVarSet));

	while (!search.empty()) {
		pair<CFGNode*, unordered_set<int>> currPair = search.front();
		search.pop();
		CFGNode* currCFG = currPair.first;
		unordered_set<int>& currVar = currPair.second;
		if (seen.count(currCFG) > 0) {
			unordered_set<int>& seenVar = seen[currCFG];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				currVar.erase(*it);
			if (currVar.size() == 0)
				continue;
			for (auto it = currVar.begin(); it != currVar.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<CFGNode*, unordered_set<int>>(currCFG, currVar));

		bool toStep = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
			if (currVar.count(*it) > 0) {
				toStep = true;
				break;
			}
		if (toStep)
			for (int i = currCFG->last; i >= currCFG->first; i--)
				if (PKB::assignTable.count(i) > 0) {
					const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (currVar.count(stmtModifiesVar) > 0) {
						answer.insert(i);
						currVar.erase(stmtModifiesVar);
						const vector<VAR>& stmtcurrVar = PKB::uses.getUsedByStmt(i);
						currVar.insert(stmtcurrVar.begin(), stmtcurrVar.end());
						if (currVar.size() == 0)
							break;
					}
				} else if (PKB::callTable.count(i) > 0) {
					const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
					for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++) {
						if (currVar.count(*it) > 0) {
							currVar.erase(*it);
							if (currVar.size() == 0)
								break;
						}
						if (currVar.size() == 0)
							break;
					}
				}
		
		if (currVar.size() != 0)
			for (auto it = currCFG->parents.begin(); it != currCFG->parents.end(); it++)
				search.push(pair<CFGNode*, unordered_set<int>>(*it, currVar));
	}
	return vector<STMT>(answer.begin(), answer.end());
}


//bool PQLAffectsProcessor::isAffectsBip(STMT a1, STMT a2)
//{
//	if (a1 < 0 || a1 > PKB::maxProgLines || a2 < 0 || a2 > PKB::maxProgLines)
//		return false;
//	if (PKB::assignTable.count(a1) == 0 || PKB::assignTable.count(a2) == 0)
//		//throw SPAException("Both arguments to Affects must be assignments");
//		return false; //TODO: double check with cristina
//
//	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(a1)[0];
//	{
//		bool ok = false;
//		const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(a2);
//		for (auto it = usesVar.begin(); it != usesVar.end(); it++)
//			if (modifiesVar == *it) {
//				ok = true;
//				break;
//			}
//		if (!ok)
//			return false;
//	}
//
//	CFGNode* s1 = PKB::stmtRefMap.at(a1).getCFGNode();
//
//	if (a2 <= s1->last) {
//		for (int i = a1 + 1; i < a2; i++)
//			if (PKB::assignTable.count(i) > 0) {
//				if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
//					return false;
//			} else if (PKB::callTable.count(i) > 0) {
//				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
//				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
//					if (modifiesVar == *it)
//						return false;
//			}
//		return true;
//	}
//
//	const CFGNode* s2 = PKB::stmtRefMap.at(a2).getCFGNode();
//	if (s1->proc != s2->proc)
//		return false;
//
//	const IntervalList* list = s1->nextList;
//	if (list == NULL)
//		return false;
//	if (a2 < list->first) {
//		list = list->prev;
//		while (list != NULL && a2 < list->first)
//			list = list->prev;
//		if (list == NULL || a2 > list->last)
//			return false;
//	} else if (a2 > list->last) {
//		list = list->next;
//		while (list != NULL && a2 > list->last)
//			list = list->next;
//		if (list == NULL || a2 < list->first)
//			return false;
//	}
//
//	for (int i = a1 + 1; i <= s1->last; i++)
//		if (PKB::assignTable.count(i) > 0) {
//			if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
//				return false;
//		} else if (PKB::callTable.count(i) > 0) {
//			const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
//			for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
//				if (modifiesVar == *it)
//					return false;
//		}
//
//	queue<CFGNode*> search;
//	unordered_set<CFGNode*> seen;
//
//	switch (s1->type) {
//	case CFGNode::StandardNode:
//		if (s1->children.oneChild != NULL)
//			search.push(s1->children.oneChild);
//		break;
//
//	case CFGNode::WhileNode:
//		if (s1->children.whileChildren.whileIn->first <= a2
//			&& a2 < s1->children.whileChildren.whileOut->first)
//			search.push(s1->children.whileChildren.whileIn);
//		else
//			search.push(s1->children.whileChildren.whileOut);
//		break;
//
//	case CFGNode::IfNode:
//		if (s1->children.ifChildren.ifThen->first <= a2
//			&& a2 < s1->children.ifChildren.ifElse->first)
//			search.push(s1->children.ifChildren.ifThen);
//		else if (s1->children.ifChildren.ifElse->first <= a2
//			&& a2 < s1->children.ifChildren.ifLater->first)
//			search.push(s1->children.ifChildren.ifElse);
//		else {
//			search.push(s1->children.ifChildren.ifThen);
//			search.push(s1->children.ifChildren.ifElse);
//		}
//		break;
//	}
//
//	while (!search.empty()) {
//		CFGNode* currCFG = search.front();
//		search.pop();
//		if (seen.count(currCFG) > 0)
//			continue;
//		seen.insert(currCFG);
//
//		if (currCFG->first != 0 && currCFG->modifySet.count(modifiesVar) > 0) {
//			if (a2 <= currCFG->last) {
//				for (int i = currCFG->first; i < a2; i++) {
//					if (PKB::assignTable.count(i) > 0) {
//						if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
//							return false;
//					} else if (PKB::callTable.count(i) > 0) { //interprocedural will change here
//						const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
//						for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
//							if (modifiesVar == *it)
//								return false;
//					}
//				}
//				return true;
//			}
//			break;
//		}
//		
//		switch (currCFG->type) {
//		case CFGNode::StandardNode:
//			if (currCFG->children.oneChild != NULL)
//				search.push(currCFG->children.oneChild);
//			break;
//
//		case CFGNode::WhileNode:
//			if (currCFG->children.whileChildren.whileIn->first <= a2
//				&& a2 < currCFG->children.whileChildren.whileOut->first)
//				search.push(currCFG->children.whileChildren.whileIn);
//			else
//				search.push(currCFG->children.whileChildren.whileOut);
//			break;
//
//		case CFGNode::IfNode:
//			if (currCFG->children.ifChildren.ifThen->first <= a2
//				&& a2 < currCFG->children.ifChildren.ifElse->first)
//				search.push(currCFG->children.ifChildren.ifThen);
//			else if (currCFG->children.ifChildren.ifElse->first <= a2
//				&& a2 < currCFG->children.ifChildren.ifLater->first)
//				search.push(currCFG->children.ifChildren.ifElse);
//			else {
//				search.push(currCFG->children.ifChildren.ifThen);
//				search.push(currCFG->children.ifChildren.ifElse);
//			}
//			break;
//		}
//	}
//	return false; //shouldn't reach this point
//}


/*
-----------------------------------------------------------------------------
OLD CODE: OBFUSCATED 
-----------------------------------------------------------------------------
*/
//
//
//vector<STMT>* Ans; 
//stack<STMT>* Stak;
//
//vector<STMT> PQLAffectsProcessor::AffectsBy2(STMT testaffectfirst)
//{
//
//	
//	vector<PROG_LINE> t1 = PKB::next.getNext(testaffectfirst);
//	vector<PROG_LINE> t2 = PKB::next.getPrevious(2);
//
//	stack<PROG_LINE> progStack; 
//	for(int i=0;i<t1.size();i++)
//	{
//		progStack.push(t1.at(i));
//	}
//	
//	vector<PROG_LINE> test;
//	vector<PROG_LINE> visited;
//
//	int prevProgLine=0;
//
//	vector<VAR> varl =  PKB::modifies.getModifiedByStmt(testaffectfirst);
//
//	int varindex = varl.at(0);
//
//	while(progStack.size() > 0)
//	{
//
//		
//		int currentLine = progStack.top();
//		progStack.pop();
//
//		if(Helper::contains(visited, currentLine))
//		{
//			//node visisted before
//			continue;
//		}
//
//		vector<PROG_LINE> ne = PKB::next.getNext(currentLine);
//		vector<PROG_LINE> pr = PKB::next.getPrevious(currentLine);
//		
//		bool isassing =true;
//		CFGNode* node = PKB::stmtRefMap.at(currentLine).getCFGNode(); 
//		
//		if(node->getType() != CFGNode::StandardNode)
//			isassing  = false;
//
//
//		if(ne.size() == 2 || pr.size() == 2)
//		{
//			
//			
//			
//
//			if(node->getType() == CFGNode::IfNode)
//			{
//				isassing = false;
//				//if start
//				//iflhs.push(pair<pair<bool,bool>,int>(pair<bool,bool>(false,false),0));
//			}
//			else if(node->getType() == CFGNode::WhileNode)
//			{
//				isassing = false;
//			}
//
//
//			
//		}
//
//
//		vector<VAR> proglinem = PKB::modifies.getModifiedByStmt(currentLine);
//		vector<VAR> varu = PKB::uses.getUsedByStmt(currentLine);
//
//
//		bool isused = false;
//		for(int z=0;z<varu.size();z++)
//		{
//			if(varu.at(z) == varindex)
//			{
//				isused = true;
//				break;
//			}
//		}
//
//		bool ismod = false;
//		for(int z=0;z<proglinem.size();z++)
//		{
//			if(proglinem.at(z) == varindex)
//			{
//				ismod = true;
//				break;
//			}
//		}
//		int lolt = currentLine;
//		if(isused && isassing && PKB::stmtRefMap.at(currentLine).getASTStmtNode()->getType() != ASTNode::Call)
//		{
//			test.push_back(currentLine);
//
//			if(find(Ans->begin(),Ans->end(),currentLine) == Ans->end())
//			{
//				Ans->push_back(currentLine); 
//				Stak->push(currentLine);
//			}
//			
//		}
//
//		CFGNode::NodeType ll = PKB::stmtRefMap.at(currentLine).getCFGNode()->getType();
//		if(ismod && PKB::stmtRefMap.at(currentLine).getCFGNode()->getType() != CFGNode::IfNode && PKB::stmtRefMap.at(currentLine).getCFGNode()->getType() != CFGNode::WhileNode)
//		{
//			
//			continue;
//		}
//		
//
//		visited.push_back(currentLine);
//		prevProgLine = currentLine;
//		
//		for(int j=(ne.size() - 1);j >= 0;j--)
//		{
//			progStack.push(ne.at(j));
//		}
//	}
//	return test;
//}
//
//vector<STMT> PQLAffectsProcessor::getAffectsFrom(STMT testaffectfirst)
//{
//	if(PKB::stmtRefMap.at(testaffectfirst).getASTStmtNode()->getType() != ASTNode::Assign)
//		return vector<STMT>();
//
//	
//	
//	
//	vector<PROG_LINE> alltest;
//	
//
//	vector<VAR> varl =  PKB::uses.getUsedByStmt(testaffectfirst);
//	for(int h=0;h<varl.size();h++)
//	{
//		VAR v = varl.at(h);
//		vector<PROG_LINE> t1 = PKB::next.getPrevious(testaffectfirst);
//	
//		stack<PROG_LINE> progStack; 
//		for(int i=0;i<t1.size();i++)
//		{
//			progStack.push(t1.at(i));
//		}
//		int prevProgLine=0;
//		vector<PROG_LINE> test;
//		vector<PROG_LINE> visited;
//		while(progStack.size() > 0)
//		{
//
//		
//			int currentLine = progStack.top();
//			progStack.pop();
//
//			if(Helper::contains(visited, currentLine))
//			{
//				//node visisted before
//				continue;
//			}
//
//			vector<PROG_LINE> ne = PKB::next.getNext(currentLine);
//			vector<PROG_LINE> pr = PKB::next.getPrevious(currentLine);
//
//	
//			bool isassing =true;
//
//
//			CFGNode* node = PKB::stmtRefMap.at(currentLine).getCFGNode(); 
//			
//			if(node->getType() != CFGNode::StandardNode)
//				isassing  = false;
//
//		
//
//
//			vector<VAR> proglinem = PKB::modifies.getModifiedByStmt(currentLine);//one only
//			vector<VAR> varu = PKB::uses.getUsedByStmt(currentLine);//multi
//
//			bool ismod = false;
//			//int modint=0; 
//			for(int z=0;z<proglinem.size();z++)
//			{
//				
//					if(proglinem.at(z) == v)
//					{
//						ismod = true;
//						//modint = a2;
//						goto cont2;
//					}
//				
//			}
//cont2:
//			
//			if(ismod && isassing &&  PKB::stmtRefMap.at(currentLine).getASTStmtNode()->getType() != ASTNode::Call) {
//				test.push_back(currentLine);
//				visited.push_back(currentLine);
//				continue;
//
//			}else if(ismod && PKB::stmtRefMap.at(currentLine).getASTStmtNode()->getType() == ASTNode::Call){
//					continue;
//			}
//
//			CFGNode::NodeType ll = PKB::stmtRefMap.at(currentLine).getCFGNode()->getType();
//		
//			visited.push_back(currentLine);
//			prevProgLine = currentLine;
//
//			for(int j=(pr.size() - 1);j >= 0;j--) {
//				progStack.push(pr.at(j));
//			}
//		}
//
//		for(int g=0;g<test.size();g++) {
//			alltest.push_back(test.at(g));
//		}
//	}
//	
//
//
//	return alltest;
//}
//

//
//bool PQLAffectsProcessor::isAffectsStar(STMT a1, STMT a2) {
//	
//
//}
//
//vector<STMT> PQLAffectsProcessor::AffectsStarBy(STMT a1)
//{
//	if (a1 < 0 || a1 > PKB::maxProgLines)
//		return vector<STMT>();
//	if (PKB::assignTable.count(a1) == 0)
//		return vector<STMT>(); //TODO: double check with cristina
//
//	const vector<VAR>& usesVarVector = PKB::uses.getUsedByStmt(a1);
//	unordered_set<VAR> usesVarSet(usesVarVector.begin(), usesVarVector.end());
//	CFGNode* s2 = PKB::stmtRefMap.at(a1).getCFGNode();
//	unordered_set<int> answer;
//
//	bool toStep = false;
//		
//	for (auto it = s2->modifySet.begin(); it != s2->modifySet.end(); it++){
//		if (usesVarSet.count(*it) > 0) {
//				toStep = true;
//				break;
//			}
//	}
//	if (toStep)
//		for (int i = a1 - 1; i >= s2->getStartLine(); i--)
//			if (PKB::assignTable.count(i) > 0) {
//				const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
//				if (usesVarSet.count(stmtModifiesVar) > 0) {
//					answer.insert(i);
//					usesVarSet.erase(stmtModifiesVar);
//					const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
//					usesVarSet.insert(stmtUsesVar.begin(), stmtUsesVar.end());
//					if (usesVarSet.size() == 0)
//						return vector<STMT>(answer.begin(), answer.end());
//				}
//			} else if (PKB::callTable.count(i) > 0) {
//				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
//				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
//					if (usesVarSet.count(*it) > 0) {
//						usesVarSet.erase(*it);
//						if (usesVarSet.size() == 0)
//							return vector<STMT>(answer.begin(), answer.end());
//					}
//			}
//
//	//queue<pair<CFGNode*, unordered_set<int>>> search;
//	//unordered_map<CFGNode*, unordered_set<int>> seen;
//	//for (auto it = s2->parents.begin(); it != s2->parents.end(); it++)
//	//	search.push(pair<CFGNode*, unordered_set<int>>(*it, usesVarSet));
//
//	//while (!search.empty()) {
//	//	pair<CFGNode*, unordered_set<int>> currPair = search.front();
//	//	search.pop();
//	//	CFGNode* currCFG = currPair.first;
//	//	unordered_set<int>& currVar = currPair.second;
//	//	if (seen.count(currCFG) > 0) {
//	//		unordered_set<int>& seenVar = seen[currCFG];
//	//		for (auto it = seenVar.begin(); it != seenVar.end(); it++)
//	//			currVar.erase(*it);
//	//		if (currVar.size() == 0)
//	//			continue;
//	//		for (auto it = currVar.begin(); it != currVar.end(); it++)
//	//			seenVar.insert(*it);
//	//	} else
//	//		seen.insert(pair<CFGNode*, unordered_set<int>>(currCFG, currVar));
//
//	//	bool toStep = false;
//	//	for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++)
//	//		if (currVar.count(*it) > 0) {
//	//			toStep = true;
//	//			break;
//	//		}
//	//	if (toStep)
//	//		for (int i = currCFG->last; i >= currCFG->first; i--)
//	//			if (PKB::assignTable.count(i) > 0) {
//	//				const VAR stmtModifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
//	//				if (currVar.count(stmtModifiesVar) > 0) {
//	//					answer.insert(i);
//	//					currVar.erase(stmtModifiesVar);
//	//					const vector<VAR>& stmtcurrVar = PKB::uses.getUsedByStmt(i);
//	//					currVar.insert(stmtcurrVar.begin(), stmtcurrVar.end());
//	//					if (currVar.size() == 0)
//	//						break;
//	//				}
//	//			} else if (PKB::callTable.count(i) > 0) {
//	//				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
//	//				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++) {
//	//					if (currVar.count(*it) > 0) {
//	//						currVar.erase(*it);
//	//						if (currVar.size() == 0)
//	//							break;
//	//					}
//	//					if (currVar.size() == 0)
//	//						break;
//	//				}
//	//			}
//	//	
//	//	if (currVar.size() != 0)
//	//		for (auto it = currCFG->parents.begin(); it != currCFG->parents.end(); it++)
//	//			search.push(pair<CFGNode*, unordered_set<int>>(*it, currVar));
//	//}
//	//return vector<STMT>(answer.begin(), answer.end());
//}
//
//
////AffectsBip.
//bool PQLAffectsProcessor::isAffectsBip(STMT a1, STMT a2) {
//	if (a1 <= 0 || a2 <= 0) 
//		return false;
//	
//	return false; 
//		
//}
