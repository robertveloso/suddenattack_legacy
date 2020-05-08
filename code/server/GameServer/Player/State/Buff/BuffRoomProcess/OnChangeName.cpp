#include "StdAfx.h"

#include "./OnChangeOption.h"

#include "../../../../IRoom.h"
#include "../../../../ICaster.h"
#include "../../../../IChannel.h"
//
#include "../../../../../Share/Packet/Builder.h"
#include "../../../../../Share/Packet/Service/Room.h"

#include "../../../../../Dispatcher/IConsole.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

void
OnChangeOption::Excute( Memento * i_pMemento, PACKET * i_pPacket )
{
	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetCaptain() )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "WARNNING : OnChangeOption 방장이 아닌 사용자가 옵션 변경 요청\n" ) );
		return;
	}

	namespace				R = ::Packet::Service::Room;

	R::NN_ChangeOption *
	pRelay					= static_cast<R::NN_ChangeOption *>( i_pPacket );
	Assert( i_pPacket->GetCommand() == R::NN_ChangeOption::s_nCode );

	CBOOL bSuccess = OnChangeOption::checkTeamCount( i_pMemento, pRelay->m_stOption.GetTeamMax() );
	if( bSuccess == FALSE )
	{
		R::SN_ChangeOptionFail *
		pAck				= ::Packet::Builder::Build<R::SN_ChangeOptionFail>();
		pAck->m_byResult	= static_cast<unsigned char>( R::SN_ChangeOptionFail::E_RESULT::FAIL_TEAMMAX );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	ICASTER()->Cast( i_pMemento->GetRoom(), pRelay );

	i_pMemento->GetRoom()->SetOption( pRelay->m_stOption );

	R::SN_ChangeOption *
	pNoti					= ::Packet::Builder::Build<R::SN_ChangeOption>();
	pNoti->m_nRoomIndex		= i_pMemento->GetRoom()->GetIndex();
	pNoti->m_stOption		= pRelay->m_stOption;
	ICASTER()->Cast( i_pMemento->GetChannel()->GetLobby(), pNoti );
}

inline CBOOL
OnChangeOption::checkTeamCount( Memento * i_pMemento, CINT i_nCount )
{
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;
	INT nTeamRed			= 0;
	INT nTeamBlue			= 0;

	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->GetTeam() == E_TEAM::RED )
		{
			nTeamRed ++;
		}
		else
		{
			nTeamBlue ++;
		}
		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}

	if( nTeamRed > i_nCount &&
		nTeamBlue > i_nCount )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */