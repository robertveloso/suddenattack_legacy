#pragma once

#include "./Provider.h"
#include "../../System/Memory/Segment.h"

namespace GameServer {		namespace Manager {

class Builder
:	public					IManager::IBuilder
{
public:
	typedef					::Dispatcher::IClient
							ICLIENT;

public:
							Builder();
	virtual					~Builder();

	virtual ICLIENT *		Build( PACKET * i_pPacket, PEER * i_pPeer );
	virtual void			Destroy( IManager * i_pManager );

private:
	typedef					Memory::Segment<IManager::s_nMaxCount, Provider>
							POOL;

private:
	POOL *					m_pPool;

};

} /* Manager */				} /* GamerServer */