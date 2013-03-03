#include "TestWrapper.h"
#include <iterator>

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
	
	try{
		PKBController::initializePKB(filename);
	}catch (exception& e){
		cout<< e.what();
	}
		
}

// method to evaluate a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
	try{
		vector<string> answers = PQLController::evaluateQuery(query);
		std::copy (answers.begin(), answers.end(), std::back_inserter(results));
	}catch (exception& e)	{
		results.push_back(e.what());
	}
}
