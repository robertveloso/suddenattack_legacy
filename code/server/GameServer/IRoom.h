#pragma once

#include "../System.h"

#include "../Share/LogicObject/Room/Type.h"
#include "../Share/LogicObject/Room/Option.h"

namespace GameServer {
class IPlayer;
class IChannel;
} /* GameServer */

namespace GameServer {		namespace Room {
class IResult;
} /* Room */				} /* GameServer */

namespace GameServer {

class IRoom
{
public:
	static CINT				s_nMaxCount = 4096;
	static CINT				s_nMaxPlayer = 12;

public:
	typedef					const String<::LogicObject::Room::Type::s_nPass>
							CPASS;
	typedef					const String<::LogicObject::Room::Type::s_nName>
							CNAME;
	typedef					::LogicObject::Room::Option
							OPTION;
	typedef					const OPTION
							COPTION;
	typedef					Room::IResult
							IRESULT;

public:
	struct E_JOIN {			enum ENUM {
		SUCCESS				= 0,
		FULL,
		WRONG_PASS,
		PLAYING,
	}; /* ENUM */			}; /* E_JOIN */

	class					IFactory
	:	public				Singleton<IFactory>
	{
	public:
		typedef				IRoom::CNAME				CNAME;
		typedef				IRoom::CPASS				CPASS;
		typedef				::LogicObject::Room::Option	OPTION;
		typedef				const OPTION				COPTION;

	public:
		virtual				~IFactory() {}

		virtual IRoom *		Create( IChannel * i_pChannel, IPlayer * i_pPlayer, CNAME & i_strName, CPASS & i_strPass, COPTION & i_stOption ) = 0;
		virtual IRoom *		Create( IChannel * i_pChannel ) = 0;
		virtual void		Destroy( IRoom * i_pRoom ) = 0;

		virtual void		SetSpectatorTime( OPTION::E_MODE::ENUM i_eMode, CINT i_nSec ) = 0;
		virtual CINT		GetSpectatorTime( OPTION::E_MODE::ENUM i_eMode ) = 0;
	};
	static inline			IFactory *
	IFACTORY()				{ return IFactory::GetInstance(); }

public:
	virtual					~IRoom() {}

	virtual CINT			GetIndex() = 0;
	virtual CINT			GetPlayerCount() = 0;
	
	virtual IChannel *		GetChannel() = 0;

	virtual void			AddPlayer( IPlayer * i_pPlayer ) = 0;
	virtual void			RemovePlayer( IPlayer * i_pPlayer ) = 0;
	
	virtual IPlayer *		GetFirstPlayer() = 0;
	virtual IPlayer *		GetNextPlayer() = 0;

	virtual IPlayer *		FindPlayer( CINT i_nIndex ) = 0;

//	virtual CBOOL			IsPass() = 0;
	virtual CPASS &			GetPass() = 0;
	virtual void			SetPass( CPASS & i_strPass ) = 0;
//	virtual void			ReleasePass() = 0;

	virtual CNAME &			GetName() = 0;
	virtual void			SetName( CNAME & i_strName ) = 0;

	virtual IPlayer *		GetCaptain() = 0;
//	virtual void			ChangeCaptain() = 0;
	virtual void			SetCaptain( IPlayer * i_pCaptain ) = 0;

	virtual IPlayer *		GetSuperPeer() = 0;
	virtual void			SetSuperPeer( IPlayer * i_pSuperPeer ) = 0;

	virtual IPlayer *		GetTempSuperPeer() = 0;
	virtual void			SetTempSuperPeer( IPlayer * i_pSuperPeer ) = 0;

	//virtual void			SetSuperPeerPort( CWORD i_wPort ) = 0;
	//virtual CWORD			GetSuperPeerPort() = 0;

	virtual E_JOIN::ENUM	IsJoinable( CPASS & i_strPass ) = 0;

	virtual COPTION &		GetOption() const = 0;
	virtual OPTION &		GetOption() = 0;
	virtual void			SetOption( COPTION & i_stOption ) = 0;

	virtual IRESULT *		GetResult() = 0;

	virtual void			SetPause( CBOOL i_bPause ) = 0;
	virtual CBOOL			IsPause() = 0;

	virtual void			SetBombReady( CBOOL i_bReady ) = 0;
	virtual CBOOL			IsBombReady() = 0;

	virtual void			CheckWinner() = 0;

	virtual void			ResetStartTick() = 0;
	virtual CBOOL			IsSpectatorTick() = 0;

	virtual void			SetNat( CBOOL i_bNat ) = 0;
	virtual CBOOL			IsNat() = 0;

};

} /* GameServer */