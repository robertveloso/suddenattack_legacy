#include "StdAfx.h"

#include "./Provider.h"
#include "../Provider.h"
#include "../../IPlayer.h"
#include "../../IChannel.h"
#include "../../ICaster.h"
#include "../../IFormula.h"
#include "../../Player/Memento.h"
#include "../../Player/IResult.h"
#include "../../Room/IResult.h"

#include "../../../Dispatcher/IConsole.h"
#include "../../../Dispatcher/ITime.h"

#include "../../../Share/Packet/Builder.h"
#include "../../../Share/Packet/Service/Game.h"
#include "../../../Share/Packet/Service/Room.h"
#include "../../../Share/Packet/Service/Player.h"
#include "../../../Share/Packet/Service/Play.h"

namespace GameServer {		namespace Room {			namespace Process {

Provider::Provider()
{
	this->install();
}

Provider::~Provider()
{
	this->uninstall();
}

void
Provider::CastCreate( IRoom * i_pRoom )
{
	namespace				R = ::Packet::Service::Room;
	typedef					::Dispatcher::IClient
							ICLIENT;
	R::SN_Enter * pNoti		= ::Packet::Builder::Build<R::SN_Enter>();
	pNoti->m_nRoomIndex		= i_pRoom->GetIndex();
	pNoti->m_byUsers		= i_pRoom->GetPlayerCount();
	pNoti->m_stOption		= i_pRoom->GetOption();
	pNoti->m_byNetLoss		= i_pRoom->GetCaptain()->GetMemento()->GetNetLoss();
	pNoti->Set( i_pRoom->GetCaptain()->GetMemento()->GetStock().GetNick(), i_pRoom->GetName().GetCSTR() );

	ICASTER()->Cast( i_pRoom->GetChannel()->GetLobby(), pNoti );
}

void
Provider::CastDestroy( IRoom * i_pRoom )
{
	namespace				R = ::Packet::Service::Room;

	R::SN_Leave * pNoti		= ::Packet::Builder::Build<R::SN_Leave>();
	pNoti->m_nRoomIndex		= i_pRoom->GetIndex();

	ICASTER()->Cast( i_pRoom->GetChannel()->GetLobby(), pNoti );
}

void
Provider::ChangeCaptain( IRoom * i_pRoom )
{
	Room::Provider * pRoom	= static_cast<Room::Provider *>( i_pRoom );

	IPlayer * pCaptain		= NULL;
	if( i_pRoom->GetOption().IsPlaying() == TRUE )
	{
		IPlayer * pPlayer	= pRoom->GetFirstPlayer();
		while( pPlayer != NULL )
		{
			if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY &&
				pPlayer->GetMemento()->IsLoading() == TRUE )
			{
				pCaptain	= pPlayer;
				break;
			}
			pPlayer			= pRoom->GetNextPlayer();
		}
	}
	
	if( pCaptain == NULL )
	{
		IPlayer * pPlayer	= pRoom->GetFirstPlayer();
		if( pPlayer != NULL )
		{
			pCaptain		= pPlayer;
		}
	}

	if( pCaptain == NULL )
	{
#ifndef _DEVDEBUG
		pRoom->SetCaptain( pCaptain );
#endif
		return;
	}

	pRoom->SetCaptain( pCaptain );
	
	typedef					::Packet::Service::Room::NN_ChangeCaptain
							NN_CHANGE;
	NN_CHANGE * pNoti		= ::Packet::Builder::Build<NN_CHANGE>();
	pNoti->m_nCaptainIndex	= pCaptain->GetIndex();
	ICASTER()->Cast( pRoom, pNoti );

	typedef					::Packet::Service::Room::SN_ChangeCaptain
							SN_CHANGE;
	SN_CHANGE * pNotiLobby	= ::Packet::Builder::Build<SN_CHANGE>();
	pNotiLobby->m_nRoomIndex= pRoom->GetIndex();
	pNotiLobby->SetNick( pCaptain->GetMemento()->GetStock().GetNick() );
	ICASTER()->Cast( pRoom->GetChannel()->GetLobby(), pNotiLobby );
}

void
Provider::CastEnter( IPlayer * i_pPlayer, IRoom * i_pRoom )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;

