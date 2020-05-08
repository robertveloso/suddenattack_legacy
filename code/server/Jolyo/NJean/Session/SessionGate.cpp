#include "StdAfx.h"
#include ".\SessionGate.h"

#include "../../../Dispatcher/IConsole.h"

#include "..\Application\Packet.h"
using namespace Application::Packets;

using namespace Session::Gate;
using namespace Session::Message;
using namespace Session::Manage;
using namespace Application;

CSessionGate::CSessionGate(void)
{
}

CSessionGate::~CSessionGate(void)
{
}

BOOL CSessionGate::Create( INT nTickTime, INT nMaxTick, INT nTimerEventQueueSize,
						   INT nAliveTickError, INT nAliveTimeoutCount, INT nNumOfAliveManaged,
						   CSessionManager::NetworkType networkType,
						   CSessionManager::QueryType queryType,
						   INT nRecvQueueSize,	INT nSendQueueSize,
						   INT nQueryQueueSize,	INT nEventQueueSize,
						   INT nNumOfListener,	INT nNumOfConnectSockets,
						   INT nPacketSize,		INT nResponseBuffer,
						   HWND hWindow,		UINT uiMessage
						    )
{
//	m_pAssembleBuffer	= new char[nPacketSize];
//	m_nAssemble			= 0;
//	m_nPacketSize	= nPacketSize;
	m_nPacketSize	= CPacket::PredefineLength::BasePacketSize;

	if( !m_MessageRecv.Create( m_nPacketSize ) )		return FALSE;
	if( !m_MessageHttp.Create( m_nPacketSize ) )		return FALSE;
	if( !m_MessageData.Create( m_nPacketSize ) )		return FALSE;
	if( !m_HttpResponser.Create( nResponseBuffer ) )	return FALSE;	


	m_MessageData.SetType( CMessage::MessageType::DataType );
	m_MessageHttp.SetType( CMessage::MessageType::HttpRequest	);
	m_HttpResponser.Reset();

	m_RequestObjectPool.Create( nNumOfConnectSockets );
	m_QeuryBufferPool.Create( nNumOfConnectSockets );


	m_QueryPeer.Init();
	BOOL bSuccess = m_Server.Create( networkType, queryType, nRecvQueueSize, nSendQueueSize, nQueryQueueSize, nEventQueueSize, nNumOfListener, nNumOfConnectSockets, m_nPacketSize, hWindow, uiMessage );
	if( bSuccess == FALSE )
	{
		return FALSE;
	}


	// »ó¼ÓÇÑ Å¬·¡½º´Â ¾Æ·¡¸¦ ±¸ÇöÇÑ´Ù.

	// CreateListener
	// CreateEnd

	// 1000ms ¸¶´Ù Tick ¹ß»ý, 10°³ÀÇ timeout Ã³¸® ¹è¿­, ÀÌº¥Æ® Å¥ 1000°³
	m_Server.StartTimer( nTickTime, nMaxTick, nTimerEventQueueSize );

	if( nAliveTimeoutCount * nAliveTickError > 0 )
	{
		m_AliveManager.Create( nAliveTimeoutCount );
		m_Server.AddTimer( nAliveTickError, CSignal( 0, 0 ) );
	}

	return TRUE;
}

