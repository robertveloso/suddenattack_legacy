#include "StdAfx.h"

#include "./OnChangeTeam.h"

#include "../../../../IRoom.h"
#include "../../../../ICaster.h"

#include "../../../../../dispatcher/IConsole.h"

#include "../../../../../Share/Packet/Builder.h"
#include "../../../../../Share/Packet/Service/Player.h"
#include "../../../../../Share/LogicObject/Room/Option.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

void
OnChangeTeam::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;
	namespace				P = ::Packet::Service::Player;

	P::NN_ChangeTeam *
	pRelay					= static_cast<P::NN_ChangeTeam  *>( i_pPacket );
	Assert( i_pPacket->GetCommand() == P::NN_ChangeTeam ::s_nCode );

	if( pRelay->m_nIndex != i_pMemento->GetPlayer()->GetIndex() )
	{
		return;
	}

	if( OnChangeTeam::isChangable( i_pMemento, pRelay->m_byTeam ) == FALSE )
	{
		return;
	}

	if( pRelay->m_byTeam == E_TEAM::RED )
	{
		i_pMemento->SetTeam( static_cast<E_TEAM::ENUM>( E_TEAM::RED ) );
	}
	else if( pRelay->m_byTeam == E_TEAM::BLUE )
	{
		i_pMemento->SetTeam( static_cast<E_TEAM::ENUM>( E_TEAM::BLUE ) );
	}
	else
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "[BAD] Wrong Change Team\r\n" ) );
		::Dispatcher::ICONSOLE()->Write( STRING( "[BAD] Wrong Change Team\r\n" ) );
		i_pMemento->SetTeam( static_cast<E_TEAM::ENUM>( E_TEAM::BLUE ) );
	}

	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		ICASTER()->Cast( pPlayer, pRelay );
		pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
	}

}

inline CBOOL
OnChangeTeam::isChangable( Memento * i_pMemento, CBYTE i_byTeam )
{
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	UINT nTeamCount			= 0;
	E_TEAM::ENUM eTeam		= static_cast<E_TEAM::ENUM>( i_byTeam );

	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->GetTeam() == eTeam )
		{
			nTeamCount ++;
		}
		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}

	if( nTeamCount + 1 <= i_pMemento->GetRoom()->GetOption().GetTeamMax() )
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