#pragma once
#include "StdAfx.h"
#include "SPAException.h"
#include <algorithm>

/**
* Constructor for SPAException, a custom exception used for the SPA program
*/
SPAException::SPAException(){
	
}

/**
* Constructor for SPAException, a custom exception used for the SPA program
* @param msg	the error message of the Exception
*/
SPAException::SPAException(string msg){
	this->message = msg;
}

/**
* Returns a null terminated character sequence containing a generic description of the exception.
* @return the error message that is thrown
*/
const char* SPAException::what() const throw(){
	return message.c_str();
}


