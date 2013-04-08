#include "PQLAffectsProcessor.h"
#include "Helper.h"

//Affects
/**
* This method will be used to check whether a1 and a2 satisfy Affect Condition
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether Affect(a1,a2) satisfy the condition of affect
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

	/*if (!(PKB::next.isNextStar(a1, a2) && PKB::uses.isUsedStmt(a2, modifiedVar))) { //if nextStar(a1, a2) and uses(a2, v) does not hold 
		return false;
	}*/

	return true;
}

/**
* This method will be used to check Affect(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether Affect(a1,a2) holds
*/
bool PQLAffectsProcessor::isAffects(STMT a1, STMT a2) {

	if (a1 < 0 || a1 > PKB::maxProgLines || a2 < 0 || a2 > PKB::maxProgLines)
		return false;
	if (PKB::assignTable.count(a1) == 0 || PKB::assignTable.count(a2) == 0)
		return false;

	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(a1)[0];

	{
		bool ok = false;
		const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(a2);
		for (auto it = usesVar.begin(); it != usesVar.end(); it++)
			if (modifiesVar == *it) {
				ok = true;
				break;
			}
		if (!ok)
			return false;
	}

	CFGNode* s1 = PKB::stmtRefMap.at(a1).getCFGNode();

	if (a1 < a2 && a2 <= s1->last) {
		for (int i = a1 + 1; i < a2; i++)
			if (PKB::assignTable.count(i) > 0) {
				if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
					return false;
			} else if (PKB::callTable.count(i) > 0) {
				const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
				for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
					if (modifiesVar == *it)
						return false;
			}
		return true;
	}

	const CFGNode* s2 = PKB::stmtRefMap.at(a2).getCFGNode();
	if (s1->proc != s2->proc)
		return false;

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
			if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
				return false;
		} else if (PKB::callTable.count(i) > 0) {
			const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
			for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
				if (modifiesVar == *it)
					return false;
		}

	queue<CFGNode*> search;
	unordered_set<CFGNode*> seen;

	switch (s1->type) {
	case CFGNode::StandardNode:
	case CFGNode::DummyNode:
		if (s1->children.oneChild != NULL)
			search.push(s1->children.oneChild);
		break;

	case CFGNode::WhileNode:
		if (s1->children.whileChildren.whileIn->first <= a2
			&& a2 < s1->children.whileChildren.whileOut->first)
			search.push(s1->children.whileChildren.whileIn);
		else
			search.push(s1->children.whileChildren.whileOut);
		break;

	case CFGNode::IfNode:
		if (s1->children.ifChildren.ifThen->first <= a2
			&& a2 < s1->children.ifChildren.ifElse->first)
			search.push(s1->children.ifChildren.ifThen);
		else if (s1->children.ifChildren.ifElse->first <= a2
			&& a2 < s1->children.ifChildren.ifLater->first)
			search.push(s1->children.ifChildren.ifElse);
		else {
			search.push(s1->children.ifChildren.ifThen);
			search.push(s1->children.ifChildren.ifElse);
		}
		break;
	}

	while (!search.empty()) {
		CFGNode* currCFG = search.front();
		search.pop();
		if (seen.count(currCFG) > 0)
			continue;
		seen.insert(currCFG);

		if (currCFG->first != 0) {
			if (currCFG->modifySet.count(modifiesVar) > 0) {
				if (currCFG->first <= a2 && a2 <= currCFG->last) {
					for (int i = currCFG->first; i < a2; i++) {
						if (PKB::assignTable.count(i) > 0) {
							if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0])
								return false;
						} else if (PKB::callTable.count(i) > 0) { //interprocedural will change here
							const vector<VAR>& stmtModifiesVar = PKB::modifies.getModifiedByStmt(i);
							for (auto it = stmtModifiesVar.begin(); it != stmtModifiesVar.end(); it++)
								if (modifiesVar == *it)
									return false;
						}
					}
					return true;
				}
				break;
			} else if (currCFG->first <= a2 && a2 <= currCFG->last)
				return true;
		}
		
		switch (currCFG->type) {
		case CFGNode::StandardNode:
		case CFGNode::DummyNode:
			if (currCFG->children.oneChild != NULL)
				search.push(currCFG->children.oneChild);
			break;

		case CFGNode::WhileNode:
			if (currCFG->children.whileChildren.whileIn->first <= a2
				&& a2 < currCFG->children.whileChildren.whileOut->first)
				search.push(currCFG->children.whileChildren.whileIn);
			else
				search.push(currCFG->children.whileChildren.whileOut);
			break;

		case CFGNode::IfNode:
			if (currCFG->children.ifChildren.ifThen->first <= a2
				&& a2 < currCFG->children.ifChildren.ifElse->first)
				search.push(currCFG->children.ifChildren.ifThen);
			else if (currCFG->children.ifChildren.ifElse->first <= a2
				&& a2 < currCFG->children.ifChildren.ifLater->first)
				search.push(currCFG->children.ifChildren.ifElse);
			else {
				search.push(currCFG->children.ifChildren.ifThen);
				search.push(currCFG->children.ifChildren.ifElse);
			}
			break;
		}
	}
	return false;
}

