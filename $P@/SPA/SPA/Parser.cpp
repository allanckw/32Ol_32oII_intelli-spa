#pragma once
#include "StdAfx.h"
#include "Parser.h"
#include "PKB.h"
#include "SPAException.h"
#include "ASTNode.h"
#include "ASTStmtNode.h"
#include "ASTStmtLstNode.h"
#include "AssignmentParser.h"
#include "Helper.h"

//TODO: For Nick, put your parser into this controller
vector<vector<string>> Parser::tokenized_codes;

stack<char> brackets;

Parser::Parser(string fileName)
{
	string codings;
	//try{
	  string line;
	  
	  ifstream myfile (fileName);//CS3201test6.txt");

	  if (myfile.is_open()) {
		  while ( myfile.good() )		{		  
	
			  getline(myfile,line);
			  codings.append(line);
		  }

		  myfile.close();
	  }else 
		  cout << "Unable to open file"<<endl; 
	  int currentline = 0;

	  tokenizer(codings);

	  if(brackets.size() != 0){
		throw SPAException("Error during Parsing, Invalid Bracket matching");//error, bracket matching fail
	  }	else {
		PKB::maxProgLines = currentline;
	  }

	// 
	//}catch (exception& e) {
	//	cout << e.what() << endl;
	//} 
}

Parser::~Parser(void)
{
}

bool Parser::isStrCheckNoSpecialChar(string newtoken)
 {
	 int result = newtoken.find_first_of("!@#$%^&*()_+{}|\\][~`", 0);
		 
	 return result != string::npos;
 }

bool Parser::isEmpty(string str)
{
	for(unsigned int index=0;index<str.size();index++)
	{
		if(str.at(index)!= ' ')
			return false;
	}
	return true;
}

bool Parser::is_number(const std::string& str)
{
    std::string::const_iterator currentIterator = str.begin();
    while (currentIterator != str.end() && isdigit(*currentIterator)) ++currentIterator;
    return !str.empty() && currentIterator == str.end();
}

string Parser::Trim(string str)
{
	str.erase(remove(str.begin(), str.end(), '\t'), str.end());
	str.erase(remove(str.begin(), str.end(), ' '), str.end());

	return str;
}

void Parser::AddTables(vector<string> list, string newtoken)
{
	int size = list.size();

	if(newtoken.size() == 0 || newtoken.compare("while") == 0 || newtoken.compare("if") == 0 || newtoken.compare("procedure") == 0 || 
		newtoken.compare("+") == 0 || newtoken.compare("-") == 0 || 
		newtoken.compare("*") == 0 ||  newtoken.compare("=") == 0 ||
		newtoken.compare("call") == 0||newtoken.compare("then") == 0||
		newtoken.compare("else") == 0||newtoken.compare(";") == 0)//found a reserve token
	{
		return;
	}
	else if(newtoken[0] == '{' || newtoken[0] == '}')//found a bracket
	{
		
		if(newtoken[0] == '{' )
		{
			brackets.push('{');//push open braket to stack
		}
		else if(newtoken[0] == '}' )
		{
			
			if(brackets.size() == 0)
			{
				throw SPAException("Error during Parsing, Invalid Bracket matching");//error bracket matching
				
			}
			else if(brackets.top() != '{')
			{
				throw SPAException("Error during Parsing, Invalid Bracket matching"); //error bracket matching
			}
			brackets.pop();//pop the bracket from the stack
			
		}
		
	}
	else if(!isdigit(newtoken[0]))//if currentoken is not a number hence may be a variable or procedure
	{
		if (isStrCheckNoSpecialChar(newtoken)) {
			throw SPAException("Error invalid source Syntax");
		}else{
			if(size != 0 && list.at(size-1) == "procedure"){
				PKB::procedures.insertPROC(newtoken);
			}else if(size == 0 || list.at(size-1) != "call"){
				PKB::variables.insertVAR(newtoken);
			}
		}
	}
	else
	{
		if(size != 0 && list.at(size-1) == "procedure")
			throw SPAException("Error during Parsing, Invalid Procedure Starting Number");//error procedure start with a number
		else if(!is_number(newtoken))
		{
			throw SPAException("Error during Parsing, Invalid Variable Starting Number");//error variable start with a number
		}
	}

}

void Parser::AddToList(vector<string>& list, string str)
{//Parser::tokenized_codes.push_back(lststr);
	str = Trim(str);
	if(str.size() > 0)
	{
		if(str == "while" || str == "call" || str == "if")	{
			//new
			Parser::tokenized_codes.push_back(list);
			vector<string> newlist;
			list = newlist;
		}else if(str=="=")	{
			string tempstr = list.at(list.size()-1);
			list.pop_back();
			Parser::tokenized_codes.push_back(list);
			vector<string> newlist;
			list = newlist;
			list.push_back(tempstr);
		}
		
		AddTables(list,str);
		list.push_back(str);
	}
}

