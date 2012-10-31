#pragma once
#include "IEvalQuery.h"
#include "stdafx.h"
#include "QueryTreeNode.h"
#include "QueryTreeBuilder.h"
#include "QueryPreprocessor.h"
#include "QueryCondNode.h"
#include "QueryRelNode.h"
#include "QuerySelNode.h"
#include "QueryCondNode.h"
#include "QueryEnums.h"
#include "PKB.h"
#include "Helper.h"


void IEvalQuery::initNewQuery()
{
	allStmtsFirst = false;
	allStmtsSecond = false;

	allProcsFirst = false;
	allProcsSecond = false;
	
	allVarsSecond = false;

	firstNumber = false;
	secondNumber = false;

	firstFixedProcedure = false; 
	secondFixedProcedure = false;
	secondFixedVariable = false;

	projectBool = false;
}

//I Evaluate Query, I PWNED ALL THE CLASSES HERE ! @.@
vector<string> IEvalQuery::evaluateQuery(QueryTree qt)
{
	//Outer vector is vector of cluster
	//inner vector is the cluster
	initNewQuery();

	for (int i = 0; i< qt.size(); i++)
	{
		vector<QueryTreeNode*> cluster = qt.at(i);
		for (int j = 0; j < cluster.size(); j++)
		{
			currentNode = cluster.at(j);
			currentNodeType = currentNode->getNodeType();			
			switch (currentNodeType){
			case QueryTreeNode::Relationship:
				currentRelationshipNode = new QueryRelNode();
				currentRelationshipNode = (QueryRelNode*) currentNode;
				currentRelationshipType = currentRelationshipNode->getRelationshipType();
				currentFirstVariableName = currentRelationshipNode->getFirstVariableName();
				currentSecondVariableName = currentRelationshipNode->getSecondVariableName();
				currentFirstVariableType = currentRelationshipNode->getFirstVariableType();
				currentSecondVariableType = currentRelationshipNode->getSecondVariableType();
				switch (currentRelationshipType){
				case QueryEnums::Follows:
					EvaluateFollows();
					break;
				case QueryEnums::FollowsStar:
					EvaluateFollowsStar();
					break;
				case QueryEnums::Parent:
					EvaluateParent();
					break;
				case QueryEnums::ParentStar:
					EvaluateParentStar();
					break;
				case QueryEnums::Modifies:
					EvaluateModifies();
					break;
				case QueryEnums::Uses:
					EvaluateUses();
					break;
				case QueryEnums::Calls:
					EvaluateCalls();
					break;
				case QueryEnums::CallsStar:
					EvaluateCallsStar();
					break;
				case QueryEnums::Next:
		//			EvaluateNext();
					break;
				case QueryEnums::NextStar:
		//			EvaluateNextStar();
					break;
				case QueryEnums::Affects:
		//			EvaluateAffects();
					break;
				case QueryEnums::AffectsStar:
		//			EvaluateAffectsStar();
					break;
				default:
					break;
				}
			case QueryTreeNode::Select:
			{
				currentSelNode = new QuerySelNode();
				currentSelNode = (QuerySelNode*) currentNode;
				selected = currentSelNode->getSelectedVariables();
				for (int x = 0; x < selected.size(); x++)
				{
					if (currentFirstVariableName.compare(selected.at(x).second) == 0)
					{
						for (auto y = firstVariableAnswer.begin(); y != firstVariableAnswer.end(); y++)
							answer.push_back((*y));
					}
					if (currentSecondVariableName.compare(selected.at(x).second) == 0)
					{
						for (auto y = secondVariableAnswer.begin(); y != secondVariableAnswer.end(); y++)
							answer.push_back((*y));
					}
				}
			}
				break;
			case QueryTreeNode::Project:
			{
				//what do i do with this useless piece of crap?
			}
				break;
			default:
				throw SPAException("Unidentified QT node");
				break;
			}
		}
	}

	if (projectBool == false && answer.empty())
	{
		answer.push_back("None");
		return answer;
	}
	else if (projectBool == true)
		return boolAnswer;
	else
		return answer;
}

