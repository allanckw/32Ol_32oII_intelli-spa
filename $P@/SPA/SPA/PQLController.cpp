#include "PQLController.h"
#include "MultiQueryEval.h"
#include "PKB.h"
#include "Dnf.h"
/**
* Public method for evaluating queries.
* @param query query string
* @param results the list to store the answers of the query
*/
void PQLController::evaluateQuery(const string& query, list<string>& results)
{
	size_t open = std::count(query.begin(), query.end(), '(');
	size_t close = std::count(query.begin(), query.end(), ')');

	int or = query.find("or");
	int  emptybracketClause = query.find("()");

	if (emptybracketClause > 0)
		throw SPAException("Empty Bracket Clause Found!");


	if (open != close)
		throw SPAException("Bracket matching Failed!");

	if(or > 0) {
		Dnf::Eval(query, results);
	} else {
		MultiQueryEval::evaluateQuery(query, results);
	}

	PKB::next.tearDownCache();
	PKB::affects.tearDownCache();
}

