#include "Dnf.h"


Dnf::Dnf(void)
{
}


Dnf::~Dnf(void)
{
}


string Dnf::getToken(const string& query, int& pos)
{
	int first = query.find_first_not_of(' ', pos);
	if (first == string::npos)
		return "";
	pos = first + 1;
	if ((query.at(first) >= 'A' && query.at(first) <= 'Z') ||
		(query.at(first) >= 'a' && query.at(first) <= 'z')) { //IDENT
			while (pos < query.length() && ((query.at(pos) >= 'A' && query.at(pos) <= 'Z') ||
				(query.at(pos) >= 'a' && query.at(pos) <= 'z') || //letter or
				(query.at(pos) >= '0' && query.at(pos) <= '9') || query.at(pos) == '#' || //digit
				(query.at(pos) == '*') || //relation star
				(query.at(pos) == '_' && pos == first + 4 && query.substr(first, 4) == "prog")))
				pos++;
	} else if (query.at(first) >= '0' && query.at(first) <= '9') { //DIGIT
		while (pos < query.length() && (query.at(pos) >= '0' && query.at(pos) <= '9'))
			pos++;
	} else if (query.at(first) == '\"')
		pos = query.find_first_of('\"', pos) + 1;
	//else return single character
	return query.substr(first, pos - first);
}

FormNode* Dnf::Convert(FormNode* c)
{
	
	if(c->fType == FormNode::query)
	{
	return c;
	}

	if(c->isneg != true)
	{

	if(c->fType == FormNode::Operator && c->value == 0)
	{
		c->children.at(0) = Convert(c->children.at(0));
		c->children.at(1) = Convert(c->children.at(1));

		//need cartesian here
		
		std::vector<FormNode*> f1;
		std::vector<FormNode*> f2;
		c->children.at(0)->getVect_break_or(&f1);//if see and just take as it is
		c->children.at(1)->getVect_break_or(&f2);

		vector<FormNode*> new1;

		for(int i=0;i<f1.size();i++)
		{
			for(int j=0;j<f2.size();j++)
			{
				FormNode* newnode = new FormNode(FormNode::Operator,0);//put and
				newnode->value = 0;
				newnode->fType = FormNode::Operator;
				newnode->children.push_back(f1.at(i));
				newnode->children.at(0)->isneg = f1.at(i)->isneg;
				newnode->children.at(0)->value = f1.at(i)->value;
				newnode->children.at(0)->fType = f1.at(i)->fType;

				newnode->children.push_back(f2.at(j));
				newnode->children.at(1)->isneg = f2.at(j)->isneg;
				newnode->children.at(1)->value = f2.at(j)->value;
				newnode->children.at(1)->fType = f2.at(j)->fType;



				
				new1.push_back(newnode);

			}
		}

		FormNode* head;

		FormNode* prev=0;
		
		while(new1.size() > 1)
		{
			FormNode* curr = new FormNode(FormNode::Operator,1);
				curr->value = 1;
				curr->fType = FormNode::Operator;
			if(prev == 0)
			{
				head = curr;
			}
			else
			{
				prev->children.push_back(curr);
			}
			//put left


			

			curr->children.push_back(new1.at(new1.size() -1));
			new1.pop_back();
		//	prev->children.push_back(curr);

			prev = curr;
		}
		if(prev != 0)
		{
		prev->children.push_back(new1.at(0));
		c=head;
		}
		else
		{
			c = new1.at(0);
		}
		
		return c;
	}

	if(c->fType == FormNode::Operator && c->value == 1)
	{
		c->children.at(0) = Convert(c->children.at(0));
		c->children.at(1) = Convert(c->children.at(1));

		//need cartesian here
		//ok
		return c;
	}
	}

	if(c->isneg == true)
	{
	
		if(c->fType == FormNode::Operator && c->value == 1)
		{
			c->value = 0;
			c->children.at(0)->negate();
			c->children.at(1)->negate();
			c->negate();
			return Convert(c);
			//If φ has the form ~(P ^ Q), then return CONVERT(~P v ~Q).  // de Morgan's Law
			//If φ has the form ~(P v Q), then return CONVERT(~P ^ ~Q).  // de Morgan's Law
		}
		if(c->fType == FormNode::Operator && c->value == 0)//or
		{
			c->value = 1;
			c->children.at(0)->negate();
			c->children.at(1)->negate();
			c->negate();
			return Convert(c);
		}


	}
	

	return c;
	
}

