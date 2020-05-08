#include "StdAfx.h"

#include "./NoneCharacter.h"
#include "./Character.h"
#include "./../Fin.h"
#include "../../Memento.h"
#include "../../../ICaster.h"
#include "../../../Query.h"
#include "../../../../Dispatcher/IQuery.h"
#include "../../../../Dispatcher/IConsole.h"
#include "../../../../Dispatcher/ITime.h"
#include "../../../../Share/Packet/Login.h"
#include "../../../../Share/Packet/Builder.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

NoneCharacter::NoneCharacter()
{
	this->install();
}

NoneCharacter::~NoneCharacter()
{
	this->uninstall();
}

CBOOL
NoneCharacter::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Login::CN_Character
							CN_CHARACTER;
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	if( i_pPacket->GetCommand() != CN_CHARACTER::s_nCode )
	{
		return TRUE;
	}

	CN_CHARACTER * pReq		= static_cast<CN_CHARACTER *>( i_pPacket );
	i_pMemento->GetCharacters()[ E_TEAM::RED ]	= pReq->stRed;
	i_pMemento->GetCharacters()[ E_TEAM::BLUE ]	= pReq->stBlue;

	this->requestCharacter( i_pMemento );
	
	i_pMemento->SetState( CHARACTER() );

	return TRUE;
}

void
NoneCharacter::OnClose( Memento * i_pMemento )
{
	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
NoneCharacter::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
NoneCharacter::requestCharacter( Memento * i_pMemento )
{
	typedef					Query::Session
							SESSION;
	typedef					::LogicObject::Player::Character
							CHARACTER;
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	CHARACTER & stRed		= i_pMemento->GetCharacters()[ E_TEAM::RED ];
	CHARACTER & stBlue		= i_pMemento->GetCharacters()[ E_TEAM::BLUE ];

	INT nInt = -99;
	
	//STRING strLog;
	//strLog.Printf( "[DB_REQ][%s] %d %s %d %d %d %d %d %d\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), SESSION::E_CODE::REQ_CHARACTER, i_pMemento->GetUserNo().GetCSTR(),
	//					stRed.m_nModel, stRed.m_nHeadSkin, stRed.m_nBodySkin, stBlue.m_nModel, stBlue.m_nHeadSkin, stBlue.m_nBodySkin);
	//::Dispatcher::ICONSOLE()->Write( strLog );

	try
	{
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SESSION::E_CODE::REQ_CHARACTER );

		pQuery->GetLog().Printf( "%s\t%d\t%d\t%d\t%d\t%d\t%d\r\n",
						i_pMemento->GetUserNo().GetCSTR(),
						stRed.m_nModel,
						stRed.m_nHeadSkin,
						stRed.m_nBodySkin,
						stBlue.m_nModel,
						stBlue.m_nHeadSkin,
						stBlue.m_nBodySkin );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_CHARACTER_U" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_a_model_id" ), stRed.m_nModel, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_a_headskin_id" ), stRed.m_nHeadSkin, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_a_bodyskin_id" ), stRed.m_nBodySkin, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_b_model_id" ), stBlue.m_nModel, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_b_headskin_id" ), stBlue.m_nHeadSkin, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_b_bodyskin_id" ), stBlue.m_nBodySkin, TRUE );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );

		IQUERY::IREQUESTER()->Regist( pQuery, i_pMemento->GetPlayer() );
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

		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
	}
	catch( ... )
	{
		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
	}

}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */