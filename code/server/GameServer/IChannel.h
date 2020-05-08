#pragma once

#include "../System.h"

namespace GameServer {
class IRoom;
} /* GameServer */

namespace GameServer {

class IChannel
{
public:
	static CINT				s_nMaxCount = 256;

public:
	class					IFactory
	:	public				Singleton<IFactory>
	{
	public:
		virtual				~IFactory() {}

		virtual IChannel *	Create( CBYTE i_byIndex, CINT i_nMaxUser ) = 0;
		virtual void		Destroy( IChannel * i_pChannel ) = 0;
	};
	static inline			IFactory *IFACTORY()				{ return IFactory::GetInstance(); }

public:
	virtual					~IChannel() {}

	virtual CBYTE			GetIndex() = 0;
	virtual IRoom *			GetLobby() = 0;

	virtual CINT			GetMaxUser() = 0;
	virtual CINT			GetCurrentUser() = 0;

	virtual CINT			GetRoomCount() = 0;
	virtual IRoom *			Search( CINT i_nRoomIndex ) = 0;

	virtual void			AddRoom( IRoom * i_pRoom ) = 0;
	virtual void			RemoveRoom( IRoom * i_pRoom ) = 0;
	
	virtual IRoom *			GetFirstRoom() = 0;
	virtual IRoom *			GetNextRoom() = 0;

};

} /* GameServer */