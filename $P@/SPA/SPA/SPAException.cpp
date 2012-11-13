#pragma once
#include "StdAfx.h"
#include "SPAException.h"


SPAException::SPAException(){
	
}

SPAException::SPAException(string msg){
	std::replace(msg.begin(),  msg.end(), '<', ' ');
	std::replace(msg.begin(),  msg.end(), '>', ' ');
	this->message = "None: " + msg;
}

const char* SPAException::what() const throw(){
	return message.c_str();
}


