#include "PQLController.h"
#include "MultiQueryEval.h"
#include "PKB.h"

/**
* Public method for evaluating queries.
* @param query query string
* @param results the list to store the answers of the query
*/
void PQLController::evaluateQuery(const string& query, list<string>& results)
{
	MultiQueryEval::evaluateQuery(query, results);

	PKB::next.tearDownCache();
	PKB::affects.tearDownCache();
}