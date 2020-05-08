#pragma once

#include "../INoneSignal.h"

namespace GameServer {		namespace NoneSignal {

class Provider
:	public					INoneSignal
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							Provider();
	virtual					~Provider();

	virtual PEER *			GetPeer();

	virtual void			OnOpen();
	virtual void			OnPacket( PACKET * i_pPacket );
	virtual void			OnClose();
	virtual void			OnSignal( CUINT i_uiMessage );
	virtual void			OnResponse( IQUERY * i_pQuery );

private:
	void					onStart();
	void					onConnectionUpdate();
	void					onLogFile();
	void					onRanking();
};

} /* NoneSignal */			} /* GameServer */