void Parser::tokenizer(string line)//split the string into tokens
{
	vector<string> list;
	string delimiter = " -+*;{}=";//delimiters
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
					
						AddToList(list,tempstr2);
						//AddTables(list,tempstr2);
						//list.push_back(tempstr2);
					
				}
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

				
					AddToList(list,tempstr);
					//AddTables(list,tempstr);
					//list.push_back(tempstr);			
				
			}

	}while(startindex != -1 && position < line.size() && endindex != -1);
	

	//house keeping
	if(Parser::tokenized_codes.size() > 1)
	{
		vector<string> temp_vec = Parser::tokenized_codes.at(0);
		temp_vec.insert(temp_vec.end(), Parser::tokenized_codes.at(1).begin(),Parser::tokenized_codes.at(1).end());
		Parser::tokenized_codes.erase(Parser::tokenized_codes.begin());
		Parser::tokenized_codes.at(0) = temp_vec;
	}

	if(list.size() > 0)
	Parser::tokenized_codes.push_back(list);

	
}

void Parser::buildAST()
{

	int templine =0;
	int tempindex=0;
	int* line=&templine;
	int* index = &tempindex;
	//Vector of code form..
	ASTNode* root=new ASTNode(ASTNode::Program);
	PKB::rootNode = root;
	while(*line < Parser::tokenized_codes.size())
	{
		vector<string> inner =  Parser::tokenized_codes.at(*line);
		
		while(*index < inner.size()) {
			string keyword=inner.at(*index);
			if (*index == 0 && *line == 0 ){
				if (keyword == "procedure") {//Assume case sensitive
					PROCIndex i = PKB::procedures.getPROCIndex(inner.at((*index)+1));
					if (i != 0){
						  //not the first procedure as parsed...
					 }
					PKB::rootNode->setRoot(i);
					PKB::rootNode->addChild(processProcedure(line,index));
					inner=Parser::tokenized_codes.at(*line);
				}
			 }
			 else
			 {
				  if (keyword == "procedure") {
					  PKB::rootNode->addChild(processProcedure(line,index));
					  inner=Parser::tokenized_codes.at(*line);
				  }
			 }
			  //cout<<inner.at(*index)<<" ";
			 (*index)++;
		  }
		 cout<<endl;
		 *index = 0;
		 (*line)++;
	}
}

bool Parser::isName(string s) //first char of name cannot be digit
{
	return (!isdigit(s.at(0)));
}

//Reason why i put procIdx is in a while there could be a call statement possibilities
ASTStmtNode* Parser::processWhile(int *i, int *j, Index procIdx)
{
	stack<char> brackets;
	int currline =*i;
	int tempindex= *j;
	int* line=&currline;
	int* index = &tempindex;

	//Get Control Variable
	vector<string> inner=Parser::tokenized_codes.at(*line);
	(*index)++;
	string varName = inner.at(*index);

	VARIndex vi=PKB::variables.getVARIndex(varName);

	ASTStmtNode* stmtNode = new ASTStmtNode(*line, ASTNode::While, vi);
	ASTStmtLstNode* stmtLstNode=new ASTStmtLstNode();

	for(unsigned int idx=*index; idx<inner.size(); idx++)
	{
		if(inner.at(idx)=="{")
		{
			brackets.push('{');
			(*index)=idx+1;
			break;
		}
	}


	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);

		while(*index < inner.size())
		{
			string keyword=inner.at(*index);

			if (keyword=="procedure"){				
				//throw exception, procedure does not exist in procedure
				throw SPAException("Procedure cannot appear in While!Invalid Source!");
			}

			//do bracket Matching
			if (keyword=="{"){
				brackets.push('{');
				if (brackets.size()>1){
					throw SPAException("Invalid Source!");
				}
			}

			if (keyword=="}"){
				brackets.pop();
				if(brackets.size()==0){
					(*i)=(*line);
					(*j)=(*index);
					if(stmtLstNode->isHasChildren()==false)
					{
						throw SPAException("Stmt Must have one or more child!");
					}
					stmtNode->addChild(stmtLstNode);
					return stmtNode;
				}
			}			
				
			if (keyword=="call"){
				ASTStmtNode* callNode=processCall(line, index, procIdx);
				callNode->setParent(stmtNode);
				stmtLstNode->addChild(callNode);
				//break;
			}
			
			if(keyword=="while"){
				ASTStmtNode* whileNode=processWhile(line, index, procIdx);
				whileNode->setParent(stmtNode);
				stmtLstNode->addChild(whileNode);
				inner=Parser::tokenized_codes.at(*line);
				//break;
			}
			
			if(keyword=="if")
			{
				//No implementation - Not required in CS3201
				//stmtLstNode->addChild(processIf(line));
			}
			
			if(keyword=="="){
				//check for assignment Statement
				//processAssignmentNode
				ASTStmtNode* assignNode=processAssignment(line, index);
				assignNode->setParent(stmtNode);
				stmtLstNode->addChild(assignNode);
			}
			(*index)++;
		}
		(*index)=0;
		(*line)++;
	}
	
	return NULL;
}

