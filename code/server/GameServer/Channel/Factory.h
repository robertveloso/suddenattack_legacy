#pragma once

#include "./Provider.h"

#include "../../System/Memory/Segment.h"

namespace GameServer {		namespace Channel {

class Factory
:	public					IChannel::IFactory
{
public:
							Factory();
	virtual					~Factory();

	virtual IChannel *		Create( CBYTE i_byIndex, CINT i_nMaxUser );
	virtual void			Destroy( IChannel * i_pChannel );

private:
	typedef					Memory::Segment<IChannel::s_nMaxCount, Provider>
							POOL;

private:
	POOL *					m_pPool;

};

} /* Channel */				} /* GameServer */