#pragma once

#include "../Dispatcher/IClient.h"
#include "../Share/LogicObject/Player/Type.h"

namespace GameServer {		namespace Player {
class Memento;
class IResult;
} /* Player */				} /* GameServer */

namespace GameServer {

class IPlayer
:	public					::Dispatcher::IClient
{
public:
	static CINT				s_nMaxCount = 4096;
	
	typedef					Player::Memento
							MEMENTO;
	typedef					Player::IResult
							IRESULT;
	typedef					::LogicObject::Player::Type::E_STATE	//½ÇÉ«ÐÐÎª×´Ì¬
							E_STATE;
	typedef					::LogicObject::Player::Type::E_TEAM	//¶ÓÎé¹éÊô
							E_TEAM;

public:
	class					IBuilder
	:	public				::Dispatcher::IClient::IBuilder
	,	public				Singleton<IBuilder>
	{
	public:
		virtual				~IBuilder() {}

//		virtual IClient *	Build( PACKET * i_pPacket, PEER * i_pPeer ) = 0;
		virtual void		Destroy( IPlayer * i_pPlayer ) = 0;

		virtual void		SetLock( CBOOL i_bLock, CBYTE i_byFinMin, CBYTE i_byType ) = 0;
		virtual CBOOL		IsLock() = 0;
		virtual CBYTE		GetFinishType() = 0;
		virtual CBYTE		GetFinishMin() = 0;

	};
	static inline			IBuilder *
	IBUILDER ()				{ return IBuilder::GetInstance(); }

	class					IContainer			//½ÇÉ«ÈÝÆ÷
	:	public				Singleton<IContainer>
	{
	public:
		typedef				String<::LogicObject::Player::Type::s_nUserNick>	//½ÇÉ«ÄÝ³Æ
							NICK;
		typedef				const NICK
							CNICK;
		typedef				String<::LogicObject::Player::Type::s_nUserNo>	//½ÇÉ«NO
							USERNO;
		typedef				const USERNO
							CUSERNO;

	public:
		virtual				~IContainer() {}

		virtual CBOOL		Insert( IPlayer * i_pPlayer ) = 0;
		virtual void		Remove( IPlayer * i_pPlayer ) = 0;

		virtual IPlayer *	Find( CNICK & i_strNick ) = 0;
		virtual IPlayer *	FindUserNo( CUSERNO & i_struserNo ) = 0;

		virtual IPlayer *	First() = 0;
		virtual IPlayer *	Next() = 0;

		virtual CINT		GetCount() = 0;
	};
	static inline			IContainer *
	ICONTAINER()			{ return IContainer::GetInstance(); }

public:
	virtual					~IPlayer() {}

	//virtual PEER *		GetPeer() = 0;

	//virtual void			OnOpen() = 0;
	//virtual void			OnPacket( PACKET * i_pPacket ) = 0;
	//virtual void			OnClose() = 0;
	//virtual void			OnSignal( CUINT i_uiMessage ) = 0;

	//virtual void			OnResponse( IQuery * i_pQuery ) = 0;

	virtual IRESULT *		GetResult() = 0;

	virtual CDWORD			GetIndex() const = 0;
	virtual MEMENTO *		GetMemento() = 0;
//	virtual MEMENTO *		GetMemento() const = 0;

	virtual void			LogoutProcess() = 0;
	virtual void			RecordResult( E_TEAM::ENUM i_eWinner ) = 0;
	virtual void			RecordPenalty() = 0;
	virtual void			CastResult() = 0;

	virtual E_STATE::ENUM	GetState() = 0;

	virtual void			InitPlay() = 0;
	
	virtual void			SetLogined() = 0;
	virtual CBOOL			IsLogined() = 0;
};

} /* GameServer */