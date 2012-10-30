#pragma once
#include "StdAfx.h"
#include "SPAException.h"


SPAException::SPAException(){
	message = "An exception has occured!";
}

SPAException::SPAException(string msg){
	this->message = "An exception has occured: " + msg;
}

const char* SPAException::what() const throw(){
	return message.c_str();
}


