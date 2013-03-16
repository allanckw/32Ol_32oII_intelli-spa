#include "PQLController.h"
#include "QueryPreprocessor.h"
#include "MultiQueryEval.h"

/**
* Public method for evaluating queries.
* @param query query string
* @return the vector of strings of the answers of the query
*/
list<string> PQLController::evaluateQuery(const string& query)
{
	//read through the query and make sure it is valid and also looking for possible optimisations
	//then the optimisations
	//finally evaluate the relationships in the optimal order
	QueryPreprocessor::validate(query);

	MultiQueryEval result(query);

	if (result.isSelectBoolean() && result.isEarlyQuit())
		return list<string>(1, "FALSE");

	list<string> results;
	for (unsigned i=0; i<result.getFinalAnswer().size(); i++ )
			results.push_back(result.getFinalAnswer()[i]);

	PKB::next.tearDownCache();
	PKB::affects.tearDownCache();

	return results;
}