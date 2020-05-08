#include "StdAfx.h"

#include "./Provider.h"
#include "./Memento.h"
#include "./IState.h"
#include "./Result/Provider.h"
#include "../IChannel.h"
#include "../IServer.h"
#include "../ICaster.h"
#include "../IFormula.h"
#include "../IRoom.h"
#include "../../Dispatcher/IQuery.h"
#include "../../Dispatcher/IHandler.h"
#include "../../Dispatcher/IConsole.h"

#include "./State/Login/Lobby.h"
#include "./State/Buff/BuffRoom.h"
#include "./State/Buff/Inventory.h"
#include "./State/Buff/PlayReady.h"
#include "./State/Buff/ReturnBuff.h"
#include "./State/Play/BombRoom.h"
#include "./State/Play/BombReady.h"
#include "./State/Play/RespawnRoom.h"

#include "../../Share/Packet/Builder.h"
#include "../../Share/Packet/Login.h"
#include "../../Share/Packet/Service/Game.h"
#include "../../Share/Packet/Service/Player.h"

#include "../Query.h"
#include "../../Dispatcher/IQuery.h"
#include "../../Dispatcher/IConsole.h"
#include "../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {

DWORD Provider::s_dwIndex	= 0;

Provider::Provider()
{
	m_pMemento				= new Memento;
	m_pResult				= new Result::Provider;

	m_dwIndex				= s_dwIndex;
	s_dwIndex ++;
}

Provider::~Provider()
{
	SAFE_DELETE( m_pResult );
	SAFE_DELETE( m_pMemento );
}

void
Provider::Capture( PEER * i_pPeer )
{
	m_pPeer					= i_pPeer;
	m_bLogined				= FALSE;
	m_pResult->Capture();
	m_pMemento->Capture( this, ::Dispatcher::IHANDLER()->GetPeerIP( i_pPeer ) );
}

void
Provider::Release()
{
	m_pPeer					= NULL;
	m_pResult->Release();
	m_pMemento->Release();
}

Provider::PEER *
Provider::GetPeer()
{
	return m_pPeer;
}

void
Provider::OnOpen()
{
}

void
Provider::OnPacket( PACKET * i_pPacket )
{
//#ifdef _DEVDEBUG
	STRING strOut;
	strOut.Printf( "\r\nDev\tOnPacket : U(%d) %d\r\n", m_dwIndex, i_pPacket->GetCommand() );
	::Dispatcher::ICONSOLE()->Write( strOut );
//#endif
	CBOOL bResult			= m_pMemento->GetState()->OnPacket( m_pMemento, i_pPacket );

	if( bResult == FALSE )
	{
		IClient::ISESSION()->Close( this );
	}
}

void
Provider::OnClose()
{
//#ifdef _DEVDEBUG
	STRING strOut;
	strOut.Printf( "Dev\tOnClose : U(%d)\r\n", m_dwIndex );
	::Dispatcher::ICONSOLE()->Write( strOut );
//#endif
	m_pMemento->GetState()->OnClose( m_pMemento );
	
	cout << m_pMemento->GetIP().GetCSTR() << " Close" << endl;
	IBUILDER()->Destroy( this );
}

void
Provider::OnSignal( CUINT i_uiMessage )
{
//#ifdef _DEVDEBUG
	STRING strOut;
	strOut.Printf( "Dev\tOnSignal : U(%d) %d\r\n", m_dwIndex, i_uiMessage );
	::Dispatcher::ICONSOLE()->Write( strOut );
//#endif

#ifdef _DEVDEBUG
	if( m_pMemento->GetState() == NULL )
	{
		return;
	}
#endif

	typedef					::Packet::Login::CQ_Login
							TEST;
	PACKET * pPacket		= ::Packet::Builder::Build<TEST>();
	pPacket->SetCommand( i_uiMessage );
	m_pMemento->GetState()->OnPacket( m_pMemento, pPacket );
}

void
Provider::OnResponse( IQUERY * i_pQuery )
{
	m_pMemento->GetState()->OnResponse( m_pMemento, i_pQuery );

}

Provider::IRESULT *
Provider::GetResult()
{
	return m_pResult;
}

CDWORD
Provider::GetIndex() const
{
	return m_dwIndex;
}

Provider::MEMENTO *
Provider::GetMemento()
{
	return m_pMemento;
}

