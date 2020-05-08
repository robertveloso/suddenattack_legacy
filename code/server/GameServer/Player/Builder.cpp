#include "StdAfx.h"

#include "./Builder.h"

#include "../../Share/Packet/Login.h"
#include "../../Dispatcher/IHandler.h"

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

namespace GameServer {		namespace Player {

Builder::Builder()
{
	m_pPool					= new POOL;
	m_pList					= new LIST;

	m_bLock					= TRUE;

	m_pList->PushHead( new State::Fin() );

	m_pList->PushHead( new State::Login::Character() );
	m_pList->PushHead( new State::Login::Closing() );
	m_pList->PushHead( new State::Login::Init() );
	m_pList->PushHead( new State::Login::Lobby() );
	m_pList->PushHead( new State::Login::Logined() );
	m_pList->PushHead( new State::Login::Logining() );
	m_pList->PushHead( new State::Login::Nick() );
	m_pList->PushHead( new State::Login::NoneCharacter() );
	m_pList->PushHead( new State::Login::NoneNick() );
	m_pList->PushHead( new State::Login::Ready() );

	m_pList->PushHead( new State::Buff::BuffRoom() );
	m_pList->PushHead( new State::Buff::ReturnBuff() );
	m_pList->PushHead( new State::Buff::Inventory() );
	m_pList->PushHead( new State::Buff::PlayReady() );

	m_pList->PushHead( new State::Play::BombRoom() );
	m_pList->PushHead( new State::Play::BombReady() );
	m_pList->PushHead( new State::Play::RespawnRoom() );

	m_pGrade				= new Result::Grade;

	this->install();

}

Builder::~Builder()
{
	this->uninstall();

	SAFE_DELETE( m_pGrade );

	IState * pState			= m_pList->PopHead();
	while( pState != NULL )
	{
		SAFE_DELETE( pState );
		pState				= m_pList->PopHead();
	}
	SAFE_DELETE( m_pList );
	SAFE_DELETE( m_pPool );
}


Builder::ICLIENT *
Builder::Build( PACKET * i_pPacket, PEER * i_pPeer )
{
	Assert( i_pPacket != NULL );

	if( m_bLock == TRUE )
	{
		return NULL;
	}

	switch( i_pPacket->GetCommand() )
	{
	case ::Packet::Login::CQ_Login::s_nCode :
	case ::Packet::Login::CQ_LoginNetmarble::s_nCode :
		cout << ::Dispatcher::IHANDLER()->GetPeerIP( i_pPeer ) .GetCSTR() << " Login in ok" << endl;
		break;
	default :
		cout << ::Dispatcher::IHANDLER()->GetPeerIP( i_pPeer ) .GetCSTR() << " Login in error" << endl;
		return NULL;
	}

	Provider * pProvider	= m_pPool->Capture();
	pProvider->Capture( i_pPeer );

	return pProvider;
}

void
Builder::Destroy( IPlayer * i_pPlayer )
{
	Assert( i_pPlayer != NULL );

	Provider * pProvider	= static_cast<Provider *>( i_pPlayer );
	pProvider->Release();

	m_pPool->Release( pProvider );
}

void
Builder::SetLock( CBOOL i_bLock, CBYTE i_byFinMin, CBYTE i_byType )
{
	m_bLock					= i_bLock;
	m_byFinishMin			= i_byFinMin;
	m_byFinishType			= i_byType;
}

CBOOL
Builder::IsLock()
{
	return m_bLock;
}

CBYTE
Builder::GetFinishType()
{
	Assert( m_bLock == TRUE );
	
	return m_byFinishType;
}

CBYTE
Builder::GetFinishMin()
{
	Assert( m_bLock == TRUE );

	return m_byFinishMin;
}

} /* Player */				} /* GamerServer */