#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <string>
#include <iostream>
#include <list>

// include your other headers here
#include "../../SPA/PKB.h"
#include "../../SPA/Parser.h"
#include "../../SPA/ASTNode.h"
#include "../../SPA/StmtLstNode.h"
#include "../../SPA/StmtNode.h"
#include "../../SPA/AssignmentParser.h"
#include "../../SPA/DesignExtractor.h"

#include "AbstractWrapper.h"

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