//ÏûÏ¢Ñ­»·´¦Àí
BOOL CSessionGate::Start( INT nAliveTickError )
{
	HANDLE hThread = ::GetCurrentThread();
	BOOL bSuccess = ::SetThreadPriority( hThread, THREAD_PRIORITY_ABOVE_NORMAL );

	while( true )
	{
		m_Server.SendMessageFlush();	//½«µ±Ç°µÄÏûÏ¢°ü×ªÈëm_SendMessageQueue£¬È»ºóÓÐ·¢ËÍÏß³Ì·¢ËÍ
		m_Server.RecvMessage( m_MessageRecv );//µÃµ½recv»º³åm_RecvMessageQueueÖÐµÄÏûÏ¢°ü
		switch( m_MessageRecv.GetType() )
		{
		case CMessage::MessageType::DbResponse :
			{
				CDbObject* pDB = m_MessageRecv.GetDbObject();
				CPeer*	pPeer		= pDB->GetPeer();
				if( pPeer->IsRequestWait() )
				{
					m_Server.FreeDbObject( pDB );
					if( !pPeer->IsRequested() )
					{
						ULONG_PTR lpNetworkKey = pPeer->GetNetworkKey();
						FreePeer( pPeer, pPeer->GetServicerIndex() );
						m_Server.SetOwnerKey( lpNetworkKey, NULL );
						m_Server.SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::CloseType ) );
					}
					break;
				}

//				OnResponse( pDB->GetPeer(), pDB->GetMessage(), pDB->IsSuccess(), pDB->GetCommand() );
				if( pDB->GetPeer() == &m_QueryPeer )
				{
					OnResponse( NULL, pDB );
				}
				else
				{
					OnResponse( pDB->GetPeer(), pDB );
				}
				m_Server.FreeDbObject( pDB );
				pPeer->DecrementRequestCount();

			}
			break;

		case CMessage::MessageType::HttpResponse :
			{
//				CRequestObject*	pRequestObject = (CRequestObject*)m_MessageRecv.GetNetworkKey();
				CPacketBuffer* pPacketBuffer = m_MessageRecv.GetPacketBuffer();
				m_HttpResponser.AddData( pPacketBuffer->GetData(), pPacketBuffer->GetDataSize() );
			}
			break;

		case CMessage::MessageType::HttpResponseEnd :
			{
				CRequestObject*	pRequestObject = (CRequestObject*)m_MessageRecv.GetNetworkKey();

				CPeer*	pPeer		= pRequestObject->GetPeer();
				UINT	uiMessage	= pRequestObject->GetMessage();
				m_RequestObjectPool.Free( pRequestObject );
//				char a[100];
//				sprintf( a, "FREE %d\n", (int)pRequestObject );
//				::OutputDebugString( a );
				m_QeuryBufferPool.Free( pRequestObject->GetQueryBuffer() );

				pPeer->DecrementRequestCount();

				if( pPeer->IsRequestWait() )
				{
					m_HttpResponser.Reset();
					if( !pPeer->IsRequested() )
					{
						ULONG_PTR lpNetworkKey = pPeer->GetNetworkKey();
						FreePeer( pPeer, pPeer->GetServicerIndex() );
						m_Server.SetOwnerKey( lpNetworkKey, NULL );
						m_Server.SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::CloseType ) );
					}
					break;
				}

				if( pPeer->IsCloseWait() )
				{
					m_HttpResponser.Reset();
					break;
				}
				
				if( m_HttpResponser.GetError() == 0 )
					OnResponse( pPeer, uiMessage, 0, m_MessageRecv.GetHttpResult(), m_HttpResponser.GetData(), m_HttpResponser.GetDataSize() );
				else
				{
					OnResponse( pPeer, uiMessage, m_HttpResponser.GetError(), 0, NULL, 0 );
					char a[100];
					sprintf( a, "Http Error : %s\n", Common::Error::CHttpError::GetErrorMessage( m_MessageRecv.GetHttpResult() ) );
					::OutputDebugString( a );
				}

				m_HttpResponser.Reset();
			}
			break;

		case CMessage::MessageType::HttpError :
			{
//				if( m_MessageRecv.GetHttpResult() == Common::Error::CHttpError::WINHTTP_OPERATION_CANCELLED )
//					break;

				CRequestObject*	pRequestObject = (CRequestObject*)m_MessageRecv.GetNetworkKey();

				CPeer*	pPeer		= pRequestObject->GetPeer();
				UINT	uiMessage	= pRequestObject->GetMessage();
				m_RequestObjectPool.Free( pRequestObject );
				m_QeuryBufferPool.Free( pRequestObject->GetQueryBuffer() );

				pPeer->DecrementRequestCount();

				if( pPeer->IsRequestWait() )
				{
					m_HttpResponser.Reset();
					if( !pPeer->IsRequested() )
					{
						ULONG_PTR lpNetworkKey = pPeer->GetNetworkKey();
						FreePeer( pPeer, pPeer->GetServicerIndex() );
						m_Server.SetOwnerKey( lpNetworkKey, NULL );
						m_Server.SendMessage( CMessage( lpNetworkKey, CMessage::MessageType::CloseType ) );
					}
					break;
				}

				if( pPeer->IsCloseWait() )
				{
					m_HttpResponser.Reset();
					break;
				}

				OnResponse( pPeer, uiMessage, m_MessageRecv.GetHttpResult(), 0, NULL, 0 );
				m_HttpResponser.Reset();
			}
			break;


		case CMessage::MessageType::SignalType :
			{
				CSignal	signal		= m_MessageRecv.GetSignal();

				UINT		uiMessage	= signal.GetMessage();
				ULONG_PTR	pWParam		= signal.GetWParam();
				ULONG_PTR	pLParam		= signal.GetLParam();
				ULONG_PTR	lpTimerKey	= signal.GetTimerKey();

				// Alive Ã¼Å© Å¸ÀÌ¸Ó
				if( uiMessage == 0 )
				{
					CPeer*	pPeer = NULL;
					while( TRUE )
					{
						if( !m_AliveManager.PopTimeoutObject( pPeer ) )
							break;

						OnTimeout( pPeer );//Èô½ÇÉ«ÔÚµÈºò¹Ø±ÕµÄÁÐ±íÖÐ£¬ÔòÍ¨¹ý¸Ãµ÷ÓÃÔÚm_SendMessageQueue¼ÓÈëDataTypeÏûÏ¢£¬È»ºó·¢ËÍ³öÈ¥
					}

					m_AliveManager.DoTimeoutProcess();
					m_Server.AddTimer( nAliveTickError, CSignal( 0, 0 ) );
				}
				else
				{
					if( lpTimerKey == NULL )
					{
						OnSignal( uiMessage, pWParam, pLParam );
					}
					else
					{
						if( !m_KillTimerMap.Erase( lpTimerKey ) )
							OnSignal( uiMessage, pWParam, pLParam );
					}
				}				
			}
			break;


		case CMessage::MessageType::AcceptType :
			{
				INT nServicerIndex = m_Server.GetServiceIndex( m_MessageRecv.GetNetworkKey() );
				CPeer* pPeer = AllocPeer( nServicerIndex );
				if( pPeer == NULL )
				{
//#pragma message( "===============================\r\n" )
//#pragma message( "ºñÁ¤»ó Á¾·á¸¦ À§ÇÑ ÀÓ½Ã·Î±×\r\n" )
//#pragma message( "===============================\r\n" )
//					::Dispatcher::ICONSOLE()->WriteUnsafe( "DoClose\tLowLevel\PeerAllocFail\r\n" );

					m_MessageRecv.SetType( CMessage::MessageType::DoShutdownType );
					m_Server.SendMessage( m_MessageRecv );
					m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), (ULONG_PTR)pPeer );

					// ÀÓ½Ã·Î CSessionManager::OnAllocNetworkObject·Î ¿Å±ä´Ù.!!!!

					/*
					Message::CPacketCheckSum::CheckSumType		outCheckSumType;
					Message::CPacketAssembler::PacketUnitType	outPacketUnitType;
					InitListenerPacketType( nServicerIndex, outCheckSumType, outPacketUnitType );

					INT											out_nLengthFieldPos;
					Message::CPacketAssembler::LengthSizeType	out_lengthSizeType;
					Message::CPacketAssembler::LengthType		out_LengthType;
					BOOL										out_bLittleEndian;
					InitLengthFieldOfPacket( nServicerIndex, out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );

					m_Server.InitPacketType( m_MessageRecv.GetNetworkKey(), outCheckSumType, outPacketUnitType );
					m_Server.InitLengthFieldOfPacket( m_MessageRecv.GetNetworkKey(), out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );
					*/

					break;
				}

				m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), (ULONG_PTR)pPeer );
				// GetHttpResult´Â GetHandle ´ë½ÅÀ¸·Î ÀÓ½Ã·Î ³ÖÀº °ÍÀÌ´Ù.
				pPeer->SetNetworkKey( m_MessageRecv.GetNetworkKey(), m_MessageRecv.GetHttpResult() );


