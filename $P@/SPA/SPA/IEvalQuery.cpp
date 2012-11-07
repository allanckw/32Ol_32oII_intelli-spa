#pragma once
#include "IEvalQuery.h"
#include "stdafx.h"
#include "QueryParser.h"
#include "QueryTreeNode.h"
#include "QueryTreeBuilder.h"
#include "QueryPreprocessor.h"
#include "QueryCondNode.h"
#include "QueryRelNode.h"
#include "QuerySelNode.h"
#include "QueryCondNode.h"
#include "QueryLastSelNode.h"
#include "QueryEnums.h"
#include "ASTNode.h"
#include "ASTStmtNode.h"
#include "PKB.h"
#include "Helper.h"


void IEvalQuery::initNewQuery()
{
	allStmtsFirst = false;
	allStmtsSecond = false;

	allProcsFirst = false;
	allProcsSecond = false;
	
	allVarsFirst = false;
	allVarsSecond = false;

	firstNumber = false;
	secondNumber = false;

	firstFixedProcedure = false; 
	secondFixedProcedure = false;
	secondFixedVariable = false;

	boolAnswer = false;
	projectBool = false;

	answer.clear();
}

//I Evaluate Query, I PWNED ALL THE CLASSES HERE ! @.@
vector<string> IEvalQuery::evaluateQuery(QueryTree qt)
{
	initNewQuery();

	for (int i = 0; i< qt.size(); i++)
	{
		vector<QueryTreeNode*> cluster = qt.at(i);
		for (int j = 0; j < cluster.size(); j++)
		{
			currentNode = cluster.at(j);
			currentNodeType = currentNode->getNodeType();			
			switch (currentNodeType)
			{
				case QueryTreeNode::Relationship:
				{
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
					case QueryEnums::Pattern:
						EvaluatePattern();
						break;
					default:
						break;
					}
				}
				break;
				case QueryTreeNode::Select:
				{
					currentSelNode = (QuerySelNode*) currentNode;
					selected = currentSelNode->getSelectedVariables();
					if (!selected.empty())
					{
						selectType = selected.at(0).first;
						for (int x = 0; x < selected.size(); x++)
						{
							if (currentFirstVariableName.compare(selected.at(x).second) == 0)
							{
								for (auto z = firstVariableAnswer.begin(); z != firstVariableAnswer.end(); z++)
									answer.push_back((*z));
							}
							if (currentSecondVariableName.compare(selected.at(x).second) == 0)
							{
								for (auto z = secondVariableAnswer.begin(); z != secondVariableAnswer.end(); z++)
									answer.push_back((*z));
							}
						}
					}
				}
				break;
				case QueryTreeNode::Project:
				{
					//what do i do with this useless piece of crap?
				}
				break;
				case QueryTreeNode::LastSelect:
				{
					lastSelNode = (QueryLastSelNode*) currentNode;
					selected = lastSelNode->getRemaindingSelectedVariables();

					if ((!firstVariableAnswer.empty() || !secondVariableAnswer.empty() || boolAnswer == true)
						&& !selected.empty()) //If select variables were not found in reladition excluding projectBool case
					{	
						currentFirstIndices.clear();
						for (int i = 0; i < selected.size(); i++)
						{
							allStmtsFirst = false;
							allProcsFirst = false;
							allVarsFirst = false;
							for (int i = 0; i < selected.size(); i++)
							{
								selectType = selected.at(i).first;
								if (selectType == QueryEnums::Boolean)
								{
									answer.push_back("True");
									break;
								}
								else
								{
									populateVariableIndices(selectType, 1);
									if (allStmtsFirst == true)
									{
										for (int i = 1; i <= PKB::maxProgLines; i++)
											answer.push_back(Helper::intToString(i));
									}
									else if (allProcsFirst == true)
									{
										for (int i = 0; i < PKB::procedures.getSize(); i++)
											answer.push_back(PKB::procedures.getPROCName(i));
									}
									else if (allVarsFirst == true)
									{
										for (int i = 0; i < PKB::variables.getSize(); i++)
											answer.push_back(PKB::variables.getVARName(i));
									}
									else
									{
										for (auto z = currentFirstIndices.begin(); z != currentFirstIndices.end(); z++)
											answer.push_back(Helper::intToString(*z));
									}
								}
							}
						}
					}
					else if (!selected.empty())
					{
						selectType = selected.at(0).first;
						//If Boolean, then false needs to be returned
						//Otherwise, answer set should just be left blank.
						if (selectType == QueryEnums::Boolean)
							answer.push_back("False");
					}

				}
				break;
				case QueryTreeNode::Dummy: //Do nothing since this is just a dummy node, and is still a valid node type
				break;
				default:
				{
					throw SPAException("Unidentified QT node");
				}
				break;
			}
		}
	}

	if (answer.empty())
		answer.push_back("None");

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
			case QueryEnums::Variable:
				allVarsFirst = true;
				break;
			case QueryEnums::WildCard:
				{
					allStmtsFirst = true;
					allProcsFirst = true;
					allVarsFirst = true;
				}
				break;
			default:
				throw SPAException("Invalid first relationship parameter type");
				break;
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
			case QueryEnums::WildCard:
				{
					allStmtsSecond = true;
					allProcsSecond = true;
					allVarsSecond = true;
				}
				break;
			default:
				throw SPAException("Invalid second relationship parameter type");
				break;
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
		currentFirstVariableNo = atoi(currentFirstVariableName.c_str());
	}
	else if (currentFirstVariableType == QueryEnums::Procedure && currentFirstVariableName.front() == '\"' && 
			currentFirstVariableName.at(currentFirstVariableName.size() - 1) == '\"') 
	{
		firstFixedProcedure = true;
		if (currentFirstVariableName.size() == 3)
		{
			currentFirstVariableName = Helper::charToString(currentFirstVariableName.at(1));
			currentFirstVariableNo = PKB::procedures.getPROCIndex(currentFirstVariableName);
		}
		else
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
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
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2));
	}
						
	if (firstNumber == false && firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondFixedVariable == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::modifies.isModifiedProc(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (firstFixedProcedure == true && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 0; x < PKB::variables.getSize(); x++)
			if (PKB::modifies.isModifiedProc(currentFirstVariableNo, x))
			{
				boolAnswer = true; //remember that boolanswer is false by default
				break;
			}
	}
	else if (firstNumber == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::modifies.isModifiedStmt(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;

	}
	else if (firstNumber == true && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 0; x < PKB::variables.getSize(); x++)
			if (PKB::modifies.isModifiedStmt(currentFirstVariableNo, x))
			{
				boolAnswer = true; //remember that boolanswer is false by default
				break;
			}
	}
	else if (allProcsFirst == true && allVarsSecond == true)
	{
		for (int x = 0; x < PKB::procedures.getSize(); x++)
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
		for (int x = 0; x < PKB::procedures.getSize(); x++)
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
		for (int x = 0; x < PKB::variables.getSize(); x++)
		{
			if (PKB::modifies.isModifiedProc(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(PKB::variables.getVARName(x));
		}							
	}
	else if (firstNumber == true && allVarsSecond == true)
	{
		for (int x = 0; x < PKB::variables.getSize(); x++)
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
	//Checks and implementations for first parameter
	if (QueryPreprocessor::isNumber(currentFirstVariableName))
	{
		firstNumber = true;
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
			currentFirstVariableNo = PKB::procedures.getPROCIndex(currentFirstVariableName);
		}
		else
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
	}

	//Checks and implementations for second parameter
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
			currentSecondVariableNo = PKB::variables.getVARIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2));
	}
						
	if (firstNumber == false && firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondFixedVariable == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::uses.isUsedProc(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (firstFixedProcedure == true && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 0; x < PKB::variables.getSize(); x++)
			if (PKB::modifies.isModifiedProc(currentFirstVariableNo, x))
			{
				boolAnswer = true; //remember that boolanswer is false by default
				break;
			}
	}
	else if (firstNumber == true && secondFixedVariable == true)
	{
		projectBool = true; //answer is a boolean
		if (PKB::uses.isUsedStmt(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (firstNumber == true && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 0; x < PKB::variables.getSize(); x++)
			if (PKB::modifies.isModifiedStmt(currentFirstVariableNo, x))
			{
				boolAnswer = true; //remember that boolanswer is false by default
				break;
			}
	}
	else if (allProcsFirst == true && allVarsSecond == true)
	{
		for (int x = 0; x < PKB::procedures.getSize(); x++)
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
		for (int x = 0; x < PKB::procedures.getSize(); x++)
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
	else if (firstFixedProcedure == true && allVarsSecond == true)
	{
		for (int x = 0; x < PKB::variables.getSize(); x++)
		{
			if (PKB::uses.isUsedProc(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(PKB::variables.getVARName(x));
		}							
	}
	else if (firstNumber == true && allVarsSecond == true)
	{
		for (int x = 0; x < PKB::variables.getSize(); x++)
		{
			if (PKB::uses.isUsedStmt(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(PKB::variables.getVARName(x));
		}							
	}
	else
		throw SPAException("No case matches this query; not supposed to happen");
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
		currentSecondVariableNo = atoi(currentSecondVariableName.c_str());
	}
						
	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true){ //special case
		projectBool = true; //answer is a boolean
		if (PKB::parent.isParent(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (currentFirstVariableType == QueryEnums::WildCard && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::parent.isParent(x, y))
				{
					boolAnswer = true;
					break;
				}
		}
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
	}
	else
	{
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
		currentSecondVariableNo = atoi(currentSecondVariableName.c_str());
	}
						
	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true) //special case
	{
		projectBool = true; //answer is a boolean
		if (PKB::parent.isParentStar(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (currentFirstVariableType == QueryEnums::WildCard && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::parent.isParent(x, y))
				{
					boolAnswer = true;
					break;
				}
		}
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
		currentSecondVariableNo = atoi(currentSecondVariableName.c_str());
	}
						
	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);
	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true) //special case
	{
		projectBool = true; //answer is a boolean
		if (PKB::follows.isFollows(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (currentFirstVariableType == QueryEnums::WildCard && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::follows.isFollows(x, y))
				{
					boolAnswer = true;
					break;
				}
		}
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
		currentSecondVariableNo = atoi(currentSecondVariableName.c_str());
	}

	if (firstNumber == false)
		populateVariableIndices(currentFirstVariableType, 1);

	if (secondNumber == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstNumber == true && secondNumber == true) //special case	
	{
		projectBool = true; //answer is a boolean
		if (PKB::follows.isFollowsStar(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (currentFirstVariableType == QueryEnums::WildCard && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 1; x <= PKB::maxProgLines; x++)
		{
			for (int y = 1; y <= PKB::maxProgLines; y++)
				if (PKB::follows.isFollowsStar(x, y))
				{
					boolAnswer = true;
					break;
				}
		}
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
			currentFirstVariableNo = PKB::procedures.getPROCIndex(currentFirstVariableName);
		}
		else
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
	}

	if (currentSecondVariableName.front() == '\"' && 
		currentSecondVariableName.at(currentSecondVariableName.size() - 1) == '\"')
	{
		secondFixedProcedure = true;
		if (currentSecondVariableName.size() == 3)
		{
			currentSecondVariableName = Helper::charToString(currentSecondVariableName.at(1));
			currentSecondVariableNo = PKB::procedures.getPROCIndex(currentSecondVariableName);
		}
		else
			currentSecondVariableNo = (PKB::procedures.getPROCIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
	}

	if (firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);
	if (secondFixedProcedure == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedProcedure == true)
	{
		projectBool = true;
		if (PKB::calls.isCalled(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (currentFirstVariableType == QueryEnums::WildCard && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 0; x < PKB::procedures.getSize(); x++)
		{
			for (int y = 0; y < PKB::procedures.getSize(); y++)
				if (PKB::calls.isCalled(x, y))
				{
					boolAnswer = true;
					break;
				}
		}
	}
	else if (firstFixedProcedure == true && allProcsSecond == true)
	{
		for (int x = 0; x < PKB::procedures.getSize(); x++)
		{
			if (PKB::calls.isCalled(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else if (secondFixedProcedure == true && allProcsFirst == true)
	{
		for (int x = 0; x < PKB::procedures.getSize(); x++)
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
			currentFirstVariableNo = PKB::procedures.getPROCIndex(currentFirstVariableName);
		}
		else
			currentFirstVariableNo = (PKB::procedures.getPROCIndex(currentFirstVariableName.substr(1, 
				currentFirstVariableName.size() - 2)));
	}

	if (currentSecondVariableName.front() == '\"' && 
		currentSecondVariableName.at(currentSecondVariableName.size() - 1) == '\"')
	{
		secondFixedProcedure = true;
		if (currentSecondVariableName.size() == 3)
		{
			currentSecondVariableName = Helper::charToString(currentSecondVariableName.at(1));
			currentSecondVariableNo = PKB::procedures.getPROCIndex(currentSecondVariableName);
		}
		else
			currentSecondVariableNo = (PKB::procedures.getPROCIndex(currentSecondVariableName.substr(1, 
				currentSecondVariableName.size() - 2)));
	}

	if (firstFixedProcedure == false)
		populateVariableIndices(currentFirstVariableType, 1);
	if (secondFixedProcedure == false)
		populateVariableIndices(currentSecondVariableType, 2);

	if (firstFixedProcedure == true && secondFixedProcedure == true)
	{
		projectBool = true;
		if (PKB::calls.isCalledStar(currentFirstVariableNo, currentSecondVariableNo))
			boolAnswer = true;
		else
			boolAnswer = false;
	}
	else if (currentFirstVariableType == QueryEnums::WildCard && currentSecondVariableType == QueryEnums::WildCard)
	{
		projectBool = true;
		for (int x = 0; x < PKB::procedures.getSize(); x++)
		{
			for (int y = 0; y < PKB::procedures.getSize(); y++)
				if (PKB::calls.isCalledStar(x, y))
				{
					boolAnswer = true;
					break;
				}
		}
	}
	else if (firstFixedProcedure == true && allProcsSecond == true)
	{
		for (int x = 0; x < PKB::procedures.getSize(); x++)
		{
			if (PKB::calls.isCalledStar(currentFirstVariableNo, x))
				secondVariableAnswer.push_back(Helper::intToString(x));
		}
	}
	else if (secondFixedProcedure == true && allProcsFirst == true)
	{
		for (int x = 0; x < PKB::procedures.getSize(); x++)
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

//Keep getting child nodes of each node starting from the root and testing to see if they are assign nodes
//If assign node is found, TryMatch is called to DFS the children of that node to look for pattern match.
void IEvalQuery::EvaluatePattern()
{
	if (currentFirstVariableName.at(0) == '\"' && currentFirstVariableName.at(currentFirstVariableName.size() - 1) == '\"')
	{
		if (currentFirstVariableName.size() == 3)
			currentFirstVariableName = Helper::charToString(currentFirstVariableName.at(1));
		else
			currentFirstVariableName = currentFirstVariableName.substr(1, currentFirstVariableName.size() - 2);
	}
	else
		allVarsFirst = true;

	vecMatch = QueryParser::tokenize(currentSecondVariableName);

	stack<ASTNode*> nodesStack;
	PKB::rootNode;
	PKB::variables;
	PKB::procedures;

	nodesStack.push(PKB::rootNode);

	vector<int> stmtLineTrue;

	while(nodesStack.size() > 0)
	{
		int tempchk = nodesStack.size();
		if(nodesStack.top()->getType() == ASTNode::Assign)
		{
			ASTNode* temp = nodesStack.top();
			//chk if match

			/*vector<string> vecmatch;
			vecmatch.push_back("_");
*/

			//test space
			ASTStmtNode* tempnode = (ASTStmtNode*)temp;

			 //isSub is always true for now because we only need to handle _"hi"_ case
			if (allVarsFirst == true)
			{
				for (int i = 0; i < PKB::variables.getSize(); i++)
				{
					if(TryMatch(temp,PKB::variables.getVARName(i),vecMatch,true))
					{
						ASTStmtNode* tempnode = (ASTStmtNode*)temp;
						stmtLineTrue.push_back(tempnode->getStmtNumber());
					}
				}
			}
			else
			{
				if(TryMatch(temp,currentFirstVariableName,vecMatch,true))
				{
					ASTStmtNode* tempnode = (ASTStmtNode*)temp;
					stmtLineTrue.push_back(tempnode->getStmtNumber());
				}

			}
			nodesStack.pop();
			//remember to pop	
		}
		else
		{
			ASTNode* temp = nodesStack.top();
			nodesStack.pop();
			int counter = 0;

			while(true)
			{
				try
				{
				ASTNode* tempushnode = temp->getChild(counter);

				if(tempushnode == 0)
					break;
				nodesStack.push(tempushnode);
				counter++;
				}
				catch (exception ex)
				{
					break;
				}
			}
			//pop and push
		}
	}
}

//RHS for now handles patterns in the form of "a" or _"a"_
bool IEvalQuery::TryMatch(ASTNode* testedNode, string targetVar,vector<string> incCodes, bool isSubsTree)
{
	if(!(testedNode->getType() == ASTNode::Assign))
		throw SPAException("Error, this node not an assignNode");

	if(!testedNode->isHasChildren())
		throw SPAException("Assignment no child error!");

	bool leftTrue = true;

	currentFirstVariableNo = PKB::variables.getVARIndex(targetVar);
	if(!(targetVar.compare("_") == 0) && currentFirstVariableNo != testedNode->getChild(0)->getValue())
		leftTrue = false;
	////////at this point left hand side is ok

	ASTNode* head= testedNode->getChild(1);

	bool rightTrue = false;

	if(incCodes.at(0).compare("_") == 0)
		rightTrue = true;

	if(leftTrue && rightTrue)
		return true;

	int rightInt = PKB::variables.getVARIndex(incCodes.at(0));

	if(!isSubsTree)//if not a subtree, since we only handle 1 variable so right side must be a variable if is true
	{
		if(head->getChild(1)->getChild(1)->getType() != ASTNode::Variable)//right node is not a variable = auto fail
			return false;
		else if(rightInt == head->getChild(1)->getChild(1)->getValue()) //right side value is same as rightint
			return true;
		else
			return false; //if not equal return false
	}
	stack<ASTNode*> nodesStack; //if subtree = true

	/*nodesStack.push(head->getChild(0));

	nodesStack.push(head->getChild(1));
	*/
	nodesStack.push(head);

	while(nodesStack.size() > 0)
	{
		if(nodesStack.top()->getType() == ASTNode::Operator)
		{
			ASTNode* tempnode = nodesStack.top();
			nodesStack.pop();
			nodesStack.push(tempnode->getChild(1));//add right side in

			nodesStack.push(tempnode->getChild(0));//add left side in
		}
		else if(nodesStack.top()->getType() == ASTNode::Variable || nodesStack.top()->getType() == ASTNode::Constant)
		{
			//assume is subtree

			if(rightInt == nodesStack.top()->getValue())
			{
				return true;
			}
			nodesStack.pop();
		}
		else
		{
			throw SPAException("Error! invalid node kind in operator");
		}
	}

	return false;
}