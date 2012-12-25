#pragma once
#include "MultiQueryEval.h"
#include "Helper.h"
#include "QueryEnums.h"
#include "PKB.h"
#include "RulesOfEngagement.h"
#include "AnswerTable.h"
//#include "../AutoTester/source/AbstractWrapper.h"

string MultiQueryEval::getToken(string query, int& pos) {
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


vector<string> MultiQueryEval::evaluateQuery(string query)
{
	return MultiQueryEval(query).finalanswer;
}

MultiQueryEval::MultiQueryEval(string query)
{
	//parse the query statement
	int pos = 0;
	while (true) { //parse synonym declaration
		string token = getToken(query, pos);
		QueryEnums::QueryVar type;
		if (token == "Select")
			break;
		if (token == "procedure")
			type = QueryEnums::Procedure;
		else if (token == "stmt")
			type = QueryEnums::Stmt;
		else if (token == "prog_line")
			type = QueryEnums::Stmt; //Are you sure?
		else if (token == "assign")
			type = QueryEnums::Assign;
		else if (token == "constant")
			type = QueryEnums::Constant;
		else if (token == "while")
			type = QueryEnums::While;
		else if (token == "variable")
			type = QueryEnums::Variable;
		//else if (token == "stmtLst")
			//type = QueryEnums::Stmt; No idea what is this
		else if (token == "call")
			type = QueryEnums::Call;
		else if (token == "if")
			type = QueryEnums::If;
		else
			throw new SPAException("Error in parsing query");
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
	vector<QueryEnums::QueryReladition> relType;
	vector<string> relFirst;
	vector<string> relSecond;
	vector<int> relClass;
	unordered_map<string, vector<int>> relFirstToIndices;

	//parse relations, conditions and pattern
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
			QueryEnums::QueryReladition type;
			if (relation == "Calls")
				type = QueryEnums::Calls;
			else if (relation == "Calls*")
				type = QueryEnums::CallsStar;
			else if (relation == "Modifies")
				type = QueryEnums::Modifies;
			else if (relation == "Uses")
				type = QueryEnums::Uses;
			else if (relation == "Parent")
				type = QueryEnums::Parent;
			else if (relation == "Parent*")
				type = QueryEnums::ParentStar;
			else if (relation == "Follows")
				type = QueryEnums::Follows;
			else if (relation == "Follows*")
				type = QueryEnums::FollowsStar;
			else if (relation == "Next")
				type = QueryEnums::Next;
			else if (relation == "Next*")
				type = QueryEnums::NextStar;
			else if (relation == "Affects")
				type = QueryEnums::Affects;
			else if (relation == "Affects*")
				type = QueryEnums::AffectsStar;
			else
				throw new SPAException("Error in parsing query");

			string openBracket = getToken(query, pos);
			if (openBracket != "(")
				throw new SPAException("Error in parsing query");

			string firstRel = getToken(query, pos);
			if (firstRel == "_") {
				if (type == QueryEnums::Modifies || type == QueryEnums::Uses)
					return;
				firstRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(firstRel))
					firstRel = "t" + firstRel;
				synonymTable.insert(firstRel, QueryEnums::Stmt);
			} else if (!synonymTable.isInTable(firstRel)) {
				//do something smart(er)
				string input = firstRel;
				firstRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(firstRel))
					firstRel = "t" + firstRel;
				/*if (type == QueryEnums::Follows || type == QueryEnums::FollowsStar //hardcoding here
					|| type == QueryEnums::Parent || type == QueryEnums::ParentStar) {*/
				if ((type == QueryEnums::Modifies || type == QueryEnums::Uses) && (!Helper::isNumber(input))) { //then "must" be procedure (if not illegal)
					synonymTable.insert(firstRel, QueryEnums::Procedure);
					synonymTable.setProcName(firstRel, input.substr(1, input.length() - 2));
					if (type == QueryEnums::Modifies)
						type = QueryEnums::ModifiesProc;
					else
						type = QueryEnums::UsesProc;
				} else {
					synonymTable.insert(firstRel, QueryEnums::Stmt);
					synonymTable.setStmtNo(firstRel, Helper::stringToInt(input));
					if (type == QueryEnums::Modifies)
						type = QueryEnums::ModifiesStmt;
					else if (type == QueryEnums::Uses)
						type = QueryEnums::UsesStmt;
				}
			} else if (synonymTable.getType(firstRel) == QueryEnums::Procedure) {
				if (type == QueryEnums::Modifies)
					type = QueryEnums::ModifiesProc;
				else if (type == QueryEnums::Uses)
					type = QueryEnums::UsesProc;
			} else {
				if (type == QueryEnums::Modifies)
					type = QueryEnums::ModifiesStmt;
				else if (type == QueryEnums::Uses)
					type = QueryEnums::UsesStmt;
			}

			string comma = getToken(query, pos);
			if (comma != ",")
				throw new SPAException("Error in parsing query");
			
			string secondRel = getToken(query, pos);
			if (secondRel == "_") {
				secondRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(secondRel))
					secondRel = "t" + secondRel;
				if (type == QueryEnums::ModifiesProc || type == QueryEnums::ModifiesStmt ||
					type == QueryEnums::UsesProc || type == QueryEnums::UsesStmt)
					synonymTable.insert(secondRel, QueryEnums::Variable);
				else
					synonymTable.insert(secondRel, QueryEnums::Stmt);
			} else if (!synonymTable.isInTable(secondRel)) {
				//do something smart
				string input = secondRel;
				secondRel = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(secondRel))
					secondRel = "t" + secondRel;
				if (type == QueryEnums::ModifiesProc || type == QueryEnums::ModifiesStmt ||
					type == QueryEnums::UsesProc || type == QueryEnums::UsesStmt) {
						synonymTable.insert(secondRel, QueryEnums::Variable); //hardcoding here
						synonymTable.setVarName(secondRel, input.substr(1, input.length() - 2));
				} else /*(type == QueryEnums::Follows || type == QueryEnums::FollowsStar //hardcoding here
					|| type == QueryEnums::Parent || type == QueryEnums::ParentStar)*/ {
					synonymTable.insert(secondRel, QueryEnums::Stmt);
					synonymTable.setStmtNo(secondRel, Helper::stringToInt(input));
				}
			}

			string closeBracket = getToken(query, pos);
			if (closeBracket != ")")
				throw new SPAException("Error in parsing query");

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

			string dot = getToken(query, pos);
			if (dot != ".")
				throw new SPAException("Error in parsing query");

			string attribute = getToken(query, pos);

			string equals = getToken(query, pos);
			if (dot != "=")
				throw new SPAException("Error in parsing query");

			string condition = getToken(query, pos);
			bool passed;
			if (attribute == "procName")
				passed = synonymTable.setProcName(synonym, condition);
			else if (attribute == "varName")
				passed = synonymTable.setVarName(synonym, condition);
			else if (attribute == "value")
				passed = synonymTable.setValue(synonym, Helper::stringToInt(condition));
			else if (attribute == "stmt#")
				passed = synonymTable.setStmtNo(synonym, Helper::stringToInt(condition));
			else if (attribute == "prog_line#")
				passed = synonymTable.setStmtNo(synonym, Helper::stringToInt(condition)); //sure boh?
			else
				throw new SPAException("Error in parsing query");
			if (!passed)
				return;
			break;
			}
		case 2: //pattern
			//don't know, don't care
			break;
		default:
			throw new SPAException("Error in parsing query");
		}
	}

	vector<unordered_set<string>> components = disjointSet.getComponents();
	int classIndex = 1;
	for (unsigned int i = 0; i < components.size(); i++)
		for (auto it = components[i].begin(); it != components[i].end(); it++)
			synonymTable.putIntoClass(*it, i + 1);
	
	for (auto it = relFirstToIndices.begin(); it != relFirstToIndices.end(); it++)
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			relClass[*it2] = synonymTable.inClass((*it).first);
	
	vector<AnswerTable> tables;
	
	for (unsigned int rel = 0; rel < relType.size(); rel++) {
		QueryEnums::QueryReladition type = relType[rel];
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
			if (inWhichTable.count(firstRel) >= 0) {
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
				tables[secondRelIndex].combine(secondRel, firstRelTable,
					firstRel, RulesOfEngagement::getRelation(type));
				if (tables[secondRelIndex].getSize() == 0)
					return;

				inWhichTable[firstRel] = secondRelIndex;
			}
			break;
		case 2:
			int firstRelIndex = inWhichTable[firstRel];
			int secondRelIndex = inWhichTable[firstRel];
			if (firstRelIndex == secondRelIndex) {
				tables[firstRelIndex].prune(firstRel, secondRel,
					RulesOfEngagement::getRelation(type));
				if (tables[firstRelIndex].getSize() == 0)
					return;
			} else {
				int firstRelIndex = inWhichTable[firstRel];
				int secondRelIndex = inWhichTable[firstRel];
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
			QueryEnums::QueryVar type = synonymTable.getType(header[j]);
			if (type == QueryEnums::Procedure)
				answer += PKB::procedures.getPROCName(row[j]);
			else if (type == QueryEnums::Variable)
				answer += PKB::variables.getVARName(row[j]);
			else
				answer += Helper::intToString(row[j]);
			if (j + 1 < header.size())
				answer += " ";
		}
		finalanswer.push_back(answer);
	}
}

	/*for (auto it = qt.getTree().begin(); it != qt.getTree().end(); it++)
	{
		QueryCluster cluster = *it;

		for (auto it = cluster.conditions.begin(); it != cluster.conditions.end(); it++) {
			//each synonym should at most have one condition
			set<int> table;
			for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++) {
				switch ((*it2).first) {
				case QueryEnums::ProcName: //TODO: differentiate between call statement and actual proc
					if((*it2).second < PKB::procedures.getSize());
						table.insert((*it2).second);
					break;
				case QueryEnums::VarName:
					if((*it2).second < PKB::variables.getSize());
						table.insert((*it2).second);
					break;
				case QueryEnums::Value:
					if (PKB::constantsTable.count((*it2).second) > 0)
						table.insert(1);
				case QueryEnums::StmtNo:
					if((*it2).second <= PKB::maxProgLines);
						table.insert((*it2).second);
					break;
				default:
					throw SPAException("Invalid condition type");
				}
			}
			stage.initIndividualTable((*it).first, table);
		}

		for (auto it = cluster.relations.begin(); it != cluster.relations.end(); it++) {
			for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++) {
				switch ((*it2).first) {
				case QueryEnums::Assign:
					stage.join(
					break;
				case QueryEnums::While:
					for (auto it = PKB::whileTable.begin(); it != PKB::whileTable.end(); it++)
						table.insert(*it);
					break;
				case QueryEnums::If:
					for (auto it = PKB::ifTable.begin(); it != PKB::ifTable.end(); it++)
						table.insert(*it);
					break;
				case QueryEnums::Call:
					for (auto it = PKB::callTable.begin(); it != PKB::callTable.end(); it++)
						table.insert(*it);
					break;
				case QueryEnums::Procedure: {
					int size = PKB::procedures.getSize();
					for (int i = 0; i < size; i++)
						table.insert(i);
					break; }
				case QueryEnums::Stmt: {
					int size = PKB::maxProgLines;
					for (int i = 0; i < size; i++)
						table.insert(i);
					break; }
				case QueryEnums::Variable: {
					int size = PKB::variables.getSize();
					for (int i = 0; i < size; i++)
						table.insert(i);
					break; }
				case QueryEnums::Constant:
					for(auto it = PKB::constantsTable.begin(); it != PKB::constantsTable.end(); it++)
						table.insert((*it).first);
					break;
				case QueryEnums::WildCard:
					break;
				default:
					throw SPAException("Invalid first relationship parameter type");
				}
			}

		}/*
		firstcase(answers, cluster[0]) //modifies(s1,s2) -> vector<IntOrString> <s1,s2> that satisfies me
		done[0] = true;
		while (still have unevaluated reladition in cluster) {
			//get those relatditions (condiitions higher priority) that are in vector<IntOrString> (*), then get reladition where one is in, one is out
			if (last one in unstared && !evaluateStarred(answers, reladition)) {
				returnAll = false;
				break; //fail
			} else {
				evaluateUnStarred(answers, reladition);
				kill that reladition
			}
		}
		if (marked)
			bigAnswers.push_bacK(project(StarredInComponent, answers));*/
	//}
	
	/*//cartesian product the answers in bigAnswers
	vector<vector<IntOrString>>* accumulator = &bigAnswers[0];

	if (bigAnswers.size() == 0) {
		if (returnAll) {
			accumulator = getAllVariables(select[0]);
			for (int i = 1; i < select.size(); i++)
				CartesianProduct(answers, getAllVariables(select[i]));
		}
	} else {
		for (int i = 1; i < bigAnswers.size(); i++) {
			vector<vector<IntOrString>> newanswers;
			for (vector<IntOrString> t1 : accumulator)
				for(vector<IntOrString> t2 : bigAnswers[i])
					newanswers.push_back(make_vector<IntOrString>(t1, t2));
			accumulator = &newanswers;
		}
	}

	for (vector<IntOrString> t : accumulator) {
		ostringstream ansss;
		for (entries : t)
			ans << entries << " ";
		string ans = ans.str()
		results.push_back(ans.substr(0, ans.size() - 1));
	}*/

//} //TADA!

/*--------------------------
Select <s1,v2,s3> such that modifies(s1,v1) and uses(s2,v2) and modifies(s1,v2) and uses(s3,v3) and modifies(totally,unrelated)
{s1,v1,s2,v2)* , (s3,v3)* , (totally,unrelated)
modifies(s1,v1)
answers = <s1,v1>

uses(s2,v2) (if (answersNameSet.count(s2) > 0 || answersNameSet.count(v2)) > 0 -> false)
modifies(s1,v2) (if (answersNameSet.find(s1) || answersNameSet.find(v2)) -> true)
answers = <s1,v1,v2>

uses(s2,v2)  (if (answersNameSet.find(s2) || answersNameSet.find(v2)) -> true)
answers = <s1,v1,v2,s2>
Example OVER
--------------------------*/