//Provider::MEMENTO *
//Provider::GetMemento() const
//{
//	return m_pMemento;
//}

Provider::E_STATE::ENUM
Provider::GetState()
{
	using namespace State;
	IState * pState			= m_pMemento->GetState();
	if( pState == Play::BOMBROOM() ||
		pState == Play::RESPAWNROOM() ||
		pState == Play::BOMBREADY() )
	{
		return E_STATE::PLAY;
	}
	if( pState == Buff::RETURNBUFF() )
	{
		return E_STATE::RETURN_BUFF;
	}
	if( pState == Buff::PLAYREADY() )
	{
		return E_STATE::PLAY_READY;
	}
	if( pState == Buff::INVENTORY() )
	{
		return E_STATE::INVENTORY;
	}
	if( pState == Buff::BUFFROOM() &&
		m_pMemento->IsReady() == TRUE )
	{
		return E_STATE::READY;
	}
	if( pState == Buff::BUFFROOM() &&
		m_pMemento->IsReady() == FALSE )
	{
		return E_STATE::BUFF;
	}
	if( pState == Login::LOBBY() )
	{
		return E_STATE::LOBBY;
	}

	return E_STATE::LOGIN;
}

void
Provider::LogoutProcess()
{
	typedef					Query::Session
							SESSION;

	try
	{
		INT nResult				= -99;
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SESSION::E_CODE::NOT_LOGOUT );

		pQuery->GetLog().Printf( "%s\r\n", m_pMemento->GetUserNo().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_MB_LOGOUT" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_conn_no" ), STRING( m_pMemento->GetConnectID().GetCSTR() ), TRUE, 16 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( m_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_login_flag" ), 0, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ipt_ip_addr" ), STRING( m_pMemento->GetIP().GetCSTR() ), TRUE, 15 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nResult, FALSE );

		IQUERY::IREQUESTER()->Regist( pQuery );
	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[REQ_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[REQ_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );
	}
	catch( ... )
	{
	}

	if( m_bLogined == TRUE )
	{
		ICONTAINER()->Remove( this );
	}
}

