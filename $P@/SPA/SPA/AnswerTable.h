#pragma once
#include "stdAfx.h"
#include "RulesOfEngagement.h"
#include "SynonymTable.h"

class AnswerTable
{
private:
	vector<string> header;
	vector<RulesOfEngagement::QueryVar> type;
	vector<vector<pair<int, unordered_set<ASTNode*>>>> answers;

	static vector<string> MiniTokenizer(const string& line);

public:
	unordered_map<string, int> synonymPosition;
	
	AnswerTable();
	AnswerTable(const SynonymTable& synonymTable, const string& synonym);

	void combine(const string& ownSynonym, AnswerTable& otherTable,
		const string& otherSynonym, const RulesOfEngagement::QueryRelations rel);
	void prune(const string& firstSynonym,
		const string& secondSynonym, const RulesOfEngagement::QueryRelations rel);

	void withCombine(const SynonymTable& synonymTable, const string& firstSynonym, 
		const string& firstCondition, const AnswerTable& otherTable,
		const string& secondSynonym, const string& secondCondition);
	void withPrune(const SynonymTable& synonymTable, const string& firstSynonym,
		const string& firstCondition, const string& secondSynonym, const string& secondCondition);
	
	void patternPrune(const string& synonym, const RulesOfEngagement::PatternRHSType,
		const string& RHS, const ASTExprNode* RHSexprs);
	
	AnswerTable project(const vector<string>& selection);
	void cartesian(const AnswerTable& otherTable);

	bool isEmpty() const;
	vector<string> getHeader() const;
	size_t getHeaderSize() const;
	size_t getSize() const;
	vector<pair<int, unordered_set<ASTNode*>>> getRow(const int index) const;
	//int getEntry(int index, string synonym);*/
};