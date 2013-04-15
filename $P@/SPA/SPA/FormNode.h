#pragma once
#include <vector>
#include "../SPA/SPAException.h"
#include "../SPA/Helper.h"

class FormNode
{
public:

	typedef enum FormType { 
		Operator,
		query

	};

	bool isneg;
	int value;
	FormType fType;

	FormNode(FormType, int);
	FormNode(FormType, std::string value);// std::vector<std::string>);
	~FormNode(void);
	FormNode* Clone();
	std::vector<FormNode*> children;
	void getVect(std::vector<FormNode*>* f);
	
		void getVect_break_or(std::vector<FormNode*>* f);

	std::vector<std::string>* GetStringVect(std::vector<std::string>* qry);
	void GetStringVect(std::vector<std::string>* str,std::vector<std::string>* qry);

std::vector<std::string>* GetStringVectPruned(std::vector<std::string>* qry);
void GetStringVectPruned(std::vector<std::string>* str,std::vector<std::string>* qry);
	string printNoBracketPruned(vector<string>* qry);
	string removeBind(std::string str);

	string printNoBracket(vector<string>* qry);
	std::string printNoBracket();
	void negate();
	std::string print();
	bool allOpsSame();
	bool allOpsSame(int i);
	FormNode* addChild(FormNode* c, int childLoc);

	void FormNode::printNotNoBracket(vector<string>* qry,vector<string>* storage);
void FormNode::GetNotStringVect(std::vector<std::vector<std::string>>* ,std::vector<std::string>* qry);
std::vector<std::vector<std::string>>* FormNode::GetNotStringVect(std::vector<std::string>* qry);

};


