#pragma once

#include "../../System.h"
#include "../IPlayer.h"
#include "../../Share/LogicObject/Player/Type.h"
#include "../../Share/LogicObject/Player/Character.h"
#include "../../Share/LogicObject/Player/Stock.h"
#include "../../Share/LogicObject/Player/Result.h"

namespace GameServer {
class IRoom;
class IChannel;
} /* GameServer */

namespace GameServer {		namespace Player {
class IState;
} /* Player */				} /* GameServer */
	
namespace GameServer {		namespace Player {

class Memento	//╫ги╚пео╒
{
public:
	typedef					::LogicObject::Player::Type
							TYPE;
	typedef					String< TYPE::s_nUserNo >
							USERNO;
	typedef					String< TYPE::s_nIP >
							IPADDR;
	typedef					::LogicObject::Player::Character
							CHARACTER;
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;
	typedef					Array< E_TEAM::__MAX__, CHARACTER >
							ARRAY_C;
	typedef					::LogicObject::Player::Stock
							STOCK;
	//typedef					::LogicObject::Player::Result
	//						RESULT;

public:
							Memento();
	virtual					~Memento();

	void					Capture( IPlayer * i_pPlayer, const IPADDR & i_strIP );
	void					Release();

	IPlayer *				GetPlayer() const;
	const IPADDR &			GetIP() const;

	void					SetClientIP( const IPADDR & i_strIP );
	const IPADDR &			GetClientIP() const;

	IState *				GetState() const;
	void					SetState( IState * i_pState );

	const USERNO &			GetUserNo() const;
	void					SetUserNo( const USERNO & i_strUserNo );

	CWORD					GetPort() const;
	void					SetPort( CWORD i_wPort );

	IChannel *				GetChannel() const;
	void					SetChannel( IChannel * i_pChannel );

	IRoom *					GetRoom() const;
	void					SetRoom( IRoom * i_pRoom );

	CBOOL					IsReady() const;
	void					SetReady( CBOOL i_bReady );

	CBOOL					IsInventory() const;
	void					SetInventory( CBOOL i_bInventory );

	CBOOL					CanBeSuperPeer() const;
	void					DisableSuperPeer();

	String<17>				GetConnectID() const;
	void					SetConnectID( const String<17> & i_strID );

	ARRAY_C & 				GetCharacters();

	CBOOL					IsNewCharacter() const;
	void					SetNewCharacter();

	const STOCK &			GetStock() const;
	STOCK &					GetStock();

	//const RESULT &			GetResult() const;
	//RESULT &				GetResult();

	E_TEAM::ENUM			GetTeam() const;
	void					SetTeam( E_TEAM::ENUM eTeam );

	CBOOL					IsLoading() const;
	void					SetLoading( CBOOL i_bLoad );

	CBOOL					IsReplayReady() const;
	void					SetReplayReady( CBOOL i_bReady );

	CBOOL					IsIntrusion() const;
	void					SetIntrusion( CBOOL i_bIntrusion );

	CBOOL					IsDead() const;
	void					SetDead( CBOOL i_bDead );
//	void					ResetKill();

	CBOOL					IsSpectator() const;
	void					SetSpectator( CBOOL i_bSpectator );

	void					SetAge( CBYTE i_byAge );
	CBYTE					GetAge();

	void					SetRealName( CBOOL i_bRealName );
	CBOOL					IsRealName();

	void					SetNetLoss( CBYTE i_byLoss );
	CBYTE					GetNetLoss();


private:
	IState *				m_pState;
	IPlayer *				m_pPlayer;
	IPADDR					m_strIP;
	IPADDR					m_strClientIP;
	USERNO					m_strUserNo;
	WORD					m_nPort;
	IRoom *					m_pRoom;
	BOOL					m_bReady;
	BOOL					m_bInventory;
	BOOL					m_bSuperPeer;
	ARRAY_C					m_stCharacters;
	IChannel *				m_pChannel;
	String<17>				m_strConnID;
	BOOL					m_bNewCharacter;

	STOCK					m_stStock;
//	RESULT					m_stResult;
	E_TEAM::ENUM			m_eTeam;

	BYTE					m_byNetLoss;

	BOOL					m_bLoading;
	BOOL					m_bReplayReady;
	BOOL					m_bDead;
	BOOL					m_bSpectator;

	BYTE					m_byAge;
	BOOL					m_bRealName;

	BOOL					m_bIntursion;

};

} /* Player */				} /* GameServer */