//Reason why i put procIdx is in a while there could not be a self call
ASTStmtNode* Parser::processCall(int *i, int *j, Index procIdx)
{
	vector<string> inner=Parser::tokenized_codes.at(*i);

	PROCIndex pi = PKB::procedures.getPROCIndex(inner.at((*j)+1));
	if(pi==-1)
	{
		//Throw exception, Procedure not in Source
		throw SPAException("Procedure does not exist!");
	}
	
	if(pi==procIdx)
	{
		//Throw exception, Procedure cannot self call
		throw SPAException("Recursive Call is not allowed!");
	}

	ASTStmtNode* stmtCall=new ASTStmtNode(*i,ASTNode::Call,pi);
	(*j)=(*j)+2;
	return stmtCall;
}

ASTStmtNode* Parser::processAssignment(int *i, int *j)
{
	vector<string> inner=Parser::tokenized_codes.at(*i);
	string varName = inner.at((*j)-1);

	VARIndex vi=PKB::variables.getVARIndex(varName);

	vector<string> rightExpression;
	int exIdx=(*j)+1;
	while(exIdx<inner.size())
	{
		rightExpression.push_back(inner.at(exIdx));
		
		if(inner.at(exIdx)==";")
			break;
		exIdx++;
	}

	ASTStmtNode* stmtAssign=new ASTStmtNode(*i,ASTNode::Assign,vi);
	ASTExprNode* leftNode = new ASTExprNode(ASTNode::Variable, vi);
	ASTExprNode* rightNode = AssignmentParser::processAssignment(rightExpression);
	stmtAssign->addChild(leftNode,1);
	stmtAssign->addChild(rightNode,2);
	(*j)=exIdx;
	return stmtAssign;
}

ASTNode* Parser::processProcedure(int *i, int *j)
{
	stack<char> brackets;
	int currline =*i;
	int tempindex=*j;
	int* line=&currline;
	int* index = &tempindex;

	vector<string> inner=Parser::tokenized_codes.at(*line);
	(*index)++;
	string procName = inner.at(*index);

	PROCIndex pi=PKB::procedures.getPROCIndex(procName);

	ASTNode* procNode = new ASTNode(ASTNode::Procedure, pi );
	ASTStmtLstNode* stmtLstNode=new ASTStmtLstNode();
	
	for(unsigned int idx=*index; idx<inner.size(); idx++)
	{
		if(inner.at(idx)=="{")
		{
			brackets.push('{');
			(*index) = idx + 1;
			break;
		}
	}

	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);
		while(*index < inner.size())
		{
			string keyword=inner.at(*index);

			if (keyword=="procedure"){				
				//throw exception, procedure does not exist in procedure
				throw SPAException("Procedure cannot exist in another procedure!Invalid Source!");
			}

			//do bracket Matching
			if (keyword=="{"){
				brackets.push('{');
				if (brackets.size()>1){
					//throw exception
					throw SPAException("Invalid Source!");
				}
			}

			if (keyword=="}"){
				brackets.pop();
				if(brackets.size()==0){
					(*i)=(*line);
					(*j)=(*index);
					if(stmtLstNode->getSize()==0)
					{
						throw SPAException("Procedure must have one or more child!");
					}
					procNode->addChild(stmtLstNode);
					return procNode;
				}
			}			
				
			if (keyword=="call"){
				ASTStmtNode* callNode=processCall(line, index, pi);
				callNode->setParent(procNode);
				stmtLstNode->addChild(callNode);
				//break;
			}
			
			if(keyword=="while"){
				ASTStmtNode* whileNode=processWhile(line, index, pi);
				whileNode->setParent(procNode);
				stmtLstNode->addChild(whileNode);
				inner=Parser::tokenized_codes.at(*line);
				//break;
			}
			
			if(keyword=="if")
			{
				//No implementation - Not required in CS3201
				//stmtLstNode->addChild(processIf(line));
			}
			
			if(keyword=="="){
				//check for assignment Statement
				//processAssignmentNode
				ASTStmtNode* assignNode=processAssignment(line, index);
				assignNode->setParent(procNode);
				stmtLstNode->addChild(assignNode);
			}
			(*index)++;
		}
		(*index)=0;
		(*line)++;
	}
	
	return NULL;
}