/**
* This method will be used to get a list of statement that is affected by a1
* @param a1	The statement that is going to affect a2
* @return a list of statement that is affected by a1
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
	for (auto it = s2->modifySet.begin(); it != s2->modifySet.end(); it++) {
		if (usesVarSet.count(*it) > 0) {
			toStep = true;
			break;
		}
	}
	if (toStep) {
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
	}
	queue<pair<CFGNode*, unordered_set<int>>> search;
	list<pair<CFGNode*, unordered_set<int>>> ifQueue;
	unordered_set<CFGNode*> seen;
	for (auto it = s2->parents.begin(); it != s2->parents.end(); it++) {
		search.push(pair<CFGNode*, unordered_set<int>>(*it, usesVarSet));
	}
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
					if (currCFG == it->first) {
						match = true;
						usesVar = currPair.second;
						usesVar.insert(it->second.begin(), it->second.end());
						break;
					}
				if (!match) {
					ifQueue.push_back(currPair);
					continue;
				}
			} else {
				usesVar = currPair.second;
			}
		}

		if (seen.count(currCFG) > 0) {
			continue;
		}
		seen.insert(currCFG);
		
		bool toStep = false;
		for (auto it = currCFG->modifySet.begin(); it != currCFG->modifySet.end(); it++) {
			if (usesVar.count(*it) > 0) {
				toStep = true;
				break;
			}
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

		if (usesVar.size() != 0) {
			for (auto it = currCFG->parents.begin(); it != currCFG->parents.end(); it++) {
				search.push(pair<CFGNode*, unordered_set<int>>(*it, usesVar));
			}
		}
	}
	return vector<STMT>(answer.begin(), answer.end());

}

/**
* This method will be used to get a list of statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return a list of statement that is going to affect a2
*/
vector<STMT> PQLAffectsProcessor::getAffectsBy(STMT a1)
{
	if ((a1 < 0 || a1 > PKB::maxProgLines) || (PKB::assignTable.count(a1) == 0))
		return vector<STMT>(); //TODO: double check with cristina

	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(a1)[0];
	CFGNode* s1 = PKB::stmtRefMap.at(a1).getCFGNode();
	unordered_set<STMT> answer;

	for (int i = a1 + 1; i <= s1->last; i++) {
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

		if (continueCFG) {
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
	}
	return vector<STMT>(answer.begin(), answer.end());
}

//Affects*
/**
* This method will be used to check whether a1 and a2 satisfy Affect* Condition
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether Affect(a1,a2) satisfy the condition of affectStar
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

	/*if (!(PKB::next.isNextStar(a1, a2))) { //if nextStar(a1, a2) 
		return false;
	}*/

	return true;
}

/**
* This method will be used to check Affect*(a1,a2)
* @param a1	The statement that is going to affect a2
* @param a2	The statement that is affected by a1
* @return whether AffectStar(a1,a2) holds
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
					if (!match) { //kill previous value
						modifiesVarSet.erase(modifiesVar);
						if (modifiesVarSet.empty())
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
						if (modifiesVarSet.empty())
							return false;
					}
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
			if (currVar.empty())
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
								if (i == a2)
									return true;
								if (modifiesVar == *it) { //overall effect -> do nothing
									match = true;
									break;
								}
							}
						if (!match) { //kill previous value
							currVar.erase(modifiesVar);
							if (currVar.empty())
								break;
						}
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
						if (currVar.count(*it) > 0) {
							currVar.erase(*it);
							if (currVar.empty())
								break;
						}
				}

		if (currVar.empty())
			continue;

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
	return false; //shouldn't reach this point
}

/**
* This method will be used to get a list of a2 that is affects*(a1,_)
* @param a1	The statement that is going to affect a2
* @return a list of statement that is affectStar by a1
*/
vector<STMT> PQLAffectsProcessor::getAffectsStarBy(STMT a1)
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
			bool match = false;
			for (auto it = usesVar.begin(); it != usesVar.end(); it++)
				if (modifiesVarSet.count(*it) > 0) { //this statement uses a variable
					answer.insert(i);
					if (modifiesVarSet.count(modifiesVar) == 0) //this statement modifies a new variable
						modifiesVarSet.insert(modifiesVar); //add it in
					match = true;
					break;
				}
			if (!match && modifiesVarSet.count(modifiesVar) > 0) { //this statement kills value of some prev
				modifiesVarSet.erase(modifiesVar);
				if (modifiesVarSet.size() == 0)
					break;
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
					bool match = false;
					for (auto it = usesVar.begin(); it != usesVar.end(); it++)
						if (currVar.count(*it) > 0) { //this statement uses a variable
							answer.insert(i);
							if (currVar.count(modifiesVar) == 0) //this statement modifies a new variable
								currVar.insert(modifiesVar); //add it in
							match = true;
							break;
						}
					if (!match && currVar.count(modifiesVar) > 0) { //this statement kills value of some prev
						currVar.erase(modifiesVar);
						if (currVar.size() == 0)
							break;
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
* This method will be used to get a list of a2 that is affects*(_,a2)
* @param a2	The statement that is going to affect by a1
* @return a list of statement that is affectStar a2
*/
vector<STMT>  PQLAffectsProcessor::getAffectsStarFrom(STMT a2)
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


bool PQLAffectsProcessor::isAffectsBip(STMT a1, STMT a2)
{
	if (a1 < 0 || a1 > PKB::maxProgLines || a2 < 0 || a2 > PKB::maxProgLines)
		return false;
	if (PKB::assignTable.count(a1) == 0 || PKB::assignTable.count(a2) == 0)
		return false;
	
	struct Information {
		CFGNode* node;
		STMT stmt;
		stack<STMT> callStack;

		Information(CFGNode* node, STMT stmt, stack<STMT> callStack) :
		node(node), stmt(stmt), callStack(callStack) {}
	};

	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(a1)[0];

	{
		bool ok = false;
		const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(a2);
		for (auto it = usesVar.begin(); it != usesVar.end(); it++)
			if (modifiesVar == *it) {
				ok = true;
				break;
			}
		if (!ok)
			return false;
	}

	queue<Information> search;
	unordered_set<CFGNode*> seen;
	search.push(Information(NULL, a1, stack<STMT>()));

	while (!search.empty()) {
		Information info = search.front();
		search.pop();
		STMT stmt = info.stmt;
		stack<STMT>& callStack = info.callStack;
		CFGNode* node = (info.node == NULL) ? PKB::stmtRefMap.at(stmt).getCFGNode() : info.node;

		if (stmt == -1 && seen.count(node) > 0)
			continue;
		seen.insert(node);

		if (node->type != CFGNode::DummyNode) {
			if (stmt == -1)
				stmt = node->first - 1;
			bool broke = false;
			for (int i = stmt + 1; i <= node->last; i++) {
				if (i == a2)
					return true;
				if (PKB::assignTable.count(i) > 0) {
					if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0]) {
						broke = true;
						break;
					}
				} else if (PKB::callTable.count(i) > 0) {
					callStack.push(i);
					search.push(Information(NULL, -1, callStack));
					broke = true;
					break;
				}
			}
			if (broke)
				continue;
		}
		
		switch (node->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode: {
			CFGNode* child = node->children.oneChild;
			if (child == NULL) {
				if (callStack.empty()) {						
					const vector<STMT>& caller = PKB::calls.getStmtCall(node->proc);
					for (auto it = caller.begin(); it != caller.end(); it++)
						search.push(Information(NULL, *it, callStack));
				} else {
					const STMT last = callStack.top();
					callStack.pop();
					search.push(Information(NULL, last, callStack));
				}
			} else
				search.push(Information(child, -1, callStack));
									}
			break;

		case CFGNode::WhileNode:
			search.push(Information(node->children.whileChildren.whileIn, -1, callStack));
			search.push(Information(node->children.whileChildren.whileOut, -1, callStack));
			break;
		
		case CFGNode::IfNode:
			search.push(Information(node->children.ifChildren.ifThen, -1, callStack));
			search.push(Information(node->children.ifChildren.ifElse, -1, callStack));
			break;
		}
	}
	return false;
}

