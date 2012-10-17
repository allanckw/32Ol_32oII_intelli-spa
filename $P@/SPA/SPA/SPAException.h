#pragma once
#include <exception>
#include "stdafx.h"

class SPAException : 
		public exception{ 

public:
	string message;

	SPAException();
	SPAException(string);

	virtual const char* what() const throw();
};
