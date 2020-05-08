#include "StdAfx.h"

#include "./Construct.h"

namespace GameServer {

Construct::Construct()
{
	m_pServer					= new Server::Provider;
	m_pDispatcherHandler		= new ::Dispatcher::Handler::Provider;

	m_pPacketBuilder			= new ::Packet::Builder;
	
	m_pChannelFactory			= new Channel::Factory;
	m_pRoomFactory				= new Room::Factory;
	m_pPlayerBuilder			= new Player::Builder;
	m_pPlayerContainer			= new Player::Container;
	m_pNoneResponser			= new NoneResponser::Provider;
	m_pNoneSignal				= new NoneSignal::Provider;
	m_pCaster					= new Caster::Provider;
	m_pFormulaContainer			= new Formula::Container;
	m_pTableFactory				= new Table::Factory;

	m_pManagerBuilder			= new Manager::Builder;
	m_pManagerProcessContainer	= new Manager::Process::Container;

	m_pLogFactory				= new Log::Factory;

	IPlayer::IREGISTER()->Regist( m_pPlayerBuilder );
	IPlayer::IREGISTER()->Regist( m_pManagerBuilder );
}

Construct::~Construct()
{
	SAFE_DELETE( m_pLogFactory );

	SAFE_DELETE( m_pManagerProcessContainer );
	SAFE_DELETE( m_pManagerBuilder );

	SAFE_DELETE( m_pTableFactory );
	SAFE_DELETE( m_pFormulaContainer );
	SAFE_DELETE( m_pCaster );
	SAFE_DELETE( m_pRoomFactory );
	SAFE_DELETE( m_pChannelFactory );
	SAFE_DELETE( m_pNoneSignal );
	SAFE_DELETE( m_pNoneResponser );
	SAFE_DELETE( m_pPlayerBuilder );
	SAFE_DELETE( m_pPlayerContainer );

	SAFE_DELETE( m_pPacketBuilder );

	SAFE_DELETE( m_pDispatcherHandler );

	SAFE_DELETE( m_pServer );
}

} /* GameServer */