void IEvalQuery::populateVariableIndices(QueryEnums::QueryVar type, int index)
{
	if (index == 1){
		switch (type){
			case QueryEnums::Assign:
				currentFirstIndices = PKB::assignTable;
				break;
			case QueryEnums::While:
				currentFirstIndices = PKB::whileTable;
				break;
			case QueryEnums::If:
				currentFirstIndices = PKB::ifTable;
				break;
			case QueryEnums::Call:
				currentFirstIndices = PKB::callTable;
				break;
			case QueryEnums::Procedure:
				allProcsFirst = true;
				break;
			case QueryEnums::Stmt:
				allStmtsFirst = true;
				break;
			default:
				throw SPAException("Invalid first relationship parameter type");
			}
	}else if(index == 2){
		switch (type){
			case QueryEnums::Assign:
				currentSecondIndices = PKB::assignTable;
				break;
			case QueryEnums::While:
				currentSecondIndices = PKB::whileTable;
				break;
			case QueryEnums::If:
				currentSecondIndices = PKB::ifTable;
				break;
			case QueryEnums::Call:
				currentSecondIndices = PKB::callTable;
				break;
			case QueryEnums::Procedure:
				allProcsSecond = true;
				break;
			case QueryEnums::Stmt:
				allStmtsSecond = true;
				break;
			case QueryEnums::Variable:
				allVarsSecond = true;
				break;
			default:
				throw SPAException("Invalid second relationship parameter type");
		}
	}else
	{
		throw SPAException("Invalid index should only be 1 or 2");
	}
}

void IEvalQuery::EvaluateModifies()
{
	if (QueryPreprocessor::isNumber(currentFirstVariableName))
	{
		firstNumber = true;
		currentFirstIndices.insert(atoi(currentFirstVariableName.c_str()));
		currentFirstVariableNo = atoi(currentFirstVariableName.c_str());
	}
	else if (currentFirstVariableType == QueryEnums::Procedure && currentFirstVariableName.front() == '\"' && 
			currentFirstVariableName.at(currentFirstVariableName.size() - 1) == '\"') {
		firstFixedProcedure = true;
		if (currentFirstVariableName.size() == 3)
		{
			currentFirstVariableName = Helper::charToString(currentFirstVariableName.at(1));
			currentFirstVariableNo = PKB::variables.getVARIndex(currentFirstVariableName);
		}
		else
		{
			currentFirstIndices.insert(PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
		}
	}

	//For second parameter, i only expect variables either in the form of "ilovestayingupatnight" or 
	//a user defined synonym.
	if (currentSecondVariableName.front() == '\"' && 
		currentSecondVariableName.at(currentSecondVariableName.size() - 1) == '\"')
	{
		secondFixedVariable = true;
		if (currentSecondVariableName.size() == 3)
		{
			currentSecondVariableName = Helper::charToString(currentSecondVariableName.at(1));
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName);
		}
		else
		{
			currentSecondIndices.insert(PKB::variables.getVARIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2));
		}
	}
						
	if (firstNumber == false && firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondFixedVariable == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::modifies.isModifiedProc(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");
	}
	else if (firstNumber == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::modifies.isModifiedStmt(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");

	}
	else if (allProcsFirst == true && allVarsSecond == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			for (int y = 1; y <= PKB::variables.getSize(); y++)
				if (PKB::modifies.isModifiedProc(x, y))
				{
					firstVariableAnswer.push_back(PKB::procedures.getPROCName(x));
					secondVariableAnswer.push_back(PKB::variables.getVARName(y));
				}
		}
	}
	else if (allProcsFirst == true && secondFixedVariable == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			if (PKB::modifies.isModifiedProc(x, currentSecondVariableNo))
				firstVariableAnswer.push_back(PKB::procedures.getPROCName(x));	
		}
	}
	else if (allStmtsFirst == true && allVarsSecond == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::variables.getSize(); y++)
			{
				if (PKB::modifies.isModifiedStmt(x, y))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(PKB::variables.getVARName(y));
				}
			}
		}
	}
	else if (allStmtsFirst == true && secondFixedVariable == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			if (PKB::modifies.isModifiedStmt(x, currentSecondVariableNo))
				firstVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else if (firstFixedProcedure == true && allVarsSecond == true)
	{
		for (int x = 1; x <= PKB::variables.getSize(); x++)
		{
			if (PKB::modifies.isModifiedProc(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(PKB::variables.getVARName(x));
		}							
	}
	else if (firstNumber == true && allVarsSecond == true)
	{
		for (int x = 1; x <= PKB::variables.getSize(); x++)
		{
			if (PKB::modifies.isModifiedStmt(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(PKB::variables.getVARName(x));
		}
	}
	else
		throw SPAException("No case matches this query; not supposed to happen");
}

void IEvalQuery::EvaluateUses()
{
	if (QueryPreprocessor::isNumber(currentFirstVariableName))
	{
		firstNumber = true;
		currentFirstIndices.insert(atoi(currentFirstVariableName.c_str()));
		currentFirstVariableNo = atoi(currentFirstVariableName.c_str());
	}
	else if (currentFirstVariableType == QueryEnums::Procedure &&
			currentFirstVariableName.front() == '\"' && 
			currentFirstVariableName.at(currentFirstVariableName.size() - 1) == '\"')
	{
		firstFixedProcedure = true;
		if (currentFirstVariableName.size() == 3)
		{
			currentFirstVariableName = Helper::charToString(currentFirstVariableName.at(1));
			currentFirstVariableNo = PKB::variables.getVARIndex(currentFirstVariableName);
		}
		else
		{
			currentFirstIndices.insert(PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
		}
	}

	//For second parameter, i only expect variables either in the form of "ilovestayingupatnight" or 
	//a user defined synonym.
	if (currentSecondVariableName.front() == '\"' && 
		currentSecondVariableName.at(currentSecondVariableName.size() - 1) == '\"')
	{
		secondFixedVariable = true;
		if (currentSecondVariableName.size() == 3)
		{
			currentSecondVariableName = Helper::charToString(currentSecondVariableName.at(1));
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName);
		}
		else
		{
			currentSecondIndices.insert(PKB::variables.getVARIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2));
		}
	}
						
	if (firstNumber == false && firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondFixedVariable == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::uses.isUsedProc(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");
	}
	else if (firstNumber == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::uses.isUsedStmt(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");
	}
	else if (allProcsFirst == true && allVarsSecond == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			for (int y = 1; y <= PKB::variables.getSize(); y++)
				if (PKB::uses.isUsedProc(x, y))
				{
					firstVariableAnswer.push_back(PKB::procedures.getPROCName(x));
					secondVariableAnswer.push_back(PKB::variables.getVARName(y));
				}
		}
	}
	else if (allProcsFirst == true && secondFixedVariable == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			if (PKB::uses.isUsedProc(x, currentSecondVariableNo))
				firstVariableAnswer.push_back(PKB::procedures.getPROCName(x));	
		}
	}
	else if (allStmtsFirst == true && allVarsSecond == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::variables.getSize(); y++)
			{
				if (PKB::uses.isUsedStmt(x, y))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(PKB::variables.getVARName(y));
				}
			}
		}
	}
	else if (allStmtsFirst == true && secondFixedVariable == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			if (PKB::uses.isUsedStmt(x, currentSecondVariableNo))
				firstVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else if ((firstFixedProcedure == true || firstNumber == true) && allVarsSecond == true)
	{
		for (int x = 1; x <= PKB::variables.getSize(); x++)
		{
			if (PKB::uses.isUsedProc(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(PKB::variables.getVARName(x));
		}							
	}
	else{
		throw SPAException("No case matches this query; not supposed to happen");
	}
}

void IEvalQuery::EvaluateParent()
{
	if (QueryPreprocessor::isNumber(currentFirstVariableName)){
		firstNumber = true;
		currentFirstIndices.insert(atoi(currentFirstVariableName.c_str()));
		currentFirstVariableNo = atoi(currentFirstVariableName.c_str());
	}
	if (QueryPreprocessor::isNumber(currentSecondVariableName))
	{
		secondNumber = true;
		currentSecondIndices.insert(atoi(currentSecondVariableName.c_str()));
		currentSecondVariableNo = atoi(currentFirstVariableName.c_str());
	}
						
	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true){ //special case{
		projectBool = true; //answer is a boolean
		if (PKB::parent.isParent(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");
	}
	else if (allStmtsFirst == true && allStmtsSecond == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::parent.isParent(x, y))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}

	}else if (allStmtsFirst == true){
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
				if (PKB::parent.isParent(x, int (*y))){
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
		}
	}
	else if (allStmtsSecond == true){
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++){
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::parent.isParent(int (*x), y))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}
	}else{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++){
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++){
				if (PKB::parent.isParent(int (*x),  int (*y))){
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
			}
		}
	}
}

void IEvalQuery::EvaluateParentStar()
{
	if (QueryPreprocessor::isNumber(currentFirstVariableName))
	{
		firstNumber = true;
		currentFirstIndices.insert(atoi(currentFirstVariableName.c_str()));
		currentFirstVariableNo = atoi(currentFirstVariableName.c_str());
	}
	if (QueryPreprocessor::isNumber(currentSecondVariableName))
	{
		secondNumber = true;
		currentSecondIndices.insert(atoi(currentSecondVariableName.c_str()));
		currentSecondVariableNo = atoi(currentFirstVariableName.c_str());
	}
						
	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true) //special case
	{
		projectBool = true; //answer is a boolean
		if (PKB::parent.isParentStar(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");
	}
	else if (allStmtsFirst == true && allStmtsSecond == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::parent.isParentStar(x, y))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}
	}
	else if (allStmtsFirst == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
				if (PKB::parent.isParentStar(x, int (*y)))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
		}
	}
	else if (allStmtsSecond == true)
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::parent.isParentStar(int (*x), y))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}
	}
	else
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
				if (PKB::parent.isParentStar(int (*x),  int (*y)))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
		}
	}
}

void IEvalQuery::EvaluateFollows()
{
	if (QueryPreprocessor::isNumber(currentFirstVariableName))
	{
		firstNumber = true;
		currentFirstIndices.insert(atoi(currentFirstVariableName.c_str()));
		currentFirstVariableNo = atoi(currentFirstVariableName.c_str());
	}
	if (QueryPreprocessor::isNumber(currentSecondVariableName))
	{
		secondNumber = true;
		currentSecondIndices.insert(atoi(currentSecondVariableName.c_str()));
		currentSecondVariableNo = atoi(currentFirstVariableName.c_str());
	}
						
	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true) //special case
	{
		projectBool = true; //answer is a boolean
		if (PKB::follows.isFollows(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");
	}
	else if (allStmtsFirst == true && allStmtsSecond == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::follows.isFollows(x, y))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}
	}
	else if (allStmtsFirst == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
				if (PKB::follows.isFollows(x, (*y)))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
		}
	}
	else if (allStmtsSecond == true)
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::follows.isFollows((*x), y))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}
	}
	else
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
				if (PKB::follows.isFollows(int (*x),  int (*y)))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
		}
	}

	//Resetting all the flags and stuff, todo: put at bottom of 2nd for loop
	firstNumber = false;
	secondNumber = false;
	allStmtsFirst = false;
	allStmtsSecond = false;
	currentFirstIndices.clear();
	currentSecondIndices.clear();
	firstVariableAnswer.clear();
	secondVariableAnswer.clear();
}

void IEvalQuery::EvaluateFollowsStar()
{
	if (QueryPreprocessor::isNumber(currentFirstVariableName))
	{
		firstNumber = true;
		currentFirstIndices.insert(atoi(currentFirstVariableName.c_str()));
		currentFirstVariableNo = atoi(currentFirstVariableName.c_str());
	}
	if (QueryPreprocessor::isNumber(currentSecondVariableName))
	{
		secondNumber = true;
		currentSecondIndices.insert(atoi(currentSecondVariableName.c_str()));
		currentSecondVariableNo = atoi(currentFirstVariableName.c_str());
	}

	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true) //special case	
	{
		projectBool = true; //answer is a boolean
		if (PKB::follows.isFollowsStar(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("true");
		else
			boolAnswer.push_back("false");
	}
	else if (allStmtsFirst == true && allStmtsSecond == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)				
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::follows.isFollowsStar(x, y))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}
	}
	else if (allStmtsFirst == true)
	{
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
				if (PKB::follows.isFollowsStar(x, *y))
				{
					firstVariableAnswer.push_back(Helper::intToString(x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
		}
	}
	else if (allStmtsSecond == true)
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::follows.isFollowsStar(*x, y))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(y));
				}
		}
	}
	else
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
				if (PKB::follows.isFollowsStar(*x,  *y))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
		}
	}
}

