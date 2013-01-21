#pragma once
#include "MultiQueryEval.h"
#include "Helper.h"
#include "PKB.h"
#include "RulesOfEngagement.h"
#include "AnswerTable.h"
#include "ASTStmtNode.h"
#include "ASTStmtLstNode.h"
#include "AssignmentParser.h"

string MultiQueryEval::getToken(const string& query, int& pos)
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";
	pos = query.find_first_of(" ;,<>().", first);
	if (pos == first) {
		pos++;
		return query.substr(first, 1);
	}
	return query.substr(first, pos - first);
}

string MultiQueryEval::getToken2(string query, int& pos)
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";

	stack<char> charstack;
	int tempt = first;
	while(true)
	{
		pos = query.find_first_of("\"()", tempt);
		if(query.at(pos) == '\"')
		{
			
			if(charstack.size() > 0 && charstack.top() == '\"')
				charstack.pop();
			else
				charstack.push('\"');
		}
		else if(query.at(pos) == '(')
		{
			charstack.push('(');
		}
		else if(query.at(pos) == ')')
		{
			if(charstack.size() == 0)
				break;
			else if(charstack.size() > 0 && charstack.top() == '(')
				charstack.pop();
			else
				throw new SPAException("Error in parsing query");
		}
		else
			throw new SPAException("Error in parsing query");
		tempt = pos+1;
	}
	if(charstack.size() != 0 )
		throw new SPAException("Error in parsing query");

	if (pos == first) {
		pos++;
		return query.substr(first, 1);
	}
	return query.substr(first, pos - first);
}

void MultiQueryEval::matchToken(const string& query, int& pos, const string& match)
{
	if (getToken(query, pos) != match)
		throw new SPAException("Error in parsing query");
}

vector<string> MultiQueryEval::evaluateQuery(const string& query)
{
	MultiQueryEval temp(query);
	if (temp.selectBOOLEAN && temp.finalanswer.empty())
		temp.finalanswer.push_back("FALSE");
	return temp.finalanswer;
}

