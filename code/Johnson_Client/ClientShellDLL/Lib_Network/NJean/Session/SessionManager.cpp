#include "StdAfx.h"
#include "./SessionManager.h"
#include "../../DataStructure/List.h"
using namespace GamehiSolution::Common::DataStructure;

using namespace GamehiSolution::Network;
using namespace GamehiSolution::Session::Manage;
using namespace GamehiSolution::Session::Message;
using namespace GamehiSolution::Common::DataStructure;
using namespace GamehiSolution::Network::TcpAsyncSelect;

#include "../Application/Packet.h"
using namespace Application::Packets;

CSessionManager::CSessionManager()
{
	m_bFirstRecvMsg = TRUE;
}

CSessionManager::~CSessionManager(void)
{
}

// 내부 리소스를 생성한다.
BOOL CSessionManager::Create( NetworkType networkType, QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nEventQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize, HWND hWindow, UINT uiMessage )
{
	m_nPacketSize = nPacketSize;
	m_bFirstRecvMsg = TRUE;

	switch( networkType )
	{
	case NetworkType::TCP_ASYNCSELECT :
		m_pNetwork = new CTcpAsyncSelect( hWindow, uiMessage );
		break;

	default :
		return FALSE;
	}

	switch( queryType )
	{
	case QueryType::HTTP :
		break;

	default :
		return FALSE;
	}

	// 아래 객체도 Network 객체처럼 추상화 처리를 해야 한다.
	m_HttpMessage.Create( nPacketSize );

	if( !m_SendMessage.Create( nPacketSize ) )
	{
		return FALSE;
	}

	if( !m_SendManagePacket.Create( nPacketSize ) )
	{
		return FALSE;
	}

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

	if( !m_RecvPacketBufferPool.Create( nRecvQueueSize ) )
	{
		return FALSE;
	}
	if( !m_SendPacketBufferPool.Create( nSendQueueSize ) )
	{
		return FALSE;
	}


	/////////////////////////
	// 지워야 할 영역
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
	//
	///////////////////////////////

	if( !m_pNetwork->Create( this, nNumOfListener, nNumOfConnectSockets, nPacketSize, nPacketSize ) )
	{
		return FALSE;
	}

	m_nNumOfPacketAssembler = nNumOfConnectSockets;

	m_pPackets	= new TagPackets;


	// None DataType 설정
	m_OldMessage.SetType( CMessage::MessageType::AcceptType );

	::InitializeCriticalSection( &m_csSend );
	::InitializeCriticalSection( &m_csHttp );


	//
	// Send Thread 시작
	//
#ifndef CLIENT_MODULE
	DWORD dwThreadId = 0;
//	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );
//	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );
//	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );
//	::CloseHandle( ::CreateThread( NULL, 0, SendThreadStartingPoint, this, 0, &dwThreadId ) );
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
	
	CPacketAssembler** pPacketAssembler = new CPacketAssembler*[m_nNumOfPacketAssembler];
	for( INT i = 0; i < m_nNumOfPacketAssembler; i++ )
	{
		pPacketAssembler[i] = m_PacketAssemblerPool.Alloc();
		if( !pPacketAssembler[i]->Create( m_nPacketSize * 2 ) )
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

// 내부 리소스를 제거한다.
void CSessionManager::Destroy()
{
	/*
	CSendPacket** paSendPacket = new CSendPacket*[m_SendPacketPool.GetPoolSize()];
	for( int i = 0; i < m_SendPacketPool.GetPoolSize(); i++ )
	{
		paSendPacket[i] = m_SendPacketPool.Alloc();
		paSendPacket[i]->Destroy();
	}
	for( int j = 0; j < m_SendPacketPool.GetPoolSize(); j++ )
	{
		m_SendPacketPool.Free( paSendPacket[j] );
	}
	delete[] paSendPacket;
	*/


	m_pNetwork->Destroy();

	m_SendMessageQueue.Destroy();
	m_RecvMessageQueue.Destroy();
	m_SendPacketPool.Destroy();

	m_SendMessage.Destroy();
	m_SendManagePacket.Destroy();

	m_RecvPacketBufferPool.Destroy();
	m_SendPacketBufferPool.Destroy();

	m_HttpMessage.Destroy();

	delete m_pPackets;


	/*
	CPacketAssembler** pPacketAssembler = new CPacketAssembler*[m_PacketAssemblerPool.GetPoolSize()];
	for( INT k = 0; k < m_PacketAssemblerPool.GetPoolSize(); k++ )
	{
		pPacketAssembler[k] = m_PacketAssemblerPool.Alloc();
		pPacketAssembler[k]->Destroy();
	}
	for( INT l = 0; l < m_nNumOfPacketAssembler; l++ )
	{
		m_PacketAssemblerPool.Free( pPacketAssembler[l] );
	}
	delete[] pPacketAssembler;
	*/
	m_PacketAssemblerPool.Destroy();

	delete m_pNetwork;

	::DeleteCriticalSection( &m_csSend );
	::DeleteCriticalSection( &m_csHttp );
}

INT CSessionManager::GetServiceIndex( unsigned long lpCompletionKey )
{
	return m_pNetwork->GetServicerIndex( lpCompletionKey );
}



// Packet에 관련된 초기화
void CSessionManager::InitPacketType(
		unsigned long							lpCompletionKey,
		CPacketCheckSum::CheckSumType		checkSumType,
		CPacketAssembler::PacketUnitType	packetUnitType )
{
	//
	// lpCompletionKey에 해당하는 PacketAssembler를 초기화한다.
	//
	
	unsigned long			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	m_pNetwork->GetAssemblerKey( lpCompletionKey, pAssemblerKey );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;
	pPacketAssembler->Init( checkSumType, packetUnitType );
}

void CSessionManager::InitLengthFieldOfPacket(
	unsigned long lpCompletionKey,
	int nLengthFieldPos,
	CPacketAssembler::LengthSizeType lengthSizeType,
	CPacketAssembler::LengthType lengthType/* = Network::CPacketAssembler::LengthType::Total*/,
	BOOL bLittleEndian/* = TRUE*/ )
{
	//
	// lpCompletionKey에 해당하는 PacketAssembler를 초기화한다.
	//
	unsigned long			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	m_pNetwork->GetAssemblerKey( lpCompletionKey, pAssemblerKey );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;
	pPacketAssembler->InitLengthField( nLengthFieldPos, lengthSizeType, lengthType, bLittleEndian );
}

VOID CSessionManager::SendMessage( unsigned long pNetworkKey, const CHAR* pData, INT nDataSize )
{
	unsigned long		pAssemblerKey;
	CPacketBuffer*	pSendPacketBuffer;

	m_pNetwork->GetAssemblerKey( pNetworkKey, pAssemblerKey );
	pSendPacketBuffer	= m_SendPacketBufferPool.Alloc();
	pSendPacketBuffer->CopyData( pData, nDataSize );



	// 큐에 넣기 전에 핸들 값을 마킹한다.
	CMessage	message( CMessage::MessageType::DataType, pNetworkKey, pSendPacketBuffer );
	SendPacket( message );

}

// 패킷을 전송한다.
void CSessionManager::SendMessage( CMessage& message )
{
	// 최초의 패킷이면 곧바로 Send를 하고, 그 다음 패킷 부터는 OnSend에서 처리를 한다.
	int nError = 0;


	switch( message.GetType() )
	{
	case CMessage::MessageType::DoCloseType :
		m_pNetwork->CloseSocket( (unsigned long)message.GetNetworkKey() );
		break;

	case CMessage::MessageType::DoShutdownType :
		m_pNetwork->ShutDown( (unsigned long)message.GetNetworkKey() );
		break;

	default :
		SendPacket( message );
		break;

	}
}

void CSessionManager::SendPacket( CMessage& message, BOOL bFirst )
{
	//
	// step 1 : Assembler를 가져온다.
	//

	unsigned long			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	m_pNetwork->GetAssemblerKey( (unsigned long)message.GetNetworkKey(), pAssemblerKey );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;

	// pPacektAssembler가 NULL이면,
	if( pPacketAssembler == NULL )
	{
		if( message.GetType() != CMessage::MessageType::CloseType )
			m_SendPacketBufferPool.Free( message.GetPacketBuffer() );
		return;
	}

	pPacketAssembler->Lock();


	// Send할 List를 가져온다.
	int		nError;
	BOOL	bRet;
	CList<CPacketBuffer*>* plstPacketBuffer = pPacketAssembler->GetPacketBufferList();
	if( plstPacketBuffer->IsEmpty() )
	{
		if( message.GetType() == CMessage::MessageType::CloseType )
		{
			OnFreeNetworkObject( message.GetNetworkKey() );
			m_pNetwork->FreeNetworkKey( message.GetNetworkKey() );
		}
		// DataType
		else
		{
			CPacketBuffer*	pPacketBuffer	= message.GetPacketBuffer();

			// 우선 입력을 하려면 가장 최근 패킷 바로 이전에 삽입을 한다.
			// 가장 최근 패킷은 나뉘어 전송될 수 있기 때문이다.
			if( bFirst )
			{
				CPacketBuffer* pTmp		= plstPacketBuffer->Dequeue();
				plstPacketBuffer->AddFirst( pPacketBuffer );
				plstPacketBuffer->AddFirst( pTmp );
			}
			else
			{
				plstPacketBuffer->Add( pPacketBuffer );
			}

			// Send할 패킷이 Ping 패킷이면 시간을 기록한다.
			CPacket* pPacket = reinterpret_cast<CPacket*>( pPacketBuffer->GetData() );
			if( pPacket->GetCommand() == CPacket::PredefineCommand::Ping ||
				pPacket->GetCommand() == CPacket::PredefineCommand::PingAck )
			{
				/*/
				if( !m_pNetwork->CanStartRTT( message.GetNetworkKey() ) )
				{
					m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
					return;
				}
				/*/

				m_pNetwork->SetRttStartTime( message.GetNetworkKey() );
			}

			// Check Sum을 삽입한다.
			INT nChangeLength;
			pPacketAssembler->SetCheckSum( pPacketBuffer->GetData(), pPacketBuffer->GetDataSize(), nChangeLength );
			pPacketBuffer->SetDataSize( nChangeLength );

			bRet = m_pNetwork->Send(
				(unsigned long)message.GetNetworkKey(),
				pPacketBuffer->GetData(),
				pPacketBuffer->GetDataSize(),
				nError );

			if( bRet )
			{
				m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
			}
		}
	}
	else
	{
		if( message.GetType() == CMessage::MessageType::CloseType )
		{
			pPacketAssembler->SetFree();
		}

		// DataType
		else
		{
			CPacketBuffer*	pPacketBuffer	= message.GetPacketBuffer();

			if( bFirst )
			{
				CPacketBuffer* pTmp		= plstPacketBuffer->Dequeue();
				plstPacketBuffer->AddFirst( pPacketBuffer );
				plstPacketBuffer->AddFirst( pTmp );
			}
			else
			{
				plstPacketBuffer->Add( pPacketBuffer );
			}

			// Check Sum을 삽입한다.
			INT nChangeLength;
			pPacketAssembler->SetCheckSum( pPacketBuffer->GetData(), pPacketBuffer->GetDataSize(), nChangeLength );
			pPacketBuffer->SetDataSize( nChangeLength );
		}
	}
	pPacketAssembler->Unlock();
}

void CSessionManager::SendPacket( unsigned long lpNetworkKey, CPacketBuffer* pPacketBuffer, BOOL bFirst )
{
	//ASSERT( FALSE );
	/*
	//
	// step 2 : send할 객체가 다른 객체이면 send packet을 drop 시킨다.
	//
	unsigned long			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;
	if( pPacketBuffer->GetHandle() != dwAssemblerHandle )
	{
//		m_SendPacketPool.Free( pSendPacket );
		m_SendPacketBufferPool.Free( pPacketBuffer );
		return;
	}



	// Check Sum을 삽입한다.
	INT nChangeLength;
	pPacketAssembler->SetCheckSum( pPacketBuffer->GetData(), pPacketBuffer->GetDataSize(), nChangeLength );
	pPacketBuffer->SetDataSize( nChangeLength );

	pPacketAssembler->Lock();


	// Send할 List를 가져온다.
	int		nError;
	BOOL	bRet;
	CList<CPacketBuffer*>* plstPacketBuffer = pPacketAssembler->GetPacketBufferList();
	if( plstPacketBuffer->IsEmpty() )
	{
		// 우선 입력을 하려면 가장 최근 패킷 바로 이전에 삽입을 한다.
		// 가장 최근 패킷은 나뉘어 전송될 수 있기 때문이다.
		if( bFirst )
		{
			CPacketBuffer* pTmp		= plstPacketBuffer->Dequeue();
			plstPacketBuffer->AddFirst( pPacketBuffer );
			plstPacketBuffer->AddFirst( pTmp );
		}
		else
		{
			plstPacketBuffer->Add( pPacketBuffer );
		}

		pPacketAssembler->Unlock();

		// Send할 패킷이 Ping 패킷이면 시간을 기록한다.
		CPacket* pPacket = reinterpret_cast<CPacket*>( pPacketBuffer->GetData() );
		if( pPacket->GetCommand() == CPacket::PredefineCommand::Ping ||
			pPacket->GetCommand() == CPacket::PredefineCommand::PingAck )
		{
			if( !m_pNetwork->CanStartRTT( lpNetworkKey ) )
			{
				return;
			}

			m_pNetwork->SetRttStartTime( lpNetworkKey );
		}

		bRet = m_pNetwork->Send(
			lpNetworkKey,
			pPacketBuffer->GetData(),
			pPacketBuffer->GetDataSize(),
			nError );

		if( !bRet )
		{
			pPacketAssembler->Lock();

			// OnFree와 동시에 진행될 수 있으므로 조건검사를 먼저 한다.
			if( !plstPacketBuffer->IsEmpty() )
			{
				plstPacketBuffer->Pop();
				m_SendPacketBufferPool.Free( pPacketBuffer );
			}

			pPacketAssembler->Unlock();
		}
	}
	else
	{
		if( bFirst )
		{
			CPacketBuffer* pTmp		= plstPacketBuffer->Dequeue();
			plstPacketBuffer->AddFirst( pPacketBuffer );
			plstPacketBuffer->AddFirst( pTmp );
		}
		else
		{
			plstPacketBuffer->Add( pPacketBuffer );
		}

		pPacketAssembler->Unlock();
	}

	/*
	int		nError;
	BOOL	bRet;
	
	if( pPacketAssembler->IsEmptySendList() )
	{
		pPacketAssembler->AddToSendList( pSendPacket, bFirst );

		// Send할 패킷이 Ping 패킷이면 시간을 기록한다.
		CPacket* pPacket = (CPacket*)pPacketAssembler->GetSendBuffer();
		if( pPacket->GetCommand() == CPacket::PredefineCommand::Ping ||
			pPacket->GetCommand() == CPacket::PredefineCommand::PingAck )
		{
			if( !m_pNetwork->CanStartRTT( message.GetNetworkKey() ) )
			{
				pPacketAssembler->Unlock();
				return;
			}

			m_pNetwork->SetRttStartTime( message.GetNetworkKey() );
		}

		ASSERT( pPacketAssembler->GetSendSize() );

		char* pData = pPacketAssembler->GetSendBuffer();
		int nSize = pPacketAssembler->GetSendSize();

		pPacketAssembler->Unlock();

		bRet = m_pNetwork->Send(
			(unsigned long)message.GetNetworkKey(),
			pData,
			nSize,
			nError );

		if( !bRet )
		{
			pPacketAssembler->Lock();

			pSendPacket = pPacketAssembler->RemoveFromSendList( nSize );
			if( pSendPacket != NULL )
				
			m_SendPacketPool.Free( pSendPacket );

			pPacketAssembler->Unlock();
		}
	}
	else
	{
		pPacketAssembler->AddToSendList( pSendPacket, bFirst );
		pPacketAssembler->Unlock();
	}
	*/
}


// 패킷을 수신한다.
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
			// Message가 Data 일때의 할당된 것을 제거한다.
			CPacketBuffer* pPacketBuffer = m_OldMessage.GetPacketBuffer();
			m_RecvPacketBufferPool.Free( pPacketBuffer );
		}
	}
	m_RecvMessageQueue.DequeuePacket( outMessage );
	
	m_OldMessage = outMessage;
}