//				char a1[100];
//				sprintf( a1, "OnAccept!! %d %d\r\n", (ULONG_PTR)m_MessageRecv.GetNetworkKey(), m_MessageRecv.GetHttpResult() );
//				::OutputDebugString( a1 );


				// ÀÓ½Ã·Î CSessionManager::OnAllocNetworkObject·Î ¿Å±ä´Ù.!!!!

				/*
				Message::CPacketCheckSum::CheckSumType		outCheckSumType;
				Message::CPacketAssembler::PacketUnitType	outPacketUnitType;
				InitListenerPacketType( nServicerIndex, outCheckSumType, outPacketUnitType );

				INT											out_nLengthFieldPos;
				Message::CPacketAssembler::LengthSizeType	out_lengthSizeType;
				Message::CPacketAssembler::LengthType		out_LengthType;
				BOOL										out_bLittleEndian;
				InitLengthFieldOfPacket( nServicerIndex, out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );

				m_Server.InitPacketType( m_MessageRecv.GetNetworkKey(), outCheckSumType, outPacketUnitType );
				m_Server.InitLengthFieldOfPacket( m_MessageRecv.GetNetworkKey(), out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );
				*/

				pPeer->Init();
				pPeer->SetServicerIndex( nServicerIndex );
				if( IsAliveManageObject( nServicerIndex ) )
				{
					pPeer->SetAliveManaged( TRUE );
					m_AliveManager.Add( pPeer->GetInnerObjectForPing() );
				}

				OnAccept( pPeer, nServicerIndex );

				m_Server.SetReceiveEvent( m_MessageRecv.GetNetworkKey() );
			}
			break;

		case CMessage::MessageType::ConnectType :
			{
				CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
				pPeer->SetNetworkKey( m_MessageRecv.GetNetworkKey(), 0 );

				m_Server.GetServiceIndex( m_MessageRecv.GetNetworkKey() );
				INT nServicerIndex = pPeer->GetServicerIndex();

				// ÀÓ½Ã·Î CSessionManager::OnAllocNetworkObject·Î ¿Å±ä´Ù.!!!!

				/*
				Message::CPacketCheckSum::CheckSumType		outCheckSumType;
				Message::CPacketAssembler::PacketUnitType	outPacketUnitType;
				InitListenerPacketType( nServicerIndex, outCheckSumType, outPacketUnitType );

				INT											out_nLengthFieldPos;
				Message::CPacketAssembler::LengthSizeType	out_lengthSizeType;
				Message::CPacketAssembler::LengthType		out_LengthType;
				BOOL										out_bLittleEndian;
				InitLengthFieldOfPacket( nServicerIndex, out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );

				m_Server.InitPacketType( m_MessageRecv.GetNetworkKey(), outCheckSumType, outPacketUnitType );
				m_Server.InitLengthFieldOfPacket( m_MessageRecv.GetNetworkKey(), out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );
				*/

				pPeer->Init();
				if( IsAliveManageObject( nServicerIndex ) )
				{
					pPeer->SetAliveManaged( TRUE );
					m_AliveManager.Add( pPeer->GetInnerObjectForPing() );
				}

				OnConnect( pPeer, nServicerIndex );

				m_Server.SetReceiveEvent( m_MessageRecv.GetNetworkKey() );
			}
			break;

		case CMessage::MessageType::CloseType :
			{
				CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
				if( pPeer == NULL )
					break;

//				char a1[100];
//				sprintf( a1, "CloseType!! %d %d\r\n", (ULONG_PTR)m_MessageRecv.GetNetworkKey(), pPeer->GetHandle() );
//				::OutputDebugString( a1 );

				if( IsAliveManageObject( pPeer->GetServicerIndex() ) )
				{
					m_AliveManager.Remove( pPeer->GetInnerObjectForPing() );
				}

				OnClose( pPeer, pPeer->GetServicerIndex() );

				if( pPeer->IsRequested() )
				{
//					::OutputDebugString( "Set Request\n" );
					pPeer->SetRequestWait();
				}
				else
				{
					FreePeer( pPeer, pPeer->GetServicerIndex() );
					m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), NULL );
					m_Server.SendMessage( CMessage( m_MessageRecv.GetNetworkKey(), CMessage::MessageType::CloseType ) );
				}

			}
			break;

		case CMessage::MessageType::DataType :
			{
				CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
				if( pPeer == NULL )
					break;

				if( pPeer->IsCloseWait() )
					break;

				if( IsAliveManageObject( pPeer->GetServicerIndex() ) )
				{
					m_AliveManager.RepositeOnAliveList( pPeer->GetInnerObjectForPing() );
				}

				CPacket* pPacket = (CPacket*)m_MessageRecv.GetBuffer();
				if( pPacket->GetCommand() == CPacket::PredefineCommand::Assemble )
				{
					CPacketAssemble*	pAssemblePacket = (CPacketAssemble*) pPacket;
					char*	pAssembleBuffer	= pPeer->GetAssembleBuffer();
					int		nAssemble		= pPeer->GetAssembleSize();
					if( pAssemblePacket->GetType() == CPacketAssemble::Type::More )
					{
						::memcpy( &pAssembleBuffer[nAssemble], pAssemblePacket->GetBody(), CPacket::PredefineLength::AssembleSize );
						nAssemble += CPacket::PredefineLength::AssembleSize;
					}
					else
					{
						int nLen = pAssemblePacket->GetBodySize();
						::memcpy( &pAssembleBuffer[nAssemble], pAssemblePacket->GetBody(), nLen );
						OnData( pPeer, pPeer->GetServicerIndex(), pAssembleBuffer, nAssemble + nLen );
						nAssemble = 0;
					}
					pPeer->SetAssembleSize( nAssemble );
					break;
				}

				if( CPacket::IsPredefinePacket( pPacket->GetCommand() ) )
					break;

				OnData( pPeer, pPeer->GetServicerIndex(), m_MessageRecv.GetBuffer(), m_MessageRecv.GetPacketSize() );
			}
			break;

		default :
			break;
		}
	}
}