MultiQueryEval::MultiQueryEval(const string& query)
{
	SynonymTable synonymTable;
	DisjointSet disjointSet;
	
	//parse the query statement
	int pos = 0;
	while (true) { //parse synonym declaration
		string token = getToken(query, pos);
		RulesOfEngagement::QueryVar type;
		if (token == "Select")
			break;
		if (RulesOfEngagement::tokenToVar.count(token) == 0)
			throw new SPAException("Error in parsing query - Unrecognised synonym declaration");
		type = RulesOfEngagement::tokenToVar[token];
		
		string next;
		do {
			next = getToken(query, pos);
			synonymTable.insert(next, type);
			disjointSet.makeSet(next);
		} while ((next = getToken(query, pos)) == ",");
		if (next != ";")
			throw new SPAException("Error in parsing query");
	}
	
	//parse selected variables
	string selected = getToken(query, pos);
	vector<string> selects;
	if (selected.at(0) == '<') { //tuple -> multiple selected variables
		selected = getToken(query, pos);
		while (true) {
			synonymTable.setSelected(selected);
			selects.push_back(selected);
			selected = getToken(query, pos);
			if (selected == ",")
				selected = getToken(query, pos);
			else if (selected == ">")
				break;
			else
				throw new SPAException("Error in parsing query");
		}
		selectBOOLEAN = false;
	} else if (selected == "BOOLEAN") {
		selectBOOLEAN = true;
	} else {
		synonymTable.setSelected(selected);
		selects.push_back(selected);
		selectBOOLEAN = false;
	}

	//relationship table
	vector<RulesOfEngagement::QueryRelations> relType;
	vector<string> relFirst;
	vector<string> relSecond;
	vector<int> relClass;
	unordered_map<string, vector<int>> relFirstToIndices;

	//pattern table
	vector<string> patternSynonym;
	vector<RulesOfEngagement::QueryVar> patternType;
	vector<string> patternLHS;
	vector<string> patternRHS;

	//parse relations, conditions and patterns
	int clauseType = -1; //-1 undefined, 0 such that, 1 with, 2 pattern
	int tempVars = 0;
	while (true) {
		string clause = getToken(query, pos);
		if (clause == "") {
			break;
		} else if (clause == "such") {
			if (getToken(query, pos) == "that")
				clauseType = 0;
			else
				throw new SPAException("Error in parsing query");
		} else if (clause == "with") {
			clauseType = 1;
		} else if (clause == "pattern") {
			clauseType = 2;
		} else if (clause != "and" || clauseType == -1)
			throw new SPAException("Error in parsing query");

		switch (clauseType) {
		case 0:
			{
			string relation = getToken(query, pos);
			matchToken(query, pos, "(");
			string firstRel = getToken(query, pos);
			matchToken(query, pos, ",");			
			string secondRel = getToken(query, pos);
			matchToken(query, pos, ")");

			RulesOfEngagement::QueryRelations type = RulesOfEngagement::tokenToRel[relation];

			//count number of synonyms and verify acceptability of argument
			int numOfSynonyms = 0;
			bool firstIsSynonym = false;
			if (firstRel == "_") {
				if (RulesOfEngagement::allowableFirstArgument[type].count(RulesOfEngagement::WildCard) == 0)
					throw new SPAException(type + " not allowed to have \"_\" as its first argument");
			} else if (synonymTable.isInTable(firstRel)) {
				if (RulesOfEngagement::allowableFirstArgument[type].count(synonymTable.getType(firstRel)) == 0)
					throw new SPAException(type + " not allowed to have the type of " + firstRel + " as its first argument");
				numOfSynonyms++;
				firstIsSynonym = true;
			} else if (Helper::isNumber(firstRel)) {
				if (RulesOfEngagement::allowableFirstArgument[type].count(RulesOfEngagement::Integer) == 0)
					throw new SPAException(type + " not allowed to have an integer as its first argument");
			} else if (firstRel.at(0) == '\"' && firstRel.at(firstRel.length() - 1) == '\"') {
				if (RulesOfEngagement::allowableFirstArgument[type].count(RulesOfEngagement::String) == 0)
					throw new SPAException(type + " not allowed to have a string as its first argument");
			} else
				throw new SPAException("Could not parse the first argument");

			if (secondRel == "_") {
				if (RulesOfEngagement::allowableSecondArgument[type].count(RulesOfEngagement::WildCard) == 0)
					throw new SPAException(type + " not allowed to have \"_\" as its second argument");
			} else if (synonymTable.isInTable(secondRel)) {
				if (RulesOfEngagement::allowableSecondArgument[type].count(synonymTable.getType(secondRel)) == 0)
					throw new SPAException(type + " not allowed to have the type of " + firstRel + " as its second argument");
				numOfSynonyms++;
			} else if (Helper::isNumber(secondRel)) {
				if (RulesOfEngagement::allowableSecondArgument[type].count(RulesOfEngagement::Integer) == 0)
					throw new SPAException(type + " not allowed to have an integer as its second argument");
			} else if (secondRel.at(0) == '\"' && secondRel.at(secondRel.length() - 1) == '\"') {
				if (RulesOfEngagement::allowableSecondArgument[type].count(RulesOfEngagement::String) == 0)
					throw new SPAException(type + " not allowed to have a string as its second argument");
			} else
				throw new SPAException("Could not parse the second argument");

			switch (numOfSynonyms) {
			case 0: //handle 0 synonym, 3 cases
				//Case 1: rel( _ , _ ) -> On the spot test if the relationship is ever true
				//Case 2: rel( _ ,"1") -> rel(_,s) with s.stmtNo = 1 -> treat as a condition on s
				//Case 3: rel("1","2") -> On the spot test
				if (firstRel == "_") {
					if (secondRel == "_") { //Case 1: rel(_,_)
						if (RulesOfEngagement::emptyRel[type])
							return;
					} else { //Case 2: rel(_,"1")
						//hardcoding
						string sugar = "t" + Helper::intToString(++tempVars);
						while (synonymTable.isInTable(sugar))
							sugar = "t" + sugar;
						switch (type) {
						case RulesOfEngagement::Modifies:
						case RulesOfEngagement::Uses:
							throw new SPAException("Not allowed to have _ as first argument");
						case RulesOfEngagement::Calls:
						case RulesOfEngagement::CallsStar:
							if (secondRel.at(0) != '\"' || secondRel.at(secondRel.size() - 1) != '\"')
								throw new SPAException("Error in parsing argument");
							synonymTable.insert(sugar, RulesOfEngagement::Procedure);
							synonymTable.setAttribute(sugar, "procName", secondRel.substr(1, secondRel.length() - 2));
							break;
						default:
							if (!Helper::isNumber(secondRel))
								throw new SPAException("Error in parsing argument");
							synonymTable.insert(sugar, RulesOfEngagement::Statement);
							synonymTable.setAttribute(sugar, "stmtNo", secondRel);
						}
						synonymTable.setSecondGeneric(sugar, type, RulesOfEngagement::privilegedFirstArgument[type]);
					}
				} else {
					if (secondRel == "_") { //Case 2: rel("1",_)
						//hardcoding
						string sugar = "t" + Helper::intToString(++tempVars);
						while (synonymTable.isInTable(sugar))
							sugar = "t" + sugar;
						switch (type) {
						case RulesOfEngagement::Modifies:
						case RulesOfEngagement::Uses:
							if (Helper::isNumber(firstRel)) {
								synonymTable.insert(sugar, RulesOfEngagement::Statement);
								synonymTable.setAttribute(sugar, "stmtNo", firstRel);
								if (type == RulesOfEngagement::Modifies)
									type = RulesOfEngagement::ModifiesStmt;
								else
									type = RulesOfEngagement::UsesStmt;
							} else if (firstRel.at(0) != '\"' || secondRel.at(firstRel.size() - 1) != '\"')
								throw new SPAException("Error in parsing argument");
							else {
								synonymTable.insert(sugar, RulesOfEngagement::Procedure);
								synonymTable.setAttribute(sugar, "procName", firstRel.substr(1, firstRel.length() - 2));
								if (type == RulesOfEngagement::Modifies)
									type = RulesOfEngagement::ModifiesProc;
								else
									type = RulesOfEngagement::UsesProc;
							}
							break;
						case RulesOfEngagement::Calls:
						case RulesOfEngagement::CallsStar:
							if (firstRel.at(0) != '\"' || firstRel.at(firstRel.size() - 1) != '\"')
								throw new SPAException("Error in parsing argument");
							synonymTable.insert(sugar, RulesOfEngagement::Procedure);
							synonymTable.setAttribute(sugar, "procName", firstRel.substr(1, firstRel.length() - 2));
							break;
						default:
							if (!Helper::isNumber(firstRel))
								throw new SPAException("Error in parsing argument");
							synonymTable.insert(sugar, RulesOfEngagement::Statement);
							synonymTable.setAttribute(sugar, "stmtNo", firstRel);
						}
						synonymTable.setFirstGeneric(sugar, type, RulesOfEngagement::privilegedSecondArgument[type]);
					} else { //Case 3: rel("1","2") 
						int first = RulesOfEngagement::convertArgumentToInteger(type, true, firstRel);
						int second = RulesOfEngagement::convertArgumentToInteger(type, false, secondRel);
						if (!RulesOfEngagement::getRelation(type)(first, second))
							return;
					}
				}
				break;

			case 1: //handle 1 synonym: put as a condition
				//Case 1: rel(s, _ ) -> put in generic(first)
				//Case 2: rel(s,"1") -> put in specific(first)
				if (firstIsSynonym) {
					if (type == RulesOfEngagement::Modifies) {
						if (synonymTable.getType(firstRel) == RulesOfEngagement::Procedure)
							type = RulesOfEngagement::ModifiesProc;
						else
							type = RulesOfEngagement::ModifiesStmt;
					} else if (type == RulesOfEngagement::Uses) {
						if (synonymTable.getType(firstRel) == RulesOfEngagement::Procedure)
							type = RulesOfEngagement::UsesProc;
						else
							type = RulesOfEngagement::UsesStmt;
					}
					if (secondRel == "_")
						synonymTable.setFirstGeneric(firstRel, type, RulesOfEngagement::privilegedSecondArgument[type]);
					else
						synonymTable.setFirstSpecific(firstRel, type, secondRel);
				} else {
					if (firstRel == "_")
						synonymTable.setSecondGeneric(secondRel, type, RulesOfEngagement::privilegedFirstArgument[type]);
					else {
						if (type == RulesOfEngagement::Modifies) {
							if (Helper::isNumber(firstRel))
								type = RulesOfEngagement::ModifiesStmt;
							else
								type = RulesOfEngagement::ModifiesProc;
						} else if (type == RulesOfEngagement::Uses) {
							if (Helper::isNumber(firstRel))
								type = RulesOfEngagement::UsesStmt;
							else
								type = RulesOfEngagement::UsesProc;
						}
						synonymTable.setSecondSpecific(secondRel, type, firstRel);
					}
				}
				break;

			case 2: //handle both synonyms
				if (synonymTable.getType(firstRel) == RulesOfEngagement::Procedure) {
					if (type == RulesOfEngagement::Modifies)
						type = RulesOfEngagement::ModifiesProc;
					else if (type == RulesOfEngagement::Uses)
						type = RulesOfEngagement::UsesProc;
				} else {
					if (type == RulesOfEngagement::Modifies)
						type = RulesOfEngagement::ModifiesStmt;
					else if (type == RulesOfEngagement::Uses)
						type = RulesOfEngagement::UsesStmt;
				}

				//handle self-reference
				if (firstRel == secondRel) {
					if (RulesOfEngagement::allowableSelfReference.count(type) > 0) {
						synonymTable.setSelfReference(firstRel, type);
						break;
					} else
						return;
				}

				if (relFirstToIndices.count(firstRel) > 0)
					relFirstToIndices[firstRel].push_back(relType.size());
				else {
					vector<int> temp;
					temp.push_back(relType.size());
					relFirstToIndices[firstRel] = temp;
				}
				relType.push_back(type);
				relFirst.push_back(firstRel);
				relSecond.push_back(secondRel);
				relClass.push_back(-1);

				disjointSet.setUnion(firstRel, secondRel);
				break;
			default:
				throw new SPAException("Error in parsing query");
			}
			}
			break;
		case 1:
			{
			string synonym = getToken(query, pos);
			if (!synonymTable.isInTable(synonym))
				throw new SPAException("Error in parsing query");

			matchToken(query, pos, ".");

			string condition = getToken(query, pos);

			matchToken(query, pos, "=");

			string attribute = getToken(query, pos);
			bool passed = synonymTable.setAttribute(synonym, condition, attribute);
			if (!passed)
				return;
			}
			break;
		case 2: //pattern
			{
				string synonym = getToken(query, pos);
				switch (synonymTable.getType(synonym)) {
				case RulesOfEngagement::Assign:
					{
					matchToken(query, pos, "(");
					string modifiesVar = getToken(query, pos);
					matchToken(query, pos, ",");
					string usesVar = getToken2(query, pos);
					matchToken(query, pos, ")");

					//remove white spaces
					usesVar.erase(remove(usesVar.begin(), usesVar.end(), '\t'), usesVar.end());
					usesVar.erase(remove(usesVar.begin(), usesVar.end(), ' '), usesVar.end());

					if (modifiesVar == "_") {
						if (usesVar == "_") {
							if (PKB::assignTable.size() == 0)
								return;
						} else {
							patternSynonym.push_back(synonym);
							patternType.push_back(synonymTable.getType(synonym));
							patternLHS.push_back(modifiesVar);
							patternRHS.push_back(usesVar);
						}
					} else {
						if (usesVar == "_") {
							if (!synonymTable.isInTable(modifiesVar)) {
								string input = modifiesVar;
								modifiesVar = "t" + Helper::intToString(++tempVars);
								while (synonymTable.isInTable(modifiesVar))
									modifiesVar = "t" + modifiesVar;
								synonymTable.insert(modifiesVar, RulesOfEngagement::Variable); //hardcoding here
								synonymTable.setAttribute(modifiesVar, "varName", input.substr(1, input.length() - 2));
							}

							if (relFirstToIndices.count(synonym) > 0)
								relFirstToIndices[synonym].push_back(relType.size());
							else {
								vector<int> temp;
								temp.push_back(relType.size());
								relFirstToIndices[synonym] = temp;
							}
							relType.push_back(RulesOfEngagement::ModifiesStmt);
							relFirst.push_back(synonym);
							relSecond.push_back(modifiesVar);
							relClass.push_back(-1);

							disjointSet.setUnion(synonym, modifiesVar);
						} else {
							if (modifiesVar.at(0) != '\"') {
								if (synonymTable.getType(modifiesVar) != RulesOfEngagement::Variable)
									throw new SPAException("BOOBOOBAABAA");
								disjointSet.setUnion(synonym, modifiesVar);
							}

							patternSynonym.push_back(synonym);
							patternType.push_back(synonymTable.getType(synonym));
							patternLHS.push_back(modifiesVar);
							patternRHS.push_back(usesVar);
						}
					}
					}
					break;
				case RulesOfEngagement::If:
				case RulesOfEngagement::While:
					break;
				default:
					throw new SPAException("Not valid type for pattern");
				}
			}
			break;
		default:
			throw new SPAException("Error in parsing query");
		}
	}

	vector<unordered_set<string>> components = disjointSet.getComponents();
	for (unsigned int classIndex = 0; classIndex < components.size(); classIndex++)
		for (auto it = components[classIndex].begin(); it != components[classIndex].end(); it++)
			synonymTable.putIntoClass(*it, classIndex + 1);
	
	for (auto it = relFirstToIndices.begin(); it != relFirstToIndices.end(); it++)
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			relClass[*it2] = synonymTable.inClass((*it).first);
	
	vector<AnswerTable> tables;
	//could have incorporated in synonym table, but was not because it is implentation dependent
	unordered_map<string, int> inWhichTable;
	
	for (unsigned int rel = 0; rel < relType.size(); rel++) {
		RulesOfEngagement::QueryRelations type = relType[rel];
		string firstRel = relFirst[rel];
		string secondRel = relSecond[rel];

		int matchNumberOfTables = 0;
		if (inWhichTable.count(firstRel) > 0)
			matchNumberOfTables++;
		if (inWhichTable.count(secondRel) > 0)
			matchNumberOfTables++;

		switch (matchNumberOfTables) {
		case 0:
			{
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0)
					return;

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0)
					return;

				firstRelTable.combine(firstRel, secondRelTable,
					secondRel, RulesOfEngagement::getRelation(type));
				if (firstRelTable.getSize() == 0)
					return;

				inWhichTable[firstRel] = tables.size();
				inWhichTable[secondRel] = tables.size();
				tables.push_back(firstRelTable);
			}
			break;
		case 1:
			if (inWhichTable.count(firstRel) > 0) {
				int firstRelIndex = inWhichTable[firstRel];

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0)
					return;

				tables[firstRelIndex].combine(firstRel, secondRelTable,
					secondRel, RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0)
					return;

				inWhichTable[secondRel] = firstRelIndex;
			} else {
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0)
					return;

				int secondRelIndex = inWhichTable[secondRel];
				firstRelTable.combine(firstRel, tables[secondRelIndex],
					secondRel, RulesOfEngagement::getRelation(type));
				if (firstRelTable.getSize() == 0)
					return;

				tables[secondRelIndex] = firstRelTable;
				inWhichTable[firstRel] = secondRelIndex;
			}
			break;
		case 2:
			int firstRelIndex = inWhichTable[firstRel];
			int secondRelIndex = inWhichTable[secondRel];
			if (firstRelIndex == secondRelIndex) {
				tables[firstRelIndex].prune(firstRel, secondRel,
					RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0)
					return;
			} else {
				tables[firstRelIndex].combine(firstRel, tables[secondRelIndex],
					secondRel, RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0)
					return;

				for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
					if ((*it).second == secondRelIndex)
						(*it).second = firstRelIndex;
			}
		}
	}

	//evaluate patterns
	for (unsigned int i = 0; i < patternType.size(); i++) {
		string synonym = patternSynonym[i];
		RulesOfEngagement::QueryVar type = patternType[i];
		string modifiesVar = patternLHS[i];
		string usesVar = patternRHS[i];
		
		RulesOfEngagement::PatternRHSType RHS;
		string RHSVarName;
		if(usesVar.at(0) == '_' && usesVar.at(usesVar.size() - 1) == '_') {
			RHS = RulesOfEngagement::PRSub;
			RHSVarName = usesVar.substr(1, usesVar.length() - 2);
		} else {
			RHS = RulesOfEngagement::PRNoSub;
			RHSVarName = usesVar;
		}
		if (RHSVarName.at(0) != '\"' || RHSVarName.at(RHSVarName.length() - 1) != '\"')
			throw SPAException("Error, Pattern Right Hand Side Invalid");
		RHSVarName = RHSVarName.substr(1, RHSVarName.length() - 2);
		ASTExprNode* RHSexprs = AssignmentParser::processAssignment(MiniTokenizer(RHSVarName));

		if (synonymTable.isInTable(modifiesVar)) {
			int matchNumberOfTables = 0;
			if (inWhichTable.count(synonym) > 0)
				matchNumberOfTables++;
			if (inWhichTable.count(modifiesVar) > 0)
				matchNumberOfTables++;

			switch (matchNumberOfTables) {
			case 0:
				{
					AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
					if (firstRelTable.getSize() == 0)
						return;

					AnswerTable secondRelTable = AnswerTable(synonymTable, modifiesVar);
					if (secondRelTable.getSize() == 0)
						return;

					firstRelTable.cartesian(secondRelTable);
					firstRelTable.patternPrune(synonym, true, 1, RHS, RHSVarName, RHSexprs);
					if (firstRelTable.getSize() == 0)
						return;

					inWhichTable[synonym] = tables.size();
					inWhichTable[modifiesVar] = tables.size();
					tables.push_back(firstRelTable);
				}
				break;
			case 1:
				if (inWhichTable.count(synonym) > 0) {
					int firstRelIndex = inWhichTable[synonym];

					AnswerTable secondRelTable = AnswerTable(synonymTable, modifiesVar);
					if (secondRelTable.getSize() == 0)
						return;

					tables[firstRelIndex].cartesian(secondRelTable);
					tables[firstRelIndex].patternPrune(synonym, true,
						tables[firstRelIndex].synonymPosition[modifiesVar], RHS, RHSVarName, RHSexprs);
					if (tables[firstRelIndex].getSize() == 0)
						return;

					inWhichTable[modifiesVar] = firstRelIndex;
				} else {
					AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
					if (firstRelTable.getSize() == 0)
						return;

					int secondRelIndex = inWhichTable[modifiesVar];
					tables[secondRelIndex].cartesian(firstRelTable);
					tables[secondRelIndex].patternPrune(synonym, true,
						tables[secondRelIndex].synonymPosition[modifiesVar], RHS, RHSVarName, RHSexprs);
					if (tables[secondRelIndex].getSize() == 0)
						return;

					inWhichTable[synonym] = secondRelIndex;
				}
				break;
			case 2:
				int firstRelIndex = inWhichTable[synonym];
				int secondRelIndex = inWhichTable[modifiesVar];
				if (firstRelIndex != secondRelIndex) {
					tables[firstRelIndex].cartesian(tables[secondRelIndex]);

					for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
						if ((*it).second == secondRelIndex)
							(*it).second = firstRelIndex;
				}
				tables[firstRelIndex].patternPrune(synonym, true,
						tables[firstRelIndex].synonymPosition[modifiesVar], RHS, RHSVarName, RHSexprs);
				if (tables[firstRelIndex].getSize() == 0)
					return;
			}
		} else { //modifiesVar is not a synonym
			if (inWhichTable.count(synonym) == 0) {
				AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
				if (firstRelTable.getSize() == 0)
					return;

				inWhichTable[synonym] = tables.size();
				tables.push_back(firstRelTable);
			}
			int synonymIndex = inWhichTable[synonym];

			int modifiesIndex;
			if (modifiesVar.at(0) == '\"' && modifiesVar.at(modifiesVar.size() - 1) == '\"') {
				modifiesIndex = PKB::variables.getVARIndex(modifiesVar.substr(1, modifiesVar.size() - 2));
			} else if (modifiesVar == "_") {
				modifiesIndex = -1;
			} else
				throw SPAException("Error, Pattern Left Hand Side Invalid");

			tables[synonymIndex].patternPrune(synonym, false, modifiesIndex, RHS, RHSVarName, RHSexprs);
			if (tables[synonymIndex].getSize() == 0)
				return;
		}
	}

	//examine table sizes
	vector<string>& synonyms = synonymTable.getAllNames();
	for (auto it = synonyms.begin(); it != synonyms.end(); it++)
		if (inWhichTable.count(*it) == 0) {
			AnswerTable table = AnswerTable(synonymTable, *it);
			if (table.getSize() == 0)
				return;
			if (synonymTable.isSelected(*it)) {
				inWhichTable[*it] = tables.size();
				tables.push_back(table);
			}
		}

	if (selectBOOLEAN) {
		finalanswer.push_back("TRUE");
		return;
	}

	//project out selected variables
	vector<string> selecteds = synonymTable.getAllSelected();
	vector<vector<string>> projections(components.size());
	int firstNonEmpty = components.size();
	for (auto it = selecteds.begin(); it != selecteds.end(); it++) {
		if (inWhichTable.count(*it) == 0) {
			AnswerTable table = AnswerTable(synonymTable, *it);
			inWhichTable[*it] = tables.size();
			tables.push_back(table);
		}
		if (inWhichTable[*it] < firstNonEmpty)
			firstNonEmpty = inWhichTable[*it];
		projections[inWhichTable[*it]].push_back(*it);
	}

	AnswerTable concatenated = tables[firstNonEmpty].project(projections[firstNonEmpty]);
	for (unsigned int i = firstNonEmpty + 1; i < components.size(); i++)
		if (!projections[i].empty())
			concatenated.cartesian(tables[i].project(projections[i]));

	vector<int> orderOfSelection;
	for (auto it = selects.begin(); it != selects.end(); it++)
		orderOfSelection.push_back(concatenated.synonymPosition[*it]);
	
	//convert vector of vector of int to vector of string
	vector<string> header = concatenated.getHeader();
	for (unsigned int i = 0; i < concatenated.getSize(); i++) {
		vector<int> row = concatenated.getRow(i);
		string answer;
		for (auto it = orderOfSelection.begin(); it != orderOfSelection.end(); it++) {
			RulesOfEngagement::QueryVar type = synonymTable.getType(header[*it]);
			if (type == RulesOfEngagement::Procedure)
				answer += PKB::procedures.getPROCName(row[*it]);
			else if (type == RulesOfEngagement::Variable)
				answer += PKB::variables.getVARName(row[*it]);
			else
				answer += Helper::intToString(row[*it]);
			if (it + 1 != orderOfSelection.end())
				answer += " ";
		}
		finalanswer.push_back(answer);
	}
}