// 타이머를 동작시킨다.
void CSessionManager::StartTimer( int nTickTime, int nMaxTickCount, int nSize )
{
	m_Timer.Create( nTickTime, nMaxTickCount, nSize, (ITimerHandler*)this );
}

// 타이머를 추가한다.
void CSessionManager::AddTimer( int nTickCount, const CSignal& signal )
{
	m_Timer.SetTimer( nTickCount, signal );
}

// ITimerHandler
void CSessionManager::OnTimer( const CSignal& signal )
{
	m_RecvMessageQueue.AddSignalPacket( signal );
}

// 이벤트를 추가한다.
void CSessionManager::AddSignal( const CSignal& signal )
{
	m_RecvMessageQueue.AddSignalPacket( signal );
}

void CSessionManager::SetReceiveEvent( unsigned long lpNetworkKey )
{
	m_pNetwork->SetConnectRecvEvent( lpNetworkKey );
}


void CSessionManager::OnAccept( unsigned long lpNetworkKey )
{
	// Receive / AcceptConnect 동기화에 대한 초기화를 한다.
	m_pNetwork->ResetConnectRecvEvent( lpNetworkKey );

	m_pNetwork->ResetRttTime( lpNetworkKey );

	m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, CMessage::MessageType::AcceptType ) );
}

