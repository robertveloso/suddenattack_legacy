#pragma once

#include "../System.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace Dispatcher {		namespace Handler {
class Peer;
} /* Handler */				} /* Dispatcher */

namespace Dispatcher {
class IQuery;
} /* Dispatcher */


namespace Dispatcher {

class IClient	
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;
	typedef					Handler::Peer
							PEER;

public:
	class					IBuilder 
	{
	public:
		typedef				::Application::Packets::CPacket
							PACKET;
		typedef				Handler::Peer
							PEER;

	public:
		virtual				~IBuilder() {}
		
		virtual IClient *	Build( PACKET * i_pPacket, PEER * i_pPeer ) = 0;
	};

	class					IRegister 
	:	public				Singleton<IRegister>
	{
	public:
		typedef				IClient::IBuilder
							IBUILDER;

	public:
		virtual				~IRegister() {}

		virtual void		Regist( IBUILDER * pBuilder ) = 0;
		virtual IBUILDER *	First() = 0;
		virtual IBUILDER *	Next() = 0;
	};
	static inline			IRegister *
	IREGISTER()				{ return IRegister::GetInstance(); };

	class					ISession	
	:	public				Singleton<ISession>
	{
	public:
		typedef				::Application::Packets::CPacket
							PACKET;

	public:
		virtual				~ISession() {}

		virtual void		Alarm( CUINT i_uiMessage, CUINT i_uiSecond, IClient * i_pClient ) = 0;	//NoneSignal中对数据库操作之后的调用，main在start之前调用
		virtual void		Signal( CUINT i_uiMessage, IClient * i_pClient ) = 0;//Caster::Provider发生错误是调用
		virtual void		Send( PACKET * i_pPacket, IClient * i_pClient ) = 0;//Caster::Provider cast时调用
		virtual void		Close( IClient * i_pClient ) = 0;

		virtual CDWORD		GetRTT( IClient * i_pClient ) = 0;

	};
	static inline			ISession *
	ISESSION()				{ return ISession::GetInstance(); }

public:
	virtual					~IClient() {}

	virtual PEER *			GetPeer() = 0;

	virtual void			OnOpen() = 0;
	virtual void			OnPacket( PACKET * i_pPacket ) = 0;
	virtual void			OnClose() = 0;
	virtual void			OnSignal( CUINT i_uiMessage ) = 0;
	
	virtual void			OnResponse( IQuery * i_pQuery ) = 0;

};

} /* Dispatcher */