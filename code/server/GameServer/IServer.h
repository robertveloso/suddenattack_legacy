#pragma once

#include "../System.h"

namespace GameServer {
class IChannel;
} /* GameServer */

namespace GameServer {		namespace Server {
class IRank;
} /* Server */				} /* GameServer */

namespace GameServer {

class IServer
:	public					Singleton<IServer>
{
public:
	typedef					Server::IRank
							IRANK;

public:
	virtual					~IServer() {}

	virtual void			Open( CSTRING & i_strIP, CINT i_nPort ) = 0;
	
	virtual CSTRING &		GetID() = 0;
	virtual void			SetID( CSTRING & i_strID ) = 0;

	virtual CSTRING &		GetName() = 0;
	virtual void			SetName( CSTRING & i_strName ) = 0;

	virtual CSTRING &		GetIP() = 0;
	virtual CINT			GetPort() = 0;

	virtual CINT			GetChannelCount() = 0;
	virtual IChannel *		Search( CBYTE i_byIndex ) = 0;

	virtual void			AddChannel( IChannel * i_pChannel ) = 0;
	virtual void			RemoveChannel( IChannel * i_pChannel ) = 0;
	
	virtual IChannel *		GetFirstChannel() = 0;
	virtual IChannel *		GetNextChannel() = 0;

	virtual void			SetLoginGamehi( CBOOL i_bCan ) = 0;
	virtual void			SetLoginNetmarble( CBOOL i_bCan ) = 0;

	virtual CBOOL			CanLoginGamehi() = 0;
	virtual CBOOL			CanLoginNetmarble() = 0;

	virtual IRANK *			GetRank() = 0;

};
static inline				IServer *
ISERVER()					{ return IServer::GetInstance(); }

} /* GameServer */
