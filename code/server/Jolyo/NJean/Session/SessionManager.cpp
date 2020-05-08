#include "StdAfx.h"
#include ".\SessionManager.h"
#include "..\..\DataStructure\List.h"

using namespace Common::DataStructure;

using namespace Network;
using namespace Session::Manage;
using namespace Session::Message;
using namespace Common::DataStructure;
#ifndef CLIENT_MODULE
	using namespace Query::HTTP;
	using namespace Network::TcpIOCP;
#else
	using namespace Network::TcpAsyncSelect;
#endif

#include "..\Application\Packet.h"

#include "../../../Dispatcher/IConsole.h"

CSessionManager::CSessionManager()
{
	m_bFirstRecvMsg = TRUE;
}

CSessionManager::~CSessionManager(void)
{
}

// 郴何 府家胶甫 积己茄促.
BOOL CSessionManager::Create( NetworkType networkType, QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nEventQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize, HWND hWindow, UINT uiMessage )
{
	m_nPacketSize = nPacketSize;
	m_bFirstRecvMsg = TRUE;

	switch( networkType )
	{
#ifndef CLIENT_MODULE
	case NetworkType::TCP_IOCP :
		m_pNetwork = new CTcpIocp();
		break;
#else
	case NetworkType::TCP_ASYNCSELECT :
		m_pNetwork = new CTcpAsyncSelect( hWindow, uiMessage );
		break;
#endif

	default :
		return FALSE;
	}

	switch( queryType )
	{
	case QueryType::HTTP :
		break;

	case QueryType::ADO :
		break;

	default :
		return FALSE;
	}

	// 酒贰 按眉档 Network 按眉贸烦 眠惑拳 贸府甫 秦具 茄促.
	m_HttpMessage.Create( nPacketSize );
#ifndef CLIENT_MODULE
	m_InetHttp.Create( nQueryQueueSize, this );
	m_SyncInet1.Create();
	m_SyncInet2.Create();
#endif

	if( !m_SendMessage.Create( nPacketSize ) )			return FALSE;
	if( !m_SendManagePacket.Create( nPacketSize ) )		return FALSE;

	if( !m_RecvMessageQueue.Create( nRecvQueueSize, nPacketSize, nEventQueueSize ) )
	{
		m_SendMessage.Destroy();
		return FALSE;
	}

//	nSendQueueSize = 15;
	if( !m_SendMessageQueue.Create( nSendQueueSize, nPacketSize ) )
	{
		m_SendMessage.Destroy();
		m_RecvMessageQueue.Destroy();
		return FALSE;
	}

	if( !m_QueryMessageQueue.Create( nNumOfConnectSockets, nPacketSize ) )		return FALSE;
	if( !m_RecvPacketBufferPool.Create( nRecvQueueSize ) )						return FALSE;
	if( !m_SendPacketBufferPool.Create( nSendQueueSize ) )						return FALSE;


	/////////////////////////
	// 瘤况具 且 康开
	if( !m_SendPacketPool.Create( nSendQueueSize * 2 ) )
	{
		return FALSE;
	}
	CSendPacket** paSendPacket = new CSendPacket*[nSendQueueSize * 2];

	for( int i = 0; i < nSendQueueSize * 2; i++ )
	{
		paSendPacket[i] = m_SendPacketPool.Alloc();
		paSendPacket[i]->Create( nPacketSize*2 );
	}
	for( int j = 0; j < nSendQueueSize * 2; j++ )
	{
		m_SendPacketPool.Free( paSendPacket[j] );
	}
	delete[] paSendPacket;

	m_lpRecentNetworkKey	= NULL;
	m_PacketBufferListPool.Create( nSendQueueSize * 2 );

	//
	///////////////////////////////

	if( !m_pNetwork->Create( this, nNumOfListener, nNumOfConnectSockets, nPacketSize, nPacketSize ) )
	{
		return FALSE;
	}

	m_nNumOfPacketAssembler = nNumOfConnectSockets;

	m_pPackets	= new TagPackets;


	// None DataType 汲沥
	m_OldMessage.SetType( CMessage::MessageType::AcceptType );

	::InitializeCriticalSection( &m_csSend );
	::InitializeCriticalSection( &m_csHttp );


//	m_nDebug=0;

	//
	// Send Thread 矫累
	//
#ifndef CLIENT_MODULE
	DWORD dwThreadId = 0;
	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );
	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );
//	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );
//	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );

	//::CloseHandle( ::CreateThread( NULL, 0, QueryThreadStartingPoint1, this, 0, &dwThreadId ) );
	//::CloseHandle( ::CreateThread( NULL, 0, QueryThreadStartingPoint2, this, 0, &dwThreadId ) );
#endif
	return TRUE;
}

BOOL CSessionManager::CreateListener( INT nListenIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent/* = FALSE*/ )
{
	if( !m_pNetwork->CreateListener( nListenIndex, nNumOfCommSockets, nAcceptPoolSize, bAcceptRecvEvent ) )
		return FALSE;

	m_nNumOfPacketAssembler += nNumOfCommSockets;
	return TRUE;
}

BOOL CSessionManager::CreateEnd()
{
	if( !m_PacketAssemblerPool.Create( m_nNumOfPacketAssembler ) )
		return FALSE;

//	m_nDebugAssem = 0;
	
	CPacketAssembler** pPacketAssembler = new CPacketAssembler*[m_nNumOfPacketAssembler];
	for( INT i = 0; i < m_nNumOfPacketAssembler; i++ )
	{
		pPacketAssembler[i] = m_PacketAssemblerPool.Alloc();
		if( !pPacketAssembler[i]->Create( m_nPacketSize * 2, m_nNumOfPacketAssembler ) )
		{
			return FALSE;
		}
	}
	for( INT j = 0; j < m_nNumOfPacketAssembler; j++ )
	{
		m_PacketAssemblerPool.Free( pPacketAssembler[j] );
	}
	delete[] pPacketAssembler;

	return TRUE;
}

// 郴何 府家胶甫 力芭茄促.
void CSessionManager::Destroy()
{
	m_pNetwork->Destroy();

	m_SendMessageQueue.Destroy();
	m_RecvMessageQueue.Destroy();
	m_QueryMessageQueue.Destroy();
	m_SendPacketPool.Destroy();

#ifndef CLIENT_MODULE
	m_InetHttp.Destroy();
	m_SyncInet1.Destroy();
	m_SyncInet2.Destroy();
#endif
	m_HttpMessage.Destroy();

	::DeleteCriticalSection( &m_csSend );
	::DeleteCriticalSection( &m_csHttp );
}

INT CSessionManager::GetServiceIndex( ULONG_PTR lpCompletionKey )
{
	return m_pNetwork->GetServicerIndex( lpCompletionKey );
}



// 初始化消息包的定义类型.
void CSessionManager::InitPacketType(
		ULONG_PTR							lpCompletionKey,
		CPacketCheckSum::CheckSumType		checkSumType,
		CPacketAssembler::PacketUnitType	packetUnitType )
{
	//
	// lpCompletionKey俊 秦寸窍绰 PacketAssembler甫 檬扁拳茄促.
	//
	
	ULONG_PTR			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	DWORD				dwAssemblerHandle;
	m_pNetwork->GetAssemblerKey( lpCompletionKey, pAssemblerKey, dwAssemblerHandle );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;
	pPacketAssembler->Init( checkSumType, packetUnitType );
}

