#include "StdAfx.h"

#include "ClientGate.h"

#include "NJean/Application/Packet.h"
using namespace Application::Packets;

using namespace GamehiSolution::ClientModule;
using namespace GamehiSolution::Application;
using namespace GamehiSolution::Session::Message;
using namespace GamehiSolution::Session::Manage;
using namespace GamehiSolution::Session::Gate;
using namespace GamehiSolution::Common::DataStructure;


CClientGate::CClientGate()
{
}

CClientGate::~CClientGate()
{
}

BOOL CClientGate::Create( /*IClientGateHandler* pClientGateHandler,*/ HWND hWindow, UINT uiMessage, INT nMaxPacketSize )
{
	CSessionManager::NetworkType
			networkType				= CSessionManager::NetworkType::TCP_ASYNCSELECT;
	CSessionManager::QueryType
			queryType				= CSessionManager::QueryType::HTTP;
	int		nRecvQueueSize			= 1000;
	int		nSendQueueSize			= 1000;
	int		nQueryQueueSize			= 1000;
	int		nSignalQueueSize		= 1000;
	int		nNumOfListener			= 1;
	int		nNumOfConnectSockets	= 10;
	int		nPacketSize				= 256;
	int		nResponseBuffer			= 65536;	// 64KB

//	m_pClientGateHandler			= pClientGateHandler;

	BOOL bRet = CSessionGate::Create(
					networkType,
					queryType,
					nRecvQueueSize,
					nSendQueueSize,
					nQueryQueueSize,
					nSignalQueueSize,
					nNumOfListener,
					nNumOfConnectSockets,
					nPacketSize,
					nResponseBuffer,
					hWindow,
					uiMessage );
	if( !bRet ) return FALSE;

	m_peer.Create( nMaxPacketSize );

	//
	// 여기에 소스 코드를 추가한다.
	//


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 작성자 : 강진협
	// 작성일 : 2004년 5월 21일
	// 내  용 : Client Listener 생성
	//		
	//
	
	int		nNumOfCommSockets;		// 해당 서비서를 통해 연결될 수 있는 클라이언트의 개수

	// Listener 생성
//	nServicerIndex		= ServicerIndex::Client;
	nNumOfCommSockets	= 10;
	bRet = CreateListener( ServicerType::Peers, nNumOfCommSockets, 0 );
	if( !bRet ) return FALSE;

	// 해당 Listener의 Client들의 메모리 풀 생성
	bRet = m_PeerPool.Create( nNumOfCommSockets );
	if( !bRet ) return FALSE;

	m_nNumOfClients		= nNumOfCommSockets;
//	m_nAliveManaged		= nNumOfCommSockets;

	// Listener 생성의 끝
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////



	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	bRet = CreateEnd();
	if( !bRet ) return FALSE;

	m_bConnected = FALSE;

	return TRUE;
}

INT CClientGate::Start( INT nPort )
{
	int		nServicerIndex;			// 연결되는 서비서 인덱스
//	int		nPort;					// 서비스할 포트 번호

	nServicerIndex		= ServicerType::Peers;
//	nPort				= 4444;
	while( TRUE )
	{
		if( StartListen( nServicerIndex, nPort ) )
			break;
		nPort++;
	}

	return nPort;

	// 여기에 소스 코드를 추가한다.

	/*

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 작성자 : 강진협
	// 작성일 : 2004년 5월 21일
	// 내  용 : Client Listener Accept 시작
	//
	int		nServicerIndex;			// 연결되는 서비서 인덱스
	int		nPort;					// 서비스할 포트 번호

	nServicerIndex		= ServicerIndex::Client;
	nPort				= 5555;
	StartListen( nServicerIndex, nPort );
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	*/

	int		nTickTime;				// Tick 발생 타임 (ms 단위)
	int		nMaxTick;				// 등록할 타이머의 최대 Tick (반드시 Max값을 넣지 않아도 된다. 다만 이 값을 넘는 비율만큼의 오버헤드가 있다.
	int		nTimerSignalQueueSize;	// 타이머가 한번 발생시키는 시그널의 개수 : Create에서 선언한 최대 시그널의 개수를 넘어서는 안된다.
	int		nAliveTickError;		// Alive 처리를 위한 Timeout Tick값의 오차 허용 범위
	int		nAliveTimeoutCount;		// nAliveTimeoutCount * nAliveTickError = Timeout Tick

	nTickTime				= 1000;
	nMaxTick				= 10;
	nTimerSignalQueueSize	= 500;
	nAliveTickError			= 3;
	nAliveTimeoutCount		= 10;		// 30초 단위로 Ping 체크
	// 나중에 모두 0으로 Set
	
	// Start는 blocking 함수이다.
	// Start는 모든 서버의 메시지를 처리하는 부분을 담고 있다.
	// 만일 return FALSE가 생긴다면, Start 초기화 실패인 경우이다.
	return CSessionGate::Start( nTickTime, nMaxTick, nTimerSignalQueueSize, nAliveTickError, nAliveTimeoutCount, m_nAliveManaged );
}

void CClientGate::Destroy()
{
	// 여기에 소스 코드를 추가한다.
	m_peer.Destroy();
	m_PeerPool.Destroy();

	CSessionGate::Destroy();
}


void CClientGate::OnSiganl( DWORD wParam, DWORD lParam )
{
	// 여기에 소스 코드를 추가한다.
}

void CClientGate::OnResponse( CPeer* pPeer, DWORD dwError, INT nResult, char* pBuffer )
{
	// 여기에 소스 코드를 추가한다.
}