	IRoom * pLobby			= i_pRoom->GetChannel()->GetLobby();
	if( i_pRoom == pLobby )
	{
		IRoom * pRoom		= i_pRoom->GetChannel()->GetFirstRoom();
		while( pRoom != NULL )
		{
			typedef				::Dispatcher::IClient
								ICLIENT;
			R::SN_Enter * pNoti	= ::Packet::Builder::Build<R::SN_Enter>();
			pNoti->m_nRoomIndex	= pRoom->GetIndex();
			pNoti->m_byUsers	= pRoom->GetPlayerCount();
			pNoti->m_stOption	= pRoom->GetOption();
			pNoti->m_byNetLoss	= pRoom->GetCaptain()->GetMemento()->GetNetLoss();
			pNoti->Set( pRoom->GetCaptain()->GetMemento()->GetStock().GetNick(), pRoom->GetName().GetCSTR() );

			ICASTER()->Cast( i_pPlayer, pNoti );

			pRoom			= i_pRoom->GetChannel()->GetNextRoom();
		}
	}
	else
	{
		if( i_pRoom->GetPlayerCount() > 1 )
		{
			R::SN_AddUser * pAdd	= ::Packet::Builder::Build<R::SN_AddUser>();
			pAdd->m_nRoomIndex		= i_pRoom->GetIndex();
			ICASTER()->Cast( pLobby, pAdd );
		}
	}

	IPlayer * pHostPlayer	= i_pRoom->GetFirstPlayer();
	while( pHostPlayer != NULL )
	{
		if( pHostPlayer == i_pPlayer )
		{
			pHostPlayer		= i_pRoom->GetNextPlayer();
			continue;
		}

		if( i_pRoom == pLobby )
		{
			P::SN_EnterLobby * pHost	= ::Packet::Builder::Build<P::SN_EnterLobby>();
			pHost->m_nIndex		= pHostPlayer->GetIndex();
			pHost->m_byGrade	= pHostPlayer->GetMemento()->GetStock().m_byGrade;
			pHost->m_byTeam		= pHostPlayer->GetMemento()->GetTeam();
			pHost->m_byState	= pHostPlayer->GetState();
			if( i_pRoom->GetCaptain() == pHostPlayer )
			{
				pHost->m_bCaptain	= TRUE;
			}
			else
			{
				pHost->m_bCaptain	= FALSE;
			}
			pHost->SetNick( pHostPlayer->GetMemento()->GetStock().GetNick() );

			ICASTER()->Cast( i_pPlayer, pHost );

			if( pHostPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
			{
				P::SN_EnterLobby * pGuest= ::Packet::Builder::Build<P::SN_EnterLobby>();
				pGuest->m_nIndex	= i_pPlayer->GetIndex();
				pGuest->m_byGrade	= i_pPlayer->GetMemento()->GetStock().m_byGrade;
				pGuest->m_byTeam	= i_pPlayer->GetMemento()->GetTeam();
				pGuest->m_byState	= i_pPlayer->GetState();
				if( i_pRoom->GetCaptain() == i_pPlayer )
				{
					pGuest->m_bCaptain	= TRUE;
				}
				else
				{
					pGuest->m_bCaptain	= FALSE;
				}
				pGuest->SetNick( i_pPlayer->GetMemento()->GetStock().GetNick() );

				ICASTER()->Cast( pHostPlayer, pGuest );
			}
		}
		else
		{
			P::SN_Enter * pHost	= ::Packet::Builder::Build<P::SN_Enter>();
			pHost->m_nIndex		= pHostPlayer->GetIndex();
			pHost->m_byGrade	= pHostPlayer->GetMemento()->GetStock().m_byGrade;
			pHost->m_byTeam		= pHostPlayer->GetMemento()->GetTeam();
			pHost->m_byState	= pHostPlayer->GetState();
			if( i_pRoom->GetCaptain() == pHostPlayer )
			{
				pHost->m_bCaptain	= TRUE;
			}
			else
			{
				pHost->m_bCaptain	= FALSE;
			}
			pHost->SetNick( pHostPlayer->GetMemento()->GetStock().GetNick() );

			ICASTER()->Cast( i_pPlayer, pHost );

//			if( pHostPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
			{
				P::SN_Enter * pGuest= ::Packet::Builder::Build<P::SN_Enter>();
				pGuest->m_nIndex	= i_pPlayer->GetIndex();
				pGuest->m_byGrade	= i_pPlayer->GetMemento()->GetStock().m_byGrade;
				pGuest->m_byTeam	= i_pPlayer->GetMemento()->GetTeam();
				pGuest->m_byState	= i_pPlayer->GetState();
				if( i_pRoom->GetCaptain() == i_pPlayer )
				{
					pGuest->m_bCaptain	= TRUE;
				}
				else
				{
					pGuest->m_bCaptain	= FALSE;
				}
				pGuest->SetNick( i_pPlayer->GetMemento()->GetStock().GetNick() );

				ICASTER()->Cast( pHostPlayer, pGuest );
			}
		}

		pHostPlayer			= i_pRoom->GetNextPlayer();
	}
}

void
Provider::CastLeave( IPlayer * i_pPlayer, IRoom * i_pRoom )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;

