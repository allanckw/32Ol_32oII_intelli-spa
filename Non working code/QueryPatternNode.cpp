#include "QueryPatternNode.h"

QueryPatternNode::QueryPatternNode()
{
	this->nodeType = Dummy;
}

QueryPatternNode::QueryPatternNode
	(pair<pair<QueryEnums::QueryVar, string>, pair<pair<QueryEnums::QueryVar, string>, pair<QueryEnums::QueryVar, string>>> contents)
{
	this->nodeType = Pattern;
	this->patternVariableType = contents.first.first;
	this->patternVariableName = contents.first.second;
	this->firstPatternParameterType = contents.second.first.first;
	this->firstPatternParameterName = contents.second.first.second;
	this->secondPatternParameterType = contents.second.second.first;
	this->secondPatternParameterName = contents.second.second.second;
}

QueryEnums::QueryVar QueryPatternNode::getPatternVariableType()
{
	return patternVariableType;
}
QueryEnums::QueryVar QueryPatternNode::getFirstPatternParameterType()
{
	return firstPatternParameterType;
}

QueryEnums::QueryVar QueryPatternNode::getSecondPatternParameterType()
{
	return secondPatternParameterType;
}

string QueryPatternNode::getPatternVariableName()
{
	return patternVariableName;
}

string QueryPatternNode::getFirstPatternParameterName()
{
	return firstPatternParameterName;
}

string QueryPatternNode::getSecondPatternParameterName()
{
	return secondPatternParameterName;
}