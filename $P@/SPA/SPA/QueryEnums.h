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

	typedef enum QueryReladition { //relationship QueryEnums
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
		AffectsStar,
		Pattern,
		ProcName,
		VarName,
		Value,
		StmtNo
	};
};