void CSessionManager::OnConnect( unsigned long lpNetworkKey )
{
		// Receive / AcceptConnect 동기화에 대한 초기화를 한다.
	m_pNetwork->ResetConnectRecvEvent( lpNetworkKey );

	m_pNetwork->ResetRttTime( lpNetworkKey );

	m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, CMessage::MessageType::ConnectType ) );
}

void CSessionManager::OnBeginConnect( unsigned long lpNetworkKey )
{
	m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, CMessage::MessageType::BeginConnectType ) );
}

void CSessionManager::OnReceive( unsigned long lpNetworkKey, char* pRecvBuff, int nReceivedLength )
{
	// 첫번째 Receive이면, Connect/Accept에 대한 이벤트를 기다린다.
	m_pNetwork->ResetRecvDisconnectEvent( lpNetworkKey );
	m_pNetwork->WaitConnectRecvEvent( lpNetworkKey );

	unsigned long			pAssemblerKey;
	CPacketAssembler*	pPacketAssembler;
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;

	if( !pPacketAssembler->Assemble( pRecvBuff, nReceivedLength ) )
	{
		m_pNetwork->SetRecvDisconnectEvent( lpNetworkKey );
		return;
	}

	char* pPacket;
	int nPacketSize = 0;
	try
	{
		while( pPacket = (char*)pPacketAssembler->GetCompletePacket( nPacketSize ) )
		{
			CPacket* pDataPacket = (CPacket*)pPacket;

			CPacketBuffer*	pRecvPacketBuffer;
			pRecvPacketBuffer	= m_RecvPacketBufferPool.Alloc();
			pRecvPacketBuffer->CopyData( pPacket, nPacketSize );

			switch( pDataPacket->GetCommand() )
			{
			//
			// Step 1 : 우선 처리될 패킷은 Message Q에 Enqueue하지 않고 곧바로 처리한다.
			//
				// Client -> Server에 보내는 패킷
			case CPacket::PredefineCommand::Ping :
				{
					CPacketPing*	pPingPacket;
					pPingPacket		= (CPacketPing*)pDataPacket;
					if( pPingPacket->GetRealLength() != nPacketSize )
					{
						SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::DoShutdownType ) );
					}

					// Alive를 위해서 Ping Packet을 App로 보낸다.
					m_RecvMessageQueue.EnqueuePacket(
						CMessage( CMessage::MessageType::DataType, lpNetworkKey, pRecvPacketBuffer ) );


					// 여기서부터 할 차례 : ackPacket을 모든 패킷 클래스의 스트럭쳐 만들고.. TagPackets.. 처리할것..
					// 아래의 SendPacket은 Send Q 를 통하지 않고 우선적으로 보내기 위한 처리

					unsigned long	pAssemblerKey;
					m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey );

					CPacketBuffer* pSendPacketBuffer = m_SendPacketBufferPool.Alloc();
					pSendPacketBuffer->CopyData(
						reinterpret_cast<CHAR*>( &m_pPackets->PingAck ),
						m_pPackets->PingAck.GetLength()
						);

					CMessage	message( CMessage::MessageType::DataType, lpNetworkKey, pSendPacketBuffer );
					SendPacket( message );
				}
				break;

				// Server -> Client에 보내는 패킷
			case CPacket::PredefineCommand::PingAck :
				{
					CPacketPingAck*	pPingAckPacket;
					pPingAckPacket	= (CPacketPingAck*)pDataPacket;
					if( pPingAckPacket->GetRealLength() != nPacketSize )
					{
						SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::DoShutdownType ) );
						break;
					}

					// Rtt 타임을 체크한다.
					m_pNetwork->SetRttEndTime( lpNetworkKey );

					unsigned long	pAssemblerKey;
					m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey );

					CPacketBuffer* pSendPacketBuffer = m_SendPacketBufferPool.Alloc();
					pSendPacketBuffer->CopyData(
						reinterpret_cast<CHAR*>( &m_pPackets->Ack ),
						m_pPackets->Ack.GetLength()
						);
					CMessage	message( CMessage::MessageType::DataType, lpNetworkKey, pSendPacketBuffer );
					SendPacket( message );
				}
				break;

				// Client -> Server에 보내는 패킷
			case CPacket::PredefineCommand::Ack :
				{
					CPacketAck*		pAckPacket;
					pAckPacket		= (CPacketAck*)pDataPacket;
					if( pAckPacket->GetRealLength() != nPacketSize )
					{
						SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::DoShutdownType ) );
						break;
					}

					// Rtt 타임을 체크한다.
					m_pNetwork->SetRttEndTime( lpNetworkKey );

					DWORD dwRttTime;
					m_pNetwork->GetRttTime( lpNetworkKey, dwRttTime );
				}
				break;

			//
			// Step 2 : 패킷을 Message Q에 Enqueue한다.
			//
			default :
				m_RecvMessageQueue.EnqueuePacket(
					CMessage( CMessage::MessageType::DataType, lpNetworkKey, pRecvPacketBuffer ) );
				break;
			}

		}
	}
	catch( int nErrorCode )
	{
		m_pNetwork->ShutDown( lpNetworkKey );
		m_pNetwork->SetRecvDisconnectEvent( lpNetworkKey );
		nErrorCode = 0;
		return;
	}
	m_pNetwork->SetRecvDisconnectEvent( lpNetworkKey );
}