bool Dnf::isQuery(std::string str)
{
	string data2 = str;
	std::transform(data2.begin(), data2.end(), data2.begin(), ::tolower);

	if(data2.compare("follows") == 0 || data2.compare("follows*") == 0 
		|| data2 == "uses" 
		|| data2 == "modifies" 
		|| data2 == "pattern"
		|| data2 == "contains"
		|| data2 == "affects" || data2 == "affects*"
		|| data2 == "parent" || data2 == "parent*" 
		|| data2 == "next" || data2 == "next*"
		|| data2 == "calls" || data2 == "calls*"
		|| data2 == "nextbip" || data2 == "nextbip*"
		
		)//need to add the rest here
		return true;
	return false;

}

bool Dnf::iscon(std::string str)
{
	string data2 = str;
	std::transform(data2.begin(), data2.end(), data2.begin(), ::tolower);

	if(data2.compare("and") == 0 || data2 == "or" || data2 == "st")// || data2=="such")
		return true;
	return false;

}

string Dnf::Trim(string str)
{
	str.erase(remove(str.begin(), str.end(), '\t'), str.end());
	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	return str;
}
vector<string> Dnf::tokenizer(std::string line)//split the string into tokens
{
	vector<string> list;
	string delimiter = " !()-+;{},\"";//delimiters
	int position = 0;//starting position
	int startindex = -1;
	int endindex = -1;
	
	do	{ //loop thru the string
		startindex = line.find_first_not_of(delimiter,position);

		if(position == 0 && startindex>0) {
			int tempchk = 0;
			while(tempchk < startindex)
			{
				string t = line.substr(tempchk,1) ;
				if(t != " ")
					list.push_back(t);
				tempchk++;
			}
		}

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
				for(unsigned int i=0;i<tempstr1.size();) {
					string tempstr2 = tempstr1.substr(0,1);
					tempstr1 = tempstr1.substr(1,tempstr1.size()-1);
					
						//AddToList(list,tempstr2);
					if(Trim(tempstr2).size() > 0)
					list.push_back(tempstr2);
				}
			}

			endindex = line.find_first_of(delimiter,startindex);
			position = endindex;
		
			if(startindex != -1 || line.size() == 1) {
				string tempstr;
				if(line.size() == 1)
					tempstr= line;
				else
					tempstr= line.substr(startindex,endindex-startindex);

				
				if(Trim(tempstr).size() > 0)
				list.push_back(tempstr);
			}

	}while(startindex != -1 && position < line.size() && endindex != -1);

	return list;
	//house keeping
	//if(Parser::tokenized_codes.size() > 1)	{
	//	vector<string> temp_vec = Parser::tokenized_codes.at(0);
	//	temp_vec.insert(temp_vec.end(), Parser::tokenized_codes.at(1).begin(),Parser::tokenized_codes.at(1).end());
	//	Parser::tokenized_codes.erase(Parser::tokenized_codes.begin());
	//	Parser::tokenized_codes.at(0) = temp_vec;
	//}

	//if(list.size() > 0)
	//Parser::tokenized_codes.push_back(list);
}
char Dnf::easytolower(char in){
										  if(in<='Z' && in>='A')
											return in-('Z'-'z');
										  return in;
										} 