	IRoom * pLobby			= i_pRoom->GetChannel()->GetLobby();
	if( i_pRoom != pLobby )
	{
		R::SN_RemoveUser *
		pRem				= ::Packet::Builder::Build<R::SN_RemoveUser>();
		pRem->m_nRoomIndex	= i_pRoom->GetIndex();
		ICASTER()->Cast( pLobby, pRem );
	}

	if( i_pRoom == pLobby )
	{
		P::SN_LeaveLobby * pLeave	= ::Packet::Builder::Build<P::SN_LeaveLobby>();
		pLeave->m_nPlayerIndex		= i_pPlayer->GetIndex();
		IPlayer * pPlayer			= i_pRoom->GetFirstPlayer();
		while( pPlayer != NULL )
		{
			if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY &&
				pPlayer != i_pPlayer )
			{
				ICASTER()->Cast( pPlayer, pLeave );
			}
			pPlayer				= i_pRoom->GetNextPlayer();
		}
	}
	else
	{
		P::SN_Leave * pLeave	= ::Packet::Builder::Build<P::SN_Leave>();
		pLeave->m_nPlayerIndex	= i_pPlayer->GetIndex();
		IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
		while( pPlayer != NULL )
		{
			if( /*pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY &&*/
				pPlayer != i_pPlayer )
			{
				ICASTER()->Cast( pPlayer, pLeave );
			}
			pPlayer				= i_pRoom->GetNextPlayer();
		}
	}
}