void CSessionManager::OnSend( unsigned long lpNetworkKey, char* pRecvBuff, int nSendedLength )
{
	unsigned long				pAssemblerKey;
	CPacketAssembler*		pPacketAssembler;
	CList<CPacketBuffer*>*	plstPacketBuffer;		// SendPacket List
	CPacketBuffer*			pSendPacketBuffer;		// Send한 패킷 버퍼

	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey );
	pPacketAssembler	= reinterpret_cast<CPacketAssembler*>( pAssemblerKey );
	plstPacketBuffer	= pPacketAssembler->GetPacketBufferList();

	pPacketAssembler->Lock();
	
	int		nError;
	BOOL	bRet;

	if( pPacketAssembler->IsFree() )
	{
		OnFreeNetworkObject( lpNetworkKey );
		m_pNetwork->FreeNetworkKey( lpNetworkKey );
		pPacketAssembler->Unlock();
		return;
	}
	
	if( plstPacketBuffer->IsEmpty() )
	{
		pPacketAssembler->Unlock();
		return;
	}
	

	// 큐잉된 모든 페킷 전달
	pSendPacketBuffer	= plstPacketBuffer->GetRearElement();
	pSendPacketBuffer->ReplaceDataPos( nSendedLength );

	if( pSendPacketBuffer->GetDataSize() == 0 )
	{
		m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
		pSendPacketBuffer = plstPacketBuffer->GetRearElement();
	}

	while( pSendPacketBuffer )
	{
		bRet = m_pNetwork->Send(
			lpNetworkKey,
			pSendPacketBuffer ->GetData(),
			pSendPacketBuffer ->GetDataSize(),
			nError );

		if( !bRet )
			break;

		m_SendPacketBufferPool.Free( plstPacketBuffer->Dequeue() );
		pSendPacketBuffer = plstPacketBuffer->GetRearElement();
	}
	pPacketAssembler->Unlock();
}