void IEvalQuery::EvaluateCalls()
{
		//Assume correct input, ie. 2 procedures and nothing else
	if (currentFirstVariableName.front() == '\"' && 
		currentFirstVariableName.at(currentFirstVariableName.size() - 1) == '\"')
	{
		firstFixedProcedure = true;
		if (currentFirstVariableName.size() == 3)
		{
			currentFirstVariableName = Helper::charToString(currentFirstVariableName.at(1));
			currentFirstVariableNo = PKB::variables.getVARIndex(currentFirstVariableName);
		}
		else
		{
			currentFirstIndices.insert(PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
		}
	}

	if (currentSecondVariableName.front() == '\"' && 
		currentSecondVariableName.at(currentSecondVariableName.size() - 1) == '\"')
	{
		secondFixedProcedure = true;
		if (currentSecondVariableName.size() == 3)
		{
			currentSecondVariableName = Helper::charToString(currentSecondVariableName.at(1));
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName);
		}
		else
		{
			currentSecondIndices.insert(PKB::procedures.getPROCIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
			currentSecondVariableNo = (PKB::procedures.getPROCIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
		}
	}

	if (firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);
	if (secondFixedProcedure == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedProcedure == true)
	{
		projectBool = true;
		if (PKB::calls.isCalled(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("True");
		else
			boolAnswer.push_back("False");
	}
	else if (firstFixedProcedure == true && allProcsSecond == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			if (PKB::calls.isCalled(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else if (secondFixedProcedure == true && allProcsFirst == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			if (PKB::calls.isCalled(x, currentSecondVariableNo))
				firstVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
			{
				if (PKB::calls.isCalled(*x, *y))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
			}
		}
	}
}

void IEvalQuery::EvaluateCallsStar()
{
		//Assume correct input, ie. 2 procedures and nothing else
	if (currentFirstVariableName.front() == '\"' && 
		currentFirstVariableName.at(currentFirstVariableName.size() - 1) == '\"')
	{
		firstFixedProcedure = true;
		if (currentFirstVariableName.size() == 3)
		{
			currentFirstVariableName = Helper::charToString(currentFirstVariableName.at(1));
			currentFirstVariableNo = PKB::variables.getVARIndex(currentFirstVariableName);
		}
		else
		{
			currentFirstIndices.insert(PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
		}
	}

	if (currentSecondVariableName.front() == '\"' && 
		currentSecondVariableName.at(currentSecondVariableName.size() - 1) == '\"')
	{
		secondFixedProcedure = true;
		if (currentSecondVariableName.size() == 3)
		{
			currentSecondVariableName = Helper::charToString(currentSecondVariableName.at(1));
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName);
		}
		else
		{
			currentSecondIndices.insert(PKB::procedures.getPROCIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
			currentSecondVariableNo = (PKB::procedures.getPROCIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
		}
	}

	if (firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);
	if (secondFixedProcedure == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedProcedure == true)
	{
		projectBool = true;
		if (PKB::calls.isCalledStar(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer.push_back("True");
		else
			boolAnswer.push_back("False");
	}
	else if (firstFixedProcedure == true && allProcsSecond == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			if (PKB::calls.isCalledStar(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else if (secondFixedProcedure == true && allProcsFirst == true)
	{
		for (int x = 1; x <= PKB::procedures.getSize(); x++)
		{
			if (PKB::calls.isCalledStar(x, currentSecondVariableNo))
				firstVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else
	{
		for (auto x = currentFirstIndices.begin(); x != currentFirstIndices.end(); x++)
		{
			for (auto y = currentSecondIndices.begin(); y != currentSecondIndices.end(); y++)
			{
				if (PKB::calls.isCalledStar(*x, *y))
				{
					firstVariableAnswer.push_back(Helper::intToString(*x));
					secondVariableAnswer.push_back(Helper::intToString(*y));
				}
			}
		}
	}
}