void CSessionManager::InitLengthFieldOfPacket(
	ULONG_PTR lpCompletionKey,
	int nLengthFieldPos,
	CPacketAssembler::LengthSizeType lengthSizeType,
	CPacketAssembler::LengthType lengthType/* = Network::CPacketAssembler::LengthType::Total*/,
	BOOL bLittleEndian/* = TRUE*/ )
{
	//
	// lpCompletionKey俊 秦寸窍绰 PacketAssembler甫 檬扁拳茄促.
	//
	ULONG_PTR			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	DWORD				dwAssemblerHandle;
	m_pNetwork->GetAssemblerKey( lpCompletionKey, pAssemblerKey, dwAssemblerHandle );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;
	pPacketAssembler->InitLengthField( nLengthFieldPos, lengthSizeType, lengthType, bLittleEndian );
}

//发送当前缓冲中的消息到消息链中
VOID CSessionManager::SendMessageFlush()
{
	if( m_lpRecentNetworkKey == NULL )
		return;

	CMessage	message( CMessage::MessageType::DataType, m_lpRecentNetworkKey, m_pPacketBufferList );
	m_SendMessageQueue.EnqueuePacket( message );
	m_lpRecentNetworkKey	= NULL;
}
//将pData发送到当前缓冲或消息链中
VOID CSessionManager::SendMessage( ULONG_PTR pNetworkKey, const CHAR* pData, INT nDataSize, DWORD dwHandle )
{
	DWORD				dwAssemblerHandle;
	ULONG_PTR			pAssemblerKey;
	CPacketBuffer*		pSendPacketBuffer;
	CPacketAssembler*	pAssembler;

	m_pNetwork->GetAssemblerKey( pNetworkKey, pAssemblerKey, dwAssemblerHandle );

	// 粮犁窍瘤 臼绰 Peer俊 Send甫 且 版快 惯积茄促. 绢敲府纳捞记 饭骇俊辑狼 俊矾
	if( pAssemblerKey == NULL )
		return;
	ASSERT( pAssemblerKey != NULL );


	pAssembler = reinterpret_cast<CPacketAssembler*>( pAssemblerKey );

	pSendPacketBuffer	= m_SendPacketBufferPool.Alloc();
	pSendPacketBuffer->CopyData( pData, nDataSize, dwHandle, pAssembler->AllocSendIndex() );
	//以上根据参数pData,组成一个消息包

	if( nDataSize == 0 )
		int a = 3;

	if( m_lpRecentNetworkKey == NULL )
	{
		//当前没有发送的对象时,从内存池中取出一块内存
		m_lpRecentNetworkKey	= pNetworkKey;
		m_pPacketBufferList		= m_PacketBufferListPool.Alloc();
	}

	if( m_lpRecentNetworkKey == pNetworkKey )
	{
		m_pPacketBufferList->Add( pSendPacketBuffer );	//若该需要发送的对象与当前发送对象一致,向其中添加
	}
	else
	{
		//否则,将消息组包组合到m_SendMessageQueue中
		CMessage	message( CMessage::MessageType::DataType, m_lpRecentNetworkKey, m_pPacketBufferList );
		m_SendMessageQueue.EnqueuePacket( message );
		m_pPacketBufferList		= m_PacketBufferListPool.Alloc();
		m_pPacketBufferList->Add( pSendPacketBuffer );
		m_lpRecentNetworkKey	= pNetworkKey;
	}
	

	/*

#ifndef CLIENT_MODULE
		// 钮俊 持扁 傈俊 勤甸 蔼阑 付欧茄促.
		CMessage	message( CMessage::MessageType::DataType, pNetworkKey, pSendPacketBuffer );
		m_SendMessageQueue.EnqueuePacket( message );

#else
		// 钮俊 持扁 傈俊 勤甸 蔼阑 付欧茄促.
		CMessage	message( CMessage::MessageType::DataType, pNetworkKey, pSendPacketBuffer );
		SendPacket( message );

#endif
	*/
}

// 根据具体的消息将msg发送到发送消息链中或Query消息链中
void CSessionManager::SendMessage( CMessage& message )
{
	// 弥檬狼 菩哦捞搁 梆官肺 Send甫 窍绊, 弊 促澜 菩哦 何磐绰 OnSend俊辑 贸府甫 茄促.
	int nError = 0;

	DWORD				dwAssemblerHandle;
	ULONG_PTR			pAssemblerKey;
	CPacketAssembler*	pAssembler;

	Common::DataStructure::CListEx<CPacketBuffer*>*
						pPacketBufferList;
	CPacketBuffer*		pSendPacketBuffer;

	switch( message.GetType() )
	{
	case CMessage::MessageType::HttpRequest :
		m_QueryMessageQueue.EnqueuePacket( message );
		break;

	case CMessage::MessageType::DbReqeust :
		m_DbReqeustMessageQueue.EnqueuePacket( message );
		break;

	case CMessage::MessageType::CloseType :
		{
			m_pNetwork->GetAssemblerKey( message.GetNetworkKey(), pAssemblerKey, dwAssemblerHandle );
			pAssembler = reinterpret_cast<CPacketAssembler*>( pAssemblerKey );
			pPacketBufferList		= m_PacketBufferListPool.Alloc();//为什么不能从message.GetPacketBufferList()
			pSendPacketBuffer		= m_SendPacketBufferPool.Alloc();
			// Close烙阑 汲沥
			pSendPacketBuffer->CopyData( NULL, 0, 0, pAssembler->AllocSendIndex() );
			pPacketBufferList->Add( pSendPacketBuffer );
//			message.SetSendingIndex( pAssembler->AllocSendIndex() );
			CMessage	closeMessage( CMessage::MessageType::DataType, message.GetNetworkKey(), pPacketBufferList );
			m_SendMessageQueue.EnqueuePacket( closeMessage );
		}
		break;

	default:
		m_SendMessageQueue.EnqueuePacket( message );
		break;

	}
}