void CSessionManager::OnClose( unsigned long lpNetworkKey )
{
	m_pNetwork->WaitRecvDisconnectEvent( lpNetworkKey );

	m_RecvMessageQueue.EnqueuePacket( CMessage( lpNetworkKey, CMessage::MessageType::CloseType ) );
}

void CSessionManager::OnAllocNetworkObject( unsigned long lpNetworkKey )
{
	// 패킷 어셈블리 할당 및 초기화
	CPacketAssembler* pPacketAssembler = m_PacketAssemblerPool.Alloc();

	if( !pPacketAssembler->GetPacketBufferList()->IsEmpty() )
	{
		int a = 3;
	}


	if( !pPacketAssembler->IsCreated() )
	{
		pPacketAssembler->Create( m_nPacketSize * 2 );
	}
	pPacketAssembler->ResetCloseWait();


	// 패킷 어셈블리를 등록
	m_pNetwork->SetAssemblerKey( lpNetworkKey, (unsigned long)pPacketAssembler );


	// 임시로 SessionGate::Start에서 여기로 옮긴다.
	InitPacketType( lpNetworkKey, CPacketCheckSum::CheckSumType::TwoByteZoroOne, CPacketAssembler::PacketUnitType::Length );
	InitLengthFieldOfPacket( lpNetworkKey, CPacket::GetLengthPos(), CPacketAssembler::LengthSizeType::TwoByte, CPacketAssembler::LengthType::Total );
}