void
Provider::CastStateForPlay( IPlayer * i_pPlayer, IRoom * i_pRoom )
{
	typedef					::Packet::Service::Play::SN_State
							SN_STATE;
	typedef					::Packet::Service::Game::SN_PlayerInfo
							SN_PLAYERINFO;

	SN_STATE * pState		= ::Packet::Builder::Build<SN_STATE>();
	pState->m_nIndex		= i_pPlayer->GetIndex();
	pState->m_byState		= i_pPlayer->GetState();
	pState->m_byGrade		= i_pPlayer->GetMemento()->GetStock().m_byGrade;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() == IPlayer::E_STATE::BUFF )
		{
			ICASTER()->Cast( pPlayer, pState );
		}
		else if( i_pRoom->IsNat() == TRUE )
		{
			if( i_pRoom->GetCaptain()->GetMemento()->GetIP() != pPlayer->GetMemento()->GetIP() )
			{
				ICASTER()->Cast( pPlayer, pState );
			}
		}
		
		pPlayer				= i_pRoom->GetNextPlayer();
	}

	pPlayer					= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY ||
			pPlayer == i_pPlayer )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		SN_PLAYERINFO *
		pHost				= ::Packet::Builder::Build<SN_PLAYERINFO>();
		pHost->m_nIndex		= pPlayer->GetIndex();
		pHost->m_nKill		= pPlayer->GetResult()->GetKillCount();
		pHost->m_nDead		= pPlayer->GetResult()->GetDeadCount();
		if( pPlayer->GetMemento()->GetTeam() == Player::Memento::E_TEAM::RED )
		{
			pHost->m_stCharacter= pPlayer->GetMemento()->GetCharacters()[ Player::Memento::E_TEAM::RED ];
		}
		else
		{
			pHost->m_stCharacter= pPlayer->GetMemento()->GetCharacters()[ Player::Memento::E_TEAM::BLUE ];
		}
		ICASTER()->Cast( i_pPlayer, pHost );

		SN_PLAYERINFO *
		pGuest				= ::Packet::Builder::Build<SN_PLAYERINFO>();
		pGuest->m_nIndex	= i_pPlayer->GetIndex();
		pGuest->m_nKill		= i_pPlayer->GetResult()->GetKillCount();
		pGuest->m_nDead		= i_pPlayer->GetResult()->GetDeadCount();
		if( i_pPlayer->GetMemento()->GetTeam() == Player::Memento::E_TEAM::RED )
		{
			pGuest->m_stCharacter= i_pPlayer->GetMemento()->GetCharacters()[ Player::Memento::E_TEAM::RED ];
		}
		else
		{
			pGuest->m_stCharacter= i_pPlayer->GetMemento()->GetCharacters()[ Player::Memento::E_TEAM::BLUE ];
		}
		ICASTER()->Cast( pPlayer, pGuest );
		
		pPlayer				= i_pRoom->GetNextPlayer();
	}
}

void
Provider::CastStateForReturn( IPlayer * i_pPlayer, IRoom * i_pRoom )
{
	typedef					::Packet::Service::Player::SN_Enter
							SN_ENTER;
	typedef					::Packet::Service::Play::SN_State
							SN_STATE;
	typedef					::LogicObject::Player::Type::E_STATE
							E_STATE;

	i_pPlayer->GetMemento()->SetReady( FALSE );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer == i_pPlayer )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		SN_ENTER * pHost	= ::Packet::Builder::Build<SN_ENTER>();
		pHost->m_nIndex		= pPlayer->GetIndex();
		pHost->m_byGrade	= pPlayer->GetMemento()->GetStock().m_byGrade;
		pHost->m_byTeam		= pPlayer->GetMemento()->GetTeam();
		pHost->m_byState	= pPlayer->GetState();
		if( i_pRoom->GetCaptain() == pPlayer )
		{
			pHost->m_bCaptain	= TRUE;
		}
		else
		{
			pHost->m_bCaptain	= FALSE;
		}
		pHost->SetNick( pPlayer->GetMemento()->GetStock().GetNick() );
		ICASTER()->Cast( i_pPlayer, pHost );

		if( pPlayer->GetState() == E_STATE::BUFF || 
			pPlayer->GetState() == E_STATE::READY )
		{
			SN_STATE * pGuest	= ::Packet::Builder::Build<SN_STATE>();
			pGuest->m_nIndex	= i_pPlayer->GetIndex();
			pGuest->m_byState	= i_pPlayer->GetState();
			pGuest->m_byGrade	= i_pPlayer->GetMemento()->GetStock().m_byGrade;

			ICASTER()->Cast( pPlayer, pGuest );
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}
}