int Dnf::find_closer(vector<string> l, int cur)
{
	stack<string> nodesStack;
	int index=cur;
	//case1: found and query
	//case2: found )
	//case3: found and (
	//case 4 found such
	/*if(cur==20)
				int l=1;*/
	for(int i=(cur+1);i<l.size();i++)
	{
		/*if(cur==20 && i==28)
			int lol =1;*/
		string temp = l.at(i);
		if((isQuery(temp) || temp == "!") && iscon(l.at(i-1)))//case 1
		{
			return i-1;
		}


		if(l.at(i) == "\"")
			{
				if(nodesStack.size() == 0 || nodesStack.top() != "str")
				{
					nodesStack.push("str");
				}
				else if(nodesStack.top() == "str")
				{
					nodesStack.pop();
				}
			}
			else if(nodesStack.size()> 0 &&nodesStack.top() == "str")
			{
				
			}
			else if(l.at(i) == "such" || l.at(i)=="Such")
			{
				return i;
			}
			else if((l.at(i) == "(" || l.at(i) == "!" ) && i>0 && iscon(l.at(i-1)))
			{
				return i-1;
			}
			else if(l.at(i) == "(")
				nodesStack.push("(");
			else if(l.at(i) == ")")
			{
				if(nodesStack.size() >0 && nodesStack.top() == "(")
					nodesStack.pop();
				else
					return i;
			}
	}


	return l.size();
}
int Dnf::find_closer(string d)
{
	stack<string> nodesStack;
	vector<string> test;
	vector<int> testnum;

	int previndex = 0;
	while(d.find_first_not_of(" ",previndex) != -1)
	{
		int f = d.find_first_not_of(" ",previndex);
		int end =d.find_first_of(" ",f);

		string key = d.substr(f,end-f);
		

		//chk izzit a queries
		if(key.size() > 3)
		{
			string comparing = key.substr(0,3);
			if(comparing == "Use")
				int zlo = 1;
//			std::transform(comparing.begin(), comparing.end(), comparing.begin(), easytolower);
			if(comparing == "fol" ||comparing == "Fol" || comparing == "nex" || comparing == "pat" || comparing == "use" 
				||comparing == "Use" || comparing == "mod" || comparing == "aff")
			{
				if(test.at(test.size() -1) == "and" || test.at(test.size() -1) == "or")
					return testnum.at(test.size() -1);
			}
		}
		//std::transform(key.begin(), key.end(), key.begin(), easytolower);
		
		for(int i=0;i<key.size();i++)
		{
			if(key.at(i) == '"')
			{
				if(nodesStack.size() == 0 || nodesStack.top() != "str")
				{
					nodesStack.push("str");
				}
				else if(nodesStack.top() == "str")
				{
					nodesStack.pop();
				}
			}
			else if(nodesStack.size()> 0 &&nodesStack.top() == "str")
			{
				
			}
			else if(key.at(i) == '(' && (test.at(test.size() -1) == "and" || test.at(test.size() -1) == "or"))
			{
				return testnum.at(test.size() -1);
			}
			else if(key.at(i) == '(')
				nodesStack.push("(");
			else if(key.at(i) == ')')
			{
				if(nodesStack.size() >0 && nodesStack.top() == "(")
					nodesStack.pop();
				else
					return f+i;
			}
		}

		test.push_back(key);
		testnum.push_back(f);

		//do till key = follows or )
		previndex = end;
	}


	return d.size();
}

bool Dnf::isDNF(FormNode* n)
{
	//bfs //chk if previous had and and i am or
	queue<pair<FormNode*,bool>> q;//bool for if previous was and

	q.push(pair<FormNode*,bool>(n,false));

	do
	{
		pair<FormNode*,bool> top = q.front();
		q.pop();
		
		FormNode* cur = top.first;

		if(cur->fType == FormNode::Operator && cur->value == 1 && top.second == true)//its and or
		{
			return false;
		}

		if(cur->fType == FormNode::Operator)//if op then add
		{
			FormNode* left =cur->children.at(0);
			bool b=false;
			if(cur->value == 0)
				b = true;

			FormNode* right = cur->children.at(1);

			q.push(pair<FormNode*,bool>(left,b));
			q.push(pair<FormNode*,bool>(right,b));
			



		}
		
	
	}while(q.size() >0);

	return true;
	
}

std::vector<pair<std::string,std::string>>* Dnf::CreateDNF(string str)
{  
	if(str=="")
	{
		std::vector<pair<std::string,std::string>>* nth = new std::vector<pair<std::string,std::string>>();
		nth->push_back(pair<std::string,std::string>("",""));
		return nth;
	}
		vector<string> qry;

		vector<string> tokens = tokenizer(str);
		vector<string> fin;
		int counter=0;
		for(int i=0;i<tokens.size();i++)
		{
			
			if(isQuery(tokens.at(i)))
			{
				int k = find_closer(tokens,i);
				int h=1;

				string temp="";

				bool bracketopenfound = false;
				bool nextnospace = false;

				for(int j=i;j<k;j++)
				{
					if(tokens.at(j) == "\"")
					{
						bracketopenfound = !bracketopenfound;

						if(!bracketopenfound)
						temp.append(tokens.at(j));
						else
							temp.append(" "+tokens.at(j));
							nextnospace = true;
						continue;
					}
					if(!nextnospace)
						temp.append(" "+tokens.at(j));
					else
					{
						temp.append(tokens.at(j));
						nextnospace = false;
					}
				}
				qry.push_back(temp);
				ostringstream convert; 

				convert<<counter;
				string temp2 = "" + convert.str()+"stub";
				fin.push_back(temp2);
				counter++;
				i = k-1;
				int l=1;
				//find its closure
			}
			else
			{
				fin.push_back(tokens.at(i));
			}
		}


		FormNode *head;		
		//at this pt i want to change such that to s

		

		vector<string> finwost;

		bool skipnext = false;
		for(int i=0;i<fin.size();i++)
		{
			if(skipnext)
			{
				skipnext=false;
				continue;
			}
			string t = fin.at(i);
			std::transform(t.begin(), t.end(), t.begin(), ::tolower);

			if(t == "such" && i <= (fin.size()-1))
			{
				string t2 = fin.at(i+1);
				std::transform(t2.begin(), t2.end(), t2.begin(), ::tolower);
				if(t2 == "that")
				{
					finwost.push_back("st");
					skipnext = true;
					continue;
				}
			}

			finwost.push_back(t);
		}

		head = processAssignment(finwost);


		
		FormNode *newhead =  Convert(head);;	
		
		//string data1 = newhead->print();

		vector<string>* dnfform = newhead->GetStringVect(&qry);
		vector<string>* dnfformPruned = newhead->GetStringVectPruned(&qry);

		vector<pair<string,string>>* finalans = new vector<pair<string,string>>();

		for(int i=0;i<dnfform->size();i++)
		{
			pair<string,string> ans;
			ans.first = dnfform->at(i);
			ans.second = dnfformPruned->at(i);
			finalans->push_back(ans);
		}

		return finalans;		

	
}

FormNode* Dnf::processAssignment(vector<string> expr)
{
	
	stack<bool> negstack;
	stack<string> operators, subExprBrackets; 
	stack<FormNode*> operands;

	vector<string> subExpr;

	bool isneg = false;

	
	for (unsigned int i = 0; i < expr.size(); i++ ) {
		string token = expr[i]; 
		if(token == "!" && subExprBrackets.size() == 0)
		{
			isneg =true;
			continue;
		}

		if (token == "/" || token == "^" || token == "%")
			throw SPAException("Operator not supported, use '+', '-' or '*' only");
		
		if (token == ";") //terminating sequence for assignment
			break; 

		if (token == ")" && subExprBrackets.size() == 0)
			throw SPAException("Please check your expression, additional brackets are found!");

		if (token == "(" && subExprBrackets.size() == 0){ 	//Bracket For Shunting Yard
			subExprBrackets.push(token);

		} else if (subExprBrackets.size() > 0){
			if (token == "("){
				subExprBrackets.push(token);
				subExpr.push_back(token);

			} else if (token == ")") {
				
				subExprBrackets.pop();

				if (subExprBrackets.size() > 0) {
					subExpr.push_back(token);

				} else if (subExprBrackets.size() == 0) {
					subExpr.push_back(";");
					 //recursive call here
					FormNode* tempnode = processAssignment(subExpr);
						if(isneg)
						{
							if(tempnode->isneg == true)
								tempnode->isneg = false;
							else
								tempnode->isneg = true;
						}
						operands.push(tempnode);
					//negate the new into opposite
					subExpr.clear();
				}
			} else {
				subExpr.push_back(token);
			}

		} else if (iscon(token)) {
			if (operators.empty()) {//if the operator stack is empty simply push
				operators.push(token);
			
			} else { //Compare the precedence of + with the top of the stack 
				//if (compareOprPrecedence(token, operators.top()) > 0)	{

				//	operators.push(token); //if it is greater, push
				//
				//} else 
				{ //else pop and form a sub tree
					
					FormNode* oprNode = new FormNode(FormNode::Operator, operators.top());
					oprNode->isneg = isneg;
					isneg = false;

					operators.pop();

					FormNode* rightChild = operands.top();
					operands.pop();
					
					FormNode* leftChild = operands.top();
					operands.pop();

					oprNode->addChild(leftChild, 1);
					oprNode->addChild(rightChild, 2);

					operands.push(oprNode);
					operators.push(token);
				}
			}
		} else { //it is an operand

		
					FormNode* varNode = new FormNode(FormNode::query, token);
					varNode->isneg = isneg;
					isneg = false;
					operands.push(varNode);
				
		}
	}
	
	//Build the complete right sub tree to be returned to assign node
	while (!operators.empty()) {
		FormNode* oprNode = new FormNode(FormNode::Operator, operators.top());
					
		FormNode* rightChild = operands.top();
		operands.pop();
					
		FormNode* leftChild = operands.top();
		operands.pop();

		oprNode->addChild(leftChild, 1);
		oprNode->addChild(rightChild, 2);

		operands.push(oprNode);
		operators.pop();
	}
	return operands.top();
}


