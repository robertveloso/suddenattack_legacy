#include "StdAfx.h"

#include "./Provider.h"
#include "../IPlayer.h"
#include "../IManager.h"
#include "../IRoom.h"

#include "../../Dispatcher/IConsole.h"
#include "../../Dispatcher/ITime.h"

#include "../../Share/Packet/Service/Play.h"

namespace GameServer {		namespace Caster {

Provider::Provider()
{
	this->install();
}

Provider::~Provider()
{
	this->uninstall();
}

void
Provider::Cast( IManager * i_pManager, PACKET * i_pPacket )
{
	Assert( i_pManager != NULL );
	Assert( i_pPacket != NULL );

	IManager::ISESSION()->Send( i_pPacket, i_pManager );
}

void
Provider::Cast( IPlayer * i_pPlayer, PACKET * i_pPacket )
{
	Assert( i_pPlayer != NULL );
	Assert( i_pPacket != NULL );

	IPlayer::ISESSION()->Send( i_pPacket, i_pPlayer );

	if( i_pPacket->GetCommand() == ::Packet::Service::Play::SN_ReturnBuff::s_nCode )
	{
		STRING strLog;
		strLog.Printf( "Dev\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"SN_ReturnBuff",
					i_pPlayer->GetIndex() );
		::Dispatcher::ICONSOLE()->Write( strLog );
	}
}

void
Provider::Cast( IRoom * i_pRoom, PACKET * i_pPacket )
{
	Assert( i_pRoom != NULL );
	Assert( i_pPacket != NULL );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		IPlayer::ISESSION()->Send( i_pPacket, pPlayer );

		if( i_pPacket->GetCommand() == ::Packet::Service::Play::SN_ReturnBuff::s_nCode )
		{
			STRING strLog;
			strLog.Printf( "Dev\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"SN_ReturnBuff",
					pPlayer->GetIndex() );
			::Dispatcher::ICONSOLE()->Write( strLog );
		}
		
		pPlayer				= i_pRoom->GetNextPlayer();
	}
}

void
Provider::Cast( IRoom * i_pRoom, IPlayer * i_pExceptPlayer, PACKET * i_pPacket )
{
	Assert( i_pRoom != NULL );
	Assert( i_pExceptPlayer != NULL );
	Assert( i_pPacket != NULL );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer == i_pExceptPlayer )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}
		IPlayer::ISESSION()->Send( i_pPacket, pPlayer );
		
		if( i_pPacket->GetCommand() == ::Packet::Service::Play::SN_ReturnBuff::s_nCode )
		{
			STRING strLog;
			strLog.Printf( "Dev\t%s\t%s\tU(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"SN_ReturnBuff",
					pPlayer->GetIndex() );
			::Dispatcher::ICONSOLE()->Write( strLog );
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}
}

void
Provider::Signal( IPlayer * i_pPlayer, CUINT i_uiMessage )
{
	Assert( i_pPlayer != NULL );

	IPlayer::ISESSION()->Signal( i_uiMessage, i_pPlayer );
}

} /* Catster */				} /* GameServer */