CBOOL
Provider::InspactFinish( IRoom * i_pRoom )
{
	BOOL bRed				= FALSE;
	BOOL bBlue				= FALSE;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY &&
			pPlayer->GetMemento()->IsLoading() == TRUE )
		{
			if( pPlayer->GetMemento()->GetTeam() == E_TEAM::RED )
			{
				bRed		= TRUE;
			}
			else
			{
				bBlue		= TRUE;
			}
		}
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY &&
			pPlayer->GetMemento()->IsIntrusion() == FALSE )
		{
			if( pPlayer->GetMemento()->GetTeam() == E_TEAM::RED )
			{
				bRed		= TRUE;
			}
			else
			{
				bBlue		= TRUE;
			}
		}
		pPlayer				= i_pRoom->GetNextPlayer();
	}

	if( bRed == FALSE )
	{
		i_pRoom->GetResult()->SetWinnerTeam( E_TEAM::BLUE );
		i_pRoom->GetResult()->SetWinRound( E_TEAM::BLUE );
		this->castFinishRound( i_pRoom );
		this->calculate( i_pRoom );
		this->castFinishGame( i_pRoom );
		return TRUE;
	}

	if( bBlue == FALSE )
	{
		i_pRoom->GetResult()->SetWinnerTeam( E_TEAM::RED );
		i_pRoom->GetResult()->SetWinRound( E_TEAM::RED );
		this->castFinishRound( i_pRoom );
		this->calculate( i_pRoom );
		this->castFinishGame( i_pRoom );
		return TRUE;
	}

	return FALSE;
}

void
Provider::FinishGame( IRoom * i_pRoom )
{
	i_pRoom->CheckWinner();

	this->calculate( i_pRoom );
	this->castFinishGame( i_pRoom );
}

void
Provider::FinishRound( IRoom * i_pRoom )
{
	namespace				G = ::Packet::Service::Game;
	G::SN_RoundResult *
	pNoti					= ::Packet::Builder::Build<G::SN_RoundResult>();

	if( i_pRoom->GetResult()->IsDefusedBomb() == TRUE )
	{
		pNoti->m_byType		= G::SN_RoundResult::E_TYPE::DEFUSED;
	}
	else if( i_pRoom->GetResult()->IsDetonatedBomb() == TRUE )
	{
		pNoti->m_byType		= G::SN_RoundResult::E_TYPE::DETONATED;
	}
	else if( i_pRoom->GetResult()->IsAllKilled() == TRUE )
	{
		pNoti->m_byType		= G::SN_RoundResult::E_TYPE::ALL_KILL;
	}
	else
	{
		pNoti->m_byType		= G::SN_RoundResult::E_TYPE::TIMEOUT;
	}

	if( i_pRoom->GetOption().GetMode() == ::LogicObject::Room::Option::E_MODE::RESPAWN )
	{
		while( i_pRoom->GetOption().GetRoundLimit() != i_pRoom->GetResult()->GetRound() )
		{
			i_pRoom->GetResult()->NextRound();
		}
	}
	else
	{
		i_pRoom->GetResult()->NextRound();
	}

	pNoti->m_byWinnerTeam	= i_pRoom->GetResult()->GetWinRound();
	pNoti->m_byRestRound	= i_pRoom->GetOption().GetRoundLimit() - i_pRoom->GetResult()->GetRound();
	pNoti->m_byRedWin		= i_pRoom->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::RED );
	pNoti->m_byBlueWin		= i_pRoom->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::BLUE );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
//		pPlayer->GetMemento()->SetDead( FALSE );

		if( pPlayer->GetMemento()->IsLoading() == FALSE )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		ICASTER()->Cast( pPlayer, pNoti );

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	if( i_pRoom->GetOption().GetRoundLimit() == i_pRoom->GetResult()->GetRound() )
	{
		IPROCESS()->FinishGame( i_pRoom );
		return;
	}

	pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		ICASTER()->Signal( pPlayer, G::SS_EndRound::s_nCode );

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	i_pRoom->ResetStartTick();
	i_pRoom->GetResult()->ResetRound();
	i_pRoom->SetBombReady( TRUE );
}