void CSessionManager::OnFreeNetworkObject( unsigned long lpNetworkKey )
{
	unsigned long				pAssemblerKey;
	CPacketAssembler*		pPacketAssembler;
//	DWORD					dwAssemblerHandle;
	CList<CPacketBuffer*>*	plstPacketBuffer;		// SendPacket List
	CPacketBuffer*			pPacketBuffer;
	
	m_pNetwork->GetAssemblerKey( lpNetworkKey, pAssemblerKey );
	pPacketAssembler = (CPacketAssembler*)pAssemblerKey;

	pPacketAssembler->Reset();

	plstPacketBuffer	= pPacketAssembler->GetPacketBufferList();
	while( !plstPacketBuffer->IsEmpty() )
	{
		pPacketBuffer	= plstPacketBuffer->Dequeue();
		m_SendPacketBufferPool.Free( pPacketBuffer );
	}

	m_PacketAssemblerPool.Free( pPacketAssembler );
	m_pNetwork->SetAssemblerKey( lpNetworkKey, NULL );
}




void CSessionManager::OnRecvInet( unsigned long lpInetKey, unsigned long lpContext )
{
	/*
#ifndef CLIENT_MODULE
	::EnterCriticalSection( &m_csHttp );
	int nResult = m_InetHttp.HttpResult( lpInetKey );

	int nRet;
	while( TRUE )
	{
		CPacketBuffer* pPacketBuffer = m_RecvPacketBufferPool.Alloc();
		nRet = m_InetHttp.HttpResponse( lpInetKey, pPacketBuffer->GetData(), pPacketBuffer->GetBufferSize() );
		if( nRet <= 0 )
			break;

		pPacketBuffer->SetDataSize( nRet );
//		m_HttpMessage.SetHttpData( nRet, lpContext );
		m_RecvMessageQueue.EnqueuePacket(
			CMessage( CMessage::MessageType::HttpResponse, (CRequestObject*) lpContext, pPacketBuffer )
			);
	}
    
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
	::LeaveCriticalSection( &m_csHttp );
#endif
	*/
}

void CSessionManager::OnErrorInet( unsigned long lpContext, DWORD nError )
{
	m_RecvMessageQueue.EnqueuePacket( CMessage( lpContext, CMessage::MessageType::HttpError, nError ) );
}

DWORD __stdcall CSessionManager::SendThreadStartingPoint(PVOID pvParam)
{
	CSessionManager*	pThis		= (CSessionManager*) pvParam;
	CMessage&			sendMessage2	= pThis->m_SendMessage;
	CMessage sendMessage;
//	sendMessage.Create( 256 );

	while( TRUE )
	{
		pThis->m_SendMessageQueue.DequeuePacket( sendMessage );
		switch( sendMessage.GetType() )
		{

		case CMessage::MessageType::DoCloseType :
			pThis->m_pNetwork->CloseSocket( (unsigned long)sendMessage.GetNetworkKey() );
			break;

		case CMessage::MessageType::DoShutdownType :
			pThis->m_pNetwork->ShutDown( (unsigned long)sendMessage.GetNetworkKey() );
			break;

		case CMessage::MessageType::DataType :
			pThis->SendPacket( sendMessage );
			break;

		default:
			break;

		}
	}
	return 0;
}