vector<STMT> PQLAffectsProcessor::getAffectsBipBy(STMT a1)
{
	if (a1 < 0 || a1 > PKB::maxProgLines)
		return vector<STMT>();
	if (PKB::assignTable.count(a1) == 0)
		return vector<STMT>();
	
	struct Information {
		CFGNode* node;
		STMT stmt;
		stack<STMT> callStack;

		Information(CFGNode* node, STMT stmt, stack<STMT> callStack) :
		node(node), stmt(stmt), callStack(callStack) {}
	};
	
	unordered_set<STMT> answer;
	const VAR modifiesVar = PKB::modifies.getModifiedByStmt(a1)[0];
	queue<Information> search;
	unordered_set<CFGNode*> seen;
	search.push(Information(NULL, a1, stack<STMT>()));

	while (!search.empty()) {
		Information info = search.front();
		search.pop();
		STMT stmt = info.stmt;
		stack<STMT>& callStack = info.callStack;
		CFGNode* node = (info.node == NULL) ? PKB::stmtRefMap.at(stmt).getCFGNode() : info.node;

		if (stmt == -1 && seen.count(node) > 0)
			continue;
		seen.insert(node);

		if (node->type != CFGNode::DummyNode) {
			if (stmt == -1)
				stmt = node->first - 1;
			bool broke = false;
			for (int i = stmt + 1; i <= node->last; i++) {
				if (PKB::assignTable.count(i) > 0) {
					const vector<VAR>& stmtUsesVar = PKB::uses.getUsedByStmt(i);
					for (auto it = stmtUsesVar.begin(); it != stmtUsesVar.end(); it++)
						if (modifiesVar == *it) {
							answer.insert(i);
							break;
						}
					if (modifiesVar == PKB::modifies.getModifiedByStmt(i)[0]) {
						broke = true;
						break;
					}
				} else if (PKB::callTable.count(i) > 0) {
					callStack.push(i);
					search.push(Information(NULL, -1, callStack));
					broke = true;
					break;
				}
			}
			if (broke)
				continue;
		}
		
		switch (node->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode: {
			CFGNode* child = node->children.oneChild;
			if (child == NULL) {
				if (callStack.empty()) {						
					const vector<STMT>& caller = PKB::calls.getStmtCall(node->proc);
					for (auto it = caller.begin(); it != caller.end(); it++)
						search.push(Information(NULL, *it, callStack));
				} else {
					const STMT last = callStack.top();
					callStack.pop();
					search.push(Information(NULL, last, callStack));
				}
			} else
				search.push(Information(child, -1, callStack));
									}
			break;

		case CFGNode::WhileNode:
			search.push(Information(node->children.whileChildren.whileIn, -1, callStack));
			search.push(Information(node->children.whileChildren.whileOut, -1, callStack));
			break;
		
		case CFGNode::IfNode:
			search.push(Information(node->children.ifChildren.ifThen, -1, callStack));
			search.push(Information(node->children.ifChildren.ifElse, -1, callStack));
			break;
		}
	}
	return vector<STMT>(answer.begin(), answer.end());
}

vector<STMT> PQLAffectsProcessor::getAffectsBipFrom(STMT a2)
{
	vector<STMT> answer;

	return answer;
}

