#pragma once
#include "StdAfx.h"
#include "SPAException.h"


SPAException::SPAException(){
	
}

SPAException::SPAException(string msg){
	this->message = msg;
}

const char* SPAException::what() const throw(){
	return message.c_str();
}