void CSessionGate::RequestADO( CPeer* pPeer, UINT uiMessage, CDbObject* pDbObject, void * i_pArg )
{
	if( pPeer == NULL )
	{
		pPeer = &m_QueryPeer;
	}
	pDbObject->SetProperty( pPeer, uiMessage, i_pArg );
	if( pPeer->IsCloseWait() )
	{
		m_Server.FreeDbObject( pDbObject );
		return;
	}

	pPeer->IncrementRequestCount();

	m_Server.SendMessage( CMessage( CMessage::MessageType::DbReqeust, pDbObject ) );
}

void CSessionGate::RequestHttp( CPeer* pPeer, UINT uiMessage, LPCTSTR lpszAddr, LPCTSTR lpszObjectName, LPCTSTR lpszBody )
{
	if( pPeer->IsCloseWait() )
		return;

	pPeer->IncrementRequestCount();

	CRequestObject* pRequestObject = m_RequestObjectPool.Alloc();
//	char a[100];
//	sprintf( a, "ALLOC %d\n", (int)pRequestObject );
//	::OutputDebugString( a );
	CQueryBuffer* pQueryBuffer = m_QeuryBufferPool.Alloc();
	pRequestObject->SetProperty( pPeer, uiMessage, pQueryBuffer );

	/*
	m_MessageHttp.SetHttpData(
		(ULONG_PTR)pRequestObject,
		lpszAddr,
		lpszObjectName,
		lpszBody
		);
	*/
	m_Server.SendMessage( m_MessageHttp );	// ³ªÁß¿¡ ´Ù½ÃÇÒ°Í!!!!!!!!

}