////mini tokenizer
vector<string> MultiQueryEval::MiniTokenizer(string line)
{
	/*string tempstr = " ";
	line = tempstr.append(line);*/
	vector<string> list;
	string delimiter = " -+*;(){}=";//delimiters
	int position = 0;//starting position
	int startindex = -1;
	int endindex = -1;

	
	do//loop thru the string
	{
		
		startindex = line.find_first_not_of(delimiter,position);

		

		if(endindex != -1 && endindex<line.size())
			{
				string tempstr1; //temp str to store subset of currently working substring
				if(startindex == -1)
				{
					tempstr1 = line.substr(endindex,line.size() - endindex);
				}
				else
				{
					tempstr1 = line.substr(endindex,startindex - endindex);
				}
				for(unsigned int i=0;i<tempstr1.size();)
				{
					string tempstr2 = tempstr1.substr(0,1);
					tempstr1 = tempstr1.substr(1,tempstr1.size()-1);
					
					if(tempstr2 != " " && tempstr2 != "")
					list.push_back(tempstr2);
					//	AddToList(list,tempstr2);
						//AddTables(list,tempstr2);
						//list.push_back(tempstr2);
					
				}
			} else if(endindex == -1)
			{
				if(line.substr(0,startindex) != "")
				list.push_back(line.substr(0,startindex));
			}

			endindex = line.find_first_of(delimiter,startindex);

			position = endindex;
		
			if(startindex != -1 || line.size() == 1)
			{
				string tempstr;
				if(line.size() == 1)
					tempstr= line;
				else
					tempstr= line.substr(startindex,endindex-startindex);

				if(tempstr != " " && tempstr != "")
				list.push_back(tempstr);
					//AddToList(list,tempstr);
					//AddTables(list,tempstr);
					//list.push_back(tempstr);			
				
			}

	}while(startindex != -1 && position < line.size() && endindex != -1);
	

	//house keeping
	/*if(Parser::tokenized_codes.size() > 1)
	{
		vector<string> temp_vec = Parser::tokenized_codes.at(0);
		temp_vec.insert(temp_vec.end(), Parser::tokenized_codes.at(1).begin(),Parser::tokenized_codes.at(1).end());
		Parser::tokenized_codes.erase(Parser::tokenized_codes.begin());
		Parser::tokenized_codes.at(0) = temp_vec;
	}*/

	//if(list.size() > 0)
	//Parser::tokenized_codes.push_back(list);

	//vector<string> tokens;
	return list;
}