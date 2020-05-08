#include "StdAfx.h"
#include ".\SessionGate.h"

#include "..\Application\Packet.h"
using namespace Application::Packets;

using namespace GamehiSolution::Session::Gate;
using namespace GamehiSolution::Session::Message;
using namespace GamehiSolution::Session::Manage;
using namespace GamehiSolution::Application;

CSessionGate::CSessionGate(void)
{
}

CSessionGate::~CSessionGate(void)
{
}

BOOL CSessionGate::Create( CSessionManager::NetworkType networkType, CSessionManager::QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nEventQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize, INT nResponseBuffer, HWND hWindow, UINT uiMessage )
{
//	m_pAssembleBuffer	= new char[nPacketSize];
//	m_nAssemble			= 0;
//	m_nPacketSize	= nPacketSize;
	m_nPacketSize	= CPacket::PredefineLength::BasePacketSize;

	if( !m_MessageRecv.Create( m_nPacketSize ) )
		return FALSE;
	if( !m_MessageHttp.Create( m_nPacketSize ) )
		return FALSE;
	if( !m_MessageData.Create( m_nPacketSize ) )
		return FALSE;

	if( !m_HttpResponser.Create( nResponseBuffer ) )
		return FALSE;	


	m_MessageData.SetType( CMessage::MessageType::DataType );
	m_MessageHttp.SetType( CMessage::MessageType::HttpRequest	);
	m_HttpResponser.Reset();

	m_RequestObjectPool.Create( nNumOfConnectSockets );
	m_QeuryBufferPool.Create( nNumOfConnectSockets );


	return m_Server.Create( networkType, queryType, nRecvQueueSize, nSendQueueSize, nQueryQueueSize, nEventQueueSize, nNumOfListener, nNumOfConnectSockets, m_nPacketSize, hWindow, uiMessage );


	// 상속한 클래스는 아래를 구현한다.

	// CreateListener
	// CreateEnd
}

void CSessionGate::Destroy()
{
	m_MessageRecv.Destroy();
	m_MessageHttp.Destroy();
	m_MessageData.Destroy();
	m_HttpResponser.Destroy();

	m_RequestObjectPool.Destroy();
	m_QeuryBufferPool.Destroy();

	m_Server.Destroy();
}

BOOL CSessionGate::Start( INT nTickTime, INT nMaxTick, INT nTimerEventQueueSize, INT nAliveTickError, INT nAliveTimeoutCount, INT nNumOfAliveManaged )
{
	return FALSE;
}

void CSessionGate::RequestHttp( CPeer* pPeer, UINT uiMessage, LPCTSTR lpszAddr, LPCTSTR lpszObjectName, LPCTSTR lpszBody )
{
	if( pPeer->IsCloseWait() )
		return;

	CRequestObject* pRequestObject = m_RequestObjectPool.Alloc();

	CQueryBuffer* pQueryBuffer = m_QeuryBufferPool.Alloc();
	pRequestObject->SetProperty( pPeer, uiMessage, pQueryBuffer );

	m_Server.SendMessage( m_MessageHttp );	// 나중에 다시할것!!!!!!!!
}

void CSessionGate::RequestHttp( CPeer* pPeer, UINT uiMessage, LPCTSTR lpszURL )
{
	if( pPeer->IsCloseWait() )
		return;

	CRequestObject* pRequestObject = m_RequestObjectPool.Alloc();

	CQueryBuffer* pQueryBuffer = m_QeuryBufferPool.Alloc();
	pRequestObject->SetProperty( pPeer, uiMessage, pQueryBuffer );
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
			m_Server.SendMessage( pPeer->GetNetworkKey(), reinterpret_cast<const CHAR*>( &assemblePacket ), assemblePacket.GetLength() );

			nRest -= nLen;
		}
	}
	else
	{
//		m_MessageData.SetSendPacket( pPeer->GetNetworkKey(), (void*)pData, nDataLen );
//		m_Server.SendMessage( m_MessageData );
		m_Server.SendMessage( pPeer->GetNetworkKey(), reinterpret_cast<const CHAR*>( pData ), nDataLen );
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
	pPeer->SetCloseWait();
	m_Server.SendMessage( CMessage( pPeer->GetNetworkKey(), CMessage::MessageType::DoCloseType ) );
}

