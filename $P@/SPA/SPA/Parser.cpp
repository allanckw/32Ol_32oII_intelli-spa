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

vector<vector<string>> Parser::tokenized_codes;

stack<char> brackets;

/**
* This method will be used to create a new parser with a source file
* @param fileName The location of source file
*/
Parser::Parser(string fileName)
{
	string codings;
	string line;
	  
	ifstream myfile (fileName);

	if (myfile.is_open()) {
		while ( myfile.good() ) {		  
			getline(myfile,line);
			int temp = line.find("//");
			if(temp >= 0) 
				line = line.substr(0, temp);

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
}


/**
* This method will check if the string for special character
* @param newtoken string of token being tested
*/
bool Parser::isStrCheckNoSpecialChar(string newtoken)
 {
	 int result = newtoken.find_first_of("!@#$%^&*()_+{}|\\][~`", 0);
	 return result != string::npos;
 }

/**
* This method will be used to check for empty string in a string
* @param str The string to be check for empty space
*/
bool Parser::isEmpty(string str)
{
	for(size_t index=0;index<str.size();index++){
		if(str.at(index)!= ' ')
			return false;
	}
	return true;
}

/**
* This method will be used to check whether the string is number
* @param str The string to be check is it number
*/
bool Parser::is_number(const std::string& str)
{
    std::string::const_iterator currentIterator = str.begin();
    while (currentIterator != str.end() && isdigit(*currentIterator)) ++currentIterator;
    return !str.empty() && currentIterator == str.end();
}

/**
* This method will be used to trim string
* @param str The string to be trimmed
*/
string Parser::Trim(string str)
{
	str.erase(remove(str.begin(), str.end(), '\t'), str.end());
	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	return str;
}

/**
* This method will be used to add the token to its respective variable table of procedure table if needed
* @param list the list of tokenized codes
* @param newtoken the token being added
*/
void Parser::AddTables(vector<string> list, string newtoken)
{
	int size = list.size();

	if(newtoken.size() == 0 || newtoken.compare("while") == 0 || newtoken.compare("if") == 0 || newtoken.compare("procedure") == 0 || 
		newtoken.compare("+") == 0 || newtoken.compare("-") == 0 || 
		newtoken.compare("(") == 0 ||
		newtoken.compare(")") == 0 ||
		newtoken.compare("*") == 0 ||  newtoken.compare("=") == 0 ||
		newtoken.compare("call") == 0||newtoken.compare("then") == 0||
		newtoken.compare("else") == 0||newtoken.compare(";") == 0)//found a reserve token
	{
		return;
	}else if(newtoken[0] == '{' || newtoken[0] == '}'){ //found a bracket
		
		if(newtoken[0] == '{' )	{
			brackets.push('{');//push open braket to stack
		}else if(newtoken[0] == '}' ){
			
			if(brackets.size() == 0)	{
				throw SPAException("Error during Parsing, Invalid Bracket matching");//error bracket matching
			}
			else if(brackets.top() != '{'){
				throw SPAException("Error during Parsing, Invalid Bracket matching"); //error bracket matching
			}
			brackets.pop();//pop the bracket from the stack
		}
	}else if(!isdigit(newtoken[0]))//if currentoken is not a number hence may be a variable or procedure
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
	}else{
		if(size != 0 && list.at(size-1) == "procedure")
			throw SPAException("Error during Parsing, Invalid Procedure Starting Number");//error procedure start with a number
		else if(!is_number(newtoken))
		{
			throw SPAException("Error during Parsing, Invalid Variable Starting Number");//error variable start with a number
		}
	}
}

/**
* This method will determine if a the token needs to be on the next line and add it into the list
* @param list multiple list of codes where each line represent a program line
* @param str the code being added
*/
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

/**
* This method will tokenize the code into a list of tokens
* @param line the line of code being tokenized
*/
void Parser::tokenizer(string line)//split the string into tokens
{
	vector<string> list;
	string delimiter = " ()-+*;{}=";//delimiters
	int position = 0;//starting position
	int startindex = -1;
	int endindex = -1;
	
	do	{ //loop thru the string
		startindex = line.find_first_not_of(delimiter,position);

		if(startindex == 42) {
			int lolt = 1;
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
				for(size_t i=0;i<tempstr1.size();) {
					string tempstr2 = tempstr1.substr(0,1);
					tempstr1 = tempstr1.substr(1,tempstr1.size()-1);
					
						AddToList(list,tempstr2);
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

				AddToList(list,tempstr);
				
			}

	}while(startindex != -1 && position < line.size() && endindex != -1);

	if(list.size() > 0)
	Parser::tokenized_codes.push_back(list);

	//house keeping
	if(Parser::tokenized_codes.size() > 1)	{
		vector<string> temp_vec = Parser::tokenized_codes.at(0);
		temp_vec.insert(temp_vec.end(), Parser::tokenized_codes.at(1).begin(),Parser::tokenized_codes.at(1).end());
		Parser::tokenized_codes.erase(Parser::tokenized_codes.begin());
		Parser::tokenized_codes.at(0) = temp_vec;
	}
}

/**
* This method will be used to buld AST Tree
*/
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

/**
* This method will be used to for name grammer rule
* @param s The string to be checked
*/
bool Parser::isName(string s) //first char of name cannot be digit
{
	return (!isdigit(s.at(0)));
}

/**
* This method will be used to process while statement and build While AST Node
* @param *i A pointer to keep track of current line index
* @param *j A pointer to keep track of current word index of the line
* @param procIdx For checking of self call
*/
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

	for(size_t idx=*index; idx<inner.size(); idx++)
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
				ASTStmtNode* ifNode=processIf(line, index, procIdx);
				ifNode->setParent(stmtNode);
				stmtLstNode->addChild(ifNode);
				inner=Parser::tokenized_codes.at(*line);
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

/**
* This method will be used to process If statement and build If AST Node
* @param *i A pointer to keep track of current line index
* @param *j A pointer to keep track of current word index of the line
* @param procIdx For checking of self call
*/
ASTStmtNode* Parser::processIf(int *i, int *j, Index procIdx)
{
	stack<char> brackets;
	int state=0; //1 is 'then', 2 is 'else'
	int currline =*i;
	int tempindex= *j;
	int* line=&currline;
	int* index = &tempindex;

	//Get Control Variable
	vector<string> inner=Parser::tokenized_codes.at(*line);
	(*index)++;
	string varName = inner.at(*index);

	VARIndex vi=PKB::variables.getVARIndex(varName);

	ASTStmtNode* stmtNode = new ASTStmtNode(*line, ASTNode::NodeType::If, vi);
	ASTStmtLstNode* thenStmtLstNode=new ASTStmtLstNode();
	ASTStmtLstNode* elseStmtLstNode=new ASTStmtLstNode();
	(*index)++;

	state=1;
	if (inner.at(*index)!="then"){				
			//throw exception, procedure does not exist in procedure
			throw SPAException("Invalid Source!Keyword 'Then' is expected!");
	}

	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);

		while(*index < inner.size())
		{
			string keyword=inner.at(*index);

			if (keyword=="procedure"){				
				//throw exception, procedure does not exist in procedure
				throw SPAException("Procedure cannot appear in If!Invalid Source!");
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
					if(thenStmtLstNode->isHasChildren()==false)
					{
						throw SPAException("Stmt Must have one or more child!");
					}
					stmtNode->addChild(thenStmtLstNode);
					(*index)++;
					state=2;
					break;
				}
			}			
				
			if (keyword=="call"){
				ASTStmtNode* callNode=processCall(line, index, procIdx);
				callNode->setParent(stmtNode);
				thenStmtLstNode->addChild(callNode);
				//break;
			}
			
			if(keyword=="while"){
				ASTStmtNode* whileNode=processWhile(line, index, procIdx);
				whileNode->setParent(stmtNode);
				thenStmtLstNode->addChild(whileNode);
				inner=Parser::tokenized_codes.at(*line);
				//break;
			}
			
			if(keyword=="if")
			{
				//No implementation - Not required in CS3201
				ASTStmtNode* ifNode=processIf(line, index, procIdx);
				ifNode->setParent(stmtNode);
				thenStmtLstNode->addChild(ifNode);
				inner=Parser::tokenized_codes.at(*line);
			}
			
			if(keyword=="="){
				//check for assignment Statement
				//processAssignmentNode
				ASTStmtNode* assignNode=processAssignment(line, index);
				assignNode->setParent(stmtNode);
				thenStmtLstNode->addChild(assignNode);
			}
			(*index)++;
		}
		if(state==1)
		{
		(*index)=0;
		(*line)++;
		}

		if (state==2)
			break;
	}
	
	//Processing Else Statement
	string NextKey=inner.at(*index);
	if (NextKey!="else"){				
			//throw exception, procedure does not exist in procedure
			throw SPAException("Invalid Source!Keyword 'else' is expected!");
	}

	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);

		while(*index < inner.size())
		{
			string keyword=inner.at(*index);

			if (keyword=="procedure"){				
				//throw exception, procedure does not exist in procedure
				throw SPAException("Procedure cannot appear in If!Invalid Source!");
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
					if(elseStmtLstNode->isHasChildren()==false)
					{
						throw SPAException("Stmt Must have one or more child!");
					}
					stmtNode->addChild(elseStmtLstNode);
					return stmtNode;
				}
			}

			if (keyword=="call"){
				ASTStmtNode* callNode=processCall(line, index, procIdx);
				callNode->setParent(stmtNode);
				elseStmtLstNode->addChild(callNode);
				//break;
			}
			
			if(keyword=="while"){
				ASTStmtNode* whileNode=processWhile(line, index, procIdx);
				whileNode->setParent(stmtNode);
				elseStmtLstNode->addChild(whileNode);
				inner=Parser::tokenized_codes.at(*line);
				//break;
			}
			
			if(keyword=="if")
			{
				//No implementation - Not required in CS3201
				ASTStmtNode* ifNode=processIf(line, index, procIdx);
				ifNode->setParent(stmtNode);
				elseStmtLstNode->addChild(ifNode);
				inner=Parser::tokenized_codes.at(*line);
			}
			
			if(keyword=="="){
				//check for assignment Statement
				//processAssignmentNode
				ASTStmtNode* assignNode=processAssignment(line, index);
				assignNode->setParent(stmtNode);
				elseStmtLstNode->addChild(assignNode);
			}
			(*index)++;
		}
		(*index)=0;
		(*line)++;
	}
	return NULL;
}