//直接发送消息包，recv消息收到Ping消息时直接回复，send线程从m_SendMessageQueue中得到一个包后发送
void CSessionManager::SendPacket( CMessage& message )
{
	//
	// step 1 : Assembler甫 啊廉柯促.
	//

	ULONG_PTR			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	DWORD				dwAssemblerHandle;
	m_pNetwork->GetAssemblerKey( message.GetNetworkKey(), pAssemblerKey, dwAssemblerHandle );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey; //该message对应的消息块

	
	if( pPacketAssembler == NULL )//没有建立过该soket的消息链，一般是错误的不明消息
	{
		Common::DataStructure::CListEx<CPacketBuffer*>*
			pPacketBufferList	= message.GetPacketBufferList();

		while( !pPacketBufferList->IsEmpty() )
		{
			m_SendPacketBufferPool.Free( pPacketBufferList->Dequeue() );//从sendpacketbuffer中将该socket对应的所有消息占用的内存释放
		}
		m_PacketBufferListPool.Free( pPacketBufferList );//在packetbufferlist中也释放
		return;
	}

	pPacketAssembler->Lock();


	// Send且 List甫 啊廉柯促.
	int		nError;
	BOOL	bRet;
	CListEx<CPacketBuffer*>* plstPacketBuffer = pPacketAssembler->GetPacketBufferList();

	Common::DataStructure::CListEx<CPacketBuffer*>*
		pPacketBufferList	= message.GetPacketBufferList();
	while( !pPacketBufferList->IsEmpty() )
	{
		CPacketBuffer*	pPacketBuffer	= pPacketBufferList->Dequeue();
		plstPacketBuffer->SortAdd( pPacketBuffer, pPacketBuffer->GetSendIndex() );//将该message中的消息包顺序加入包buffer中
	}
	m_PacketBufferListPool.Free( pPacketBufferList ); //若在packet buff中有message带来的消息包，需要将他们释放

	// List狼 霉锅掳 菩哦阑 傈价茄促.
	CPacketBuffer* pPacketBuffer = plstPacketBuffer->GetRearElement(); //先用最后一个消息包进行判断

	/*/
	if( plstPacketBuffer->GetElementsCount() > 100 )
	{
		::printf( "⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕\n" );
		::printf( "Send Over %d\n", plstPacketBuffer->GetElementsCount() );
		::printf( "⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕⒕\n" );
//		char a[100];
//		scanf( "%s", a );
//		int b;
//		b = 3;
	}
	/*/

	if( pPacketBuffer == NULL )
	{
		pPacketAssembler->Unlock();
		return;
	}

	if( !pPacketAssembler->IsSending( pPacketBuffer->GetSendIndex() ) ||
		pPacketAssembler->IsOverlapped() )
	{
		//当前消息包ID比已发送包id小或正在发送中，退出
		pPacketAssembler->Unlock();
		return;
	}
	if( pPacketBuffer->GetSendIndex() > 0 )
		pPacketAssembler->AddSendIndex();

	// CloseType
	if( pPacketBuffer->GetDataSize() == 0 )
	{
		//若没有消息内容，释放对应内存
		m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
		pPacketAssembler->Unlock();

		OnFreeNetworkObject( message.GetNetworkKey() );
		m_pNetwork->FreeNetworkKey( message.GetNetworkKey() );
		return;
	}

	if( pPacketBuffer->GetHandle() != dwAssemblerHandle ) //socket 一样，不见得是同一个角色。一般是错误的消息包
	{
		while( !plstPacketBuffer->IsEmpty() )//释放内存
		{
			pPacketBuffer	= plstPacketBuffer->GetRearElement();
			if( !pPacketAssembler->IsSending( pPacketBuffer->GetSendIndex() ) )
			{
				break;
			}

			if( pPacketBuffer->GetSendIndex() > 0 )
			{
				pPacketAssembler->AddSendIndex();
			}

			if( pPacketBuffer->GetDataSize() == 0 )//close过来的
			{
				m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
				pPacketAssembler->Unlock();

				OnFreeNetworkObject( message.GetNetworkKey() );
				m_pNetwork->FreeNetworkKey( message.GetNetworkKey() );
				return;
			}
			m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
		}
		pPacketAssembler->Unlock();
		return;
	}

	// 正确的消息包
	CPacket* pPacket = reinterpret_cast<CPacket*>( pPacketBuffer->GetData() );
	if( pPacket->GetCommand() == CPacket::PredefineCommand::Ping ||
		pPacket->GetCommand() == CPacket::PredefineCommand::PingAck )
	{
		/*/
		if( !m_pNetwork->CanStartRTT( message.GetNetworkKey() ) )
		{
			printf( "++++++++++++++++++++++++++++++++++++++++++++++++\n" );
			printf( "Do not Recv Ack Ping!!\n" );
			printf( "++++++++++++++++++++++++++++++++++++++++++++++++\n" );
//			pPacketAssembler->Unlock();
//			return;
		}
		/*/

		m_pNetwork->SetRttStartTime( message.GetNetworkKey() );//ping消息设置对应socket的时间戳，在mygate中用于确定host
	}

	//发送消息包
	INT nChangeLength;
	pPacketAssembler->SetCheckSum( pPacketBuffer->GetData(), pPacketBuffer->GetDataSize(), nChangeLength );
	pPacketBuffer->SetDataSize( nChangeLength );

	pPacketAssembler->SetOverlapped( TRUE );
	bRet = m_pNetwork->Send(
		(ULONG_PTR)message.GetNetworkKey(),
		pPacketBuffer->GetData(),
		pPacketBuffer->GetDataSize(),
		nError );

	if( !bRet )
	{
		m_pNetwork->CloseSocket( (ULONG_PTR)message.GetNetworkKey() );
		pPacketAssembler->SetOverlapped( FALSE );
	}
	pPacketAssembler->Unlock();
}

void CSessionManager::SendPacket( ULONG_PTR lpNetworkKey, CPacketBuffer* pPacketBuffer )
{
	ASSERT( FALSE );
}


// 鸥捞赣甫 悼累矫挪促.
void CSessionManager::StartTimer( int nTickTime, int nMaxTickCount, int nSize )
{
	m_Timer.Create( nTickTime, nMaxTickCount, nSize, (ITimerHandler*)this );
}

// 鸥捞赣甫 眠啊茄促.
ULONG_PTR CSessionManager::AddTimer( int nTickCount, const CSignal& signal )
{
	return m_Timer.SetTimer( nTickCount, signal );
}

// ITimerHandler
void CSessionManager::OnTimer( const CSignal& signal )
{
	m_RecvMessageQueue.AddSignalPacket( signal );
}

// 捞亥飘甫 眠啊茄促.
void CSessionManager::AddSignal( const CSignal& signal )
{
	m_RecvMessageQueue.AddSignalPacket( signal );
}

void CSessionManager::SetReceiveEvent( ULONG_PTR lpNetworkKey )
{
	m_pNetwork->SetConnectRecvEvent( lpNetworkKey );
}


//accept响应，初始化m_RecvMessageQueue
void CSessionManager::OnAccept( ULONG_PTR lpNetworkKey )
{
	// Receive / AcceptConnect 悼扁拳俊 措茄 檬扁拳甫 茄促.
	//char strLog[1024];
	//sprintf( strLog, "Accept %d\r\n", (int)lpNetworkKey );
	//OutputDebugString( strLog );


	m_pNetwork->ResetConnectRecvEvent( lpNetworkKey );

	m_pNetwork->ResetRttTime( lpNetworkKey );

	ULONG_PTR			pAssemblerKey;
//	CPacketAssembler*	pPacketAssembler;
	DWORD				dwAssemblerHandle;
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );

	//char a[100];
	//sprintf( a, "accept %d\r\n", (int) lpNetworkKey );
	//::OutputDebugString( a );

	m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, CMessage::MessageType::AcceptType, dwAssemblerHandle ) );
}

void CSessionManager::OnConnect( ULONG_PTR lpNetworkKey )
{
	// Receive / AcceptConnect 悼扁拳俊 措茄 檬扁拳甫 茄促.
	m_pNetwork->ResetConnectRecvEvent( lpNetworkKey );

	m_pNetwork->ResetRttTime( lpNetworkKey );

	m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, CMessage::MessageType::ConnectType ) );
}