void
Provider::RecordResult( E_TEAM::ENUM i_eWinner )
{
	typedef					Query::Result
							RESULT;
	typedef					::Dispatcher::IQuery
							IQUERY;

	if( m_pMemento->IsLoading() == FALSE || m_pMemento->IsSpectator() == TRUE )
	{
		return;
	}

	INT nWin				= 0;
	INT nLose				= 0;
	INT nDraw				= 0;
	if( i_eWinner == m_pMemento->GetTeam() )
	{
		nWin				= 1;
	}
	else if( i_eWinner == E_TEAM::DRAW )
	{
		nDraw				= 1;
	}
	else if( i_eWinner != E_TEAM::CANCEL )
	{
		nLose				= 1;
	}

	STRING strLog;
	if( i_eWinner == E_TEAM::CANCEL )
	{
		strLog.Printf( "Record\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Cancel",
					m_pMemento->GetRoom()->GetIndex(),
					m_pMemento->GetStock().GetNick() );
	}
	else
	{
		strLog.Printf( "Record\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Result",
					m_pMemento->GetRoom()->GetIndex(),
					m_pMemento->GetStock().GetNick() );
	}
	::Dispatcher::ICONSOLE()->Write( strLog );

	try
	{
		INT nInt				= -99;
		INT64 nInt64			= 0;
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( RESULT::E_CODE::REQ_RECORD );

		pQuery->GetLog().Printf( "%s\t%s\tR(%d)\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n",
					m_pMemento->GetUserNo().GetCSTR(),
					ISERVER()->GetID().GetCSTR(),
					m_pMemento->GetRoom()->GetIndex(),
					m_pResult->GetExpChange(),
					m_pResult->GetMoneyChange(),
					m_pResult->GetKillCount(),
					m_pResult->GetDeadCount(),
					m_pResult->GetHeadKillCount(),
					m_pResult->GetHeadDeadCount(),
					nWin,
					nLose,
					nDraw );

		STRING		strNewCharacter;
		STRING		strReason;
		if( m_pMemento->IsNewCharacter() == TRUE )
		{
			strNewCharacter = "Y";
		}
		else
		{
			strNewCharacter = "N";
		}
		if( i_eWinner == E_TEAM::CANCEL )
		{
			strReason		= "C";
		}
		else
		{
			strReason		= "N";
		}

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_RESULT_U" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( m_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_exp" ), m_pResult->GetExpChange(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_money" ), m_pResult->GetMoneyChange(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_kill_cnt" ), m_pResult->GetKillCount(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_death_cnt" ), m_pResult->GetDeadCount(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_h_kill_cnt" ), m_pResult->GetHeadKillCount(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_h_death_cnt" ), m_pResult->GetHeadDeadCount(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_win_cnt" ), nWin, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_lose_cnt" ), nLose, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_draw_cnt" ), nDraw, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_room_no" ), m_pMemento->GetRoom()->GetIndex(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment1" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment2" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment3" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment4" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment5" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment6" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_new_user_flag" ), strNewCharacter, TRUE, 1 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_reason_sort" ), strReason, TRUE, 1 );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_no" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_name" ), STRING( "" ), FALSE, 20 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_exp" ), nInt64, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_money" ), nInt64, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_death_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_h_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_h_death_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_win_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_lose_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_draw_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_max_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_max_h_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_rank" ), nInt, FALSE );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );
		IQUERY::IREQUESTER()->Regist( pQuery );
	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[REQ_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[REQ_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );
	}
	catch( ... )
	{
	}

	::LogicObject::Player::Result *
	pResult					= m_pResult->GetTotalResult();
	if( pResult->m_llExp + m_pResult->GetExpChange() > 0 )
	{
		pResult->m_llExp	+= m_pResult->GetExpChange();
	}
	else
	{
		pResult->m_llExp	= 0;
	}
	if( pResult->m_llMoney + m_pResult->GetMoneyChange() > 0 )
	{
		pResult->m_llMoney	+= m_pResult->GetMoneyChange();
	}
	else
	{
		pResult->m_llMoney	= 0;
	}
	pResult->m_nKill		+= m_pResult->GetKillCount();
	pResult->m_nDeath		+= m_pResult->GetDeadCount();
	pResult->m_nHeadKill	+= m_pResult->GetHeadKillCount();
	pResult->m_nHeadDead	+= m_pResult->GetHeadDeadCount();
	pResult->m_nWin			+= nWin;
	pResult->m_nLose		+= nLose;
	pResult->m_nDraw		+= nDraw;

	m_pMemento->GetStock().m_byGrade = IResult::IGRADE()->Get( pResult->m_llExp, m_pMemento->GetStock().m_byGrade );

	this->castResultTotal();
}