void Dnf::Eval(std::string query,list<string>& results)
{
	int pos =0;
		string token="";// = QueryPreprocessor::getToken(query, pos);
		do
		{
			token = getToken(query, pos);
		}while(token != "Select");

		string selected = getToken(query, pos);//nick see=>pos = identify the select something
	unordered_set<string> selects;
	if (selected.at(0) == '<') { //tuple -> multiple selected variables
		do {
			selected = getToken(query, pos);
			selects.insert(selected);
			selected = getToken(query, pos);
			if (selected == ">")
				break;
		} while (true);
		//selectBOOLEAN = false;
	} else if (selected == "BOOLEAN") {
		//selectBOOLEAN = true;
	} else {
		selects.insert(selected);
		//selectBOOLEAN = false;

	}

	//naive case just put such that

	string sub = query.substr(pos,query.size()-pos);

	int temp=0;

	if(getToken(sub, temp) == "such" && getToken(sub, temp) == "that")//remove the beginning such that
	{
		sub = sub.substr(temp,sub.size()-temp);
	}


	int cutoffpt = pos;
	string querystart = query.substr(0,pos);
	vector<pair<string,string>>* substrs = Dnf::CreateDNF(sub);

	vector<list<string>> allans;
	vector<string> newsubstrs;
	try{
	for(int i=0;i<substrs->size();i++)
	{
		 //list<string> results;
		 
		//vector<string> answers;
		if(substrs->at(i).first.compare(substrs->at(i).second)==0)
		{
			//both same just do 1 query
			string newqry = "";
			int pfound = substrs->at(i).first.find("pattern", 0);
			if((pfound == -1 || pfound > 3) && substrs->at(i).first != "")
				newqry = querystart + " such that " +substrs->at(i).first;
			else 
				newqry = querystart +" " +substrs->at(i).first;
			
			PQLController::evaluateQuery(newqry,results);
		}
		else
		{
			//do 2 and intersect .second is the big 1
		
			list<string> results1;
			list<string> results2;
			{
			string newqry = "";
			int pfound = substrs->at(i).second.find("pattern",0);
			if(pfound == -1 || pfound > 3)
				newqry = querystart + " such that " +substrs->at(i).second;
			else 
				newqry = querystart + " " +substrs->at(i).second;
		
			PQLController::evaluateQuery(newqry, results1);
			
			}
			{
			string newqry = "";
			int pfound = substrs->at(i).first.find("pattern",0);
			if(pfound == -1 || pfound > 3)
				newqry = querystart + " such that " +substrs->at(i).first;
			else 
				newqry = querystart +" " +substrs->at(i).first;
			PQLController::evaluateQuery(newqry,results2);
			}
			//intersect

			for (auto it1 = results1.begin(); it1 != results1.end(); it1++) {
				const string arg = (*it1);
				std::list<string>::iterator findIter = std::find(results2.begin(), results2.end(), arg);

				if(findIter == results2.end())
				{
					//not found
					//int zzsfa=1;
					results.push_back(arg);
				}

				//int zzz=1;
			}
			
		}

			
		
			
			
			
			//allans.push_back(results);
		
	}	

	//int test1=1;
	////union and remove dup

	//list<string> lastans;
	//for(int i=0;i<allans.size();i++)
	//{
	//	for (auto it1 = allans.at(i).begin(); it1 != allans.at(i).end(); it1++) {
	//			const string arg = (*it1);
	//			std::list<string>::iterator findIter = std::find(lastans.begin(), lastans.end(), arg);

	//			if(findIter == lastans.end())
	//			{
	//				//not found
	//				
	//				lastans.push_back(arg);
	//			}

	//			
	//		}


	//}

	////remove dup
	//lastans.sort(compare);
	//results = lastans;

	}catch(exception e){

	}
}


bool Dnf::compare(string s1,string s2)
{
	return (s1.compare(s2) == 0);
}