#pragma once
#include "MultiQueryTree.h"

void MultiQueryTree::insertSynonym(string synonym, QueryEnums::QueryVar var)
{
	synonymToType[synonym] = var;
	typeToSynonym[var] = synonym;
}

bool MultiQueryTree::insertCondition(string synonym,
	QueryEnums::QueryCond attribute, int condition)
{
	if (unsortedConditions[synonym].count(attribute) > 0 &&
		unsortedConditions[synonym][attribute] != condition)
		return false;
	else
		unsortedConditions[synonym][attribute] = condition;
}

void MultiQueryTree::insertRelationship(QueryEnums::QueryRel rel, string synonym1, string synonym2)
{
	synonyms.setUnion(synonym1, synonym2);
	unsortedRelations[synonym1].insert(make_pair(rel, synonym2));
}

void MultiQueryTree::insertSelectSynonym(string synonym)
{
	selectedSynonyms.push_back(synonym);
	selectedSynonymsSet.insert(synonym);
}

void MultiQueryTree::buildTree()
{
	queue<QueryCluster> markedTree;
	vector<unordered_set<string>> components = synonyms.getComponents();
	for (auto it = components.begin(); it != components.end(); it++) {
		QueryCluster cluster;
		cluster.synonymsSet = *it;
		for (auto it2 = (*it).begin(); it2 != (*it).end(); it2++)
			cluster.synonyms.push_back(*it2);

		for (auto it2 = unsortedConditions.begin(); it2 != unsortedConditions.end(); it2++)
			if (cluster.synonymsSet.count((*it2).first) > 0)
				cluster.conditions[(*it2).first] = (*it2).second;

		for (auto it2 = unsortedRelations.begin(); it2 != unsortedRelations.end(); it2++)
			if (cluster.synonymsSet.count((*it2).first) > 0)
				cluster.relations[(*it2).first] = (*it2).second;

		cluster.marked = false;
		for (auto it2 = selectedSynonymsSet.begin(); it2 != selectedSynonymsSet.end(); it2++)
			if (cluster.synonymsSet.count(*it2) > 0) {
				cluster.marked = true;
				markedTree.push(cluster);
				break;
			}

		if (!cluster.marked)
			tree.push_back(cluster);
	}

	while (!markedTree.empty()) {
		tree.push_back(markedTree.front());
		markedTree.pop();
	}
}

unordered_map<string, QueryEnums::QueryVar> MultiQueryTree::getSelectedSynonymToType()
{
	return synonymToType;
}

unordered_map<QueryEnums::QueryVar, string> MultiQueryTree::getSelectedTypeToSynonym()
{
	return typeToSynonym;
}

vector<QueryCluster> MultiQueryTree::getTree()
{
	return tree;
}