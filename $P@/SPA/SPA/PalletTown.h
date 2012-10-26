#pragma once
#include "stdAfx.h"

class PalletTown
{
public:
	typedef enum ashKetchum { //variable PalletTown
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
	typedef enum garyOak { //relationship PalletTown
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

	typedef enum mistyWaterflower { //condition attribute PalletTown
		ProcName,
		VarName,
		Value,
		StmtNo
	};
};