bool PQLAffectsProcessor::isAffectsBipStar(STMT a1, STMT a2)
{
	if (a1 < 0 || a1 > PKB::maxProgLines || a2 < 0 || a2 > PKB::maxProgLines)
		return false;
	if (PKB::assignTable.count(a1) == 0 || PKB::assignTable.count(a2) == 0)
		return false;
	
	struct Information {
		CFGNode* node;
		STMT stmt;
		unordered_set<VAR> activeVars;
		stack<STMT> callStack;

		Information(CFGNode* node, STMT stmt,
			unordered_set<VAR> activeVars, stack<STMT> callStack) :
		node(node), stmt(stmt), activeVars(activeVars), callStack(callStack) {}
	};

	queue<Information> search;
	unordered_map<CFGNode*, unordered_set<int>> seen;

	{
	const vector<VAR>& modifiesVarVector = PKB::modifies.getModifiedByStmt(a1);
	const unordered_set<VAR> modifiesVarSet(modifiesVarVector.begin(), modifiesVarVector.end());
	search.push(Information(NULL, a1, modifiesVarSet, stack<STMT>()));
	}

	while (!search.empty()) {
		Information info = search.front();
		search.pop();
		STMT stmt = info.stmt;
		unordered_set<VAR>& activeVars = info.activeVars;
		stack<STMT>& callStack = info.callStack;
		CFGNode* node = (info.node == NULL) ? PKB::stmtRefMap.at(stmt).getCFGNode() : info.node;

		if (stmt == -1 && seen.count(node) > 0) {
			unordered_set<int>& seenVar = seen[node];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				activeVars.erase(*it);
			if (activeVars.empty())
				continue;
			for (auto it = activeVars.begin(); it != activeVars.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<CFGNode*, unordered_set<int>>(node, activeVars));

		if (node->type != CFGNode::DummyNode) {
			if (stmt == -1)
				stmt = node->first - 1;
			bool broke = false;
			for (int i = stmt + 1; i <= node->last; i++) {
				if (PKB::assignTable.count(i) > 0) {
					const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
					const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (activeVars.count(modifiesVar)) { //this statement kills value of some prev
						bool match = false;
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (activeVars.count(*it) > 0) { //this statement uses a variable
								if (i == a2)
									return true;
								if (modifiesVar == *it) { //overall effect -> do nothing
									match = true;
									break;
								}
							}
						if (!match) { //kill previous value
							activeVars.erase(modifiesVar);
							if (activeVars.empty()) {
								broke = true;
								break;
							}
						}
					} else { //this statement does not kill anything
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (activeVars.count(*it) > 0) { //this statement uses a variable
								activeVars.insert(modifiesVar); //overall effect -> add it in
								break;
							}
					}
				} else if (PKB::callTable.count(i) > 0) {
					callStack.push(i);
					search.push(Information(NULL, -1, activeVars, callStack));
					broke = true;
					break;
				}
			}

			if (broke)
				continue;
		}
		
		switch (node->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode: {
			CFGNode* child = node->children.oneChild;
			if (child == NULL) {
				if (callStack.empty()) {						
					const vector<STMT>& caller = PKB::calls.getStmtCall(node->proc);
					for (auto it = caller.begin(); it != caller.end(); it++)
						search.push(Information(NULL, *it, activeVars, callStack));
				} else {
					const STMT last = callStack.top();
					callStack.pop();
					search.push(Information(NULL, last, activeVars, callStack));
				}
			} else
				search.push(Information(child, -1, activeVars, callStack));
									}
			break;

		case CFGNode::WhileNode:
			search.push(Information(node->children.whileChildren.whileIn,
				-1, activeVars, callStack));
			search.push(Information(node->children.whileChildren.whileOut,
				-1, activeVars, callStack));
			break;
		
		case CFGNode::IfNode:
			search.push(Information(node->children.ifChildren.ifThen, -1, activeVars, callStack));
			search.push(Information(node->children.ifChildren.ifElse, -1, activeVars, callStack));
			break;
		}
	}
	return false;
}

vector<STMT> PQLAffectsProcessor::getAffectsBipStarBy(STMT a1)
{
	if (a1 < 0 || a1 > PKB::maxProgLines)
		return vector<STMT>();
	if (PKB::assignTable.count(a1) == 0)
		return vector<STMT>();
	
	struct Information {
		CFGNode* node;
		STMT stmt;
		unordered_set<VAR> activeVars;
		stack<STMT> callStack;

		Information(CFGNode* node, STMT stmt,
			unordered_set<VAR> activeVars, stack<STMT> callStack) :
		node(node), stmt(stmt), activeVars(activeVars), callStack(callStack) {}
	};
	
	unordered_set<STMT> answer;
	queue<Information> search;
	unordered_map<CFGNode*, unordered_set<int>> seen;

	{
	const vector<VAR>& modifiesVarVector = PKB::modifies.getModifiedByStmt(a1);
	const unordered_set<VAR> modifiesVarSet(modifiesVarVector.begin(), modifiesVarVector.end());
	search.push(Information(NULL, a1, modifiesVarSet, stack<STMT>()));
	}

	while (!search.empty()) {
		Information info = search.front();
		search.pop();
		STMT stmt = info.stmt;
		unordered_set<VAR>& activeVars = info.activeVars;
		stack<STMT>& callStack = info.callStack;
		CFGNode* node = (info.node == NULL) ? PKB::stmtRefMap.at(stmt).getCFGNode() : info.node;

		if (stmt == -1 && seen.count(node) > 0) {
			unordered_set<int>& seenVar = seen[node];
			for (auto it = seenVar.begin(); it != seenVar.end(); it++)
				activeVars.erase(*it);
			if (activeVars.empty())
				continue;
			for (auto it = activeVars.begin(); it != activeVars.end(); it++)
				seenVar.insert(*it);
		} else
			seen.insert(pair<CFGNode*, unordered_set<int>>(node, activeVars));

		if (node->type != CFGNode::DummyNode) {
			if (stmt == -1)
				stmt = node->first - 1;
			bool broke = false;
			for (int i = stmt + 1; i <= node->last; i++) {
				if (PKB::assignTable.count(i) > 0) {
					const vector<VAR>& usesVar = PKB::uses.getUsedByStmt(i);
					const VAR modifiesVar = PKB::modifies.getModifiedByStmt(i)[0];
					if (activeVars.count(modifiesVar)) { //this statement kills value of some prev
						bool match = false;
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (activeVars.count(*it) > 0) { //this statement uses a variable
								answer.insert(i);
								if (modifiesVar == *it) { //overall effect -> do nothing
									match = true;
									break;
								}
							}
						if (!match) { //kill previous value
							activeVars.erase(modifiesVar);
							if (activeVars.empty()) {
								broke = true;
								break;
							}
						}
					} else { //this statement does not kill anything
						for (auto it = usesVar.begin(); it != usesVar.end(); it++)
							if (activeVars.count(*it) > 0) { //this statement uses a variable
								activeVars.insert(modifiesVar); //overall effect -> add it in
								break;
							}
					}
				} else if (PKB::callTable.count(i) > 0) {
					callStack.push(i);
					search.push(Information(NULL, -1, activeVars, callStack));
					broke = true;
					break;
				}
			}

			if (broke)
				continue;
		}
		
		switch (node->type) {
		case CFGNode::DummyNode:
		case CFGNode::StandardNode: {
			CFGNode* child = node->children.oneChild;
			if (child == NULL) {
				if (callStack.empty()) {						
					const vector<STMT>& caller = PKB::calls.getStmtCall(node->proc);
					for (auto it = caller.begin(); it != caller.end(); it++)
						search.push(Information(NULL, *it, activeVars, callStack));
				} else {
					const STMT last = callStack.top();
					callStack.pop();
					search.push(Information(NULL, last, activeVars, callStack));
				}
			} else
				search.push(Information(child, -1, activeVars, callStack));
									}
			break;

		case CFGNode::WhileNode:
			search.push(Information(node->children.whileChildren.whileIn,
				-1, activeVars, callStack));
			search.push(Information(node->children.whileChildren.whileOut,
				-1, activeVars, callStack));
			break;
		
		case CFGNode::IfNode:
			search.push(Information(node->children.ifChildren.ifThen, -1, activeVars, callStack));
			search.push(Information(node->children.ifChildren.ifElse, -1, activeVars, callStack));
			break;
		}
	}
	return vector<STMT>(answer.begin(), answer.end());
}

vector<STMT> PQLAffectsProcessor::getAffectsBipStarFrom(STMT a2)
{
	vector<STMT> answer;

	return answer;
}