/**
* This method will be used to process Call statement and build Call AST Node
* @param *i A pointer to keep track of current line index
* @param *j A pointer to keep track of current word index of the line
* @param procIdx For checking of self call
*/
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

/**
* This method will be used to process Assignment statement and build Assignment AST Node
* @param *i A pointer to keep track of current line index
* @param *j A pointer to keep track of current word index of the line
*/
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

/**
* This method will be used to process Procedure statement and build Procedure AST Node
* @param *i A pointer to keep track of current line index
* @param *j A pointer to keep track of current word index of the line
*/
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
	
	for(size_t idx=*index; idx<inner.size(); idx++)
	{
		if(inner.at(idx)=="{")
		{
			brackets.push('{');
			(*index) = idx + 1;
			break;
		}
	}
	int checkLeftChar = 0;

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
			else if (keyword=="{"){
				brackets.push('{');
				if (brackets.size()>1){
					//throw exception
					throw SPAException("Invalid Source!");
				}
			}

			else if (keyword=="}"){
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
				
			else if (keyword=="call"){
				if(checkLeftChar > 0)
					throw SPAException("Invalid Code!");

				ASTStmtNode* callNode=processCall(line, index, pi);
				callNode->setParent(procNode);
				stmtLstNode->addChild(callNode);
				checkLeftChar = 0;
				//break;
			}
			
			else if(keyword=="while"){
				ASTStmtNode* whileNode=processWhile(line, index, pi);
				whileNode->setParent(procNode);
				stmtLstNode->addChild(whileNode);
				inner=Parser::tokenized_codes.at(*line);
				//break;
			}
			
			else if(keyword=="if")
			{
				//No implementation - Not required in CS3201
				ASTStmtNode* ifNode=processIf(line, index, pi);
				ifNode->setParent(procNode);
				stmtLstNode->addChild(ifNode);
				inner=Parser::tokenized_codes.at(*line);
			}
			
			else if(keyword=="="){
				if(checkLeftChar > 1)
					throw SPAException("Invalid Code!");
				//check for assignment Statement
				//processAssignmentNode
				ASTStmtNode* assignNode=processAssignment(line, index);
				assignNode->setParent(procNode);
				stmtLstNode->addChild(assignNode);
				checkLeftChar = 0;
				//v=false;
			}
			/*else if(keyword == "callRingo")
			{
				int ll=1;
			}*/
			else if(keyword == ";")
			{

				if(checkLeftChar > 0)
					throw SPAException("Invalid Code!");
				checkLeftChar = 0;
			}
			else
				checkLeftChar++;
				
			(*index)++;
		}
		(*index)=0;
		(*line)++;
	}
	
	return NULL;
}