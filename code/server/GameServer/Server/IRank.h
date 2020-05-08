#pragma once

#include "../../System.h"

namespace GameServer {		namespace Server {

class IRank
{
public:
	virtual					~IRank() {}

	virtual void			SetRankTick( CINT nTick ) = 0;
	virtual CINT			GetRankTick() = 0;

	virtual void			SetRank( CSTRING & strRank ) = 0;
	virtual CSTRING *		GetRank() = 0;

};

} /* Server */				} /* GameServer */