void CSessionManager::OnReceive( ULONG_PTR lpNetworkKey, char* pRecvBuff, int nReceivedLength )
{
	// 霉锅掳 Receive捞搁, Connect/Accept俊 措茄 捞亥飘甫 扁促赴促.
	m_pNetwork->ResetRecvDisconnectEvent( lpNetworkKey );
	m_pNetwork->WaitConnectRecvEvent( lpNetworkKey );

	ULONG_PTR			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	DWORD				dwAssemblerHandle;
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );	//获取当前NetworkKey中的原有消息
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;

	if( !pPacketAssembler->Assemble( pRecvBuff, nReceivedLength ) ) //将接收的消息加入Queue中
	{
		m_pNetwork->SetRecvDisconnectEvent( lpNetworkKey );
		return;
	}

	char* pPacket;
	int nPacketSize = 0;
	int nResult = 0;
//	try
	{
		while( pPacket = (char*)pPacketAssembler->GetCompletePacket( nPacketSize, nResult ) )
		{
			//处理每一个包
			CPacket* pDataPacket = (CPacket*)pPacket;

			CPacketBuffer*	pRecvPacketBuffer;
//			::OutputDebugString( "Recv\r\n" );
			pRecvPacketBuffer	= m_RecvPacketBufferPool.Alloc();
			pRecvPacketBuffer->CopyData( pPacket, nPacketSize );

			switch( pDataPacket->GetCommand() )
			{

			//
			// Step 1 : 快急 贸府瞪 菩哦篮 Message Q俊 Enqueue窍瘤 臼绊 梆官肺 贸府茄促.
			//
				// Client -> Server俊 焊郴绰 菩哦
			case CPacket::PredefineCommand::Ping :
				{
					CPacketPing*	pPingPacket;
					pPingPacket		= (CPacketPing*)pDataPacket;
					if( pPingPacket->GetRealLength() != nPacketSize )//消息包长度验证
					{
//#pragma message( "===============================\r\n" )
//#pragma message( "厚沥惑 辆丰甫 困茄 烙矫肺弊\r\n" )
//#pragma message( "===============================\r\n" )
//						::Dispatcher::ICONSOLE()->WriteUnsafe( "DoClose\tLowLevel\tPing\r\n" );
						SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::DoShutdownType ) );//发送立即关闭命令
						m_RecvPacketBufferPool.Free( pRecvPacketBuffer );
						break;
					}

					// 先将本块内存标记为释放
//					m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, pPacket, nPacketSize ) );
					m_RecvMessageQueue.EnqueuePacket(
						CMessage( CMessage::MessageType::DataType, lpNetworkKey, pRecvPacketBuffer ) );


					// 咯扁辑何磐 且 瞒肥 : ackPacket阑 葛电 菩哦 努贰胶狼 胶飘钒媚 父甸绊.. TagPackets.. 贸府且巴..
					// 酒贰狼 SendPacket篮 Send Q 甫 烹窍瘤 臼绊 快急利栏肺 焊郴扁 困茄 贸府

//					break;
					ULONG_PTR	pAssemblerKey;
					DWORD		dwAssemblerHandle;
					m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );

					CPacketBuffer* pSendPacketBuffer = m_SendPacketBufferPool.Alloc();
					pSendPacketBuffer->CopyData(
						reinterpret_cast<CHAR*>( &m_pPackets->PingAck ),
						m_pPackets->PingAck.GetLength(),
						dwAssemblerHandle,
						0
						);

					Common::DataStructure::CListEx<CPacketBuffer*>* pPacketBufferList;
					pPacketBufferList = m_PacketBufferListPool.Alloc();
					pPacketBufferList->Add( pSendPacketBuffer );
					CMessage	message( CMessage::MessageType::DataType, lpNetworkKey, pPacketBufferList );
					SendPacket( message );	//发送消息包

//					CMessage	message( CMessage::MessageType::DataType, lpNetworkKey, pSendPacketBuffer );
//					SendPacket( message );
//					SendPacket( lpNetworkKey, pSendPacketBuffer, TRUE );

					/*
					m_SendManagePacket.SetSendPacket(
						lpNetworkKey,
						reinterpret_cast<VOID*>( &m_pPackets->PingAck ), m_pPackets->PingAck.GetLength() );

					DWORD		dwAssemblerHandle;
					ULONG_PTR	pAssemblerKey;
					m_pNetwork->GetAssemblerKey( m_SendManagePacket.GetNetworkKey(), pAssemblerKey, dwAssemblerHandle );
					m_SendManagePacket.SetSendHandle( dwAssemblerHandle );

					SendPacket( m_SendManagePacket, TRUE );
					*/

//					m_RecvPacketBufferPool.Free( pRecvPacketBuffer );
				}
				break;

				// Server -> Client俊 焊郴绰 菩哦
//			case CPacket::PredefineCommand::PingAck :
//				{
//					CPacketPingAck*	pPingAckPacket;
//					pPingAckPacket	= (CPacketPingAck*)pDataPacket;
//					if( pPingAckPacket->GetRealLength() != nPacketSize )
//					{
////#pragma message( "===============================\r\n" )
////#pragma message( "厚沥惑 辆丰甫 困茄 烙矫肺弊\r\n" )
////#pragma message( "===============================\r\n" )
////						::Dispatcher::ICONSOLE()->WriteUnsafe( "DoClose\tLowLevel\tPingAck\r\n" );
//						SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::DoShutdownType ) );
//						break;
//					}
//
//					// Rtt 鸥烙阑 眉农茄促.
//					m_pNetwork->SetRttEndTime( lpNetworkKey );
//
//					ULONG_PTR	pAssemblerKey;
//					DWORD		dwAssemblerHandle;
//					m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );
//
//					CPacketBuffer* pSendPacketBuffer = m_SendPacketBufferPool.Alloc();
//					pSendPacketBuffer->CopyData(
//						reinterpret_cast<CHAR*>( &m_pPackets->Ack ),
//						m_pPackets->Ack.GetLength(),
//						dwAssemblerHandle,
//						0
//						);
//					Common::DataStructure::CListEx<CPacketBuffer*>* pPacketBufferList;
//					pPacketBufferList = m_PacketBufferListPool.Alloc();
//					pPacketBufferList->Add( pSendPacketBuffer );
//					CMessage	message( CMessage::MessageType::DataType, lpNetworkKey, pPacketBufferList );
//					SendPacket( message );
////					CMessage	message( CMessage::MessageType::DataType, lpNetworkKey, pSendPacketBuffer );
////					SendPacket( message );
//
//					/*
//					CPacketAck	ackPacket;
//					m_SendManagePacket.SetSendPacket( lpNetworkKey, (void*)&ackPacket, ackPacket.GetLength() );
//
//					DWORD		dwAssemblerHandle;
//					ULONG_PTR	pAssemblerKey;
//					m_pNetwork->GetAssemblerKey( m_SendManagePacket.GetNetworkKey(), pAssemblerKey, dwAssemblerHandle );
//					m_SendManagePacket.SetSendHandle( dwAssemblerHandle );
//
//					SendPacket( m_SendManagePacket, TRUE );
//					*/
//				}
//				break;
//
				// Client -> Server俊 焊郴绰 菩哦
			case CPacket::PredefineCommand::Ack :
				{
					CPacketAck*		pAckPacket;
					pAckPacket		= (CPacketAck*)pDataPacket;
					if( pAckPacket->GetRealLength() != nPacketSize )
					{
//#pragma message( "===============================\r\n" )
//#pragma message( "厚沥惑 辆丰甫 困茄 烙矫肺弊\r\n" )
//#pragma message( "===============================\r\n" )
//						::Dispatcher::ICONSOLE()->WriteUnsafe( "DoClose\tLowLevel\tAck\r\n" );
						SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::DoShutdownType ) );
						m_RecvPacketBufferPool.Free( pRecvPacketBuffer );
						break;
					}

					// Rtt 鸥烙阑 眉农茄促.
					m_pNetwork->SetRttEndTime( lpNetworkKey ); //得到PING的延时

					DWORD dwRttTime;
					m_pNetwork->GetRttTime( lpNetworkKey, dwRttTime );

					m_RecvPacketBufferPool.Free( pRecvPacketBuffer );
				}
				break;

			//
			// Step 2 : 菩哦阑 Message Q俊 Enqueue茄促.
			//
			default :
