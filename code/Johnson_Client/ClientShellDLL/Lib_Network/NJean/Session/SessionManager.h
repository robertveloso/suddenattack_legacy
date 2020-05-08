/*****************************************************************************************************************
	CSessionManager

	작성자 : 강진협
	작성일 : 2004년 3월 4 ~ 5일

	설  명 :
		IOCP를 이용한 Server를 상속
		OnReceive 이벤트를 메시지 단위가 아닌, 지정한 패킷 단위로 호출
	사  용 :
		- CIocp
		1. Create를 사용하여 Iocp Server를 생성한다.
		2. Bind와 Listen을 한다. 이 값은 기본 소켓 프로그램과 동일하다.
		3. Start를 통해서 Iocp Server를 수행시킨다.
		4. Iocp Server는 소켓 핸들로부터 OnAccept, OnReceive, OnSend, OnClose 이벤트를 발생시킨다.
		5. Send를 통해서 Iocp Server에 연결되어 있는 클라이언트에게 메시지를 전달한다.
		6. CloseSocket을 통해서 Iocp Server에 연결되어 있는 특정 소켓을 닫는다.
		7. 모두 사용한 뒤에는 Destroy를 통해 내부 리소스를 제거를 한다.

		- CSessionManager
		1. CIocp의 3번 Start를 하기전에 패킷에 관련된 초기화를 해준다.
*****************************************************************************************************************/

#pragma once

#include "../Network/TcpAsyncSelect.h"

#include "../../DataStructure/MemoryPoolSafty.h"

#include "PacketAssembler.h"
#include "MessageQueue.h"
#include "Timer.h"
#include "SendPacket.h"

#include "PacketBuffer.h"
#include "QueryBuffer.h"

namespace GamehiSolution{	namespace Session{		namespace Manage{

	class CSessionManager : public ITimerHandler, Network::ITcpNetworkHandler
	{
		// 생성/소멸자
	public:
		CSessionManager();
		~CSessionManager(void);

	public:
		enum	NetworkType
		{
			TCP_IOCP			= 0,
			TCP_ASYNCSELECT		= 1,
		};
		enum	QueryType
		{
			HTTP		= 1,
		};
		enum	AssembleType
		{
			LengthBase	= 0,
			CharBase	= 1,
		};

	private:
		struct	TagNetworkOwner
		{
			AssembleType	assembleType;
			Common::DataStructure::CMemoryPoolSafty<Message::CPacketAssembler>*
				pPacketAssembler;
			unsigned long		pSessionOwner;
		};

		struct	TagPackets
		{
			CPacketPingAck	PingAck;
			CPacketAck		Ack;
		};

	private:
		TagPackets*		m_pPackets;

		Common::DataStructure::CMemoryPoolSafty<Message::CPacketAssembler>
			m_PacketAssemblerPool;		// 패킷 조립용 객체 메모리 풀
		INT							m_nNumOfPacketAssembler;	// Assembler의 최대 개수


		//		CRecvPacketQueue		m_RecvPacketQueue;	// Recv 패킷를 직렬화 시키는 큐
		Message::CMessageQueue		m_RecvMessageQueue;		// Recv 패킷를 직렬화 시키는 큐

		// SendPacketQueue는 따로 만들어야 한다. 
		Message::CMessageQueue		m_SendMessageQueue;		// Send 패킷를 직렬화 시키는 큐

		Message::CMessage			m_SendMessage;			// Dequeue시 Send를 위한 임시 패킷
		Message::CMessage			m_SendManagePacket;		// Manage Pakcet을 위한 임시 Send 패킷
		Message::CMessage			m_HttpMessage;			// Http를 위한 임시 패킷
		int							m_nPacketSize;

		CRITICAL_SECTION			m_csSend;				// Send 동기화를 위한 객체
		CRITICAL_SECTION			m_csHttp;				// Http 동기화를 위한 객체

		Common::DataStructure::CMemoryPoolSafty<CSendPacket>
			m_SendPacketPool;		// Send Packet 풀

		Common::DataStructure::CMemoryPoolSafty<CPacketBuffer>
									m_RecvPacketBufferPool;		// Recv 패킷 버퍼 풀
		Common::DataStructure::CMemoryPoolSafty<CPacketBuffer>
									m_SendPacketBufferPool;		// Send 패킷 버퍼 풀

		CTimer						m_Timer;

		Network::INetwork*			m_pNetwork;

		BOOL						m_bFirstRecvMsg;
		Message::CMessage			m_OldMessage;


		// Operations
	public:
		// 패킷을 전송한다.
		void	SendMessage( Message::CMessage& message );
		VOID	SendMessage( unsigned long pNetworkKey, const CHAR* pData, INT nDataSize );

		// 패킷을 수신한다. // Blocking
		void	RecvMessage( Message::CMessage& outMessage );

		BOOL	IsEmptyRecvMessage()
		{
			return m_RecvMessageQueue.IsEmpty();
		}

		// 타이머를 동작시킨다.
		void	StartTimer( int nTickTime, int nMaxTickCount, int nSize );

