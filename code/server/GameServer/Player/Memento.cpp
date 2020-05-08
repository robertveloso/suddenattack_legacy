#include "StdAfx.h"

#include "./Memento.h"
#include "./State/Login/Init.h"

#include "./State/Fin.h"

#include "./State/Login/Character.h"
#include "./State/Login/Closing.h"
#include "./State/Login/Init.h"
#include "./State/Login/Lobby.h"
#include "./State/Login/Logined.h"
#include "./State/Login/Logining.h"
#include "./State/Login/Nick.h"
#include "./State/Login/NoneCharacter.h"
#include "./State/Login/NoneNick.h"
#include "./State/Login/Ready.h"

#include "./State/Buff/BuffRoom.h"
#include "./State/Buff/ReturnBuff.h"
#include "./State/Buff/Inventory.h"
#include "./State/Buff/PlayReady.h"

#include "./State/Play/BombRoom.h"
#include "./State/Play/BombReady.h"
#include "./State/Play/RespawnRoom.h"

#include "../../Dispatcher/IConsole.h"
#include "../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {

Memento::Memento()
{
}

Memento::~Memento()
{
}

void
Memento::Capture( IPlayer * i_pPlayer, const IPADDR & i_strIP )
{
	m_pState				= State::Login::INIT();
	m_pPlayer				= i_pPlayer;
	m_strIP					= i_strIP;
	m_nPort					= 0;
	m_pChannel				= NULL;
	m_pRoom					= NULL;
	m_bReady				= FALSE;
	m_bSuperPeer			= TRUE;
	m_bDead					= FALSE;
	m_bSpectator			= FALSE;
	m_bNewCharacter			= FALSE;
	m_stStock.m_byGrade		= 0;

	STRING strLog;
	strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\tIP(%s)\r\n",
		::Dispatcher::ITIME()->GetTime().GetCSTR(),
		"State",
		"INIT",
		m_pPlayer->GetIndex(),
		m_strIP.GetCSTR());
	::Dispatcher::ICONSOLE()->Write( strLog);
}

void
Memento::Release()
{
	Assert( m_pRoom == NULL );

	m_pState				= NULL;
	m_pPlayer				= NULL;
}

IPlayer *
Memento::GetPlayer() const
{
	return m_pPlayer;
}

const Memento::IPADDR &
Memento::GetIP() const
{
	return m_strIP;
}

void
Memento::SetClientIP( const IPADDR & i_strIP )
{
	m_strClientIP			= i_strIP;
}

const Memento::IPADDR &
Memento::GetClientIP() const
{
	return m_strClientIP;
}

IState *
Memento::GetState() const
{
	return m_pState;
}

void
Memento::SetState( IState * i_pState )
{
	Assert( i_pState != NULL );
	m_pState				= i_pState;


	STRING strLog;
	if( i_pState == State::FIN() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"FIN",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::CHARACTER() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"CHARACTER",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::CLOSING() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"CLOSING",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::INIT() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"INIT",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::LOBBY() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"LOBBY",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::LOGINED() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"LOGINED",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::LOGINING() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"LOGINING",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::NICK() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"NICK",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::NONECHARACTER() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"NONECHARACTER",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::NONENICK() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"NONENICK",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Login::READY() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"READY",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Buff::BUFFROOM() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"BUFFROOM",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Buff::RETURNBUFF() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"RETURNBUFF",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Buff::INVENTORY() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"INVENTORY",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Buff::PLAYREADY() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"PLAYREADY",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Play::BOMBROOM() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"BOMBROOM",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Play::BOMBREADY() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"BOMBREADY",
					m_pPlayer->GetIndex() );
	}
	else if( i_pState == State::Play::RESPAWNROOM() )
	{
		strLog.Printf( "Dev\t%s\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"State",
					"RESPAWNROOM",
					m_pPlayer->GetIndex() );
	}
	else
	{
		strLog.Printf( "Dev\t???????????????\r\n" );
	}

	::Dispatcher::ICONSOLE()->Write( strLog );
}

const Memento::USERNO &
Memento::GetUserNo() const
{
	return m_strUserNo;
}

void
Memento::SetUserNo( const USERNO & i_strUserNo )
{
	m_strUserNo				= i_strUserNo;
}

