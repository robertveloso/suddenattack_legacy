#pragma once

#include "../Dispatcher/IClient.h"

namespace GameServer {

class IManager :	public		::Dispatcher::IClient
{
public:
	static CINT				s_nMaxCount = 64;

public:
	class					IBuilder	:	public				::Dispatcher::IClient::IBuilder
										,	public				Singleton<IBuilder>
	{
	public:
		virtual				~IBuilder() {}

//		virtual IClient *	Build( PACKET * i_pPacket, PEER * i_pPeer ) = 0;
		virtual void		Destroy( IManager * i_pMamanger ) = 0;

	};
	static inline			IBuilder *
	IBUILDER ()				{ return IBuilder::GetInstance(); }

public:
	virtual					~IManager() {}

	//virtual PEER *		GetPeer() = 0;

	//virtual void			OnOpen() = 0;
	//virtual void			OnPacket( PACKET * i_pPacket ) = 0;
	//virtual void			OnClose() = 0;
	//virtual void			OnSignal( CUINT i_uiMessage ) = 0;

	//virtual void			OnResponse( IQuery * i_pQuery ) = 0;

};

} /* GameServer */