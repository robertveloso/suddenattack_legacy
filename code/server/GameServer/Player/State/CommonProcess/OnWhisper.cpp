#include "StdAfx.h"

#include "./OnWhisper.h"

#include "../../../ICaster.h"
#include "../../../ILog.h"

#include "../../../../Dispatcher/ITime.h"

#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Player.h"

namespace GameServer {		namespace Player {			namespace State {
namespace CommonProcess {

void
OnWhisper::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Player::NN_Whisper
							NN_WHISPER;
	typedef					::Packet::Service::Player::SN_WhisperFail
							SN_WHISPERFAIL;
	typedef					IPlayer::IContainer::NICK
							NICK;
	NN_WHISPER * pRelay		= static_cast<NN_WHISPER *>( i_pPacket );

	IPlayer * pPlayer		= IPlayer::ICONTAINER()->Find( NICK( pRelay->GetNick() ) );
	if( pPlayer == NULL )
	{
		SN_WHISPERFAIL *
		pAck				= ::Packet::Builder::Build<SN_WHISPERFAIL>();
		pAck->m_byResult	= SN_WHISPERFAIL::E_RESULT::NOT_EXIST;
		pAck->SetNick( pRelay->GetNick() );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	if( pPlayer->GetState() == IPlayer::E_STATE::INVENTORY )
	{
		SN_WHISPERFAIL *
		pAck				= ::Packet::Builder::Build<SN_WHISPERFAIL>();
		pAck->m_byResult	= SN_WHISPERFAIL::E_RESULT::INVENTORY;
		pAck->SetNick( pRelay->GetNick() );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	NN_WHISPER * pSend		= ::Packet::Builder::Build<NN_WHISPER>();
	pSend->Set( i_pMemento->GetStock().GetNick(), pRelay->GetChat() );
	ICASTER()->Cast( pPlayer, pSend );

	STRING					strLog;
	strLog.Printf( "%s\tWHISPER\t%s\t%s\t%s\r\n",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							i_pMemento->GetStock().GetNick(),
							pPlayer->GetMemento()->GetStock().GetNick(),
							pRelay->GetChat() );
	ILog::IFACADE()->Chatting( strLog );
}

} /* CommonProcess */
} /* State */				} /* Player */				} /* GameServer */
