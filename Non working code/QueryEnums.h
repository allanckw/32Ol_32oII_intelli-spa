#pragma once
#include "stdAfx.h"

class QueryEnums
{
public:
	typedef enum QueryVar { //variable QueryEnums
		WildCard, //can be any type
		Constant, //for type = constant, NOT for anything else
		Procedure,
		Stmt,
		Assign,
		Call,
		While,
		If,
		Variable,
		Expression,
		Boolean //Cannot by a variable type, only select. Put it here because idk where else to put it
	};
	typedef enum QueryRel { //relationship QueryEnums
		Modifies,
		Uses,
		Calls,
		CallsStar,
		Follows,
		FollowsStar,
		Parent,
		ParentStar,
		Next,
		NextStar,
		Affects,
		AffectsStar
	};

	typedef enum QueryPattern{
		Pattern
	};

	typedef enum QueryCond { //condition attribute QueryEnums
		ProcName,
		VarName,
		Value,
		StmtNo
	};
};