//				m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, pPacket, nPacketSize ) );
				m_RecvMessageQueue.EnqueuePacket(
					CMessage( CMessage::MessageType::DataType, lpNetworkKey, pRecvPacketBuffer ) );//消息进队列
				break;
			}

		}
	}
//	catch( int nErrorCode )
	if( nResult > 0 ) //1：缓冲不够；2：缓冲空间正好但消息类型不对；3：缓冲空间多但消息类型不对
	{
//		int a = nErrorCode;
		m_pNetwork->CloseSocket( lpNetworkKey );
		m_pNetwork->SetRecvDisconnectEvent( lpNetworkKey );
		
		//FILE		*fp = NULL;
		//SYSTEMTIME	st;
		//GetLocalTime( &st );
		//fp = fopen( "ERROR_LOG.txt", "a" );
		//if( fp==NULL ) return;
		//fprintf( fp,
		//		 "[%d/%d/%d %d:%d:%d] Packet Error    ERROR CODE: %d From %s\n",
		//		 st.wYear,
		//		 st.wMonth,
		//		 st.wDay,
		//		 st.wHour, 
		//		 st.wMinute,
		//		 st.wSecond,
		//		 nErrorCode,
		//		 m_pNetwork->GetPeerAddr( lpNetworkKey ) );
		//fclose( fp );

		//printf( "+++++++++++++++++++++++++++++++++++++++++++++++++\n" );
		//printf( "Packet Error    ERROR CODE: %d From %s\n", nErrorCode, m_pNetwork->GetPeerAddr( lpNetworkKey ) );
		//printf( "+++++++++++++++++++++++++++++++++++++++++++++++++\n" );

		return;
	}
	m_pNetwork->SetRecvDisconnectEvent( lpNetworkKey );
}

void CSessionManager::OnSend( ULONG_PTR lpNetworkKey, char* pRecvBuff, int nSendedLength )
{
	ULONG_PTR				pAssemblerKey;
	CPacketAssembler*		pPacketAssembler;
	CListEx<CPacketBuffer*>*	plstPacketBuffer;		// SendPacket List
	CPacketBuffer*			pSendPacketBuffer;		// Send茄 菩哦 滚欺
	DWORD				dwAssemblerHandle;

	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );
	pPacketAssembler	= reinterpret_cast<CPacketAssembler*>( pAssemblerKey );
	plstPacketBuffer	= pPacketAssembler->GetPacketBufferList();

	pPacketAssembler->Lock();

	if( !pPacketAssembler->IsOverlapped() )
	{
		pPacketAssembler->Unlock();
		return;
	}
	
	pPacketAssembler->SetOverlapped( FALSE );
	

	int		nError;
	BOOL	bRet;

	/*
	if( pPacketAssembler->IsFree() )
	{
		OnFreeNetworkObject( lpNetworkKey );
		m_pNetwork->FreeNetworkKey( lpNetworkKey );
		pPacketAssembler->Unlock();
		return;
	}
	*/

	// Send茄 单捞鸥樊苞 角力 傈价樊阑 厚背
	pSendPacketBuffer	= plstPacketBuffer->GetRearElement();

	if( pSendPacketBuffer == NULL )
	{
		//？？？
		CPacket* pPacket = reinterpret_cast<CPacket*>( pRecvBuff );
		int a = pPacket->GetCommand();
		char a1[100];
		sprintf( a1, "Assert!! %d\r\n", a );
		::OutputDebugString( a1 );
		pPacketAssembler->Unlock();
		return;
	}


	/*
	if( pSendPacketBuffer->GetHandle() != dwAssemblerHandle )
	{
		while( !plstPacketBuffer->IsEmpty() )
		{
			if( !pPacketAssembler->IsSending( pSendPacketBuffer->GetSendIndex() ) )
			{
				break;
			}

			if( pSendPacketBuffer->GetSendIndex() > 0 )
				pPacketAssembler->AddSendIndex();

			pSendPacketBuffer	= plstPacketBuffer->Dequeue();
			if( pSendPacketBuffer->GetDataSize() == 0 )
			{
				OnFreeNetworkObject( lpNetworkKey );
				m_pNetwork->FreeNetworkKey( lpNetworkKey );
				pPacketAssembler->Unlock();

				m_SendPacketBufferPool.Free( pSendPacketBuffer );
				return;
			}
			m_SendPacketBufferPool.Free( pSendPacketBuffer );
		}
		pPacketAssembler->Unlock();
		return;
	}
	*/


	// 促福搁 缔何盒阑 犁傈价
	if( pSendPacketBuffer->GetDataSize() > nSendedLength )
	{
		if( pSendPacketBuffer->GetHandle() != dwAssemblerHandle )
		{
			m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );

			while( !plstPacketBuffer->IsEmpty() )
			{
				pSendPacketBuffer	= plstPacketBuffer->GetRearElement();
				if( !pPacketAssembler->IsSending( pSendPacketBuffer->GetSendIndex() ) )
				{
					break;
				}

				if( pSendPacketBuffer->GetSendIndex() > 0 )
				{
					pPacketAssembler->AddSendIndex();
				}

				if( pSendPacketBuffer->GetDataSize() == 0 )
				{
					m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
					pPacketAssembler->Unlock();

					OnFreeNetworkObject( lpNetworkKey );
					m_pNetwork->FreeNetworkKey( lpNetworkKey );
					return;
				}
				m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
			}
			pPacketAssembler->Unlock();
			return;
		}

		pSendPacketBuffer->ReplaceDataPos( nSendedLength );//指针后移

		pPacketAssembler->SetOverlapped( TRUE );

		// 犁傈价篮 捞固 盲农级捞 嘿绢 乐促.
		bRet = m_pNetwork->Send(
			lpNetworkKey,
			pSendPacketBuffer->GetData(),
			pSendPacketBuffer->GetDataSize(),
			nError );

		if( !bRet )
		{
			m_pNetwork->CloseSocket( lpNetworkKey );
			pPacketAssembler->SetOverlapped( FALSE );
		}
	}
	else
	{
		// 促澜 菩哦阑 傈价
		pSendPacketBuffer = plstPacketBuffer->Dequeue();
		m_SendPacketBufferPool.Free( pSendPacketBuffer );

		pSendPacketBuffer = plstPacketBuffer->GetRearElement();
		if( pSendPacketBuffer == NULL )
		{
			pPacketAssembler->Unlock();
			return;
		}

		if( pPacketAssembler->IsFree() )
		{
			while( !plstPacketBuffer->IsEmpty() )
			{
				pSendPacketBuffer	= plstPacketBuffer->GetRearElement();
				if( !pPacketAssembler->IsSending( pSendPacketBuffer->GetSendIndex() ) )
				{
					break;
				}

				if( pSendPacketBuffer->GetSendIndex() > 0 )
				{
					pPacketAssembler->AddSendIndex();
				}

				if( pSendPacketBuffer->GetDataSize() == 0 )
				{
					m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
					pPacketAssembler->Unlock();

					OnFreeNetworkObject( lpNetworkKey );
					m_pNetwork->FreeNetworkKey( lpNetworkKey );
					return;
				}
				m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
			}
			pPacketAssembler->Unlock();
			return;
		}


		if( !pPacketAssembler->IsSending( pSendPacketBuffer->GetSendIndex() ) )
		{
			pPacketAssembler->Unlock();
			return;
		}
		if( pSendPacketBuffer->GetSendIndex() > 0 )
			pPacketAssembler->AddSendIndex();

		if( pSendPacketBuffer->GetDataSize() == 0 )
		{
			m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
			pPacketAssembler->Unlock();

			OnFreeNetworkObject( lpNetworkKey );
			m_pNetwork->FreeNetworkKey( lpNetworkKey );
			return;
		}

		// Send且 菩哦捞 Ping 菩哦捞搁 矫埃阑 扁废茄促.
		CPacket* pPacket = reinterpret_cast<CPacket*>( pSendPacketBuffer->GetData() );
		if( pPacket->GetCommand() == CPacket::PredefineCommand::Ping ||
			pPacket->GetCommand() == CPacket::PredefineCommand::PingAck )
		{
			m_pNetwork->SetRttStartTime( lpNetworkKey );
		}

		// Check Sum阑 火涝茄促.
		INT nChangeLength;
		pPacketAssembler->SetCheckSum( pSendPacketBuffer->GetData(), pSendPacketBuffer->GetDataSize(), nChangeLength );
		pSendPacketBuffer->SetDataSize( nChangeLength );

		pPacketAssembler->SetOverlapped( TRUE );
		bRet = m_pNetwork->Send(
			lpNetworkKey,
			pSendPacketBuffer->GetData(),
			pSendPacketBuffer->GetDataSize(),
			nError );

		if( !bRet )
		{
			m_pNetwork->CloseSocket( lpNetworkKey );
			pPacketAssembler->SetOverlapped( FALSE );
		}
	}
	pPacketAssembler->Unlock();
}

