#pragma once

#include "../IRank.h"

namespace GameServer {		namespace Server {			namespace Rank {

class Provider
:	public					IRank
{
public:
							Provider();
	virtual					~Provider();

	virtual void			SetRankTick( CINT nTick );
	virtual CINT			GetRankTick();

	virtual void			SetRank( CSTRING & strRank );
	virtual CSTRING *		GetRank();

private:
	STRING					m_strRank;
	BOOL					m_bLoaded;
	INT						m_nTick;

};

} /* Rank */				} /* Server */				} /* GameServer */
