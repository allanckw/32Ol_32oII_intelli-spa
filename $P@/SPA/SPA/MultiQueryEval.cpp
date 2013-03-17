#pragma once
#include "MultiQueryEval.h"
#include "Helper.h"


/**
* Returns whether the query is of type Select Boolean
* @return true if it is of type select boolean, false otherwise
*/
bool MultiQueryEval::isSelectBoolean(){
	return this->selectBOOLEAN;
}

/**
* Returns whether the result of the query terminating early, i.e. query is 
* always false after checks
* @return true if it terminating early, false otherwise
*/
bool MultiQueryEval::isEarlyQuit(){
	return this->earlyQuit;
}


/**
* Does all the query evaluation.
* @param query query string
* @return the MultiQueryEval object with answers if any
*/
vector<string> MultiQueryEval::getFinalAnswer(){
	return this->finalanswer;
}

/**
* Does all the query evaluation.
* @param query query string
* @return the MultiQueryEval object with answers if any
*/
MultiQueryEval::MultiQueryEval(const string& query)
{
	//parse the query statement
	int pos = 0;
	earlyQuit = false;

	//parse synonym declaration
	unordered_map<string, RulesOfEngagement::QueryVar> stringToQueryVar;
	//unordered_map<string, int> stringCount;
	while (true) {
		string token = QueryPreprocessor::getToken(query, pos);
		if (token == "Select")
			break;
		RulesOfEngagement::QueryVar type = RulesOfEngagement::tokenToVar[token];
		
		string variable;
		do {
			variable = QueryPreprocessor::getToken(query, pos);
			stringToQueryVar.insert(pair<string, RulesOfEngagement::QueryVar>(variable, type));
			//stringCount.insert(pair<string, int>(variable, 0));
		} while ((variable = QueryPreprocessor::getToken(query, pos)) == ",");
	}

	//parse selected variables
	string selected = QueryPreprocessor::getToken(query, pos);
	unordered_set<string> selects;
	if (selected.at(0) == '<') { //tuple -> multiple selected variables
		do {
			selected = QueryPreprocessor::getToken(query, pos);
			selects.insert(selected);
			selected = QueryPreprocessor::getToken(query, pos);
			if (selected == ">")
				break;
		} while (true);
		selectBOOLEAN = false;
	} else if (selected == "BOOLEAN") {
		selectBOOLEAN = true;
	} else {
		selects.insert(selected);
		selectBOOLEAN = false;
	}

	//parse relationships, conditions and pattern
	//look for possible optimisations
	unordered_map<RulesOfEngagement::QueryRelations, unordered_map<string,
		unordered_set<string>>> relationStore;
	unordered_map<string, unordered_map<string, string>> conditionStore;
	vector<tuple<string, string, string, string>> condition2Store;
	unordered_map<string, unordered_set<string>> patternAssignUsesStore;
	
	//for new synonyms created due to synthetic sugaring
	int tempVars = 0;

	enum Clauses { Undefined, Such_That, With, Pattern };
	Clauses clauseType = Undefined;
	while (true) {
		string clause = QueryPreprocessor::getToken(query, pos);
		if (clause == "") {
			break;
		} else if (clause == "such") {
			QueryPreprocessor::matchToken(query, pos, "that");
			clauseType = Such_That;
		} else if (clause == "with") {
			clauseType = With;
		} else if (clause == "pattern") {
			clauseType = Pattern;
		}

		switch (clauseType) {
		case Such_That:
			{
				string relation = QueryPreprocessor::getToken(query, pos);
				QueryPreprocessor::matchToken(query, pos, "(");
				string firstRel = QueryPreprocessor::getToken(query, pos);
				QueryPreprocessor::matchToken(query, pos, ",");			
				string secondRel = QueryPreprocessor::getToken(query, pos);
				QueryPreprocessor::matchToken(query, pos, ")");

				unordered_set<RulesOfEngagement::QueryRelations>& types =
					RulesOfEngagement::tokenToRel[relation];

				RulesOfEngagement::QueryVar firstRelType;
				if (firstRel == "_")
					firstRelType = RulesOfEngagement::WildCard;
				else if (stringToQueryVar.count(firstRel) > 0)
					firstRelType = stringToQueryVar[firstRel];
				else if (firstRel.at(0) == '\"' && firstRel.at(firstRel.length() - 1) == '\"')
					firstRelType = RulesOfEngagement::String;
				else if (Helper::isNumber(firstRel))
					firstRelType = RulesOfEngagement::Integer;

				RulesOfEngagement::QueryRelations type = RulesOfEngagement::PatternUses; //sentinel value
				for (auto it = types.begin(); it != types.end(); it++)
					if (RulesOfEngagement::allowableFirstArgument[*it].count(firstRelType) > 0) {
						type = *it;
						break;
					}

				/*if (stringToQueryVar.count(firstRel) > 0) {
					if (stringToQueryVar.count(secondRel) > 0) {
						if (firstRel != secondRel) {
							stringCount[firstRel]++;
							stringCount[secondRel]++;
						}
					} else
						stringCount[firstRel]++;
				} else if (stringToQueryVar.count(secondRel) > 0)
					stringCount[secondRel]++;*/
				relationStore[type][firstRel].insert(secondRel);
			}
			break;

		case With:
			{
				string synonym = QueryPreprocessor::getToken(query, pos);
				if (stringToQueryVar.count(synonym) == 0) { // LHS is "known" value
				QueryPreprocessor::matchToken(query, pos, "=");
				string token = QueryPreprocessor::getToken(query, pos);
				if (stringToQueryVar.count(token) == 0) {
					earlyQuit |= (synonym == token);
				} else {
				RulesOfEngagement::QueryVar type = stringToQueryVar[token];
				string condition;
				RulesOfEngagement::QueryVar RHSType;
				if (type == RulesOfEngagement::Prog_Line) {
					condition = "";
					RHSType = RulesOfEngagement::Integer;
				} else {
					QueryPreprocessor::matchToken(query, pos, ".");
					condition = QueryPreprocessor::getToken(query, pos);
					RHSType = RulesOfEngagement::conditionTypes[condition];
				}
					if (conditionStore[token].count(condition) > 0)
						earlyQuit |= (conditionStore[token][condition] == token);
					conditionStore[token].insert(pair<string, string>(condition, synonym));
				}
				} else {
				RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];
				string condition;
				RulesOfEngagement::QueryVar LHSType;
				if (type == RulesOfEngagement::Prog_Line) {
					condition = "";
					LHSType = RulesOfEngagement::Integer;
				} else {
					QueryPreprocessor::matchToken(query, pos, ".");
					condition = QueryPreprocessor::getToken(query, pos);
					LHSType = RulesOfEngagement::conditionTypes[condition];
				}

				QueryPreprocessor::matchToken(query, pos, "=");
				//either c.value = 10 OR c.value = s.stmt#
				string token = QueryPreprocessor::getToken(query, pos);
				if (stringToQueryVar.count(token) == 0) { //c.value = 10
					if (conditionStore[synonym].count(condition) > 0)
						earlyQuit |= (conditionStore[synonym][condition] == token);
					conditionStore[synonym].insert(pair<string, string>(condition, token));
					//stringCount[synonym]++;
				} else { //c.value = s.stmt# OR c.value = n
					RulesOfEngagement::QueryVar type2 = stringToQueryVar[token];
					string condition2;
					if (type2 == RulesOfEngagement::Prog_Line) {
						condition2 = "";
					} else {
						QueryPreprocessor::matchToken(query, pos, ".");
						condition2 = QueryPreprocessor::getToken(query, pos);
					}

					if (synonym == token && condition == condition2)
						break;

					condition2Store.push_back(tuple<string, string, string, string>
						(synonym, condition, token, condition2));
					//stringCount[synonym]++;
					//stringCount[token]++;
				}
				}
			}
			break;

		case Pattern:
			{
				string synonym = QueryPreprocessor::getToken(query, pos);
				RulesOfEngagement::QueryVar type = stringToQueryVar[synonym];

				QueryPreprocessor::matchToken(query, pos, "(");
				string firstRel = QueryPreprocessor::getToken(query, pos);

				switch (type) {
				case RulesOfEngagement::Assign:
					{
						if (firstRel != "_") {
							//stringCount[synonym]++;
							relationStore[RulesOfEngagement::ModifiesStmt][synonym].insert(firstRel);
						}
						
						QueryPreprocessor::matchToken(query, pos, ",");
						string secondRel = QueryPreprocessor::getToken2(query, pos);
						QueryPreprocessor::matchToken(query, pos, ")");

						if (secondRel != "_") {
							//remove white spaces
							secondRel.erase(remove(secondRel.begin(), secondRel.end(), '\t'), secondRel.end());
							secondRel.erase(remove(secondRel.begin(), secondRel.end(), ' '), secondRel.end());

							//stringCount[synonym]++;
							patternAssignUsesStore[synonym].insert(secondRel);
						}
					}
					break;

				case RulesOfEngagement::If:
					QueryPreprocessor::matchToken(query, pos, ",");
					QueryPreprocessor::matchToken(query, pos, "_");
					QueryPreprocessor::matchToken(query, pos, ",");
					QueryPreprocessor::matchToken(query, pos, "_");
					QueryPreprocessor::matchToken(query, pos, ")");

					if (firstRel == "_")
						earlyQuit |= !RulesOfEngagement::isExistType(type);
					else {
						//stringCount[synonym]++;
						relationStore[RulesOfEngagement::PatternModifies][synonym].insert(firstRel);
					}
					break;

				case RulesOfEngagement::While:
					QueryPreprocessor::matchToken(query, pos, ",");
					QueryPreprocessor::matchToken(query, pos, "_");
					QueryPreprocessor::matchToken(query, pos, ")");

					if (firstRel == "_")
						earlyQuit |= !RulesOfEngagement::isExistType(type);
					else {
						//stringCount[synonym]++;
						relationStore[RulesOfEngagement::PatternModifies][synonym].insert(firstRel);
					}
					break;
				} //end switch of synonym type in pattern
			} //end case of pattern
		} //end switch of clauseType
	}
	//end of query validation

	if (earlyQuit)
		return;

	//apply special rules
	//1) stmt s, t; Follows(s,t) and Follows(t,s) -> earlyQuit
	//TODO: Generalise to Follows(s1,s2) and Follows(s2,s3) and ... and Follows(sk,s1) -> earlyQuit
	unordered_map<string, unordered_set<string>>* followsRel;
	if (relationStore.count(RulesOfEngagement::Follows) > 0)
		followsRel = &relationStore[RulesOfEngagement::Follows];
	else
		followsRel = 0;
	unordered_map<string, unordered_set<string>>* followsStarRel;
	if (relationStore.count(RulesOfEngagement::FollowsStar) > 0)
		followsStarRel = &relationStore[RulesOfEngagement::FollowsStar];
	else
		followsStarRel = 0;
	if (followsRel != 0)
		for (auto it = followsRel->begin(); it != followsRel->end(); it++) {
			const string& firstRel = it->first;
			if (stringToQueryVar.count(firstRel) == 0)
				continue;
			const unordered_set<string>& secondRels = it->second;
			for (auto it2 = secondRels.begin(); it2 != secondRels.end(); it2++) {
				if (stringToQueryVar.count(*it2) == 0)
					continue;
				if (firstRel == *it2 || 
					(followsRel->count(*it2) > 0 && followsRel->at(*it2).count(firstRel) > 0)
					|| (followsStarRel != 0 && followsStarRel->count(*it2) > 0
					&& followsStarRel->at(*it2).count(firstRel) > 0)) {
					earlyQuit = true;
					return;
				}
			}
		}
	if (followsStarRel != 0)
		for (auto it = followsStarRel->begin(); it != followsStarRel->end(); it++) {
			const string& firstRel = it->first;
			if (stringToQueryVar.count(firstRel) == 0)
				continue;
			const unordered_set<string>& secondRels = it->second;
			for (auto it2 = secondRels.begin(); it2 != secondRels.end(); it2++) {
				if (stringToQueryVar.count(*it2) == 0)
					continue;
				if (firstRel == *it2 ||
					(followsRel != 0 && followsRel->count(*it2) > 0
					&& followsRel->at(*it2).count(firstRel) > 0) ||
					(followsStarRel->count(*it2) > 0 &&
					followsStarRel->at(*it2).count(firstRel) > 0)) {
					earlyQuit = true;
					return;
				}
			}
		}
	//end special rules

	//second stage
	//analyse synonyms part 1. look for those with count of 1 to be folded in
	/*unordered_set<string> toFold; //todo:: alias synonyms
	for (auto it = stringCount.begin(); it != stringCount.end(); it++)
		if (selects.count(it->first) == 0 && it->second == 1)
			toFold.insert(it->first);*/

	//parse equality of attributes, alias those that can be aliased
	SynonymTable synonymTable;
	DisjointSet dsSynonym;
	DisjointSet dsAlias;

	//condition, with double synonyms, table
	vector<string> condFirstRel;
	vector<string> condFirstCondition;
	vector<string> condSecondRel;
	vector<string> condSecondCondition;
	for (auto it = condition2Store.begin(); it != condition2Store.end(); it++) {
		string& firstRel = get<0>(*it);
		string& firstCondition = get<1>(*it);
		string& secondRel = get<2>(*it);
		string& secondCondition = get<3>(*it);

		if (firstCondition == secondCondition) { // a.cond = b.cond
			if (firstRel == secondRel) // s.cond = s.cond -> skip //todo: must check if s exists
				continue;
			if (firstCondition == "stmt#") { // a.stmt# = b.stmt#
				if (stringToQueryVar[firstRel] == RulesOfEngagement::Statement) { // s.stmt# = b.stmt#
					if (stringToQueryVar[secondRel] != RulesOfEngagement::Statement) // s.stmt# = b.stmt#, b.type != statement
						stringToQueryVar[firstRel] = stringToQueryVar[secondRel]; //-> a1.stmt# = a2.stmt#
				} else { // b.stmt# = s.stmt#
					if (stringToQueryVar[secondRel] == RulesOfEngagement::Statement) // b.stmt# = s.stmt#, b.type != statement
						stringToQueryVar[secondRel] = stringToQueryVar[firstRel]; //-> a1.stmt# = a2.stmt#, a1.type = a2.type
					else { // a.stmt# = b.stmt#, a.type != b.type
						earlyQuit = true;
						return;
					}
				}
				//at this point, either quit already, else firstRel.type = secondRel.type
			}
			if (stringToQueryVar[firstRel] == stringToQueryVar[secondRel]) { // s1.cond = s2.cond
				dsAlias.setUnion(firstRel, secondRel);
				string& firstRelParent = dsAlias.find(firstRel);
				//int firstRelCount = stringCount[firstRelParent];
				string& secondRelParent = dsAlias.find(secondRel);
				//int secondRelCount = stringCount[secondRelParent];
				//stringCount[firstRelParent] = stringCount[secondRelParent] =
					//firstRelCount + secondRelCount - 2;
				continue;
			}
		}
		
		//Fold in conditions if possible (optimisation)
		//stmt s1, s2; constant c; Select s1 with s2.stmt# = c.value -> no idea how to fold
		//stmt s; constant c; Select s with s.stmt# = c.value -> fold in (constant, value) into s
		/*if (selects.count(firstRel) == 0 && stringCount[firstRel] == 1) {
			if (selects.count(secondRel) == 0 && stringCount[secondRel] == 1) { //don't fold
				stringCount[firstRel] = stringCount[secondRel] = 1 << 16;
			} else { //stmt s; constant c; Select s with c.value = s.stmt#
				synonymTable.insert(secondRel, stringToQueryVar[secondRel]);
				synonymTable.setGenericAttribute(secondRel,
					secondCondition, stringToQueryVar[firstRel], firstCondition);
				stringCount[firstRel]--;
				stringCount[secondRel]--;
			}
		} else {
			if (selects.count(secondRel) == 0 && stringCount[secondRel] == 1) { //stmt s; constant c; Select s with s.stmt# = c.value
				synonymTable.insert(firstRel, stringToQueryVar[firstRel]);
				synonymTable.setGenericAttribute(firstRel,
					firstCondition, stringToQueryVar[secondRel], secondCondition);
				stringCount[firstRel]--;
				stringCount[secondRel]--;
			} else { //stmt s; constant c; Select <s,c> with s.stmt# = c.value*/
				condFirstRel.push_back(firstRel);
				condFirstCondition.push_back(firstCondition);
				condSecondRel.push_back(secondRel);
				condSecondCondition.push_back(secondCondition);
				dsSynonym.setUnion(firstRel, secondRel);
			/*}
		}*/

	}
	
	//analyse synonyms part 2. those selected and with count of more than 1 are put into the synonym table
	for (auto it = stringToQueryVar.begin(); it != stringToQueryVar.end(); it++) {
		const string& synonym = it->first;
		const int count = it->second;
		synonymTable.insert(synonym, stringToQueryVar[synonym]);
		dsSynonym.makeSet(synonym);
		if (selects.count(synonym) > 0) {
			/*synonymTable.insert(synonym, stringToQueryVar[synonym]);
			dsSynonym.makeSet(synonym);*/
			synonymTable.setSelected(synonym);
		} /*else if (count > 1) {
			synonymTable.insert(synonym, stringToQueryVar[synonym]);
			dsSynonym.makeSet(synonym);
		}*/
	}
	
	//relationship table
	vector<RulesOfEngagement::QueryRelations> relType;
	vector<string> relFirst;
	vector<string> relSecond;
	vector<int> relClass;
	unordered_map<string, vector<int>> relFirstToIndices;

	/*//pattern table
	vector<string> patternSynonym;
	vector<RulesOfEngagement::QueryVar> patternType;
	vector<string> patternLHS;
	vector<string> patternRHS;*/


	vector<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> zerosyno;
	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> onesyno;
	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> twosyno;

	//separate all 
	for (auto it = relationStore.begin(); it != relationStore.end(); it++) {
		const RulesOfEngagement::QueryRelations& type = it->first;
		const unordered_map<string, unordered_set<string>>& itsecond = it->second;
		
		for (auto it2 = itsecond.begin(); it2 != itsecond.end();it2++) {
			const string& first = it2->first;
			int amount_of_synoname;
			if (synonymTable.isInTable(first))
				amount_of_synoname = 1;
			else
				amount_of_synoname = 0;

			for (auto it3 = it2->second.begin(); it3 != it2->second.end();it3++) {
				//if not a var dont add into pool
				const string& second = *it3;
				int amount_of_synoname2 = amount_of_synoname;
				if (synonymTable.isInTable(second))
					amount_of_synoname2++;

				switch (amount_of_synoname2) {
				case 0:
					zerosyno.push_back(pair<RulesOfEngagement::QueryRelations,pair<string, string>>(type,pair<string, string>(first,second)));
					break;
				case 1:
					onesyno.push_back(pair<RulesOfEngagement::QueryRelations,pair<string, string>>(type,pair<string, string>(first,second)));
					break;
				case 2:
					twosyno.push_back(pair<RulesOfEngagement::QueryRelations,pair<string, string>>(type,pair<string, string>(first,second)));
					break;
				}
			}
			
		}
	}

	//sort nick


	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> temponesyno;
	list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> temptwosyno;

	//sort
	for(int i=0;i<PKB::sortorder.size();i++)
	{
		std::list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>>::iterator j = onesyno.begin();
		while (j != onesyno.end())
		{
			if(j->first == PKB::sortorder.at(i).first)
			{
				temponesyno.push_back(*j);
				onesyno.erase(j++);
			}
			else
			{
				++j;
			}
			
			
		}
		

		j = twosyno.begin();
		while (j != twosyno.end())
		{
			if(j->first == PKB::sortorder.at(i).first)
			{
				temptwosyno.push_back(*j);
				twosyno.erase(j++);
			}
			else
			{
				++j;
			}		
		}
	}
	//add remaining back
	std::list<pair<RulesOfEngagement::QueryRelations,pair<string, string>>>::iterator j = onesyno.begin();
		while (j != onesyno.end())
		{
				temponesyno.push_back(*j);
				++j;
		}

		j = twosyno.begin();
		while (j != twosyno.end())
		{
				temptwosyno.push_back(*j);
				++j;
		}

		onesyno = temponesyno;
		twosyno = temptwosyno;
	
	/*
	*/
	//================================

	bool newpool = true;
	unordered_set<string> pool;

	vector<pair<RulesOfEngagement::QueryRelations,pair<string, string>>> newcontainer;

	while (onesyno.size() > 0 || twosyno.size() > 0)
	{
		if (newpool) {
			if (onesyno.size() > 0) {
				pool = unordered_set<string>();
				pair<RulesOfEngagement::QueryRelations,pair<string, string>>& ones = onesyno.front();

				if (synonymTable.isInTable(ones.second.first))
					pool.insert(ones.second.first);
				else //if (synonymTable.isInTable(ones.second))
					pool.insert(ones.second.second);

				newcontainer.push_back(ones);
				onesyno.erase(onesyno.begin());
				//newpool = false;
			} else { //if (twosyno.size() > 0) {
				pool = unordered_set<string>();
				pair<RulesOfEngagement::QueryRelations,pair<string, string>>& two = twosyno.front();

				pool.insert(two.second.first);
				pool.insert(two.second.second);

				newcontainer.push_back(two);
				twosyno.erase(twosyno.begin());
				//newpool = false;
			}
			newpool = false;
		}

		bool addedtwosyno = false;

		//loop two syno for once
		for (auto it = twosyno.begin(); it != twosyno.end(); it++)
		{
			unordered_set<string>::const_iterator got1 = pool.find(it->second.first);
			unordered_set<string>::const_iterator got2;
			if (got1 == pool.end())
				got2 = pool.find(it->second.second);

			if (got1 != pool.end() || got2 != pool.end()) { //exist
				newcontainer.push_back(*it);
				pool.insert(it->second.first);
				pool.insert(it->second.second);
				//remove
				twosyno.erase(it);

				addedtwosyno = true;
				break;
			}
		}

		//loop one syno for all
		for (auto it = onesyno.begin(); it != onesyno.end();)
		{
			bool toadd = false;
			if (synonymTable.isInTable(it->second.first))
			{
				if (pool.find(it->second.first) != pool.end())
				{
					//exist
					toadd = true;
					pool.insert(it->second.first);
				}
			}
			else //if (synonymTable.isInTable(it->second))
			{
				if (pool.find(it->second.second) != pool.end())
				{
					//exist
					toadd = true;
					pool.insert(it->second.second);
				}
			}

			if (toadd) {
				//at this pt added to pool

				//add to new container
				newcontainer.push_back(*it);

				//now need to remove from the twosyno
				auto it2 = it;
				it2++;
				onesyno.erase(it);
				it = it2;
			} else
				it++;
		}

		if(!addedtwosyno)
			newpool = true;
	}

	//now the best order is stored in newcontainer but with vector<string,string> instead


	//parse relations
	//parse trivial relations
	for (auto it = zerosyno.begin(); it != zerosyno.end(); it++) {
		const RulesOfEngagement::QueryRelations& type = it->first;
		const string& firstRel= it->second.first;
		const string& secondRel= it->second.second;

		if (firstRel == "_") {
			if (secondRel == "_") { //Case 1: rel(_,_)
				if (RulesOfEngagement::emptyRel[type]) {
					earlyQuit = true;
					return;
				}
			} else { //Case 2: rel(_,"1")
				string sugar = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(sugar))
					sugar = "t" + sugar;
							
				RulesOfEngagement::QueryVar secondArg = RulesOfEngagement::privilegedSecondArgument[type];
				synonymTable.insert(sugar, secondArg);
				synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[secondArg].begin()),
					secondRel);
				synonymTable.setSecondGeneric(sugar, type);
			}
		} else {
			if (secondRel == "_") { //Case 2: rel("1",_)
				string sugar = "t" + Helper::intToString(++tempVars);
				while (synonymTable.isInTable(sugar))
					sugar = "t" + sugar;

				RulesOfEngagement::QueryVar firstArg = RulesOfEngagement::privilegedFirstArgument[type];
				synonymTable.insert(sugar, firstArg);
				synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[firstArg].begin()),
					firstRel);
				synonymTable.setFirstGeneric(sugar, type);
			} else { //Case 3: rel("1","2") 
				int first = RulesOfEngagement::convertArgumentToInteger(type, true, firstRel);
				int second = RulesOfEngagement::convertArgumentToInteger(type, false, secondRel);
				if (!RulesOfEngagement::getRelation(type)(first, second)) {
					earlyQuit = true;
					return;
				}
			}
		}
	}

	//parse normal relations
	for (auto it = newcontainer.begin(); it != newcontainer.end(); it++) {
	//for (auto it = relationStore.begin(); it != relationStore.end(); it++) {
		const RulesOfEngagement::QueryRelations& type = it->first;
		const string& firstRel= it->second.first;
		const string& secondRel= it->second.second;
		//const unordered_map<string, unordered_set<string>>& itsecond = it->second;
		//for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			/*string firstRel;
			if (toFold.count(it2->first) > 0) {
				if (stringToQueryVar[it2->first] == RulesOfEngagement::privilegedFirstArgument[type])
					firstRel = "_";
				else {
					firstRel = it2->first;
					synonymTable.insert(firstRel, stringToQueryVar[firstRel]);
					dsSynonym.makeSet(firstRel);
				}
			} else
				firstRel = it2->first;*/
			//const string& firstRel= it2->first;
			//const unordered_set<string>& it2second = it2->second;
			//for (auto it3 = it2second.begin(); it3 != it2second.end(); it3++) {
				/*string secondRel;
				if (toFold.count(*it3) > 0) {
					if (stringToQueryVar[*it3] == RulesOfEngagement::privilegedSecondArgument[type])
						secondRel = "_";
					else {
						secondRel = *it3;
						synonymTable.insert(secondRel, stringToQueryVar[secondRel]);
						dsSynonym.makeSet(secondRel);
					}
				} else
					secondRel = *it3;*/
				//const string& secondRel= *it3;
				//actual setting up of relations
				//count number of synonyms
				int numOfSynonyms = 0;
				bool firstIsSynonym = false;
				if (synonymTable.isInTable(firstRel)) {
					numOfSynonyms++;
					firstIsSynonym = true;
				}
				if (synonymTable.isInTable(secondRel))
					numOfSynonyms++;

				switch (numOfSynonyms) {
				/*case 0: //handle 0 synonym, 3 cases
					//Case 1: rel( _ , _ ) -> On the spot test if the relationship is ever true
					//Case 2: rel( _ ,"1") -> rel(_,s) with s.stmtNo = 1 -> treat as a condition on s
					//Case 3: rel("1","2") -> On the spot test
					if (firstRel == "_") {
						if (secondRel == "_") { //Case 1: rel(_,_)
							if (RulesOfEngagement::emptyRel[type]) {
								earlyQuit = true;
								return;
							}
						} else { //Case 2: rel(_,"1")
							string sugar = "t" + Helper::intToString(++tempVars);
							while (synonymTable.isInTable(sugar))
								sugar = "t" + sugar;
							
							RulesOfEngagement::QueryVar secondArg = RulesOfEngagement::privilegedSecondArgument[type];
							synonymTable.insert(sugar, secondArg);
							synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[secondArg].begin()),
								secondRel);
							synonymTable.setSecondGeneric(sugar, type);
						}
					} else {
						if (secondRel == "_") { //Case 2: rel("1",_)
							string sugar = "t" + Helper::intToString(++tempVars);
							while (synonymTable.isInTable(sugar))
								sugar = "t" + sugar;

							RulesOfEngagement::QueryVar firstArg = RulesOfEngagement::privilegedFirstArgument[type];
							synonymTable.insert(sugar, firstArg);
							synonymTable.setSpecificAttribute(sugar, *(RulesOfEngagement::allowableConditions[firstArg].begin()),
								firstRel);
							synonymTable.setFirstGeneric(sugar, type);
						} else { //Case 3: rel("1","2") 
							int first = RulesOfEngagement::convertArgumentToInteger(type, true, firstRel);
							int second = RulesOfEngagement::convertArgumentToInteger(type, false, secondRel);
							if (!RulesOfEngagement::getRelation(type)(first, second)) {
								earlyQuit = true;
								return;
							}
						}
					}
					break;*/

				case 1: //handle 1 synonym: put as a condition
					//Case 1: rel(s, _ ) -> put in generic(first)
					//Case 2: rel(s,"1") -> put in specific(first)
					if (firstIsSynonym) {
						if (secondRel == "_")
							synonymTable.setFirstGeneric(firstRel, type);
						else
							synonymTable.setFirstSpecific(firstRel, type, secondRel);
					} else {
						if (firstRel == "_")
							synonymTable.setSecondGeneric(secondRel, type);
						else
							synonymTable.setSecondSpecific(secondRel, type, firstRel);
					}
					break;

				case 2: //handle both synonyms
					
					//handle self-reference
					if (firstRel == secondRel) {
						if (RulesOfEngagement::allowableSelfReference.count(type) > 0) {
							synonymTable.setSelfReference(firstRel, type);
							break;
						} else {
							earlyQuit = true;
							return;
						}
					}

					if (relFirstToIndices.count(firstRel) > 0)
						relFirstToIndices[firstRel].push_back(relType.size());
					else {
						vector<int> temp;
						temp.push_back(relType.size());
						relFirstToIndices.insert(pair<string, vector<int>>(firstRel, temp));
					}
					relType.push_back(type);
					relFirst.push_back(firstRel);
					relSecond.push_back(secondRel);
					relClass.push_back(-1);

					dsSynonym.setUnion(firstRel, secondRel);
					break;
				}
			//}
		//}
	}

	//parse conditions
	for (auto it = conditionStore.begin(); it != conditionStore.end(); it++) {
		const string& synonym = it->first;
		const unordered_map<string, string>& itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			const string& condition = it2->first;
			const string& attribute = it2->second;
			
			//actual setting up of conditions
			bool passed = synonymTable.setSpecificAttribute(synonym, condition, attribute);
			if (!passed) {
				earlyQuit = true;
				return;
			}
		}
	}

	vector<unordered_set<string>> components = dsSynonym.getComponents();
	for (unsigned int classIndex = 0; classIndex < components.size(); classIndex++)
		for (auto it = components[classIndex].begin(); it != components[classIndex].end(); it++)
			synonymTable.putIntoClass(*it, classIndex + 1);
	
	for (auto it = relFirstToIndices.begin(); it != relFirstToIndices.end(); it++)
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			relClass[*it2] = synonymTable.getClass((*it).first);
	
	vector<AnswerTable> tables;
	//could have incorporated in synonym table, but was not because it is implentation dependent
	unordered_map<string, int> inWhichTable;

	//evaluate equality of attributes
	for (unsigned i = 0; i < condFirstRel.size(); i++) {
		string& firstRel = condFirstRel[i];
		string& firstCondition = condFirstCondition[i];
		string& secondRel = condSecondRel[i];
		string& secondCondition = condSecondCondition[i];

		int matchNumberOfTables = 0;
		if (inWhichTable.count(firstRel) > 0)
			matchNumberOfTables++;
		if (inWhichTable.count(secondRel) > 0)
			matchNumberOfTables++;

		switch (matchNumberOfTables) {
		case 0:
			{
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				firstRelTable.withCombine(synonymTable, firstRel,
					firstCondition, secondRelTable, secondRel, secondCondition);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[firstRel] = tables.size();
				inWhichTable[secondRel] = tables.size();
				tables.push_back(firstRelTable);
			}
			break;
		case 1:
			if (inWhichTable.count(firstRel) > 0) {
				int firstRelIndex = inWhichTable[firstRel];

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				tables[firstRelIndex].withCombine(synonymTable, firstRel,
					firstCondition, secondRelTable, secondRel, secondCondition);
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[secondRel] = firstRelIndex;
			} else {
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				int secondRelIndex = inWhichTable[secondRel];
				firstRelTable.withCombine(synonymTable, firstRel,
					firstCondition, tables[secondRelIndex], secondRel, secondCondition);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				tables[secondRelIndex] = firstRelTable;
				inWhichTable[firstRel] = secondRelIndex;
			}
			break;
		case 2:
			int firstRelIndex = inWhichTable[firstRel];
			int secondRelIndex = inWhichTable[secondRel];
			if (firstRelIndex == secondRelIndex) {
				tables[firstRelIndex].withPrune(synonymTable,
					firstRel, firstCondition, secondRel, secondCondition);
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}
			} else {
				tables[firstRelIndex].withCombine(synonymTable, firstRel,
					firstCondition, tables[secondRelIndex], secondRel, secondCondition);
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
					if ((*it).second == secondRelIndex)
						(*it).second = firstRelIndex;
			}
		}
	}
	
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
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				firstRelTable.combine(firstRel, secondRelTable, secondRel, type);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[firstRel] = tables.size();
				inWhichTable[secondRel] = tables.size();
				tables.push_back(firstRelTable);
			}
			break;
		case 1:
			if (inWhichTable.count(firstRel) > 0) {
				int firstRelIndex = inWhichTable[firstRel];

				AnswerTable secondRelTable = AnswerTable(synonymTable, secondRel);
				if (secondRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				tables[firstRelIndex].combine(firstRel, secondRelTable, secondRel, type);
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[secondRel] = firstRelIndex;
			} else {
				AnswerTable firstRelTable = AnswerTable(synonymTable, firstRel);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				int secondRelIndex = inWhichTable[secondRel];
				firstRelTable.combine(firstRel, tables[secondRelIndex], secondRel, type);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

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
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}
			} else {
				tables[firstRelIndex].combine(firstRel, tables[secondRelIndex], secondRel, type);
				if (tables[firstRelIndex].getSize() == 0) {
					earlyQuit = true;
					return;
				}

				for (auto it = inWhichTable.begin(); it != inWhichTable.end(); it++)
					if ((*it).second == secondRelIndex)
						(*it).second = firstRelIndex;
			}
		}
	}

	//evaluate patterns
	for (auto it = patternAssignUsesStore.begin(); it != patternAssignUsesStore.end(); it++) {
		const string& synonym = it->first;
		const unordered_set<string>& itsecond = it->second;
		for (auto it2 = itsecond.begin(); it2 != itsecond.end(); it2++) {
			const string& usesVar = *it2;

			//actual setting up of patterns of assign for the right hand side
			RulesOfEngagement::PatternRHSType RHS;
			string RHSVarName;
			if (usesVar.at(0) == '_' && usesVar.at(usesVar.size() - 1) == '_') {
				RHS = RulesOfEngagement::PRSub;
				RHSVarName = usesVar.substr(1, usesVar.length() - 2);
			} else {
				RHS = RulesOfEngagement::PRNoSub;
				RHSVarName = usesVar;
			}
			RHSVarName = RHSVarName.substr(1, RHSVarName.length() - 2);
			ASTExprNode* RHSexprs;
			try {
				RHSexprs = AssignmentParser::processAssignment(MiniTokenizer(RHSVarName));
			} catch (SPAException& e) {	//exception indicates that the right hand side
				earlyQuit = true;		//is not correct, probably due to it containing
				return;					//a variable that is not actually present.
			}

			if (inWhichTable.count(synonym) == 0) {
				AnswerTable firstRelTable = AnswerTable(synonymTable, synonym);
				if (firstRelTable.getSize() == 0) {
					earlyQuit = true;
					return;
				}

				inWhichTable[synonym] = tables.size();
				tables.push_back(firstRelTable);
			}
			int synonymIndex = inWhichTable[synonym];

			tables[synonymIndex].patternPrune(synonym, RHS, RHSVarName, RHSexprs);
			if (tables[synonymIndex].getSize() == 0) {
				earlyQuit = true;
				return;
			}
		}
	}

	//examine table sizes
	vector<string>& synonyms = synonymTable.getAllNames();
	for (auto it = synonyms.begin(); it != synonyms.end(); it++)
		if (inWhichTable.count(*it) == 0) {
			AnswerTable table = AnswerTable(synonymTable, *it);
			if (table.getSize() == 0) {
				earlyQuit = true;
				return;
			}
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
	vector<vector<string>> projections(tables.size());
	int firstNonEmpty = tables.size();
	for (auto it = selecteds.begin(); it != selecteds.end(); it++) {
		if (inWhichTable[*it] < firstNonEmpty)
			firstNonEmpty = inWhichTable[*it];
		projections[inWhichTable[*it]].push_back(*it);
	}

	AnswerTable concatenated = tables[firstNonEmpty].project(projections[firstNonEmpty]);
	for (unsigned int i = firstNonEmpty + 1; i < tables.size(); i++)
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
		auto it = orderOfSelection.begin();
		while (true) {
			answer += RulesOfEngagement::convertIntegerToArgument(
				synonymTable.getType(header[*it]), row[*it]);
			it++;
			if (it != orderOfSelection.end())
				answer += " ";
			else
				break;
		}
		finalanswer.push_back(answer);
	}
}

/**
* Reads a string of an expression, and converts it to a vector of tokens.
* @param query expression string
*/
vector<string> MultiQueryEval::MiniTokenizer(const string& line)
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
			}

	}while(startindex != -1 && position < line.size() && endindex != -1);
	
	return list;
}