void CSessionGate::RequestHttp( CPeer* pPeer, UINT uiMessage, LPCTSTR lpszURL )
{
	if( pPeer->IsCloseWait() )
		return;

	pPeer->IncrementRequestCount();

	CRequestObject* pRequestObject = m_RequestObjectPool.Alloc();

	CQueryBuffer* pQueryBuffer = m_QeuryBufferPool.Alloc();
	pRequestObject->SetProperty( pPeer, uiMessage, pQueryBuffer );
	pQueryBuffer->CopyData( lpszURL, CPacket::PredefineLength::GetQuerySize );
	m_Server.SendMessage(
		CMessage( CMessage::MessageType::HttpRequest, pRequestObject )
		);
}

void CSessionGate::RequestHttp( UINT uiMessage, LPCTSTR lpszURL )
{
//	if( pPeer->IsCloseWait() )
//		return;

	m_QueryPeer.IncrementRequestCount();

	CRequestObject* pRequestObject = m_RequestObjectPool.Alloc();

	CQueryBuffer* pQueryBuffer = m_QeuryBufferPool.Alloc();
	pRequestObject->SetProperty( &m_QueryPeer, uiMessage, pQueryBuffer );
	pQueryBuffer->CopyData( lpszURL, CPacket::PredefineLength::GetQuerySize );
	m_Server.SendMessage(
		CMessage( CMessage::MessageType::HttpRequest, pRequestObject )
		);
}

void CSessionGate::SendData( CPeer* pPeer, const char* pData, INT nDataLen )
{
	if( pPeer->IsCloseWait() )
		return;

	if( nDataLen > CPacket::PredefineLength::BasePacketSize )
	{
		int		nRest = nDataLen;
		char*	pBuff = (char*)pData;
		while( nRest )
		{
			CPacketAssemble::Type	type;
			int						nLen;
			if( nRest > CPacket::PredefineLength::AssembleSize )
			{
				type	= CPacketAssemble::Type::More;
				nLen	= CPacket::PredefineLength::AssembleSize;
			}
			else
			{
				type	= CPacketAssemble::Type::End;
				nLen	= nRest;
			}

			CPacketAssemble assemblePacket(
				type,
				&pBuff[ nDataLen - nRest ],
				nLen
				);
//			m_MessageData.SetSendPacket( pPeer->GetNetworkKey(), (void*)&assemblePacket, assemblePacket.GetLength() );
//			m_Server.SendMessage( m_MessageData );
			m_Server.SendMessage( pPeer->GetNetworkKey(), reinterpret_cast<const CHAR*>( &assemblePacket ), assemblePacket.GetLength(), pPeer->GetHandle() );

			nRest -= nLen;
		}
	}
	else
	{
//		m_MessageData.SetSendPacket( pPeer->GetNetworkKey(), (void*)pData, nDataLen );
//		m_Server.SendMessage( m_MessageData );

		m_Server.SendMessage( pPeer->GetNetworkKey(), reinterpret_cast<const CHAR*>( pData ), nDataLen, pPeer->GetHandle() );
	}
}

