#include "StdAfx.h"

#include "CasualClient.h"

#include "..\NJean\Application\Packet.h"
using namespace GamehiSolution::Application::Packets;

using namespace GamehiSolution::CasualGame;
using namespace GamehiSolution::Application;
using namespace GamehiSolution::Session::Message;
using namespace GamehiSolution::Session::Manage;
using namespace GamehiSolution::Session::Gate;
using namespace GamehiSolution::Common::DataStructure;


CCasualClient::CCasualClient()
{
}

CCasualClient::~CCasualClient()
{
}

BOOL CCasualClient::Create( /*IClientModuleHandler* pClientModuleHandler,*/ HWND hWindow, UINT uiMessage, INT nMaxPacketSize )
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
	int		nNumOfConnectSockets	= 2;
	int		nPacketSize				= 256;
	int		nResponseBuffer			= 65536;	// 64KB

//	m_pClientModuleHandler			= pClientModuleHandler;

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
	//		1. NJean와 연결되는 네트워크는 1개의 Listen이며 ServicerIndex::Client의 인덱스를 가진다.
	//
	/*
	int		nNumOfClients;

	int		nServicerIndex;			// 연결되는 서비서 인덱스
	int		nNumOfCommSockets;		// 해당 서비서를 통해 연결될 수 있는 클라이언트의 개수
	int		nAcceptPoolSize;		// 해당 서비서의 Accept Pool 크기

	// Listener 생성
	nServicerIndex		= ServicerIndex::Client;
	nNumOfCommSockets	= 1000;
	nAcceptPoolSize		= 100;
	bRet = CreateListener( nServicerIndex, nNumOfCommSockets, nAcceptPoolSize );
	if( !bRet ) return FALSE;

	// 해당 Listener의 Client들의 메모리 풀 생성
	bRet = m_UserPool.Create( nNumOfCommSockets );
	if( !bRet ) return FALSE;

	nNumOfClients		= nNumOfCommSockets;
	m_nAliveManaged		= nNumOfCommSockets;

	// Listener 생성의 끝
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	*/



	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	bRet = CreateEnd();
	if( !bRet ) return FALSE;

	return TRUE;
}

BOOL CCasualClient::Start()
{
	return FALSE;

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

void CCasualClient::Destroy()
{
	// 여기에 소스 코드를 추가한다.

	CSessionGate::Destroy();
}


void CCasualClient::OnSiganl( DWORD wParam, DWORD lParam )
{
	// 여기에 소스 코드를 추가한다.
}

void CCasualClient::OnResponse( CPeer* pPeer, DWORD dwError, INT nResult, char* pBuffer )
{
	// 여기에 소스 코드를 추가한다.
}

void CCasualClient::OnAccept( CPeer* pPeer, INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	m_LastValue.pPeer		= pPeer;
	m_LastValue.packetType	= PacketType::AcceptType;
	m_LastValue.pData		= NULL;
	m_LastValue.nDataLength	= 0;
}

void CCasualClient::OnConnect( CPeer* pPeer, INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	m_LastValue.pPeer		= pPeer;
	m_LastValue.packetType	= PacketType::ConnectType;
	m_LastValue.pData		= NULL;
	m_LastValue.nDataLength	= 0;

	m_bConnected			= TRUE;
}

void CCasualClient::OnClose( CPeer* pPeer, INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	m_LastValue.pPeer		= pPeer;
	m_LastValue.packetType	= PacketType::CloseType;
	m_LastValue.pData		= NULL;
	m_LastValue.nDataLength	= 0;

	m_bConnected			= FALSE;
}

void CCasualClient::OnData( CPeer* pPeer, INT nServicerIndex, char* pPacket, INT nPacketLength )
{
	// 여기에 소스 코드를 추가한다.

	m_LastValue.pPeer		= pPeer;
	m_LastValue.packetType	= PacketType::ReceiveType;
	m_LastValue.pData		= pPacket;
	m_LastValue.nDataLength	= nPacketLength;

}

void CCasualClient::OnTimeout( CPeer* pPeer )
{
	TimeoutDisconnectRequest( pPeer );
}

CPeer* CCasualClient::AllocPeer( INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
	CPeer* pPeer = NULL;

	return pPeer;
}

void CCasualClient::FreePeer( CPeer* pPeer, INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.
}

void CCasualClient::InitListenerPacketType(
		INT									nServicerIndex,
		CPacketCheckSum::CheckSumType&		out_CheckSumType,
		CPacketAssembler::PacketUnitType&	out_PacketUnitType )
{
	out_CheckSumType	= CPacketCheckSum::CheckSumType::TwoByteZoroOne;
	out_PacketUnitType	= CPacketAssembler::PacketUnitType::Length;

	/*
	// 여기에 소스 코드를 추가한다.
	m_pClientModuleHandler->InitListenerPacketType(
		out_CheckSumType,
		out_PacketUnitType
		);
	*/
}

void CCasualClient::InitLengthFieldOfPacket(
		INT									nServicerIndex,
		INT&								out_nLengthFieldPos,
		CPacketAssembler::LengthSizeType&	out_lengthSizeType,
		CPacketAssembler::LengthType&		out_lengthType,
		BOOL&								out_bLittleEndian )
{
	// 여기에 소스 코드를 추가한다.
	out_nLengthFieldPos		= CPacket::GetLengthPos();
	out_lengthSizeType		= CPacketAssembler::LengthSizeType::TwoByte;
	out_lengthType			= CPacketAssembler::LengthType::Total;
	out_bLittleEndian		= TRUE;

	/*
	m_pClientModuleHandler->InitLengthFieldOfPacket(
		out_nLengthFieldPos,
		out_lengthSizeType,
		out_lengthType,
		out_bLittleEndian
		);
	*/
}

BOOL CCasualClient::IsAliveManageObject( INT nServicerIndex )
{
	// 여기에 소스 코드를 추가한다.

	return FALSE;
}

BOOL CCasualClient::Connect( LPCTSTR lpszIP, INT nPort )
{
	m_bConnected	= FALSE;
	return CSessionGate::Connect( &m_peer, CCasualClient::ServicerIndex::GameServer, lpszIP, nPort );
}

void CCasualClient::Send( char* pData, INT nDataSize )
{
	SendData( &m_peer, pData, nDataSize );
}

void CCasualClient::Close()
{
	CloseRequest( &m_peer );
}

void CCasualClient::Pinging()
{
	CPacketPing pingPacket;

	SendData( &m_peer, (char*)&pingPacket, pingPacket.GetLength() );
}