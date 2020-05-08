#include "StdAfx.h"

#include "./Provider.h"
#include "./Peer.h"
#include "../IClient.h"
#include "../Console/Provider.h"
#include "../Query/Requester.h"
#include "../Query/Responser.h"
#include "../Query/Factory.h"
#include "../Query/Builder.h"
#include "../Client/Register.h"
#include "../Client/Session.h"
#include "../ITime.h"
#include "../../Jolyo/NJean/Application/Packet.h"


namespace Dispatcher {		namespace Handler {

Provider::Provider()
{
	m_pPeerPool				= new PEER_POOL;
	
	m_pClientRegister		= new Client::Register;
	m_pClientSession		= new Client::Session( this );

	m_pQueryFactory			= NULL;
	m_pQueryBuilder			= new Query::Builder;
	m_pQueryRequester		= new Query::Requester( this );
	m_pQueryResponser		= new Query::Responser();

	m_pConsole				= new Console::Provider();
	m_pTime					= new Time::Provider();

	this->install();
}

Provider::~Provider()
{
	this->uninstall();

	SAFE_DELETE( m_pTime );
	SAFE_DELETE( m_pConsole );
	SAFE_DELETE( m_pQueryResponser );
	SAFE_DELETE( m_pQueryRequester );
	SAFE_DELETE( m_pQueryBuilder );
	SAFE_DELETE( m_pQueryFactory );

	SAFE_DELETE( m_pClientSession );
	SAFE_DELETE( m_pClientRegister );

	SAFE_DELETE( m_pPeerPool );

}

CBOOL
Provider::Initialize( const Argument & i_stArg )
{
	Assert( m_pQueryFactory == NULL );

	m_pQueryFactory			= new Query::Factory( this, i_stArg.nQueryQSize );

	typedef					::Session::Manage::CSessionManager
							MANAGER;

	m_nClientCount			= i_stArg.nClientCount;
	
	m_pPeerPool->Create( m_nClientCount );


	//
	// Create Engine
	//
	BOOL bRet = this->Create(
							1000,			// TickTime
							10,				// MaxTick
							500,			// TimerEventQueueSize
							4,				// AliveTickError
							15,				// AliveTimeoutCount
							m_nClientCount,
							MANAGER::TCP_IOCP,
							MANAGER::ADO,
							i_stArg.nRecvQSize,
							i_stArg.nSendQSize,
							i_stArg.nQueryQSize,
							i_stArg.nSignalQSize,
							1,
							i_stArg.nClientCount,
							0,						// Packet Size는 다른곳에서 로드한다.
							i_stArg.nResponseBufferSize
							);
	
	Assert( bRet == TRUE );

	//
	// Create Listener
	//
	bRet					= this->CreateListener( s_nService, m_nClientCount, i_stArg.nAcceptPoolSize );
	Assert( bRet == TRUE );


	//
	// Create Peer
	//
	bRet					= m_pPeerPool->Create( m_nClientCount );
	Assert( bRet == TRUE );

	Peer ** paPeer = new Peer * [ m_nClientCount ];
	for( INT i = 0; i < m_nClientCount; i++ )
	{
		paPeer[i]			= m_pPeerPool->Alloc();
//		paPeer[i]->Create( CPacket::PredefineLength::BasePacketSize );
		paPeer[i]->Create( 4096 );
	}
	for( INT i = 0; i < m_nClientCount; i++ )
	{
		m_pPeerPool->Free( paPeer[i] );
	}
	delete[] paPeer;


	//
	// Create End
	//
	bRet					= this->CreateEnd();
	Assert( bRet == TRUE );


	//
	// Create DB
	//
	INT nConnect			= this->CreateDatabase(
								i_stArg.nDbConnect,
								i_stArg.strDbConnection.GetCSTR() );
	Assert( nConnect == i_stArg.nDbConnect );


	CSessionGate::StartListen( s_nService, i_stArg.nPortNumber );
	

	return TRUE;
}

void
Provider::Start( const Argument & i_stArg )
{
	CSessionGate::Start( 3 );

	m_bTickCount = FALSE;
	ITIME()->SetTime( FALSE );
}

const String<17> &
Provider::GetPeerIP( PEER * i_pPeer )
{
	m_strIP					= this->GetPeerAddr( i_pPeer );
	
	return m_strIP;
}

CULONG
Provider::GetTickCount()
{
	if( m_bTickCount == TRUE )
	{
		return m_ulTickCount;
	}
	
	m_bTickCount			= TRUE;
	m_ulTickCount			= ::GetTickCount();
	return m_ulTickCount;
}

Provider::COMMAND *
Provider::AllocDbCommand()
{
	return this->AllocDbObject();
}

void
Provider::FreeDbCommand( COMMAND * i_pCommand )
{
	return this->FreeDbObject( i_pCommand );
}

void
Provider::Request( Application::CPeer* pPeer, UINT uiMessage, ::Session::Manage::CDbObject* pDbObject, void * i_pArg )
{
	this->RequestADO( pPeer, uiMessage, pDbObject, i_pArg );
}

void
Provider::Alarm( UINT uiMessage, UINT uiSecond, ULONG_PTR pParam )
{
	this->AddTimerSignal( uiSecond, uiMessage, pParam, NULL );
}

void
Provider::Signal( UINT uiMessage, ULONG_PTR pParam )
{
	this->AddSignal( uiMessage, pParam, NULL );
}

void
Provider::Send( Application::CPeer* pPeer, const char* pData, INT nDataLen )
{
	this->SendData( pPeer, pData, nDataLen );
}

void
Provider::Close( Application::CPeer * i_pPeer )
{
	this->CloseRequest( i_pPeer );
}

CDWORD
Provider::GetRTT( Application::CPeer * i_pPeer )
{
	return this->GetRttTime( i_pPeer );
}

void
Provider::OnSignal( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam )
{
	IClient * pClient		= reinterpret_cast<IClient *>( wParam );
	Assert( pClient != NULL );
	pClient->OnSignal( uiMessage );
}

void
Provider::OnResponse( Application::CPeer * i_pPeer, ::Session::Manage::CDbObject * i_pDbObject )
{
	m_bTickCount			= FALSE;
	ITIME()->SetTime( FALSE );

	IQuery * pQuery			= reinterpret_cast<IQuery *>( i_pDbObject->GetArg() );
	if( i_pDbObject->IsSuccess() )
	{
		pQuery->SetResult( IQuery::E_RESULT::SUCCESS );
	}
	else
	{
		pQuery->SetResult( IQuery::E_RESULT::FAIL );
	}

	if( i_pPeer == NULL )
	{
		IQuery::IREQUESTER()->GetObserver()->OnResponse( pQuery );
	}
	else
	{
		Peer * pPeer		= static_cast<Peer *>( i_pPeer );
		pPeer->GetClient()->OnResponse( pQuery );
	}

	IQuery::IFACTORY()->Destroy( pQuery );
}

//void
//Provider::OnAccept( Application::CPeer* pPeer, INT nServicerIndex )
//{
//}
//
//void
//Provider::OnConnect( Application::CPeer* pPeer, INT nServicerIndex )
//{
//}

void
Provider::OnClose( Application::CPeer* i_pPeer, INT nServicerIndex )
{
	m_bTickCount			= FALSE;
	ITIME()->SetTime( FALSE );

	typedef					IClient::PACKET
							PACKET;

	Peer * pPeer			= static_cast<Peer *>( i_pPeer );
	IClient * pClient		= pPeer->GetClient();

	if( pClient == NULL )
	{
		return;
	}

	pClient->OnClose();
	pPeer->SetClient( NULL );

}

void
Provider::OnData( Application::CPeer* i_pPeer, INT i_nServicerIndex, char* i_pPacket, int i_nPacketLength )
{
	m_bTickCount			= FALSE;
	ITIME()->SetTime( FALSE );

	typedef					IClient::PACKET
							PACKET;

	Peer * pPeer			= static_cast<Peer *>( i_pPeer );
	PACKET * pPacket		= reinterpret_cast<PACKET *>( i_pPacket );

	if( pPacket->GetLength() != i_nPacketLength )
	{
//		DEBUG_BREAK;
		STRING strOutput;
		strOutput.Printf( "Wrong Packet : %d\r\n", pPacket->GetCommand() );
		ICONSOLE()->Output( strOutput );
		return;
	}

	IClient * pClient		= pPeer->GetClient();
	if( pClient == NULL )
	{
		IClient::IBuilder *
		pBuilder			= IClient::IREGISTER()->First();
		while( pBuilder != NULL )
		{
			pClient			= pBuilder->Build( pPacket, pPeer );
			if( pClient != NULL )
			{
				pPeer->SetClient( pClient );
				break;
			}
			pBuilder		= IClient::IREGISTER()->Next();
		}
	}

	if( pClient == NULL )
	{
		this->CloseRequest( i_pPeer );
		return;
	}

	pClient->OnPacket( pPacket );
}

void
Provider::OnTimeout( Application::CPeer* pPeer )
{
#ifndef _DEVDEBUG
	TimeoutDisconnectRequest( pPeer );
	STRING strLog;
	strLog.Printf( "OnTimeout\t%s\r\n", ITIME()->GetTime().GetCSTR() );
	ICONSOLE()->Write( strLog );
#endif

}

Application::CPeer *
Provider::AllocPeer( INT nListenIndex )
{
	Peer * pPeer			= NULL;
	if( m_pPeerPool->GetAllocatedSize() < m_nClientCount )
	{
		pPeer	= m_pPeerPool->Alloc();
		pPeer->Capture();
	}

	return pPeer;

}

void
Provider::FreePeer( Application::CPeer * i_pPeer, INT nListenIndex )
{
	Peer * pPeer			= static_cast<Peer *>( i_pPeer );
	pPeer->Release();
	m_pPeerPool->Free( pPeer );
}

void
Provider::InitListenerPacketType( INT nServicerIndex, CHECKSUM::CheckSumType & out_CheckSumType, PACKETASSEMBLER::PacketUnitType & out_PacketUnitType )
{
	out_CheckSumType		= CHECKSUM::CheckSumType::TwoByteZoroOne;
	out_PacketUnitType		= PACKETASSEMBLER::Length;

}

void
Provider::InitLengthFieldOfPacket( INT nServicerIndex, INT & out_nLengthFieldPos, PACKETASSEMBLER::LengthSizeType & out_lengthSizeType, PACKETASSEMBLER::LengthType & out_lengthType, BOOL & out_bLittleEndian )
{
	out_nLengthFieldPos		= IClient::PACKET::GetLengthPos();
	out_lengthSizeType		= PACKETASSEMBLER::LengthSizeType::TwoByte;
	out_lengthType			= PACKETASSEMBLER::LengthType::Total;
	out_bLittleEndian		= TRUE;

}

BOOL
Provider::IsAliveManageObject( INT nServicerIndex )
{
	return TRUE;
}

} /* Handler */				} /* Dispatcher */