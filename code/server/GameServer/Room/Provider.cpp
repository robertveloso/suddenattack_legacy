#include "StdAfx.h"

#include "./Provider.h"
#include "./IProcess.h"
#include "../IPlayer.h"
#include "../Player/Memento.h"

#include "../../Dispatcher/IHandler.h"
#include "../../Dispatcher/IConsole.h"
#include "../../Dispatcher/ITime.h"

namespace GameServer {		namespace Room {

INT
Provider::s_nIndex			= 0;

Provider::Provider()
{
	m_nIndex				= s_nIndex ++;

	m_pPool					= new POOL;
	m_pPlayerTree			= new TREE;
	m_pResult				= new Result::Provider;
}

Provider::~Provider()
{
	SAFE_DELETE( m_pResult );
	SAFE_DELETE( m_pPlayerTree );
	SAFE_DELETE( m_pPool );
}

void
Provider::Capture( IChannel * i_pChannel, IPlayer * i_pPlayer, CNAME & i_strName, CPASS & i_strPass, COPTION & i_stOption )
{
	m_pChannel				= i_pChannel;
	m_strName				= i_strName;
	m_strPass				= i_strPass;
	m_stOption				= i_stOption;

	m_pCaptain				= i_pPlayer;
	m_pSuperPeer			= NULL;

	m_bLobby				= FALSE;

	m_bNat					= FALSE;

	m_stOption.SetPlaying( FALSE );

	COUPLER * pCoupler		= m_pPool->Capture();
	pCoupler->clsItemA		= i_pPlayer->GetIndex();
	pCoupler->clsItemB		= i_pPlayer;
	CBOOL bSuccess			= m_pPlayerTree->Insert( pCoupler );

	i_pPlayer->InitPlay();

	Assert( bSuccess == TRUE );

	IPROCESS()->CastEnter( i_pPlayer, this );

	IPROCESS()->CastCreate( this );

	m_pResult->Capture();

	STRING strLog;
	strLog.Printf( "Room\t%s\t%s\tR(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Open",
					m_nIndex );
	::Dispatcher::ICONSOLE()->Write( strLog );
}

void
Provider::Capture( IChannel * i_pChannel )
{
	m_pChannel				= i_pChannel;

	m_bLobby				= TRUE;
	m_bNat					= FALSE;

	m_pResult->Capture();


	STRING strLog;
	strLog.Printf( "Room\t%s\t%s\tR(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"OpenLobby",
					m_nIndex );
	::Dispatcher::ICONSOLE()->Write( strLog );
}

void
Provider::Release()
{
	if( m_bLobby == FALSE )
	{
		IPROCESS()->CastDestroy( this );
	}

	Assert( m_pPlayerTree->GetCount() == 0 );
	m_pResult->Release();


	STRING strLog;
	strLog.Printf( "Room\t%s\t%s\tR(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Close",
					m_nIndex );
	::Dispatcher::ICONSOLE()->Write( strLog );
}

CINT
Provider::GetIndex()
{
	return m_nIndex;
}

CINT
Provider::GetPlayerCount()
{
	return m_pPlayerTree->GetCount();
}
	
IChannel *
Provider::GetChannel()
{
	return m_pChannel;
}

void
Provider::AddPlayer( IPlayer * i_pPlayer )
{
	COUPLER * pCoupler		= m_pPool->Capture();
	pCoupler->clsItemA		= i_pPlayer->GetIndex();
	pCoupler->clsItemB		= i_pPlayer;
	CBOOL bSuccess			= m_pPlayerTree->Insert( pCoupler );

	Assert( bSuccess == TRUE );

	i_pPlayer->GetMemento()->SetLoading( FALSE );

	STRING strLog;
	strLog.Printf( "Room\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Add",
					m_nIndex,
					i_pPlayer->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	IPROCESS()->CastEnter( i_pPlayer, this );
}

void
Provider::RemovePlayer( IPlayer * i_pPlayer )
{
	COUPLER * pCoupler		= m_pPlayerTree->Remove( i_pPlayer->GetIndex() );
	if( pCoupler == NULL )
	{
		DEBUG_BREAK;
		return;
	}

	m_pPool->Release( pCoupler );

	if( i_pPlayer == m_pCaptain &&
		m_bLobby == FALSE )
	{
		IPROCESS()->ChangeCaptain( this );
	}

	STRING strLog;
	strLog.Printf( "Room\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Remove",
					m_nIndex,
					i_pPlayer->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	IPROCESS()->CastLeave( i_pPlayer, this );
}

IPlayer *
Provider::GetFirstPlayer()
{
	m_stItr					= m_pPlayerTree->GetRoot();
	if( m_stItr.GetItem() == NULL )
	{
		return NULL;
	}
	return m_stItr.GetItem()->clsItemB;
}

IPlayer *
Provider::GetNextPlayer()
{
	m_stItr.MoveNext();
	if( m_stItr.GetItem() == NULL )
	{
		return NULL;
	}
	return m_stItr.GetItem()->clsItemB;
}

IPlayer *
Provider::FindPlayer( CINT i_nIndex )
{
	const COUPLER * pCoupler = m_pPlayerTree->Search( i_nIndex );
	if( pCoupler == NULL )
	{
		return NULL;
	}
	return pCoupler->clsItemB;
}

CBOOL
Provider::IsPass()
{
	Assert( m_bLobby == FALSE );
	return m_stOption.IsPassword();
}

Provider::CPASS &
Provider::GetPass()
{
	Assert( m_bLobby == FALSE );
	return m_strPass;
}

void
Provider::SetPass( CPASS & i_strPass )
{
	Assert( m_bLobby == FALSE );
	m_strPass				= i_strPass;
}

void
Provider::ReleasePass()
{
	Assert( m_bLobby == FALSE );
	m_strPass				= "";
	m_stOption.SetPassword( FALSE );
}

Provider::CNAME &
Provider::GetName()
{
	Assert( m_bLobby == FALSE );
	return m_strName;
}

void
Provider::SetName( CNAME & i_strName )
{
	Assert( m_bLobby == FALSE );
	m_strName				= i_strName;
}

IPlayer *
Provider::GetCaptain()
{
	return m_pCaptain;
}

IPlayer *
Provider::GetSuperPeer()
{
	Assert( m_bLobby == FALSE );
	return m_pSuperPeer;
}

void
Provider::SetSuperPeer( IPlayer * i_pSuperPeer )
{
	Assert( m_bLobby == FALSE );
	m_pSuperPeer			= i_pSuperPeer;
}

IPlayer *
Provider::GetTempSuperPeer()
{
	Assert( m_bLobby == FALSE );
	return m_pTempSuperPeer;
}

void
Provider::SetTempSuperPeer( IPlayer * i_pSuperPeer )
{
	Assert( m_bLobby == FALSE );
	m_pTempSuperPeer		= i_pSuperPeer;
}

Provider::E_JOIN::ENUM
Provider::IsJoinable( CPASS & i_strPass )
{
	Assert( m_bLobby == FALSE );
	if( m_stOption.GetTeamMax() * 2 <= static_cast<INT>( m_pPlayerTree->GetCount() ) )
	{
		return E_JOIN::FULL;
	}

	if( m_stOption.IsPlaying() == TRUE &&
		m_stOption.CanRunnableInBattle() == FALSE )
	{
		return E_JOIN::PLAYING;
	}

	if( m_stOption.IsPassword() == TRUE )
	{
		if( m_strPass != i_strPass )
		{
			return E_JOIN::WRONG_PASS;
		}
	}

	return E_JOIN::SUCCESS;
}

Provider::COPTION &
Provider::GetOption() const
{
	Assert( m_bLobby == FALSE );
	return m_stOption;
}

Provider::OPTION &
Provider::GetOption()
{
	Assert( m_bLobby == FALSE );
	return m_stOption;
}

void
Provider::SetOption( COPTION & i_stOption )
{
	Assert( m_bLobby == FALSE );
	bool bPass				= m_stOption.IsPassword();
	
	m_stOption				= i_stOption;
	m_stOption.SetPassword( bPass );
}

Provider::IRESULT *
Provider::GetResult()
{
	Assert( m_bLobby == FALSE );
	return m_pResult;
}

void
Provider::SetPause( CBOOL i_bPause )
{
	Assert( m_bLobby == FALSE );
	m_bPause				= i_bPause;
}

CBOOL
Provider::IsPause()
{
	Assert( m_bLobby == FALSE );
	return m_bPause;
}

void
Provider::SetBombReady( CBOOL i_bReady )
{
	m_bBombReady			= i_bReady;
}

CBOOL
Provider::IsBombReady()
{
	return m_bBombReady;
}

void
Provider::CheckWinner()
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	switch( m_stOption.GetMode() )
	{
	case E_MODE::RESPAWN :
		this->checkRespawnWinner();
		break;

	case E_MODE::BOMB :
		this->checkBombWinner();
		break;

	default :
		DEBUG_BREAK;
		this->checkRespawnWinner();
		break;
	}
}

void
Provider::ResetStartTick()
{
	m_ulStartTick			= ::Dispatcher::IHANDLER()->GetTickCount();
}

CBOOL
Provider::IsSpectatorTick()
{
	CULONG ulTickDiff		= ::Dispatcher::IHANDLER()->GetTickCount() - m_ulStartTick;
	CINT nSecDiff			= static_cast<INT>( ulTickDiff / 1000 );

	if( nSecDiff > IFACTORY()->GetSpectatorTime( m_stOption.GetMode() ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

inline void
Provider::checkRespawnWinner()
{
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	INT nRed				= m_pResult->GetKillCount( E_TEAM::RED )
							- m_pResult->GetDeadCount( E_TEAM::RED );

	INT nBlue				= m_pResult->GetKillCount( E_TEAM::BLUE )
							- m_pResult->GetDeadCount( E_TEAM::BLUE );

	if( nRed > nBlue )
	{
		m_pResult->SetWinnerTeam( E_TEAM::RED );
	}
	else if( nRed < nBlue )
	{
		m_pResult->SetWinnerTeam( E_TEAM::BLUE );
	}
	else
	{
		m_pResult->SetWinnerTeam( E_TEAM::DRAW );
	}
}

inline void
Provider::checkBombWinner()
{
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	if( m_pResult->GetWinRoundCount( E_TEAM::RED ) > m_pResult->GetWinRoundCount( E_TEAM::BLUE ) )
	{
		m_pResult->SetWinnerTeam( E_TEAM::RED );
	}
	else if( m_pResult->GetWinRoundCount( E_TEAM::RED ) < m_pResult->GetWinRoundCount( E_TEAM::BLUE ) )
	{
		m_pResult->SetWinnerTeam( E_TEAM::BLUE );
	}
	else
	{
		m_pResult->SetWinnerTeam( E_TEAM::DRAW );
	}
}

void
Provider::SetNat( CBOOL i_bNat )
{
	m_bNat					= i_bNat;
}

CBOOL
Provider::IsNat()
{
	return m_bNat;
}

void
Provider::SetCaptain( IPlayer * i_pPlayer )
{
	m_pCaptain				= i_pPlayer;
}

} /* Room */				} /* GameServer */