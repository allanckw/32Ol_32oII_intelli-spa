#include "StdAfx.h"
#include "Parser.h"
#include "PKB.h"
#include "SPAException.h"
#include "ASTNode.h"
#include "StmtNode.h"
#include "StmtLstNode.h"
#include "AssignmentParser.h"

//TODO: For Nick, put your parser into this controller
vector<vector<string>> Parser::tokenized_codes;

stack<char> brackets;

Parser::Parser(string fileName)
{
	string codings;
	

	try{
	  string line;
		ifstream myfile (fileName);//CS3201test6.txt");
	  

		if (myfile.is_open()) {
		
			while ( myfile.good() )		{		  
			
				getline(myfile,line);
			
				codings.append(line);
		}
		myfile.close();
	  }
	  else 
		  cout << "Unable to open file"<<endl; 
	  	
	  int currentline = 0;

	tokenizer(codings);

	
	  

	if(brackets.size() != 0){
		throw SPAException("Error during Parsing, Invalid Bracket matching");//error, bracket matching fail
	}
	else
	{
		PKB::maxProgLines = currentline;
	}
	 }catch (exception& e) {
		cout << e.what() << endl;
	} 
}

Parser::~Parser(void)
{
}


bool Parser::isEmpty(string str)
{
	for(int index=0;index<str.size();index++)
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

	//string replacement = Regex.Replace(s, @"\t|\n|\r", "");
	//newtoken = newtoken.replace('\t',);
	//std::remove(newtoken.begin(), newtoken.end(), '\t');
	//newtoken.erase(newtoken,newtoken.end());
	//newtoken.erase(remove(newtoken.begin(), newtoken.end(), '\t'), newtoken.end());

	//string newtoken = newtokens;
	//newtoken.erase(remove(newtoken.begin(), newtoken.end(), '\t'), newtoken.end());
	//newtoken = Trim(newtoken);

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
		if(size != 0 && list.at(size-1) == "procedure")
			PKB::procedures.insertPROC(newtoken);
		else if(size == 0 || list.at(size-1) != "call")
			PKB::variables.insertVAR(newtoken);
			
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

		if(str == "while" || str == "call" || str == "if")
		{
			//new
			Parser::tokenized_codes.push_back(list);
			vector<string> newlist;
			list = newlist;
		}else if(str=="=")
		{
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
	string delimiter = " -+*;{}";//delimiter
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
				for(int i=0;i<tempstr1.size();)
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
		*index = 0;
		
		while(*index < inner.size()) {
			  
			if (*index == 0 && *line == 0 ){
				if (inner.at(*index) == "procedure") {//Assume case sensitive
					PROCIndex i = PKB::procedures.getPROCIndex(inner.at(1));
					if (i != 0){
						  //not the first procedure as parsed...
					 }
					PKB::rootNode->setRoot(i);
					PKB::rootNode->addChild(processProcedure(line));
				}
			 }
			 else
			 {
				  if (inner.at(*index) == "procedure") {
					  PKB::rootNode->addChild(processProcedure(line));
				  }
			 }
			  //cout<<inner.at(*index)<<" ";
			 (*index)++;
		  }
		 cout<<endl;
		 (*line)++;
	}
}

bool Parser::isName(string s) //first char of name cannot be digit
{
	//Check if first char of name is digit or character
	int output; 
	istringstream (s.at(0)) >> output;
	return (output == -858993460);
}

//Reason why i put procIdx is in a while there could be a call statement possibilities
StmtNode* Parser::processWhile(int *i, Index procIdx)
{
	stack<char> brackets;
	int currline =*i;
	int tempindex=0;
	int* line=&currline;
	int* index = &tempindex;

	//Get Control Variable
	vector<string> inner=Parser::tokenized_codes.at(*line);
		
	string varName = inner.at(1);

	if ((isName(varName))==false)
	{
		throw SPAException("Invalid Variable Name!");
	}

	VARIndex vi=PKB::variables.getVARIndex(varName);

	StmtNode* stmtNode = new StmtNode(*line, ASTNode::NodeType::While, vi);
	StmtLstNode* stmtLstNode=new StmtLstNode();

	//Get Next Line
	(*line)++;

	for(int i=0; i<inner.size(); i++)
	{
		if(inner.at(i)=="{")
		{
			brackets.push('{');
		}
	}

	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);
		*index = 0;
		while(*index < inner.size())
		{
			string keyword=inner.at(*index);
			if (keyword=="procedure"){
				//throw exception, procedure does not exist in procedure
			}

			if (keyword=="{"){//do bracket Matching
				brackets.push('{');
				if (brackets.size() > 1){
					//throw exception
				}
			}
			else if (keyword=="}")
			{
				brackets.pop();
				if(brackets.size()==0)
				{
					(*i)=(*line);
					stmtNode->addChild(stmtLstNode, 2);
					return stmtNode;
				}
			}
			
			if(*index==0){	
				if (keyword=="call"){
					StmtNode* callNode=processCall(line, procIdx);
					callNode->setParent(stmtNode);
					stmtLstNode->addChild(callNode);
				}else if(keyword=="while"){
					StmtNode* whileNode=processWhile(line, procIdx);
					whileNode->setParent(stmtNode);
					stmtLstNode->addChild(whileNode);
				}
				else if(keyword=="if"){
					//No implementation - Not required in CS3201
					//procNode->addChild(processIf(line));
				}
				else{//no other keyword
					//variable which is assignment go to next token to check
				}
			}
			else{
				if (*index==1)
				{
					if(keyword=="=")//check for assignment Statement
					{
						//processAssignmentNode
						StmtNode* assignNode=processAssignment(line);
						assignNode->setParent(stmtNode);
						stmtLstNode->addChild(assignNode);
					}
				}
			}
			(*index)++;
		}
		(*line)++;
	}

	//Not reachable
	//To Throw exception as it pass over all the line
	//(*i)=(*line);
	return NULL;
}