CWORD
Memento::GetPort() const
{
	return m_nPort;
}

void
Memento::SetPort( CWORD i_wPort )
{
	m_nPort					= i_wPort;
}

IChannel *
Memento::GetChannel() const
{
	return m_pChannel;
}

void
Memento::SetChannel( IChannel * i_pChannel )
{
	m_pChannel				= i_pChannel;
}

IRoom *
Memento::GetRoom() const
{
	return m_pRoom;
}

void
Memento::SetRoom( IRoom * i_pRoom )
{
	m_pRoom					= i_pRoom;
	m_bReady				= FALSE;
}

CBOOL
Memento::IsReady() const
{
	return m_bReady;
}

void
Memento::SetReady( CBOOL i_bReady )
{
	m_bReady				= i_bReady;
}

CBOOL
Memento::IsInventory() const
{
	return m_bInventory;
}

void
Memento::SetInventory( CBOOL i_bInventory )
{
	m_bInventory			= i_bInventory;
}

CBOOL
Memento::CanBeSuperPeer() const
{
	return m_bSuperPeer;
}

void
Memento::DisableSuperPeer()
{
	m_bSuperPeer			= FALSE;
}

String<17>
Memento::GetConnectID() const
{
	return m_strConnID;
}

void
Memento::SetConnectID( const String<17> & i_strID )
{
	m_strConnID				= i_strID;
}

Memento::ARRAY_C &
Memento::GetCharacters()
{
	return m_stCharacters;
}


CBOOL
Memento::IsNewCharacter() const
{
	return m_bNewCharacter;
}

void
Memento::SetNewCharacter()
{
	m_bNewCharacter			= TRUE;
}

const Memento::STOCK &
Memento::GetStock() const
{
	return m_stStock;
}

Memento::STOCK &
Memento::GetStock()
{
	return m_stStock;
}

//const Memento::RESULT &
//Memento::GetResult() const
//{
//	return m_stResult;
//}
//
//Memento::RESULT &
//Memento::GetResult()
//{
//	return m_stResult;
//}

Memento::E_TEAM::ENUM
Memento::GetTeam() const
{
	return m_eTeam;
}

void
Memento::SetTeam( E_TEAM::ENUM eTeam )
{
	if( eTeam != 0 && eTeam != 1)
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "[BAD] Wrong Set Team\r\n" ) );
		::Dispatcher::ICONSOLE()->Write( STRING( "[BAD] Wrong Set Team\r\n" ) );
	}
	m_eTeam					= eTeam;
}

CBOOL
Memento::IsLoading() const
{
	return m_bLoading;
}

void
Memento::SetLoading( CBOOL i_bLoad )
{
	m_bLoading				= i_bLoad;
}

CBOOL
Memento::IsReplayReady() const
{
	return m_bReplayReady;
}

void
Memento::SetReplayReady( CBOOL i_bReady )
{
	m_bReplayReady			= i_bReady;
}

CBOOL
Memento::IsIntrusion() const
{
	return m_bIntursion;
}

void
Memento::SetIntrusion( CBOOL i_bIntrusion )
{
	m_bIntursion			= i_bIntrusion;
}

CBOOL
Memento::IsDead() const
{
	return m_bDead;
}

void
Memento::SetDead( CBOOL i_bDead )
{
	m_bDead					= i_bDead;
}

CBOOL
Memento::IsSpectator() const
{
	return m_bSpectator; 
}

void
Memento::SetSpectator( CBOOL i_bSpectator )
{
	m_bSpectator			= i_bSpectator;
}

void
Memento::SetAge( CBYTE i_byAge )
{
	m_byAge					= i_byAge;
}

CBYTE
Memento::GetAge()
{
	return m_byAge;
}

void
Memento::SetRealName( CBOOL i_bRealName )
{
	m_bRealName				= i_bRealName;
}

CBOOL
Memento::IsRealName()
{
	return m_bRealName;
}

void
Memento::SetNetLoss( CBYTE i_byNetLoss )
{
	m_byNetLoss				= i_byNetLoss;
}

CBYTE
Memento::GetNetLoss()
{
	return m_byNetLoss;
}

} /* Player */				} /* GameServer */