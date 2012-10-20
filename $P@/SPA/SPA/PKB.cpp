#include "PKB.h"

ASTNode* PKB::rootNode;

VARTable PKB::variables;
PROCTable PKB::procedures;

FollowsTable PKB::follows;
ParentTable PKB::parent;

UsesTable PKB::uses;
ModifiesTable PKB::modifies;
CallsTable PKB::calls;

int PKB::maxProgLines;

PKB::PKB(void)
{
}


PKB::~PKB(void)
{
}