		// 타이머를 추가한다.
		void	AddTimer( int nTickCount, const Message::CSignal& signal );

		// 이벤트를 추가한다.
		void	AddSignal( const Message::CSignal& signal );

		// Accept및 Connect의 초기 설정을 마치면 Receive를 수신하기 위해서 이벤트를 호출한다.
		void	SetReceiveEvent( unsigned long lpNetworkKey );

		// 패킷 Assembler를 초기화한다.
		// 연결되는 Session별로 패킷 타입이 다를 경우에는 OnAccept, OnConnect 시에 아래 함수를 호출한다.
		void	InitPacketType( 
			unsigned long							lpCompletionKey,
			Message::CPacketCheckSum::CheckSumType		checkSumType,
			Message::CPacketAssembler::PacketUnitType	packetUnitType );
		void	InitLengthFieldOfPacket(
			unsigned long lpCompletionKey,
			int nLengthFieldPos,
			Message::CPacketAssembler::LengthSizeType lengthSizeType,
			Message::CPacketAssembler::LengthType lengthType = Message::CPacketAssembler::LengthType::Total,
			BOOL bLittleEndian = TRUE );

		// 재정의 : CIocpServer
	public:
		// 내부 리소스를 생성한다.
		BOOL	Create( NetworkType networkType, QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nEventQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize = 256, HWND hWindow = NULL, UINT uiMessage = 0 );
		BOOL	CreateListener( INT nListenIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE );
		BOOL	CreateEnd();

		// 내부 리소스를 제거한다.
		void	Destroy();

		// 해당 키의 Service 인덱스를 가져온다.
		INT		GetServiceIndex( unsigned long lpCompletionKey );
		void	SetOwnerKey( unsigned long lpNetworkKey, unsigned long lpOwnerKey )
		{
			m_pNetwork->SetOwnerKey( lpNetworkKey, lpOwnerKey );
		}
		unsigned long	getOwnerKey( unsigned long lpNetworkKey )
		{
			return m_pNetwork->GetOwnerKey( lpNetworkKey );
		}

		BOOL	Start( INT nServicerIndex )
		{
			return m_pNetwork->StartListen( nServicerIndex );
		}

		BOOL	Connect( unsigned long lpOwnerKey, const sockaddr* addr, INT namelen )
		{
			return m_pNetwork->Connect( lpOwnerKey, addr, namelen );
		}

		BOOL	Bind( INT nServerIndex, INT nPort, u_long hostAddr = INADDR_ANY, short family = AF_INET, INT nListenQueue = 5 )
		{
			return m_pNetwork->Bind( nServerIndex, nPort, hostAddr, family, nListenQueue );
		}

		void	FirePacket( DWORD wParam, DWORD lParam )
		{
			m_pNetwork->FirePacket( wParam, lParam );
		}

		DWORD	GetRttTime( unsigned long lpNetworkKey )
		{
			DWORD dwRTT;
			m_pNetwork->GetRttTime( lpNetworkKey, dwRTT );
			return dwRTT;
		}

		LPCTSTR	GetLocalAddr( unsigned long lpNetworkKey )
		{
			return m_pNetwork->GetLocalAddr( lpNetworkKey );
		}

		LPCTSTR	GetPeerAddr( unsigned long lpNetworkKey )
		{
			return m_pNetwork->GetPeerAddr( lpNetworkKey );
		}

		// Implementaion
	private:
		// ITimerHandler
		virtual void	OnTimer( const Message::CSignal& signal );

		// IAsyncInetHandler
		virtual void	OnRecvInet		( unsigned long lpInetKey, unsigned long lpContext );
		virtual void	OnErrorInet		( unsigned long lpContext, DWORD nError );

		// ITcpNetworkHandler
		virtual void	OnAccept	( unsigned long lpNetworkKey );
		virtual void	OnConnect	( unsigned long lpNetworkKey );
		virtual void	OnBeginConnect( unsigned long lpNetworkKey );
		virtual void	OnReceive	( unsigned long lpNetworkKey, char* pRecvBuff, int nReceivedLenth );
		virtual void	OnSend		( unsigned long lpNetworkKey, char* pRecvBuff, int nSenedLenth );
		virtual void	OnClose		( unsigned long lpNetworkKey );

		// Network 객체를 할당할 때, 해당 객체와 쌍을 이루는 Packet Assembler를 할당
		virtual void	OnAllocNetworkObject	( unsigned long lpCompletionKey );
		// Network 객체가 반환될 때, 해당 객체와 쌍을 이루는 Packet Assembler를 반환
		virtual void	OnFreeNetworkObject		( unsigned long lpCompletionKey );


		void			SendPacket	( Session::Message::CMessage& message, BOOL bFirst = FALSE );
		void			SendPacket	( unsigned long lpNetworkKey, CPacketBuffer* pPacketBuffer, BOOL bFirst = FALSE );

	private:
		static	DWORD __stdcall SendThreadStartingPoint(PVOID pvParam);
	};// class CSessionManager

}/* namespace Manage */		}/* namespace Session */	}/* namespace GamehiSolution */