#pragma once
#include "MultiQueryEval.h"
#include "Helper.h"
#include "QueryEnums.h"
#include "PKB.h"
#include "RulesOfEngagement.h"
#include "AnswerTable.h"
//#include "../AutoTester/source/AbstractWrapper.h"

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

void MultiQueryEval::matchToken(string query, int& pos, string match)
{
	if (getToken(query, pos) != match)
		throw new SPAException("Error in parsing query");
}


vector<string> MultiQueryEval::evaluateQuery(string query)
{
	MultiQueryEval temp(query);
	if (temp.selectBOOLEAN && temp.finalanswer.empty())
		temp.finalanswer.push_back("FALSE");
	return temp.finalanswer;
}

MultiQueryEval::MultiQueryEval(string query)
{
	//parse the query statement
	int pos = 0;
	while (true) { //parse synonym declaration
		string token = getToken(query, pos);
		RulesOfEngagement::QueryVar type;
		if (token == "Select")
			break;
		if (RulesOfEngagement::tokenToType.count(token) == 0)
			throw new SPAException("Error in parsing query - Unrecognised synonym declaration");
		type = RulesOfEngagement::tokenToType[token];
		
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
	if (selected.at(0) == '<') { //tuple -> multiple selected variables
		selected = getToken(query, pos);
		while (true) {
			synonymTable.setSelected(selected);
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
		selectBOOLEAN = false;
	}

	//relationship table
	vector<RulesOfEngagement::QueryReladition> relType;
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
			RulesOfEngagement::QueryReladition type;
			if (relation == "Calls")
				type = RulesOfEngagement::Calls;
			else if (relation == "Calls*")
				type = RulesOfEngagement::CallsStar;
			else if (relation == "Modifies")
				type = RulesOfEngagement::Modifies;
			else if (relation == "Uses")
				type = RulesOfEngagement::Uses;
			else if (relation == "Parent")
				type = RulesOfEngagement::Parent;
			else if (relation == "Parent*")
				type = RulesOfEngagement::ParentStar;
			else if (relation == "Follows")
				type = RulesOfEngagement::Follows;
			else if (relation == "Follows*")
				type = RulesOfEngagement::FollowsStar;
			else if (relation == "Next")
				type = RulesOfEngagement::Next;
			else if (relation == "Next*")
				type = RulesOfEngagement::NextStar;
			else if (relation == "Affects")
				type = RulesOfEngagement::Affects;
			else if (relation == "Affects*")
				type = RulesOfEngagement::AffectsStar;
			else
				throw new SPAException("Error in parsing query");

			matchToken(query, pos, "(");

			string firstRel = getToken(query, pos);
			if (firstRel == "_") {
				if (RulesOfEngagement::allowableFirstArgument[type].count(RulesOfEngagement::WildCard) == 0)
					throw new SPAException(type + " not allowed to have \"_\" as its first argument");
				firstRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(firstRel))
					firstRel = "t" + firstRel;
				synonymTable.insert(firstRel, RulesOfEngagement::Statement);
			} else if (synonymTable.isInTable(firstRel)) {
				if (RulesOfEngagement::allowableFirstArgument[type].count(synonymTable.getType(firstRel)) == 0)
					throw new SPAException(type + " not allowed to have the type of " + firstRel + " as its first argument");

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
			} else {
				//do something smart(er)
				string input = firstRel;
				firstRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(firstRel))
					firstRel = "t" + firstRel;
				if ((type == RulesOfEngagement::Modifies || type == RulesOfEngagement::Uses)
					&& (!Helper::isNumber(input))) { //then "must" be procedure (otherwise is illegal)
					synonymTable.insert(firstRel, RulesOfEngagement::Procedure);
					synonymTable.setAttribute(firstRel, "procName", input.substr(1, input.length() - 2));
					if (type == RulesOfEngagement::Modifies)
						type = RulesOfEngagement::ModifiesProc;
					else
						type = RulesOfEngagement::UsesProc;
				} else {
					synonymTable.insert(firstRel, RulesOfEngagement::Statement);
					synonymTable.setAttribute(firstRel, "stmtNo", input);
					if (type == RulesOfEngagement::Modifies)
						type = RulesOfEngagement::ModifiesStmt;
					else if (type == RulesOfEngagement::Uses)
						type = RulesOfEngagement::UsesStmt;
				}
			}

			matchToken(query, pos, ",");
			
			string secondRel = getToken(query, pos);
			if (secondRel == "_") {
				if (RulesOfEngagement::allowableSecondArgument[type].count(RulesOfEngagement::WildCard) == 0)
					throw new SPAException(type + " not allowed to have \"_\" as its second argument");
				secondRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(secondRel))
					secondRel = "t" + secondRel;
				if (type == RulesOfEngagement::ModifiesProc || type == RulesOfEngagement::ModifiesStmt ||
					type == RulesOfEngagement::UsesProc || type == RulesOfEngagement::UsesStmt)
					synonymTable.insert(secondRel, RulesOfEngagement::Variable);
				else
					synonymTable.insert(secondRel, RulesOfEngagement::Statement);
			} else if (synonymTable.isInTable(secondRel)) {
				if (RulesOfEngagement::allowableSecondArgument[type].count(synonymTable.getType(secondRel)) == 0)
					throw new SPAException(type + " not allowed to have the type of " + secondRel + " as its second argument");
			} else {
				//do something smart
				string input = secondRel;
				secondRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(secondRel))
					secondRel = "t" + secondRel;
				if (type == RulesOfEngagement::ModifiesProc || type == RulesOfEngagement::ModifiesStmt ||
					type == RulesOfEngagement::UsesProc || type == RulesOfEngagement::UsesStmt) {
						synonymTable.insert(secondRel, RulesOfEngagement::Variable); //hardcoding here
						synonymTable.setAttribute(secondRel, "varName", input.substr(1, input.length() - 2));
				} else /*(type == RulesOfEngagement::Follows || type == RulesOfEngagement::FollowsStar //hardcoding here
					|| type == RulesOfEngagement::Parent || type == RulesOfEngagement::ParentStar)*/ {
					synonymTable.insert(secondRel, RulesOfEngagement::Statement);
					synonymTable.setAttribute(secondRel, "stmtNo", input);
				}
			}

			matchToken(query, pos, ")");

			//handle self-reference
			if (firstRel == secondRel) {
				synonymTable.setSelfReference(firstRel, type);
				break;
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
			}
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
			break;
			}
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

				string usesVar = getToken(query, pos);

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
	
	for (unsigned int rel = 0; rel < relType.size(); rel++) {
		RulesOfEngagement::QueryReladition type = relType[rel];
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
	
	//convert vector of vector of int to vector of string
	vector<string> header = concatenated.getHeader();
	for (unsigned int i = 0; i < concatenated.getSize(); i++) {
		vector<int> row = concatenated.getRow(i);
		string answer;
		for (unsigned int j = 0; j < header.size(); j++) {
			RulesOfEngagement::QueryVar type = synonymTable.getType(header[j]);
			if (type == RulesOfEngagement::Procedure)
				answer += PKB::procedures.getPROCName(row[j]);
			else if (type == RulesOfEngagement::Variable)
				answer += PKB::variables.getVARName(row[j]);
			else
				answer += Helper::intToString(row[j]);
			if (j + 1 < header.size())
				answer += " ";
		}
		finalanswer.push_back(answer);
	}
}