//Reason why i put procIdx is in a while there could not be a self call
StmtNode* Parser::processCall(int *i, Index procIdx)
{
	vector<string> inner=Parser::tokenized_codes.at(*i);
	PROCIndex pi = PKB::procedures.getPROCIndex(inner.at(1));
	if(pi==-1)
	{
		//Throw exception, Procedure not in Source
	}
	
	if(pi==procIdx)
	{
		//Throw exception, Procedure cannot self call
	}

	StmtNode* stmtCall=new StmtNode(*i,ASTNode::NodeType::Call,pi);
	return stmtCall;
}

StmtNode* Parser::processAssignment(int *i)
{
	vector<string> inner=Parser::tokenized_codes.at(*i);
	string varName = inner.at(0);

	if (isName(varName)==false){
		throw SPAException("Invalid Name!");
	}

	VARIndex vi=PKB::variables.getVARIndex(varName);

	vector<string> rightExpression;
	int exIdx=2;
	while(exIdx<inner.size())
	{
		rightExpression.push_back(inner.at(exIdx));
		
		if(inner.at(exIdx)==";")
			break;
		exIdx++;
	}

	StmtNode* stmtAssign=new StmtNode(*i,ASTNode::NodeType::Assign,vi);
	ExprNode* leftNode = new ExprNode(ASTNode::NodeType::Variable, vi);
	ExprNode* rightNode = AssignmentParser::processAssignment(rightExpression);
	stmtAssign->addChild(leftNode,1);
	stmtAssign->addChild(rightNode,2);
	return stmtAssign;
}

ASTNode* Parser::processProcedure(int *i)
{
	stack<char> brackets;
	int currline =*i;
	int tempindex=0;
	int* line=&currline;
	int* index = &tempindex;

	vector<string> inner=Parser::tokenized_codes.at(*line);
	string procName = inner.at(1);

	if (isName(procName)==false){
		throw SPAException("Invalid Name!");
	}
	PROCIndex pi=PKB::procedures.getPROCIndex(procName);

	ASTNode* procNode = new ASTNode(ASTNode::NodeType::Procedure, pi );
	StmtLstNode* stmtLstNode=new StmtLstNode();
	//Get Next Line
	(*line)++;

	for(int i=0; i<inner.size(); i++)
	{
		if(inner.at(i)=="{")
		{
			brackets.push('{');
		}
	}

	while(*line < Parser::tokenized_codes.size())
	{
		inner =  Parser::tokenized_codes.at(*line);
		*index = 0;
		while(*index < inner.size())
		{
			string keyword=inner.at(*index);

			if (keyword=="procedure"){				
				//throw exception, procedure does not exist in procedure
			}

			//do bracket Matching
			if (keyword=="{"){
				brackets.push('{');
				if (brackets.size()>1){
					//throw exception
				}
			}

			if (keyword=="}"){
				brackets.pop();
				if(brackets.size()==0){
					(*i)=(*line);
					procNode->addChild(stmtLstNode);
					return procNode;
				}
			}			
				
			if(*index==0){			
				if (keyword=="call"){
					StmtNode* callNode=processCall(line, pi);
					callNode->setParent(procNode);
					stmtLstNode->addChild(callNode);
					break;
				}else if(keyword=="while"){
					StmtNode* whileNode=processWhile(line, pi);
					whileNode->setParent(procNode);
					stmtLstNode->addChild(whileNode);
					break;
				}
				else if(keyword=="if")
				{
					//No implementation - Not required in CS3201
					//stmtLstNode->addChild(processIf(line));
				}
			}
			else{		
				if(*index==1)
				{
					if(keyword=="="){//check for assignment Statement
					//processAssignmentNode
						StmtNode* assignNode=processAssignment(line);
						assignNode->setParent(procNode);
						stmtLstNode->addChild(assignNode);
					}
				}
			}
			(*index)++;
		}
		(*line)++;
	}
	
	return NULL;
}