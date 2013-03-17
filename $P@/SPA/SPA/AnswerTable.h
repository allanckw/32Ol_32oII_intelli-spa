#pragma once
#include "stdAfx.h"
#include "RulesOfEngagement.h"
#include "SynonymTable.h"

class AnswerTable
{
private:
	vector<string> header;
	vector<vector<int>> answers;

	static vector<string> MiniTokenizer(const string& line);

public:
	unordered_map<string, int> synonymPosition;
	
	AnswerTable();
	AnswerTable(const SynonymTable& synonymTable, const string& synonym);

	void combine(const string& ownSynonym, const AnswerTable& otherTable, const string& otherSynonym,
		const RulesOfEngagement::QueryRelations rel); //synonyms are all disjoint
	void prune(const string& firstSynonym,
		const string& secondSynonym, const RulesOfEngagement::isRelation rel);

	void withCombine(const SynonymTable& synonymTable, const string& firstSynonym, 
		const string& firstCondition, const AnswerTable& otherTable,
		const string& secondSynonym, const string& secondCondition);
	void withPrune(const SynonymTable& synonymTable, const string& firstSynonym,
		const string& firstCondition, const string& secondSynonym, const string& secondCondition);
	
	void patternPrune(const string& synonym,
		const RulesOfEngagement::PatternRHSType, const string& RHS, const ASTExprNode* RHSexprs);
	/*void patternPrune(const string& synonym, const bool, const int modifies,
		const RulesOfEngagement::PatternRHSType, const string& RHS, const ASTExprNode* RHSexprs);*/
	
	AnswerTable project(const vector<string>& selection);
	void cartesian(const AnswerTable& otherTable);

	bool isEmpty() const;
	vector<string> getHeader() const;
	unsigned int getHeaderSize() const;
	unsigned int getSize() const;
	vector<int> getRow(const int index) const;
	//int getEntry(int index, string synonym);*/
};