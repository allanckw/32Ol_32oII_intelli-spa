// ast1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ASTNode.h"
#include "StmtNode.h"
#include "CallNode.h"

using namespace std;


bool IsEmpty(string str)
{
	for(int i=0;i<str.size();i++)
	{
		if(str.at(i)!= ' ')
			return false;
	}
	return true;
}
void error()
{
	cout<<"error"<<endl;
	system("PAUSE");
	throw 20;
}



vector<string> tokenizer3(string line)
{
	vector<string> list;
	string delimiter = " -+/*;{}";
	int position = 0;
	int startindex = -1;
	int endindex = -1;
	do//while(!line.compare(""))
	{
		startindex = line.find_first_not_of(delimiter,position);


		if(endindex != -1 && endindex<line.size())
			{
				string temp;
				if(startindex == -1)
				{
					temp = line.substr(endindex,line.size() - endindex);
				}
				else
				{
					temp = line.substr(endindex,startindex - endindex);
				}
				for(int i=0;i<temp.size();)
				{
					string tempstr = temp.substr(0,1);
					temp = temp.substr(1,temp.size()-1);
					if(tempstr != " ")
						list.push_back(tempstr);
				}
				//int a =1;
			}

		endindex = line.find_first_of(delimiter,startindex);

		position = endindex;
		
		if(startindex != -1)
		{
			string temp = line.substr(startindex,endindex-startindex);
			list.push_back(temp);			
		}

	}while(startindex != -1 && position < line.size() && endindex != -1);
	
	return list;
}







ASTNode* ProcessAssignment(ASTNode* procNode, int &line, vector<string> progline)
{

	int index = 0;

	// For each token   
	for ( int i = 0; i < progline.size(); i++ )   
	{
		//http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
		//http://www.technical-recipes.com/2011/a-mathematical-expression-parser-in-java/
		string token = progline[i]; 

		//if (isOperator(token))
		//{
		//}
		//else
		//{
		//}
	}

	return procNode;
}

ASTNode* ProcessProcedure(ASTNode* procNode, int &line, vector<vector<string>> &tokenized_codes)
{
	stack<string> brackets;
	vector<string> progline = tokenized_codes.at(line);
	//if the first line of the proc, its 3rd token or 1st token of the next line isnt a bracket then its an error~
	if ((progline.at(2) == "{") || (tokenized_codes.at(line+1).at(0) == "{"))
	{
		//AHH A VALID PROCEDURE d: 
		brackets.push("{");//push it in~
		while (!brackets.empty()) //while the brackets is not empty~
		{
			line++;
			if (line < tokenized_codes.size()){
				progline = tokenized_codes.at(line);
				
				auto result = find(progline.begin(), progline.end(), "=") != progline.end();

				if (result == true) //if there is an equal sign then its an assignment~
				{
					ASTNode* assnNode = ProcessAssignment(procNode, line, progline); 
					procNode->AddChild(assnNode);
				}
			}
			else
			{

			}
		}
	}
	else
	{
		//Invalid Program, Parsing Failure Expect { but was something else~
	}
	
	return NULL;
}




//Program CreateProgramNode(vector<vector<string>> data)
//{
//	Program pNode("");
//
//	return pNode;
//}
//
//Procedure CreateProcedureNode(vector<vector<string>> data)
//{
//	Procedure pNode("",0);
//
//	return pNode;
//}

int main(int argc, char* arg[])
{
	/*stack<string> commastack;
	
	stack<Stmt> smtmstack;
*/
	vector<string> codings;

	vector<vector<string>> tokenized_codes;

	string line;
	
	StmtNode *s = new CallNode();

	s->SetRoot();
	try{
	  ifstream myfile ("C:\\CS3201test1.txt");
	  
	  if (myfile.is_open())
	  {
		while ( myfile.good() )
		{
		  getline (myfile,line);
		  //cout << line << endl;
	
		  codings.push_back(line);

		 // codes.append(line);
		}
		myfile.close();
	  }

	  else cout << "Unable to open file"<<endl; 

	  int currentline = 0;
	  int totalsize = 0;
	  while(currentline < codings.size())
	  {
		  line = codings.at(currentline);
		  vector<string> aa = tokenizer3(line);
		  tokenized_codes.push_back(aa);
		  currentline++;

	  }
	  int templine =0;
	  int tempindex=0;
	  int* line=&templine;
	  int* index = &tempindex;

	  ASTNode* root;

	  vector<vector<string>> data;

	  //Vector of code form..
	  while(*line < tokenized_codes.size())
	  {
		  vector<string> inner = tokenized_codes.at(*line);
		  *index = 0;
		  while(*index < inner.size())
		  {
			  //if(inner.at(*index) == " ")
			  //{
				 // cout << tolower('H');
			  //}
			  //First Line of code and first token must be procedure~
			  if (*index == 0 && *line == 0 )
			  {
				  if (inner.at(*index) == "procedure") //Assume case sensitive
				  {
					  root = new ASTNode(Program, inner.at(*index+1));
					  root->SetRoot();

					  ASTNode* proc = new ASTNode(Procedure, inner.at(*index+1));
					  proc->SetParent(root);

					  ProcessProcedure(proc, *line, tokenized_codes);
					  root->AddChild(proc);
				  }
				  else
				  {
					  //Invalid Program
				  }
			  }

			  //cout<<inner.at(*index)<<" ";
			  (*index)++;
		  }
		  cout<<endl;
		 // (*line)++;

	  }

	  system("PAUSE");
	}
	catch(int e)
	{
		return 0;
	}

	  
}


