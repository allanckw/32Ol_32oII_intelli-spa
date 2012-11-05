#include "TestWrapper.h"


// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool TestWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	Parser* p = new Parser(filename);
	p->buildAST();
	DesignExtractor::extractDesign();
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
	// Stuff Required to evaluate
	vector<string> tokens; 
	QueryPreprocessor preProcessor;
	QueryParser qParser;
	QueryTreeBuilder* queryTreeBuilder = new QueryTreeBuilder();
	vector<pair<QueryEnums::QueryVar, string>> selected;
	QueryTree QT;
	IEvalQuery* EQ = new IEvalQuery();

	string ans;

	// ...code to evaluate query...
	tokens = qParser.tokenize(query);
	preProcessor.preProcess(tokens);
	queryTreeBuilder->buildQueryTree(preProcessor.getUserVariables(), preProcessor.getSelectVariables(),
									preProcessor.getRelationships(), preProcessor.getConditions());
	
	QT = queryTreeBuilder->getQueryTree();

	vector<string> answers = EQ->evaluateQuery(QT);

	for(int i = 0; i < answers.size(); i++)
	{
		results.push_back(answers.at(i));
	}

	// store the answers to the query in the results list (it is initially empty)

	// each result must be a string.
	
	//Clean up...
	delete EQ;
	delete queryTreeBuilder;
}