void CClientGate::OnAccept( CPeer* pPeer, INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	m_LastValue.nServicerIndex	= nServicerIndex;
	m_LastValue.pPeer			= pPeer;
	m_LastValue.packetType		= PacketType::AcceptType;
	m_LastValue.pData			= NULL;
	m_LastValue.nDataLength		= 0;
}

void CClientGate::OnConnect( CPeer* pPeer, INT nServicerIndex )
{
	if( !pPeer->IsBeginConnect() )
	{
		return;
	}

	// 여기에 소스 코드를 추가한다.
	m_LastValue.nServicerIndex	= nServicerIndex;
	m_LastValue.pPeer			= pPeer;
	m_LastValue.packetType		= PacketType::ConnectType;
	m_LastValue.pData			= NULL;
	m_LastValue.nDataLength		= 0;

	if( m_LastValue.nServicerIndex == ServicerType::Client )
		m_bConnected			= TRUE;
}

void CClientGate::OnBeginConnect( CPeer* pPeer, INT nServicerIndex )
{
	pPeer->SetBeginConnect( TRUE );

	// 여기에 소스 코드를 추가한다.
	m_LastValue.nServicerIndex	= nServicerIndex;
	m_LastValue.pPeer			= pPeer;
	m_LastValue.packetType		= PacketType::BeginConnectType;
	m_LastValue.pData			= NULL;
	m_LastValue.nDataLength		= 0;
}

void CClientGate::OnClose( CPeer* pPeer, INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	m_LastValue.nServicerIndex	= nServicerIndex;
	m_LastValue.pPeer			= pPeer;
	m_LastValue.packetType		= PacketType::CloseType;
	m_LastValue.pData			= NULL;
	m_LastValue.nDataLength		= 0;

	if( m_LastValue.nServicerIndex == ServicerType::Client )
		m_bConnected			= FALSE;
}

void CClientGate::OnData( CPeer* pPeer, INT nServicerIndex, char* pPacket, INT nPacketLength )
{
	// 여기에 소스 코드를 추가한다.
	m_LastValue.nServicerIndex	= nServicerIndex;
	m_LastValue.pPeer			= pPeer;
	m_LastValue.packetType		= PacketType::ReceiveType;
	m_LastValue.pData			= pPacket;
	m_LastValue.nDataLength		= nPacketLength;

}

void CClientGate::OnTimeout( CPeer* pPeer )
{
	// p2p는 ping 처리가 없다.

	// TimeoutDisconnectRequest( pPeer );
}

CPeer* CClientGate::AllocPeer( INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	CPeer* pPeer;
	switch( nServicerIndex )
	{
	case ServicerType::Peers :
//	case ServicerType::P2PServer :
		pPeer = m_PeerPool.Alloc();
		break;

	default :
		break;
	}

	return pPeer;
}

void CClientGate::FreePeer( CPeer* pPeer, INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	switch( nServicerIndex )
	{
	case ServicerType::Peers :
	case ServicerType::P2PServer :
		m_PeerPool.Free( pPeer );
		break;

	default :
		break;
	}
}

void CClientGate::InitListenerPacketType(
		INT									nServicerIndex,
		CPacketCheckSum::CheckSumType&		out_CheckSumType,
		CPacketAssembler::PacketUnitType&	out_PacketUnitType )
{
	// 여기에 소스 코드를 추가한다.
	/*
	m_pClientGateHandler->InitListenerPacketType(
		out_CheckSumType,
		out_PacketUnitType
		);
	*/
	out_CheckSumType	= CPacketCheckSum::CheckSumType::TwoByteZoroOne;
	out_PacketUnitType	= CPacketAssembler::PacketUnitType::Length;
}

void CClientGate::InitLengthFieldOfPacket(
		INT									nServicerIndex,
		INT&								out_nLengthFieldPos,
		CPacketAssembler::LengthSizeType&	out_lengthSizeType,
		CPacketAssembler::LengthType&		out_lengthType,
		BOOL&								out_bLittleEndian )
{
	// 여기에 소스 코드를 추가한다.
	/*
	m_pClientGateHandler->InitLengthFieldOfPacket(
		out_nLengthFieldPos,
		out_lengthSizeType,
		out_lengthType,
		out_bLittleEndian
		);
	*/

	out_nLengthFieldPos		= CPacket::GetLengthPos();
	out_lengthSizeType		= CPacketAssembler::LengthSizeType::TwoByte;
	out_lengthType			= CPacketAssembler::LengthType::Total;
	out_bLittleEndian		= TRUE;
}

BOOL CClientGate::IsAliveManageObject( INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.

	return FALSE;
}

BOOL CClientGate::Connect( LPCTSTR lpszIP, INT nPort, ServicerType type )
{
	if( type == ServicerType::Client )
	{
		m_bConnected	= FALSE;
		return CSessionGate::Connect( &m_peer, ServicerType::Client, lpszIP, nPort );
	}
	else
	{
		CPeer* pPeer;
		pPeer = m_PeerPool.Alloc();
		return CSessionGate::Connect( pPeer, ServicerType::P2PServer, lpszIP, nPort );
	}
}

void CClientGate::Send( char* pData, INT nDataSize )
{
	SendData( &m_peer, pData, nDataSize );
}

void CClientGate::Close()
{
	CloseRequest( &m_peer );
}

void CClientGate::Send( CPeer* pPeer, char* pData, INT nDataSize )
{
	SendData( pPeer, pData, nDataSize );
}

void CClientGate::Close( CPeer* pPeer )
{
	pPeer->SetBeginConnect( FALSE );
	CloseRequest( pPeer );
}

void CClientGate::Pinging( DWORD dwTick )
{
	CPacketPing pingPacket;

	pingPacket.SetTick( dwTick );

	SendData( &m_peer, (char*)&pingPacket, pingPacket.GetLength() );
}