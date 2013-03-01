#include "PQLController.h"
#include "QueryPreprocessor.h"
#include "MultiQueryEval.h"

/**
* Public method for evaluating queries.
* Passes call to the MultiQueryEval constructor. Only adds in the answer of FALSE
* if the evaluation was returned early and the select was for a BOOLEAN.
* @param query query string
* @return the vector of strings of the answers to the query
*/
vector<string> PQLController::evaluateQuery(const string& query)
{
	//read through the query and make sure it is valid and also looking for possible optimisations
	//then the optimisations
	//finally evaluate the relationships in the optimal order
	QueryPreprocessor::validate(query);

	MultiQueryEval result(query);

	if (result.getSelectBoolean() && result.getEarlyQuit())
		return vector<string>(1, "FALSE");

	return result.getFinalAnswer();
}