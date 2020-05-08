#pragma once

#include "../Dispatcher/IClient.h"

namespace GameServer {

class INoneSignal
:	public					::Dispatcher::IClient
,	public					Singleton<INoneSignal>
{
public:
	struct E_SIGNAL {		enum ENUM {
		START				= 100,
		CONNECTION_UPDATE,
		RANKING,

		LOG_FILE,
	}; /* ENUM */			}; /* E_SIGNAL */

public:
	virtual					~INoneSignal() {}

	//virtual PEER *		GetPeer() = 0;

	//virtual void			OnOpen() = 0;
	//virtual void			OnPacket( PACKET * i_pPacket ) = 0;
	//virtual void			OnClose() = 0;
	//virtual void			OnSignal( CUINT i_uiMessage ) = 0;

	//virtual void			OnResponse( IQuery * i_pQuery ) = 0;

};
static inline				INoneSignal *
INONESIGNAL()				{ return INoneSignal::GetInstance(); }

} /* GameServer */