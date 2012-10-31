#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <string>
#include <iostream>
#include <list>
#include "AbstractWrapper.h"

// include your other headers here
//PKB Includes
#include "../../SPA/PKB.h"
#include "../../SPA/Parser.h"
#include "../../SPA/ASTNode.h"
#include "../../SPA/ASTStmtLstNode.h"
#include "../../SPA/ASTStmtNode.h"
#include "../../SPA/AssignmentParser.h"
#include "../../SPA/DesignExtractor.h"

//Query Includes
#include "../../SPA/QueryTreeBuilder.h"
#include "../../SPA/QueryPreProcessor.h"
#include "../../SPA/QueryParser.h"
#include "../../SPA/QuerySelnode.h"
#include "../../SPA/QueryRelNode.h"
#include "../../SPA/QueryCondNode.h"
#include "../../SPA/QueryProjectNode.h"
#include "../../SPA/IEvalQuery.h"



class TestWrapper : public AbstractWrapper {
 public:
  // default constructor
  TestWrapper();
  
  // destructor
  ~TestWrapper();
  
  // method for parsing the SIMPLE source
  virtual void parse(std::string filename);
  
  // method for evaluating a query
  virtual void evaluate(std::string query, std::list<std::string>& results);

  
};

#endif