void CSessionManager::OnClose( ULONG_PTR lpNetworkKey )
{
	m_pNetwork->WaitRecvDisconnectEvent( lpNetworkKey );

	//char a[100];
	//sprintf( a, "Close %d\r\n", (int) lpNetworkKey );
	//::OutputDebugString( a );

	ULONG_PTR				pAssemblerKey;
	CPacketAssembler*		pPacketAssembler;
	DWORD					dwAssemblerHandle;
	
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;
	pPacketAssembler->Lock();

	m_pNetwork->CloseSocket( lpNetworkKey );

	pPacketAssembler->ResetHandle();
	m_pNetwork->SetAssemblerKey( lpNetworkKey, pAssemblerKey, pPacketAssembler->GetHandle() );
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );

//				char a1[100];
//				sprintf( a1, "OnClose!! %d %d\r\n", (ULONG_PTR)lpNetworkKey, dwAssemblerHandle );
//				::OutputDebugString( a1 );

	pPacketAssembler->Unlock();

	m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, CMessage::MessageType::CloseType ) );
}

void CSessionManager::OnAllocNetworkObject( ULONG_PTR lpNetworkKey )
{
	// 建立对应的缓冲区
	CPacketAssembler* pPacketAssembler = m_PacketAssemblerPool.Alloc();
	if( !pPacketAssembler->IsCreated() )
	{
		if( !pPacketAssembler->Create( m_nPacketSize * 2, m_nNumOfPacketAssembler ) )
		{
			int a = 3;
			return;
		}
	}
//	m_nDebugAssem++;

	pPacketAssembler->Lock();
	if( !pPacketAssembler->GetPacketBufferList()->IsEmpty() )
	{
		int a = 3;
	}

	if( pPacketAssembler->IsFree() )
		int a = 3;

	if( pPacketAssembler->IsOverlapped() )
		int a = 3;

	if( !pPacketAssembler->IsCreated() )
	{
		pPacketAssembler->Create( m_nPacketSize * 2, m_nNumOfPacketAssembler );
	}
	pPacketAssembler->ResetCloseWait();


	// 菩哦 绢祭喉府甫 殿废
	m_pNetwork->SetAssemblerKey( lpNetworkKey, (ULONG_PTR)pPacketAssembler, pPacketAssembler->GetHandle() );


	// 烙矫肺 SessionGate::Start俊辑 咯扁肺 颗变促.
	InitPacketType( lpNetworkKey, CPacketCheckSum::CheckSumType::TwoByteZoroOne, CPacketAssembler::PacketUnitType::Length );
	InitLengthFieldOfPacket( lpNetworkKey, CPacket::GetLengthPos(), CPacketAssembler::LengthSizeType::TwoByte, CPacketAssembler::LengthType::Total );

	if( pPacketAssembler->IsOverlapped() )
		int a = 3;

	pPacketAssembler->Unlock();
}

void CSessionManager::OnFreeNetworkObject( ULONG_PTR lpNetworkKey )
{
	ULONG_PTR				pAssemblerKey;
	CPacketAssembler*		pPacketAssembler;
	DWORD					dwAssemblerHandle;
	CListEx<CPacketBuffer*>*	plstPacketBuffer;		// SendPacket List
	CPacketBuffer*			pPacketBuffer;
	
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey, dwAssemblerHandle );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;

	if( pPacketAssembler->IsOverlapped() )
		int a = 3;
	pPacketAssembler->Reset();

	plstPacketBuffer	= pPacketAssembler->GetPacketBufferList();
	while( !plstPacketBuffer->IsEmpty() )
	{
		pPacketBuffer	= plstPacketBuffer->Dequeue();
		m_SendPacketBufferPool.Free( pPacketBuffer );
//		m_nDebug--;
	}
//					char a[100];
//					sprintf( a, "OnFree %d %d\r\n", (int)lpNetworkKey, (int)pAssemblerKey );
//					::OutputDebugString( a );

	m_PacketAssemblerPool.Free( pPacketAssembler );
	m_pNetwork->SetAssemblerKey( lpNetworkKey, NULL, 0 );
	//m_nDebugAssem--;
}


