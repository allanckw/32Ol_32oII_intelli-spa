#pragma once
#include "stdAfx.h"

class DisjointSet
{
private:
	bool dirty;
	vector<string> list;
	unordered_map<string, int> index;
	unordered_map<string, string> parent;

	vector<bool> starred;
	unordered_map<string, unordered_set<string>> components;
	vector<unordered_set<string>> unstarredComponents;
	vector<unordered_set<string>> starredComponents;
	
	void makeSet(string);
	string find(string);
	void makeComponent();

public:
	DisjointSet(void);
	~DisjointSet(void);
	void bondage(string, string);

	void star(string);
	vector<unordered_set<string>> getUnstarred();
	vector<unordered_set<string>> getStarred();
}

/*
--------------------------
for example
Select <s1,s2> such that modifies(s1,v1) and uses(s2,v2)
When we read  modifies(s1,v1)
do: bondage(s1,v1);
When we read  uses(s2,v2)
do: bondage(s2,v2);

star(s1)
star(s2)
Example OVER
--------------------------

queryevaluate(results) {

	Create querytree, which pproduces a query tree on the reladitions and a disjoint set on the variables as they are being read

	sort components in order such that unmarked first, then marked next;
	vector<vector<tuple>> bigAnswers;
	bool returnAll = true;

	for (vector<int> component : components)
	{
		vector<bool> done = vector(component.size(), false);
		//somehow the cluster of the querytree that is that component
		vector<vector<int>> answers;
		vector<string> answersNameVector; //(s1,s2, s3, s4)
		set<string> answersNameSet; //(s1,s2, s3, s4)
		firstcase(answers, cluster[0]) //modifies(s1,s2) -> tuple <s1,s2> that satisfies me
		done[0] = true;;
		while (still have unevaluated reladition in cluster) {
			//get those relatditions (condiitions higher priority) that are in tuple (*), then get reladition where one is in, one is out
			if (last one in unstared && !evaluateStarred(answers, reladition)) {
				returnAll = false;
				break; //fail
			} else {
				evaluateUnStarred(answers, reladition);
				kill that reladition
			}
		}
		if (marked)
			bigAnswers.push_bacK(project(StarredInComponent, answers));
	}
	
	//cartesian product the answers in bigAnswers
	vector<tuple>* accumulator = &bigAnswers[0];

	if (bigAnswers.size() == 0) {
		if (returnAll) {
			accumulator = getAllVariables(select[0]);
			for (int i = 1; i < select.size(); i++)
				CartesianProduct(answers, getAllVariables(select[i]));
		}
	} else {
		for (int i = 1; i < bigAnswers.size(); i++) {
			vector<tuple> newanswers;
			for (tuple t1 : accumulator)
				for(tuple t2 : bigAnswers[i])
					newanswers.push_back(make_tuple(t1, t2));
			accumulator = &newanswers;
		}
	}

	for (tuple t : accumulator) {
		ostringstream ansss;
		for (entries : t)
			ans << entries << " ";
		string ans = ans.str()
		results.push_back(ans.substr(0, ans.size() - 1));
	}

} //TADA!

--------------------------
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
--------------------------

evaluateStarred(vector<tuple> *answers, QueryTreeNode reladition) {
	if (answersNameSet.find(relatdition.first)) && answersNameSet.find(relatdition.second()) {
		int index1 = answersNameVector.find(relatdition.first);
		int index2 = answersNameVector.find(relatdition.secoond);
		int currentpos = 0;
		while (currentpos < answers.size())) {
			if satsify reladition(get<index1> ans, get<index2> ans)
				currentpos++;
			else
				remove ans; //this will be horrendously slow on a vector: O(n) time
		}
	} else {
		if (answersNameSet.find(relatdition.first)) {
			int index1 = answersNameVector.find(relatdition.first);
			int index2 = answersNameVector.find(relatdition.second);
		else {
			int index1 = answersNameVector.find(relatdition.second);
			int index2 = answersNameVector.find(relatdition.first);
		}
		vector<tuple> newanswers;
		for (ans : answers) {
			vector<type2> toCartesianProduct = getRelation(ans[index1]) //needs to switch
			for (type2 : toCartesianProduct) {
				newanswers.add(flat(new tuple(ans, type2)));
				answersNameVector.push_back(type2);
				answersNameSet.insert(type2);
			}
		}
		answers = newanswer;
	}
}

bool evaluateUnStarred(vector<tuple> *answers, QueryTreeNode reladition) {
	if (answersNameSet.find(relatdition.first)) && answersNameSet.find(relatdition.second()) {
		int index1 = answersNameVector.find(relatdition.first);
		int index2 = answersNameVector.find(relatdition.secoond);
		for (ans : answers) {
			if satsify reladition(get<index1> ans, get<index2> ans)
				return true;
		}
	} else {
		if (answersName.find(relatdition.first)) {
			int index1 = answersNameVector.find(relatdition.first);
			int index2 = answersNameVector.find(relatdition.second);
		else {
			int index1 = answersNameVector.find(relatdition.second);
			int index2 = answersNameVector.find(relatdition.first);
		}
		for (ans : answers) {
			vector<type2> toCartesianProduct = getRelation(ans[index1])
			if (toCartesianProduct.size() > 0)
				return true;
		}
	}
	return false;
}