/*
inline void CSessionGate::SendData( CMessage& message, const char* pData, INT nDataLen, CPacketCheckSum::CheckSumType checkSumType )
{
	m_MessageData.SetCheckSumType( checkSumType );
	m_MessageData.SetSendPacket( message.GetNetworkKey(), (void*)pData, nDataLen );
	m_Server.SendMessage( m_MessageData );
}*/

void CSessionGate::CloseRequest( CPeer* pPeer )
{
	m_Server.SendMessageFlush();

//#pragma message( "===============================\r\n" )
//#pragma message( "ºñÁ¤»ó Á¾·á¸¦ À§ÇÑ ÀÓ½Ã·Î±×\r\n" )
//#pragma message( "===============================\r\n" )
//	::Dispatcher::ICONSOLE()->WriteUnsafe( "DoClose\tLowLevel\tCloseRequest\r\n" );

	pPeer->SetCloseWait();
	m_Server.SendMessage( CMessage( pPeer->GetNetworkKey(), CMessage::MessageType::DoShutdownType ) );
}

void CSessionGate::TimeoutDisconnectRequest( CPeer* pPeer )
{
//#pragma message( "===============================\r\n" )
//#pragma message( "ºñÁ¤»ó Á¾·á¸¦ À§ÇÑ ÀÓ½Ã·Î±×\r\n" )
//#pragma message( "===============================\r\n" )
//	::Dispatcher::ICONSOLE()->WriteUnsafe( "DoClose\tLowLevel\tTimeout\r\n" );

	pPeer->SetCloseWait();
	m_Server.SendMessage( CMessage( pPeer->GetNetworkKey(), CMessage::MessageType::DoCloseType ) );
}

/*
inline void CSessionGate::CloseRequest( CMessage& message )
{
	message.SetType( CMessage::MessageType::DoShutdownType );
	m_Server.SendMessage( message );
}*/

BOOL CSessionGate::StartListen( INT nServicerIndex, INT nPort, INT nListenQueue, u_long hostAddr, short family )
{
	if( !m_Server.Bind( nServicerIndex, nPort, hostAddr, family, nListenQueue ) )
		return FALSE;
	
	return m_Server.Start( nServicerIndex );
}

BOOL CSessionGate::CreateListener( INT nListenIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent )
{
	return m_Server.CreateListener( nListenIndex, nNumOfCommSockets, nAcceptPoolSize, bAcceptRecvEvent );
}

BOOL CSessionGate::CreateEnd()
{
	return m_Server.CreateEnd();
}

BOOL CSessionGate::Connect( CPeer* pPeer, INT nServicerIndex, LPCTSTR lpszIP, INT nPort )
{
	pPeer->SetServicerIndex( nServicerIndex );
	sockaddr_in ServerAddr;

	ZeroMemory( &ServerAddr, sizeof( ServerAddr ) );

	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= ::inet_addr	( lpszIP );
	ServerAddr.sin_port			= ::htons		( nPort );
	
	return m_Server.Connect( (ULONG_PTR)pPeer, (sockaddr*)&ServerAddr, sizeof( ServerAddr ) );
}

