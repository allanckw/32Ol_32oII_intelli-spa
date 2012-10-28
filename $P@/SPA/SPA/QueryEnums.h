#pragma once
#include "stdAfx.h"

class QueryEnums
{
public:
	typedef enum QueryVar { //variable QueryEnums
		WildCard, //can be any type
		Procedure,
		Stmt,
		Assign,
		Call,
		While,
		If,
		Variable,
		Constant
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

	typedef enum QueryCond { //condition attribute QueryEnums
		ProcName,
		VarName,
		Value,
		StmtNo
	};
};