// 菩哦阑 荐脚茄促.
void CSessionManager::RecvMessage( CMessage& outMessage )
{
	if( m_OldMessage.GetType() == CMessage::MessageType::DataType ||
		m_OldMessage.GetType() == CMessage::MessageType::HttpResponse )
	{
		if( m_bFirstRecvMsg )
		{
			m_bFirstRecvMsg = FALSE;
		}
		else
		{
//			if( m_OldMessage.GetType() == CMessage::MessageType::DataType )
//				::OutputDebugString( "Free Data\r\n" );
//			else
//				::OutputDebugString( "Free Query\r\n" );
			// Message啊 Data 老锭狼 且寸等 巴阑 力芭茄促.
			CPacketBuffer* pPacketBuffer = m_OldMessage.GetPacketBuffer();
			m_RecvPacketBufferPool.Free( pPacketBuffer );
		}
	}
	m_RecvMessageQueue.DequeuePacket( outMessage );
	m_OldMessage = outMessage;
}




void CSessionManager::OnRecvInet( ULONG_PTR lpInetKey, ULONG_PTR lpContext )
{
	// 孽府俊 措茄 Response甫 Response Q俊 角瘤甫 给窍看促.


#ifndef CLIENT_MODULE
	::EnterCriticalSection( &m_csHttp );

	CSyncInet* pInet = reinterpret_cast<CSyncInet*>( lpInetKey );

	int nResult = pInet->HttpResult( lpInetKey );

	BOOL	bRet;
	DWORD	dwResponseSize;
	while( TRUE )
	{
		CPacketBuffer* pPacketBuffer = m_RecvPacketBufferPool.Alloc();
//		::OutputDebugString( "Query\r\n" );
//		nRet = m_InetHttp.HttpResponse( lpInetKey, pPacketBuffer->GetData(), pPacketBuffer->GetBufferSize() );
		bRet = pInet->Response( lpInetKey, pPacketBuffer->GetData(), pPacketBuffer->GetBufferSize(), dwResponseSize );
		if( !bRet || dwResponseSize == 0 )
		{
//			::OutputDebugString( "Free Query\r\n" );
			m_RecvPacketBufferPool.Free( pPacketBuffer );
			break;
		}

		pPacketBuffer->SetDataSize( dwResponseSize );
//		m_HttpMessage.SetHttpData( nRet, lpContext );
		m_RecvMessageQueue.EnqueuePacket(
			CMessage( CMessage::MessageType::HttpResponse, (CRequestObject*) lpContext, pPacketBuffer )
			);
	}

	if( bRet )
		m_RecvMessageQueue.EnqueuePacket( CMessage( lpContext, CMessage::MessageType::HttpResponseEnd, nResult ) );
	else
		m_RecvMessageQueue.EnqueuePacket( CMessage( lpContext, CMessage::MessageType::HttpError, ERROR_NOT_SUPPORTED ) );

	/*
	if( nRet == 0 )
	{
		m_InetHttp.Close( lpInetKey );
		m_RecvMessageQueue.EnqueuePacket( CMessage( lpContext, CMessage::MessageType::HttpResponseEnd, nResult ) );
	}
	else
	{
//		m_InetHttp.Close( lpInetKey );
//		m_RecvMessageQueue.EnqueuePacket( CMessage( lpContext, CMessage::MessageType::HttpError, ERROR_NOT_SUPPORTED ) );
	}
	*/
	::LeaveCriticalSection( &m_csHttp );
#endif
}

void CSessionManager::OnErrorInet( ULONG_PTR lpContext, DWORD nError )
{
	m_RecvMessageQueue.EnqueuePacket( CMessage( lpContext, CMessage::MessageType::HttpError, nError ) );
//	m_ResponseMessageQueue.EnqueuePacket( CMessage( lpContext, CMessage::MessageType::HttpError, nError ) );
}

DWORD __stdcall CSessionManager::QueryThreadStartingPoint1(PVOID pvParam)
{
#ifndef CLIENT_MODULE
	CSessionManager*	pThis			= (CSessionManager*) pvParam;
	CMessage			queryMessage;

	Query::IQueryHandler*	queryObjerver	= pThis;

	BOOL	bRet;
	DWORD	dwError;
	while( TRUE )
	{
		pThis->m_QueryMessageQueue.DequeuePacket( queryMessage );
		bRet = pThis->m_SyncInet1.Request( queryMessage.GetQueryBuffer()->GetData(), dwError );

		if( !bRet )
		{
			pThis->OnErrorInet( reinterpret_cast<ULONG_PTR>( queryMessage.GetRequestObject() ), dwError );
		}
		else
		{
			pThis->OnRecvInet( reinterpret_cast<ULONG_PTR>( &pThis->m_SyncInet1 ), reinterpret_cast<ULONG_PTR>( queryMessage.GetRequestObject() ) );
		}
	}

	/*
	// 孽府 按眉 积己
	m_hOpen = ::InternetOpen( NULL, INTERNET_OPEN_TYPE_DIRECT , NULL, NULL, NULL );

	while( TRUE )
	{
		pThis->m_SendMessageQueue.DequeuePacket( queryMessage );
		{
			pQueryBuffer = queryMessage.GetQueryBuffer();


		}
	}
	*/
#endif
	return 0;
}

DWORD __stdcall CSessionManager::QueryThreadStartingPoint2(PVOID pvParam)
{
#ifndef CLIENT_MODULE
	CSessionManager*	pThis			= (CSessionManager*) pvParam;
	CMessage			queryMessage;

	Query::IQueryHandler*	queryObjerver	= pThis;

	BOOL	bRet;
	DWORD	dwError;
	while( TRUE )
	{
		pThis->m_QueryMessageQueue.DequeuePacket( queryMessage );
		bRet = pThis->m_SyncInet2.Request( queryMessage.GetQueryBuffer()->GetData(), dwError );

		if( !bRet )
		{
			pThis->OnErrorInet( reinterpret_cast<ULONG_PTR>( queryMessage.GetRequestObject() ), dwError );
		}
		else
		{
			pThis->OnRecvInet( reinterpret_cast<ULONG_PTR>( &pThis->m_SyncInet2 ), reinterpret_cast<ULONG_PTR>( queryMessage.GetRequestObject() ) );
		}
	}

	/*
	// 孽府 按眉 积己
	m_hOpen = ::InternetOpen( NULL, INTERNET_OPEN_TYPE_DIRECT , NULL, NULL, NULL );

	while( TRUE )
	{
		pThis->m_SendMessageQueue.DequeuePacket( queryMessage );
		{
			pQueryBuffer = queryMessage.GetQueryBuffer();


		}
	}
	*/
#endif
	return 0;
}

DWORD __stdcall CSessionManager::SendThreadStartingPoint(PVOID pvParam)
{
	CSessionManager*	pThis		= (CSessionManager*) pvParam;

	CMessage sendMessage;

	while( TRUE )
	{
		pThis->m_SendMessageQueue.DequeuePacket( sendMessage );

		switch( sendMessage.GetType() )
		{
		case CMessage::MessageType::DoCloseType :
		case CMessage::MessageType::DoShutdownType :
			pThis->m_pNetwork->CloseSocket( (ULONG_PTR)sendMessage.GetNetworkKey() );
			break;

		//case CMessage::MessageType::DoCloseType :
		//	pThis->m_pNetwork->CloseSocket( (ULONG_PTR)sendMessage.GetNetworkKey() );
		//	break;

		//case CMessage::MessageType::DoShutdownType :
		//	pThis->m_pNetwork->ShutDown( (ULONG_PTR)sendMessage.GetNetworkKey() );
		//	break;

		case CMessage::MessageType::CloseType:
		case CMessage::MessageType::DataType :
			pThis->SendPacket( sendMessage );
			break;

		default:
			int a;
			a = 3;
			break;

		}
	}
	return 0;
}