BOOL CSessionGate::RefreshPackets()
{
	if( m_Server.IsEmptyRecvMessage() )
		return FALSE;

	m_Server.RecvMessage( m_MessageRecv );
	switch( m_MessageRecv.GetType() )
	{
	case CMessage::MessageType::HttpResponse :
		{
			m_HttpResponser.AddData( m_MessageRecv.GetBuffer(), m_MessageRecv.GetPacketSize() );
		}
		break;

	case CMessage::MessageType::HttpResponseEnd :
		{
			CRequestObject* pRequestObject = (CRequestObject*)m_MessageRecv.GetNetworkKey();
			
			CPeer*	pPeer		= pRequestObject->GetPeer();
			UINT	uiMessage	= pRequestObject->GetMessage();
			m_RequestObjectPool.Free( pRequestObject );

			if( pPeer->IsCloseWait() )
			{
				m_HttpResponser.Reset();
				break;
			}

			if( m_HttpResponser.GetError() == 0 )
				OnResponse( pPeer, uiMessage, 0, m_MessageRecv.GetHttpResult(), m_HttpResponser.GetData(), m_HttpResponser.GetDataSize() );
			else
				OnResponse( pPeer, uiMessage, m_HttpResponser.GetError(), 0, NULL, 0 );

			m_HttpResponser.Reset();
		}
		break;

	case CMessage::MessageType::HttpError :
		{
			CRequestObject* pRequestObject = (CRequestObject*)m_MessageRecv.GetNetworkKey();
			
			CPeer*	pPeer		= pRequestObject->GetPeer();
			UINT	uiMessage	= pRequestObject->GetMessage();
			m_RequestObjectPool.Free( pRequestObject );

			if( pPeer->IsCloseWait() )
			{
				m_HttpResponser.Reset();
				break;
			}

			OnResponse( pPeer, uiMessage, m_MessageRecv.GetHttpResult(), 0, NULL, 0 );
			m_HttpResponser.Reset();
		}
		break;


	case CMessage::MessageType::SignalType :
		{
			CSignal	signal		= m_MessageRecv.GetSignal();

			UINT		uiMessage	= signal.GetMessage();
			ULONG_PTR	pWParam		= signal.GetWParam();
			ULONG_PTR	pLParam		= signal.GetLParam();
			ULONG_PTR	lpTimerKey	= signal.GetTimerKey();

			if( lpTimerKey == NULL )
			{
				OnSignal( uiMessage, pWParam, pLParam );
			}
			else
			{
				if( !m_KillTimerMap.Erase( lpTimerKey ) )
					OnSignal( uiMessage, pWParam, pLParam );
			}
		}
		break;


	case CMessage::MessageType::AcceptType :
		{
			INT nServicerIndex = m_Server.GetServiceIndex( m_MessageRecv.GetNetworkKey() );
			CPeer* pPeer = AllocPeer( nServicerIndex );
			if( pPeer == NULL )
			{
				m_MessageRecv.SetType( CMessage::MessageType::DoShutdownType );
				m_Server.SendMessage( m_MessageRecv );
				m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), (ULONG_PTR)pPeer );

				// ÀÓ½Ã·Î CSessionManager::OnAllocNetworkObject·Î ¿Å±ä´Ù.!!!!

				/*
				Message::CPacketCheckSum::CheckSumType		outCheckSumType;
				Message::CPacketAssembler::PacketUnitType	outPacketUnitType;
				InitListenerPacketType( nServicerIndex, outCheckSumType, outPacketUnitType );

				INT											out_nLengthFieldPos;
				Message::CPacketAssembler::LengthSizeType	out_lengthSizeType;
				Message::CPacketAssembler::LengthType		out_LengthType;
				BOOL										out_bLittleEndian;
				InitLengthFieldOfPacket( nServicerIndex, out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );

				m_Server.InitPacketType( m_MessageRecv.GetNetworkKey(), outCheckSumType, outPacketUnitType );
				m_Server.InitLengthFieldOfPacket( m_MessageRecv.GetNetworkKey(), out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );
				*/

				break;
			}

			m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), (ULONG_PTR)pPeer );
			pPeer->SetNetworkKey( m_MessageRecv.GetNetworkKey(), 0 );

			// ÀÓ½Ã·Î CSessionManager::OnAllocNetworkObject·Î ¿Å±ä´Ù.!!!!

			/*
			Message::CPacketCheckSum::CheckSumType		outCheckSumType;
			Message::CPacketAssembler::PacketUnitType	outPacketUnitType;
			InitListenerPacketType( nServicerIndex, outCheckSumType, outPacketUnitType );

			INT											out_nLengthFieldPos;
			Message::CPacketAssembler::LengthSizeType	out_lengthSizeType;
			Message::CPacketAssembler::LengthType		out_LengthType;
			BOOL										out_bLittleEndian;
			InitLengthFieldOfPacket( nServicerIndex, out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );

			m_Server.InitPacketType( m_MessageRecv.GetNetworkKey(), outCheckSumType, outPacketUnitType );
			m_Server.InitLengthFieldOfPacket( m_MessageRecv.GetNetworkKey(), out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );
			*/

			pPeer->Init();
			pPeer->SetServicerIndex( nServicerIndex );
			if( IsAliveManageObject( nServicerIndex ) )
			{
				pPeer->SetAliveManaged( TRUE );
				m_AliveManager.Add( pPeer->GetInnerObjectForPing() );
			}

			OnAccept( pPeer, nServicerIndex );

			m_Server.SetReceiveEvent( m_MessageRecv.GetNetworkKey() );
		}
		break;

	case CMessage::MessageType::ConnectType :
		{
			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
			pPeer->SetNetworkKey( m_MessageRecv.GetNetworkKey(), 0 );

			m_Server.GetServiceIndex( m_MessageRecv.GetNetworkKey() );
			INT nServicerIndex = pPeer->GetServicerIndex();

			// ÀÓ½Ã·Î CSessionManager::OnAllocNetworkObject·Î ¿Å±ä´Ù.!!!!

			/*
			Message::CPacketCheckSum::CheckSumType		outCheckSumType;
			Message::CPacketAssembler::PacketUnitType	outPacketUnitType;
			InitListenerPacketType( nServicerIndex, outCheckSumType, outPacketUnitType );

			INT											out_nLengthFieldPos;
			Message::CPacketAssembler::LengthSizeType	out_lengthSizeType;
			Message::CPacketAssembler::LengthType		out_LengthType;
			BOOL										out_bLittleEndian;
			InitLengthFieldOfPacket( nServicerIndex, out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );

			m_Server.InitPacketType( m_MessageRecv.GetNetworkKey(), outCheckSumType, outPacketUnitType );
			m_Server.InitLengthFieldOfPacket( m_MessageRecv.GetNetworkKey(), out_nLengthFieldPos, out_lengthSizeType, out_LengthType, out_bLittleEndian );
			*/

			pPeer->Init();
			if( IsAliveManageObject( nServicerIndex ) )
			{
				pPeer->SetAliveManaged( TRUE );
				m_AliveManager.Add( pPeer->GetInnerObjectForPing() );
			}

			OnConnect( pPeer, nServicerIndex );

			m_Server.SetReceiveEvent( m_MessageRecv.GetNetworkKey() );
		}
		break;

	case CMessage::MessageType::CloseType :
		{
			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
			if( pPeer == NULL )
				break;

			if( IsAliveManageObject( pPeer->GetServicerIndex() ) )
			{
				m_AliveManager.Remove( pPeer->GetInnerObjectForPing() );
			}

			OnClose( pPeer, pPeer->GetServicerIndex() );
			FreePeer( pPeer, pPeer->GetServicerIndex() );
		}
		break;

	case CMessage::MessageType::DataType :
		{
			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
			if( pPeer == NULL )
				break;

			if( pPeer->IsCloseWait() )
				break;

			CPacket* pPacket = (CPacket*)m_MessageRecv.GetBuffer();
			if( pPacket->GetCommand() == CPacket::PredefineCommand::Assemble )
			{
				CPacketAssemble* pAssemblePacket = (CPacketAssemble*) pPacket;
				char*	pAssembleBuffer	= pPeer->GetAssembleBuffer();
				int		nAssemble		= pPeer->GetAssembleSize();
				if( pAssemblePacket->GetType() == CPacketAssemble::Type::More )
				{
					::memcpy( &pAssembleBuffer[nAssemble], pAssemblePacket->GetBody(), CPacket::PredefineLength::AssembleSize );
					nAssemble += CPacket::PredefineLength::AssembleSize;
					pPeer->SetAssembleSize( nAssemble );

					return FALSE;
				}
				else
				{
					int nLen = pAssemblePacket->GetBodySize();
					::memcpy( &pAssembleBuffer[nAssemble], pAssemblePacket->GetBody(), nLen );
					OnData( pPeer, pPeer->GetServicerIndex(), pAssembleBuffer, nAssemble + nLen );
					nAssemble = 0;
					pPeer->SetAssembleSize( nAssemble );

					return TRUE;
				}
				
			}

			if( CPacket::IsPredefinePacket( pPacket->GetCommand() ) )
				return FALSE;

			if( IsAliveManageObject( pPeer->GetServicerIndex() ) )
			{
				m_AliveManager.RepositeOnAliveList( pPeer->GetInnerObjectForPing() );
			}

			OnData( pPeer, pPeer->GetServicerIndex(), m_MessageRecv.GetBuffer(), m_MessageRecv.GetPacketSize() );
		}
		break;

	default :
		break;
	}

	return TRUE;
}

BOOL CSessionGate::AddSignal( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam )
{
	if( uiMessage == 0 )
		return FALSE;
	m_Server.AddSignal( CSignal( uiMessage, wParam, lParam ) );
	return TRUE;
}

ULONG_PTR CSessionGate::AddTimerSignal( INT nTick, UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam )
{
	if( uiMessage == 0 )
		return NULL;
	return m_Server.AddTimer( nTick, CSignal( uiMessage, wParam, lParam ) );
}

VOID CSessionGate::KillTimer( ULONG_PTR lpTimerKey )
{
	m_KillTimerMap.Insert( lpTimerKey );
}

/*
void CSessionGate::FireTimer( int nTick )
{
	m_Server.AddTimer( nTick, CSignal( 0, 0, 0 ) );
}
*/

void CSessionGate::FirePacket( DWORD wParam, DWORD lParam )
{
	m_Server.FirePacket( wParam, lParam );
}

DWORD CSessionGate::GetRttTime( CPeer* pPeer )
{
	return m_Server.GetRttTime( pPeer->GetNetworkKey() );
}