void
Provider::CancelRound( IRoom * i_pRoom )
{
	namespace				G = ::Packet::Service::Game;
	G::SN_RoundResult *
	pNoti					= ::Packet::Builder::Build<G::SN_RoundResult>();

	pNoti->m_byType			= G::SN_RoundResult::E_TYPE::CANCEL;

	while( i_pRoom->GetOption().GetRoundLimit() != i_pRoom->GetResult()->GetRound() )
	{
		i_pRoom->GetResult()->NextRound();
	}

	pNoti->m_byWinnerTeam	= IPlayer::E_TEAM::DRAW;
	pNoti->m_byRestRound	= i_pRoom->GetOption().GetRoundLimit() - i_pRoom->GetResult()->GetRound();
	pNoti->m_byRedWin		= i_pRoom->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::RED );
	pNoti->m_byBlueWin		= i_pRoom->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::BLUE );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->IsLoading() == FALSE )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		ICASTER()->Cast( pPlayer, pNoti );

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	if( i_pRoom->GetOption().GetRoundLimit() == i_pRoom->GetResult()->GetRound() )
	{
		STRING strLog;
		strLog.Printf( "Record\t%s\t%s\tR(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"CancelRound",
					i_pRoom->GetIndex() );

		i_pRoom->GetResult()->SetWinnerTeam( IPlayer::E_TEAM::CANCEL );
		this->calculate( i_pRoom );
		this->castFinishGame( i_pRoom );
		return;
	}

	pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		ICASTER()->Signal( pPlayer, G::SS_EndRound::s_nCode );

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	i_pRoom->ResetStartTick();
	i_pRoom->GetResult()->ResetRound();
	i_pRoom->SetBombReady( TRUE );
}

inline void
Provider::calculate( IRoom * i_pRoom )
{
	IFormula * pFormula		= IFormula::ICONTAINER()->Find( i_pRoom->GetOption().GetMode() );

	E_TEAM::ENUM eWinner	= i_pRoom->GetResult()->GetWinnerTeam();

	INT nRed				= 0;
	INT nBlue				= 0;

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->IsLoading() == FALSE )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		if( pPlayer->GetMemento()->GetTeam() == IPlayer::E_TEAM::RED )
		{
			nRed ++;
		}
		else
		{
			nBlue ++;
		}
		pPlayer				= i_pRoom->GetNextPlayer();
	}


	pPlayer					= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->IsLoading() == FALSE )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		INT nEnemy			= 0;
		if( pPlayer->GetMemento()->GetTeam() == IPlayer::E_TEAM::RED )
		{
			nEnemy			= nBlue;
		}
		else
		{
			nEnemy			= nRed;
		}


		const IFormula::Exp * pExp		= NULL;
		const IFormula::Money * pMoney	= NULL;
		if( pPlayer->GetMemento()->GetTeam() == eWinner )
		{
			pExp			= pFormula->GetWinExp();
			pMoney			= pFormula->GetWinMoney();
		}
		else
		{
			pExp			= pFormula->GetLoseExp();
			pMoney			= pFormula->GetLoseMoney();
		}

		if( pFormula == NULL )
		{
			DEBUG_BREAK;
			return;
		}

		INT nExp			= ( pPlayer->GetResult()->GetKillCount() * pExp->nKillMul )
							+ ( pPlayer->GetResult()->GetHeadKillCount() * pExp->nHeadKillMul )
							+ ( nEnemy * pExp->nEnemyMul )
							+ ( pPlayer->GetResult()->GetDeadCount() * pExp->nDeadMul )
							+ ( pPlayer->GetResult()->GetSuicideCount() * pExp->nSuicideMul )
							+ ( pPlayer->GetResult()->GetMissionCount() * pExp->nMissionMul )
							+ pExp->nAdd;
		
		if( nExp < 0 )
		{
			nExp		= 0;
		}

		INT nMoney			= ( pPlayer->GetResult()->GetKillCount() * pMoney->nKillMul )
							+ ( nEnemy * pMoney->nEnemyMul )
							+ ( pPlayer->GetResult()->GetDeadCount() * pMoney->nDeadMul )
							+ ( pPlayer->GetResult()->GetSuicideCount() * pMoney->nSuicideMul )
							+ ( pPlayer->GetResult()->GetMissionCount() * pMoney->nMissionMul )
							+ pMoney->nAdd;

		if( nMoney < 0 )
		{
			nMoney		= 0;
		}

		pPlayer->GetResult()->SetExpChange( nExp );
		pPlayer->GetResult()->SetMoneyChange( nMoney );

		pPlayer				= i_pRoom->GetNextPlayer();
	}
}