void CSessionGate::TimeoutDisconnectRequest( CPeer* pPeer )
{
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
//	m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), (unsigned long)pPeer );

	sockaddr_in ServerAddr;

	ZeroMemory( &ServerAddr, sizeof( ServerAddr ) );

	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= ::inet_addr	( lpszIP );
	ServerAddr.sin_port			= ::htons		( nPort );
	
	return m_Server.Connect( (unsigned long)pPeer, (sockaddr*)&ServerAddr, sizeof( ServerAddr ) );
}

BOOL CSessionGate::RefreshPackets()
{
REFRESH:
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
			unsigned long	pWParam		= signal.GetWParam();
			unsigned long	pLParam		= signal.GetLParam();

			OnSignal( uiMessage, pWParam, pLParam );
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
				m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), (unsigned long)pPeer );

				goto REFRESH;;
			}

			m_Server.SetOwnerKey( m_MessageRecv.GetNetworkKey(), (unsigned long)pPeer );
			pPeer->SetNetworkKey( m_MessageRecv.GetNetworkKey() );

			// 임시로 CSessionManager::OnAllocNetworkObject로 옮긴다.!!!!

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
//			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );

			m_Server.GetServiceIndex( m_MessageRecv.GetNetworkKey() );
			INT nServicerIndex = pPeer->GetServicerIndex();

			// 임시로 CSessionManager::OnAllocNetworkObject로 옮긴다.!!!!

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

			if( IsAliveManageObject( nServicerIndex ) )
			{
				pPeer->SetAliveManaged( TRUE );
				m_AliveManager.Add( pPeer->GetInnerObjectForPing() );
			}

			OnConnect( pPeer, nServicerIndex );

			m_Server.SetReceiveEvent( m_MessageRecv.GetNetworkKey() );
		}
		break;

	case CMessage::MessageType::BeginConnectType :
		{
			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
			pPeer->Init();
			pPeer->SetNetworkKey( m_MessageRecv.GetNetworkKey() );

			INT nServicerIndex = pPeer->GetServicerIndex();
			OnBeginConnect( pPeer, nServicerIndex );
		}
		break;

	case CMessage::MessageType::CloseType :
		{
			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
			if( pPeer == NULL )
				goto REFRESH;

			if( IsAliveManageObject( pPeer->GetServicerIndex() ) )
			{
				m_AliveManager.Remove( pPeer->GetInnerObjectForPing() );
			}

			OnClose( pPeer, pPeer->GetServicerIndex() );
			FreePeer( pPeer, pPeer->GetServicerIndex() );

			m_Server.SendMessage( CMessage( m_MessageRecv.GetNetworkKey(), CMessage::MessageType::CloseType ) );
		}
		break;

	case CMessage::MessageType::DataType :
		{
			CPeer*	pPeer = (CPeer*) m_Server.getOwnerKey( m_MessageRecv.GetNetworkKey() );
			if( pPeer == NULL )
				goto REFRESH;

			if( pPeer->IsCloseWait() )
				goto REFRESH;

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

					goto REFRESH;
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
				goto REFRESH;

			if( IsAliveManageObject( pPeer->GetServicerIndex() ) )
			{
				m_AliveManager.RepositeOnAliveList( pPeer->GetInnerObjectForPing() );
			}

			OnData( pPeer, pPeer->GetServicerIndex(), m_MessageRecv.GetBuffer(), m_MessageRecv.GetPacketSize() );
		}
		break;

	default :
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL CSessionGate::AddSignal( UINT uiMessage, unsigned long wParam, unsigned long lParam )
{
	if( uiMessage == 0 )
		return FALSE;
	m_Server.AddSignal( CSignal( uiMessage, wParam, lParam ) );
	return TRUE;
}

BOOL CSessionGate::AddTimerSignal( INT nTick, UINT uiMessage, unsigned long wParam, unsigned long lParam )
{
	if( uiMessage == 0 )
		return FALSE;
	m_Server.AddTimer( nTick, CSignal( uiMessage, wParam, lParam ) );
	return TRUE;
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