void
Provider::RecordPenalty()
{
	if( m_pMemento->IsLoading() == FALSE || m_pMemento->IsSpectator() == TRUE )
	{
		return;
	}

	if( m_pMemento->IsReplayReady() == TRUE )
	{
		STRING strLog;
		strLog.Printf( "Record\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"ReplayReady",
					m_pMemento->GetRoom()->GetIndex(),
					m_pMemento->GetStock().GetNick() );
		::Dispatcher::ICONSOLE()->Write( strLog );

		this->RecordResult( IPlayer::E_TEAM::CANCEL );
		return;
	}

	STRING strLog;
	strLog.Printf( "Record\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Penalty",
					m_pMemento->GetRoom()->GetIndex(),
					m_pMemento->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	typedef					Query::Result
							RESULT;
	typedef					::Dispatcher::IQuery
							IQUERY;

	try
	{
		INT nInt				= -99;
		INT64 nInt64			= 0;
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( RESULT::E_CODE::REQ_RECORD );

		pQuery->GetLog().Printf( "%s\t%s\tR(%d)\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n",
					m_pMemento->GetUserNo().GetCSTR(),
					ISERVER()->GetID().GetCSTR(),
					m_pMemento->GetRoom()->GetIndex(),
					IFormula::ICONTAINER()->GetPenaltyExp(),
					IFormula::ICONTAINER()->GetPenaltyMoney(),
					0,
					0,
					0,
					0,
					0,
					1,
					0 );

		STRING		strNewCharacter;
		STRING		strReason;
		if( m_pMemento->IsNewCharacter() == TRUE )
		{
			strNewCharacter = "Y";
		}
		else
		{
			strNewCharacter = "N";
		}
		strReason			= "P";

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_RESULT_U" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( m_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_exp" ), IFormula::ICONTAINER()->GetPenaltyExp(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_money" ), IFormula::ICONTAINER()->GetPenaltyMoney(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_kill_cnt" ), 0, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_death_cnt" ), 0, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_h_kill_cnt" ), 0, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_h_death_cnt" ), 0, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_win_cnt" ), 0, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_lose_cnt" ), 1, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_draw_cnt" ), 0, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_room_no" ), m_pMemento->GetRoom()->GetIndex(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment1" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment2" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment3" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment4" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment5" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_equipment6" ), nInt, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_new_user_flag" ), strNewCharacter, TRUE, 1 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_reason_sort" ), strReason, TRUE, 1 );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_no" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_name" ), STRING( "" ), FALSE, 20 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_exp" ), nInt64, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_money" ), nInt64, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_death_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_h_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_h_death_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_win_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_lose_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_draw_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_max_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_max_h_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_rank" ), nInt, FALSE );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );
		IQUERY::IREQUESTER()->Regist( pQuery );
	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[REQ_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[REQ_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );
	}
	catch( ... )
	{
	}

	::LogicObject::Player::Result *
	pResult					= m_pResult->GetTotalResult();
	if( pResult->m_llExp + IFormula::ICONTAINER()->GetPenaltyExp() > 0  )
	{
		pResult->m_llExp	+= IFormula::ICONTAINER()->GetPenaltyExp();
	}
	else
	{
		pResult->m_llExp	= 0;
	}
	if( pResult->m_llMoney + IFormula::ICONTAINER()->GetPenaltyMoney() > 0 )
	{
		pResult->m_llMoney	+= IFormula::ICONTAINER()->GetPenaltyMoney();
	}
	else
	{
		pResult->m_llMoney	= 0;
	}

	pResult->m_nLose		+= 1;

	m_pMemento->GetStock().m_byGrade = IResult::IGRADE()->Get( pResult->m_llExp, m_pMemento->GetStock().m_byGrade );

	this->castResultTotal();
}

void
Provider::CastResult()
{
	typedef					::LogicObject::Player::Type::E_STATE
							E_STATE;
	typedef					::Packet::Service::Game::SN_ResultPlayer
							SN_RESULT;

	if( m_pMemento->IsLoading() == FALSE )
	{
		return;
	}

	IRoom * pRoom			= m_pMemento->GetRoom();

	IPlayer * pPlayer		= pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->IsLoading() == FALSE )
		{
			pPlayer			= pRoom->GetNextPlayer();
			continue;
		}

		SN_RESULT * pResult	= ::Packet::Builder::Build<SN_RESULT>();
		
		pResult->m_byTeam	= pPlayer->GetMemento()->GetTeam();
		pResult->m_nKill	= pPlayer->GetResult()->GetKillCount();
		pResult->m_nDead	= pPlayer->GetResult()->GetDeadCount();
		pResult->m_nExp		= pPlayer->GetResult()->GetExpChange();
		pResult->m_nMoney	= pPlayer->GetResult()->GetMoneyChange();
		pResult->m_byGrade	= pPlayer->GetMemento()->GetStock().m_byGrade;
		pResult->SetNick( pPlayer->GetMemento()->GetStock().GetNick() );

		ICASTER()->Cast( m_pMemento->GetPlayer(), pResult );
		
		pPlayer				= pRoom->GetNextPlayer();
	}
}

void
Provider::InitPlay()
{
	m_pMemento->SetLoading( FALSE );
	m_pMemento->SetReplayReady( FALSE );
	m_pMemento->SetReady( FALSE );
	m_pMemento->SetDead( FALSE );

	m_pMemento->SetNetLoss( 0 );

	m_pResult->Reset();
}

void
Provider::SetLogined()
{
	m_bLogined				= TRUE;
}

CBOOL
Provider::IsLogined()
{
	return m_bLogined;
}

inline void
Provider::castResultTotal()
{
	typedef					::Packet::Service::Player::SN_Result
							SN_RESULT;

	SN_RESULT * pNoti		= ::Packet::Builder::Build<SN_RESULT>();
	pNoti->m_byGrade		= m_pMemento->GetStock().m_byGrade;
	pNoti->m_stResult		= * m_pResult->GetTotalResult();

	ICASTER()->Cast( this, pNoti );
}

} /* Player */				} /* GamerServer */