inline void
Provider::castFinishGame( IRoom * i_pRoom )
{
	namespace				G = ::Packet::Service::Game;
	namespace				R = ::Packet::Service::Room;
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;
	typedef					::LogicObject::Player::Type::E_STATE
							E_STATE;

	G::SN_ResultTotal *
	pTotal					= ::Packet::Builder::Build<G::SN_ResultTotal>();

	pTotal->m_byWinnerTeam	= i_pRoom->GetResult()->GetWinnerTeam();
	pTotal->m_byResultCount	= this->getGamePlayerCount( i_pRoom );

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%d\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Finish",
					i_pRoom->GetIndex(),
					i_pRoom->GetResult()->GetWinnerTeam() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->IsLoading() == TRUE )
		{
			ICASTER()->Cast( pPlayer, pTotal );
		}

		ICASTER()->Signal( pPlayer, G::SS_End::s_nCode );

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	if( i_pRoom->GetCaptain() == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "Captain is NULL\r\n" ) );
	}
	else
	{
		ICASTER()->Signal( i_pRoom->GetCaptain(), R::SS_Init::s_nCode );
	}

	i_pRoom->GetOption().SetPlaying( FALSE );

	typedef					::Packet::Service::Room::SN_Buff
							SN_BUFF;
	SN_BUFF * pNoti			= ::Packet::Builder::Build<SN_BUFF>();
	pNoti->m_nRoomIndex		= i_pRoom->GetIndex();
	ICASTER()->Cast( i_pRoom->GetChannel()->GetLobby(), pNoti );
}

inline CBYTE
Provider::getGamePlayerCount( IRoom * i_pRoom )
{
	typedef					::LogicObject::Player::Type::E_STATE
							E_STATE;

	BYTE byCount			= 0;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->IsLoading() == TRUE )
		{
			byCount ++;
		}
		
		pPlayer				= i_pRoom->GetNextPlayer();
	}

	return byCount;
}

inline void
Provider::castFinishRound( IRoom * i_pRoom )
{
	while( i_pRoom->GetOption().GetRoundLimit() != i_pRoom->GetResult()->GetRound() )
	{
		i_pRoom->GetResult()->NextRound();
	}

	namespace				G = ::Packet::Service::Game;
	G::SN_RoundResult *
	pNoti					= ::Packet::Builder::Build<G::SN_RoundResult>();

	pNoti->m_byWinnerTeam	= i_pRoom->GetResult()->GetWinRound();
	pNoti->m_byRestRound	= i_pRoom->GetOption().GetRoundLimit() - i_pRoom->GetResult()->GetRound();
	pNoti->m_byRedWin		= i_pRoom->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::RED );
	pNoti->m_byBlueWin		= i_pRoom->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::BLUE );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		pPlayer->GetMemento()->SetDead( FALSE );
		if( pPlayer->GetMemento()->IsLoading() == FALSE )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		ICASTER()->Cast( pPlayer, pNoti );

		pPlayer				= i_pRoom->GetNextPlayer();
	}

}

} /* Process */				} /* Room */				} /* GameServer */