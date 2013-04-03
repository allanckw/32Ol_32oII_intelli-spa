#include "TestWrapper.h"
#include <sstream>

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
	ios_base::sync_with_stdio(false);
	//setvbuf(stdout, 0, _IOFBF, 4096);
	//fclose(stdout);
	//freopen(0, "w", stdout);
	try{
		PKBController::initializePKB(filename);
	}catch (exception& e){
		cout<< e.what();
	}
		
}

// method to evaluate a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
	try {
		PQLController::evaluateQuery(query, results);
	} catch (exception& e)
	{
		//Giving u the error result in the results, instead of throwing exception
		//The exception is thrown in the pre-processor when i encounter some weird stuff
		//that you type in in your query, for example, you use a variable that you did not declare
		string s = e.what();
		replace(s.begin(), s.end(), '<', '[');
		replace(s.begin(), s.end(), '<', ']');
		results.push_back(s);
	}
}
