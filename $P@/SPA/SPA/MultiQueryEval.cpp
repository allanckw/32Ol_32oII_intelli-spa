#pragma once
#include "MultiQueryEval.h"
#include "Helper.h"
#include "PKB.h"
#include "RulesOfEngagement.h"
#include "AnswerTable.h"
#include "ASTStmtNode.h"
#include "ASTStmtLstNode.h"
#include "AssignmentParser.h"

//#include "../AutoTester/source/AbstractWrapper.h"

//string MultiQueryEval::getToken(const string& query, int& pos)
string MultiQueryEval::getToken(string query, int& pos)
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

void MultiQueryEval::matchToken(string query, int& pos, const string& match)
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

MultiQueryEval::MultiQueryEval(string query)
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

	//{synonym, modifiesIsASynonym, modifiesVar, usesVar>
	vector<tuple<string, bool, string, string>> patterns;

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
				if (synonymTable.getType(synonym) == RulesOfEngagement::Statement)
					synonymTable.changeType(synonym, RulesOfEngagement::Assign);
				else if (synonymTable.getType(synonym) != RulesOfEngagement::Assign)
					throw new SPAException("Not valid type for pattern");

				matchToken(query, pos, "(");

				string modifiesVar = getToken(query, pos);
				
				matchToken(query, pos, ",");

				vector<string> matcher;
				matcher.push_back("\")");
				matcher.push_back("_)");
				//matcher.push_back("\"_)");
				//string usesVar = getToken(query, pos,  matcher, 1);//getToken(query, pos);
				string usesVar = getToken2(query, pos);
				//nick

				//remove white spaces
				usesVar.erase(remove(usesVar.begin(), usesVar.end(), '\t'), usesVar.end());
				usesVar.erase(remove(usesVar.begin(), usesVar.end(), ' '), usesVar.end());

				
				//
				/*
				if rhs not _ then just use use table
				*/

				EvaluatePattern((ASTNode::Assign), modifiesVar,usesVar);



				if (modifiesVar == "_") {
					if (usesVar != "_") {
						string input = usesVar;
						usesVar = "t" + Helper::intToString(++tempVars);
						while (synonymTable.isInTable(usesVar))
							usesVar = "t" + usesVar;
						synonymTable.insert(usesVar, RulesOfEngagement::Variable);
						synonymTable.setAttribute(usesVar, "varName", input.substr(2, input.length() - 4));
				
						relFirstToIndices[synonym].push_back(relType.size());
						relType.push_back(RulesOfEngagement::UsesStmt); //TODO: change to patternUses
						relFirst.push_back(synonym);
						relSecond.push_back(usesVar);
						relClass.push_back(-1);
						disjointSet.setUnion(synonym, usesVar);
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
						if (synonymTable.isInTable(modifiesVar)) {
							patterns.push_back(make_tuple(synonym, true, modifiesVar,
							usesVar.substr(2, usesVar.length() - 4)));
							disjointSet.setUnion(synonym, modifiesVar);
						} else
							patterns.push_back(make_tuple(synonym, false,
							modifiesVar.substr(1, modifiesVar.length() - 2),
							usesVar.substr(2, usesVar.length() - 4)));
					}
				}

				matchToken(query, pos, ")");
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
		int classIndex = relClass[rel];

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
		/*if (AbstractWrapper::GlobalStop)
			return;*/
	}

	//evaluate patterns
	for (auto it = patterns.begin(); it != patterns.end(); it++) {
		string synonym = get<0>(*it);
		bool modifiesIsSynonym = get<1>(*it);
		string modifiesVar = get<2>(*it);
		string usesVar = get<3>(*it);

		if (modifiesIsSynonym) {
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
					firstRelTable.patternPrune(synonym, true, 1, usesVar);
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
						tables[firstRelIndex].synonymPosition[modifiesVar], usesVar);
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
						tables[secondRelIndex].synonymPosition[modifiesVar], usesVar);
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
						tables[firstRelIndex].synonymPosition[modifiesVar], usesVar);
				if (tables[firstRelIndex].getSize() == 0)
					return;
			}
		} else {
			if (inWhichTable.count(synonym) == 0) {
				AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
				if (firstRelTable.getSize() == 0)
					return;

				inWhichTable[synonym] = tables.size();
				tables.push_back(firstRelTable);
			}
			int synonymIndex = inWhichTable[synonym];
			int modifiesIndex = PKB::variables.getVARIndex(modifiesVar);

			tables[synonymIndex].patternPrune(synonym, false, modifiesIndex, usesVar);
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


void MultiQueryEval::EvaluatePattern(ASTNode::NodeType type, string PatternLHS, string PatternRHS)
{
	
	
	PattenLHSType LHS;
	string LHSVarName;
	if (PatternLHS.at(0) == '\"' && PatternLHS.at(PatternLHS.size() - 1) == '\"')
	{
		LHS = PLStringVariable;
		if (PatternLHS.size() == 3)
			LHSVarName = Helper::charToString(PatternLHS.at(1));
		else
			LHSVarName = PatternLHS.substr(1, PatternLHS.size() - 2);
	}
	else if(PatternLHS.compare("_") == 0)
		LHS = PLWildcard;
	else
		throw SPAException("Error, Pattern Left Hand Side Invalid");

	//vecMatch = QueryParser::tokenize(currentSecondVariableName);
	/*typedef enum enumPatternLHSType { 
		PLWildcard, PLStringVariable
	} PattenLHSType;
	typedef enum enumPatternRHSType { 
		PRWildcard, PRSub, PRNoSub
	} PattenRHSType;*/

	PattenRHSType RHS;
	string RHSVarName;

	if(PatternRHS.at(0) == '_' && PatternRHS.size()== 1)
	{
		RHS = PRWildcard;
		
	}
	else{

		if(PatternRHS.at(0) == '_' && PatternRHS.at(PatternRHS.size() - 1) == '_' && PatternRHS.size() > 2)
		{
			RHS = PRSub;
			
			RHSVarName = PatternRHS.substr(1,PatternRHS.length()-2);
			
		}
		else
		{
			RHSVarName = PatternRHS;
			RHS = PRNoSub;
		}

		RHSVarName.erase(remove(RHSVarName.begin(), RHSVarName.end(), '\"'), RHSVarName.end());//get rid of the start " and end "

	}

	stack<ASTNode*> nodesStack;
	PKB::rootNode;
	PKB::variables;
	PKB::procedures;

	nodesStack.push(PKB::rootNode);

	vector<int> stmtLineTrue;

	ASTExprNode* RHSexprs;
	int LHSvarnum = -1;

	if(RHS != PRWildcard)
	{
		vector<string> tokenized = MiniTokenizer(RHSVarName);;
		//tokenized.push_back(";");
		RHSexprs = AssignmentParser::processAssignment(tokenized);
		
	}

	if(LHS != PLWildcard)
	{
		LHSvarnum = PKB::variables.getVARIndex(LHSVarName);
	}

	vector<string> ans;

	//tokenize rhs first
	while(nodesStack.size() > 0)
	{
		int tempchk = nodesStack.size();
		if(nodesStack.top()->getType() == type)
		{
			ASTNode* temp = nodesStack.top();

			ASTStmtNode* tempnode = (ASTStmtNode*)temp;


			if(tempnode->getType() == ASTNode::NodeType::If ||tempnode->getType() == ASTNode::NodeType::While || TryMatch(temp,LHS,RHS,LHSvarnum,RHSexprs))//shld just throw LHSVarName, RHSVarName after rhs have been tokenize as they are emtpy if wildcard
			{
					ASTStmtNode* tempnode = (ASTStmtNode*)temp;
					string tempstr = Helper::intToString(tempnode->getStmtNumber());//ans
					ans.push_back(tempstr);
							
					
			}
			//QueryParser QP;
			

			//need to tokenize first
		//	while(exIdx<RHSVarName.size())
		//	{
		//		/*rightExpression.push_back(RHSVarName.at(exIdx));
		//
		//		if(RHSVarName.at(exIdx)==";")
		//			break;
		//		exIdx++;*/
		//	}

			
			//tokenize rhs if not wildcard
			 //isSub is always true for now because we only need to handle _"hi"_ case
			/*typedef enum enumPatternLHSType { 
		PLWildcard, PLStringVariable
	} PattenLHSType;

	PattenLHSType LHS;*/
			/*if (LHS == PLWildcard)
			{*/
				
				
			//else
			//{
			//	if(TryMatch(temp,firstVarName,vecMatch,sub))
			//	{
			//		ASTStmtNode* tempnode = (ASTStmtNode*)temp;
			//		firstVariableAnswer.push_back(tempnode->getStmtNumber());//ans
			//	}

			//}
			nodesStack.pop();
			//remember to pop	
		}
		else
		{
			ASTNode* temp = nodesStack.top();
			nodesStack.pop();
			int counter = 0;

			ASTStmtLstNode* t =	(ASTStmtLstNode*)temp;

			for(int j=0;j<t->getSize();j++)
			{
				ASTNode* tempushnode = temp->getChild(j);
				nodesStack.push(tempushnode);
			}

			/*while(true)
			{
				try
				{
				ASTStmtLstNode* t =	(ASTStmtLstNode*)temp;
				if()
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
			}*/
			//pop and push
		}
	}

	ans;
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

//RHS for now handles patterns in the form of "a" or _"a"_
bool MultiQueryEval::TryMatch(ASTNode* testedNode,PattenLHSType LHS, PattenRHSType RHS,  int LHSVarNum, ASTExprNode* RHSexpr)
{
	if(LHS == PLWildcard && RHS == PRWildcard)
		return true;

	if(LHS != PLWildcard && testedNode->getChild(0)->getValue() != LHSVarNum)
		return false;

	//at this pt LHS sure pass
	/////////////////////////////////////

	//just need verify rhs which at this pt no longer a wildcard
	///////////////////////////////////////



	ASTNode* head= testedNode->getChild(1);

	
	//int rightInt = PKB::variables.getVARIndex(incCodes.at(0));

	//if(!isSubsTree)//if not a subtree, since we only handle 1 variable so right side must be a variable if is true
	//{
	//	if(head->getType() != ASTNode::Variable)//right node is not a variable = auto fail
	//		return false;
	//	else if(rightInt == head->getValue()) //right side value is same as rightint
	//		return true;
	//	else
	//		return false; //if not equal return false
	//}
	stack<ASTNode*> nodesStack; 

	//nodesStack.push(head->getChild(0));

	//nodesStack.push(head->getChild(1));
	
	nodesStack.push(head);
	bool result = false;//default
	ASTNode* pattern = RHSexpr;

	bool isSub;
		if(RHS == PRSub)
			isSub = true;
		else
			isSub = false;

	if(isSub == false)
		result = MatcherTree(head,pattern);
	else
	{
		while(nodesStack.size() > 0)
		{

			ASTNode* tempnode = nodesStack.top();
			nodesStack.pop();
							/*
							ASTNode* temp = nodesStack.top();
				nodesStack.pop();
				int counter = 0;

				ASTStmtLstNode* t =	(ASTStmtLstNode*)temp;

				for(int j=0;j<t->getSize();j++)
				{
					ASTNode* tempushnode = temp->getChild(j);
					nodesStack.push(tempushnode);
				}
							*/
			//
				//check here
			//if()
			//ASTNode* tempnode
		
			//RHSexpr->

		

			if(result == false && MatcherTree(tempnode,pattern))//,isSub))
			{
				result = true; 
			}
			//ASTExprNode* RHSexpr
			//
			//

			if(tempnode->getType() == ASTNode::Operator)
			{
			
				nodesStack.push(tempnode->getChild(1));//add right side in

				nodesStack.push(tempnode->getChild(0));//add left side in
			}
			//else if(nodesStack.top()->getType() == ASTNode::Variable || nodesStack.top()->getType() == ASTNode::Constant)
			//{
			//	//assume is subtree

			//	if(nodesStack.top()->getType() == ASTNode::Variable && rightInt == nodesStack.top()->getValue())
			//	{
			//		return true;
			//	}
			//	nodesStack.pop();
			//}
			//else
			//{
			//	throw SPAException("Error! invalid node kind in operator");
			//}
		}
	}
	return result;
}

bool MultiQueryEval::MatcherTree(ASTNode* Original,ASTNode* Pattern)//, bool isSub)
{
	bool NodeSame=false;
	bool NodeLeftSame = false;
	bool NodeRightSame = false;

	if(Original->getType() == Pattern->getType() && Original->getValue() == Pattern->getValue())
		NodeSame = true;
	else 
		return false;

	if(Original->getType() == ASTNode::NodeType::Constant)
		return true;
	else if(Original->getType() == ASTNode::NodeType::Variable)
		return true;
	else if(Original->getType() == ASTNode::NodeType::Operator)
	{
		NodeLeftSame = MatcherTree(Original->getChild(0),Pattern->getChild(0));//, isSub);
		NodeRightSame = MatcherTree(Original->getChild(1),Pattern->getChild(1));//, isSub);
	}

	return (NodeSame && NodeLeftSame && NodeRightSame);
}