//从m_DbReqeustMessageQueue中取出消息包 -〉m_RecvMessageQueue
//执行存储过程
DWORD __stdcall CSessionManager::DbThreadStartingPoint(PVOID pvParam)
{
#ifndef CLIENT_MODULE
	if(FAILED(::CoInitialize(NULL)))
		assert( FALSE );

	CSessionManager*	pThis			= (CSessionManager*) pvParam;
	CMessage			queryMessage;
	CMessage			dbConnMessage;

	Query::IQueryHandler*	queryObjerver	= pThis;

	while( TRUE )
	{
		pThis->m_DbReqeustMessageQueue.DequeuePacket( queryMessage );
		CDbObject* pDB = queryMessage.GetDbObject();

		_ConnectionPtr	pDbConn;
		pThis->m_DbConnMessageQueue.DequeuePacket( dbConnMessage );
		pDbConn = dbConnMessage.GetDbConnObject()->GetDbConn();

		BOOL bSuccess = TRUE;
		long state = -1;
		try
		{
			state = pDbConn->GetState();
			switch( state )
			{
			case adStateOpen:
				break;
			case adStateClosed:
				pDbConn->Open(
					pThis->m_dbConnInfo.clsServer,
					pThis->m_dbConnInfo.clsID,
					pThis->m_dbConnInfo.clsPass,
					adConnectUnspecified
					);
				break;
			case adStateConnecting:
				break;
			case adStateExecuting:
				break;
			case adStateFetching:
				break;
			}
			pDB->GetCommand()->ActiveConnection = pDbConn;
	
			//Test
			ParametersPtr param = pDB->GetCommand()->GetParameters();
			long lCn = pDB->GetCommand()->Parameters->Count;
			_bstr_t strname = pDB->GetCommand()->CommandText;
			TCHAR str[2048];
			sprintf(str,"%d:%s\n", pDB->GetMessage(),static_cast<TCHAR*>(strname));
			OutputDebugString(str);
			_variant_t vIndex;

			for( int i = 0; i < lCn; i++)
			{
				vIndex = (short)i;
				_ParameterPtr  p = pDB->GetCommand()->Parameters->GetItem(&vIndex);
				sprintf(str,"Parameter name = '%s'",(LPCSTR)p->Name);
				OutputDebugString(str);
				_bstr_t strvalue = (_bstr_t)p->Value;
				sprintf(str,"value = '%s'\n",static_cast<TCHAR*>(strvalue) );
				OutputDebugString(str);
			}
			//
			pDB->GetCommand()->Execute( NULL, NULL, adExecuteNoRecords | adCmdStoredProc );
			state = pDbConn->GetState();
		}
		catch (_com_error &e)
		{
			::_bstr_t clsSource( e.Source() );
			::_bstr_t clsDetail( e.Description() );

			char strSource[ 1024 ];
			char strDetail[ 1024 ];
			sprintf( strSource, "Dev\t[LOW_DB] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
			sprintf( strDetail, "Dev\t[LOW_DB] Detail: %x, %s\r\n", e.Error(), static_cast<CHAR *>( clsDetail ) );

			::Dispatcher::ICONSOLE()->WriteUnsafe( strSource );
			::Dispatcher::ICONSOLE()->WriteUnsafe( strDetail );

			if( e.Error() == 0x80004005 )	// 老馆 匙飘况农 坷幅
			{
				bSuccess = FALSE;
			}

			pDbConn->Close();
			DEBUG_BREAK;

		}
		catch( ... )
		{
			bSuccess = FALSE;
		}
		pThis->m_DbConnMessageQueue.EnqueuePacket( dbConnMessage );

		pDB->SetSuccess( bSuccess );

		pThis->m_RecvMessageQueue.EnqueuePacket( CMessage( CMessage::DbResponse, pDB ) );
	}



#endif
	return 0;
}

INT CSessionManager::CreateDatabase( INT nDbConnectionCount, LPCTSTR lpszConnection )
{

	if(FAILED(::CoInitialize(NULL)))
		assert( FALSE );

	char					strConn[ 1024 ];
	char *					strConnection[ 100 ];
	int nCount = 0;
	
	strcpy( strConn, lpszConnection );
	char * pToken			= strtok( strConn, "\\" );
	while( pToken != NULL )
	{
		pToken				= strtok( NULL, "\\" );
	}

	m_paDbConn = new CDbConnObject[nDbConnectionCount];
	m_DbConnMessageQueue.Create( nDbConnectionCount, 1 );
	m_DbObjectPool.Create( m_nNumOfPacketAssembler );
	m_DbReqeustMessageQueue.Create( m_nNumOfPacketAssembler, 1 );

	OutputDebugString( lpszConnection );
	OutputDebugString( "\r\n" );

	m_dbConnInfo.clsServer	= lpszConnection;
	m_dbConnInfo.clsID		= "";
	m_dbConnInfo.clsPass	= "";


	INT nConnected = 0;
	for( INT i = 0; i < nDbConnectionCount; i++ )
	{
		m_paDbConn[i].GetDbConn().CreateInstance(__uuidof(Connection));

		HRESULT hr = E_FAIL;
		try
		{
			hr = m_paDbConn[i].GetDbConn()->Open(
				m_dbConnInfo.clsServer,
				m_dbConnInfo.clsID,
				m_dbConnInfo.clsPass,
				adConnectUnspecified
				);
		}
		catch (_com_error &e)
		{
			::_bstr_t clsSource( e.Source() );
			::_bstr_t clsDetail( e.Description() );

			char strSource[ 1024 ];
			char strDetail[ 1024 ];
			sprintf( strSource, "Dev\t[ERROR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
			sprintf( strDetail, "Dev\t[ERROR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

			printf( "%s\r\n%s\r\n", strSource, strDetail );
			OutputDebugString( strSource );
			OutputDebugString( strDetail );
		}
		catch( ... )
		{
		}

		if( SUCCEEDED( hr ) == TRUE )
		{
			nConnected++;
			m_DbConnMessageQueue.EnqueuePacket( CMessage( &m_paDbConn[i] ) );
		}
	}
	OutputDebugString( "DB Connect Success\r\n" );
	for( INT i = 0; i < nConnected; i++ )
	{
		DWORD dwThreadId = 0;
		::CloseHandle( ::CreateThread( NULL, 0, DbThreadStartingPoint, this, 0, &dwThreadId ) );
	}



	return nConnected;
}

CDbObject* CSessionManager::AllocDbObject()
{
	CDbObject* pDB = m_DbObjectPool.Alloc();

	pDB->GetCommand().CreateInstance( __uuidof(Command) );
	ASSERT(pDB->GetCommand());

	pDB->GetCommand()->CommandType = adCmdStoredProc;
	return pDB;
}

void CSessionManager::FreeDbObject( CDbObject* pDb )
{
	pDb->GetCommand().Release();
	pDb->GetCommand().Detach();
	m_DbObjectPool.Free( pDb );
}