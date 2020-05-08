#pragma once

#include "SessionManager.h"

#include "AliveManager.h"
#include "..\..\DataStructure\InnerManager.h"
#include "..\Application\Peer.h"
//#include "QueryBuffer.h"

#include "HttpResponser.h"

namespace GamehiSolution
{
	namespace Session
	{
		namespace Gate
		{
			class CSessionGate
			{
			public:
				CSessionGate(void);
				~CSessionGate(void);

			protected:
				Manage::CSessionManager	m_Server;
				Message::CMessage		m_MessageHttp;			// Http Request Type으로만 쓰이는 Message
				Message::CMessage		m_MessageData;			// Data Send Type으로만 쓰이는 Message
				Message::CMessage		m_MessageRecv;			// Recv용 Message
				CHttpResponser			m_HttpResponser;
				INT						m_nPacketSize;			// Packet의 크기

			private:
				CAliveManager<Application::CPeer>
										m_AliveManager;

				Common::DataStructure::CMemoryPoolSafty< Manage::CRequestObject >
										m_RequestObjectPool;
				Common::DataStructure::CMemoryPoolSafty< Manage::CQueryBuffer >
										m_QeuryBufferPool;			// Query 버퍼 풀

			public:
				virtual BOOL	Create( Manage::CSessionManager::NetworkType networkType, Manage::CSessionManager::QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nSignalQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize, INT nResponseBuffer, HWND hWindow = NULL, UINT uiMessage = 0 );
				virtual BOOL	Start( INT nTickTime, INT nMaxTick, INT nTimerEventQueueSize, INT nAliveTickError, INT nAliveTimeoutCount, INT nNumOfAliveManaged );
				virtual void	Destroy();

//				void			FireTimer( int nTick );
				void			FirePacket( DWORD wParam, DWORD lParam );

				// Do Implementation
			protected:
				virtual void	OnSignal	( UINT uiMessage, unsigned long wParam, unsigned long lParam ){}
				virtual void	OnResponse	( Application::CPeer* pPeer, UINT uiMessage, DWORD dwError, INT nResult, char* pData, int nDataSize ){}
				virtual void	OnAccept	( Application::CPeer* pPeer, INT nServicerIndex ){}
				virtual void	OnConnect	( Application::CPeer* pPeer, INT nServicerIndex ){}
				virtual void	OnBeginConnect	( Application::CPeer* pPeer, INT nServicerIndex ){}
				virtual void	OnClose		( Application::CPeer* pPeer, INT nServicerIndex ){}
				virtual void	OnData		( Application::CPeer* pPeer, INT nServicerIndex, char* pPacket, int nPacketLength ){}
				virtual void	OnTimeout	( Application::CPeer* pPeer ){}

				virtual Application::CPeer*
								AllocPeer( INT nListenIndex ){ return NULL; }
				virtual void	FreePeer( Application::CPeer* pPeer, INT nListenIndex ){}

				virtual void	InitListenerPacketType( 
					INT											nServicerIndex,
					Message::CPacketCheckSum::CheckSumType&		out_CheckSumType,
					Message::CPacketAssembler::PacketUnitType&	out_PacketUnitType ){}

				virtual void	InitLengthFieldOfPacket(
					INT											nServicerIndex,
					INT&										out_nLengthFieldPos,
					Message::CPacketAssembler::LengthSizeType&	out_lengthSizeType,
					Message::CPacketAssembler::LengthType&		out_lengthType,
					BOOL&										out_bLittleEndian ){}

				virtual BOOL	IsAliveManageObject( INT nServicerIndex ){ return TRUE; }

				void	RequestHttp( Application::CPeer* pPeer, UINT uiMessage, LPCTSTR lpszAddr, LPCTSTR lpszObjectName, LPCTSTR lpszBody );
				void	RequestHttp( Application::CPeer* pPeer, UINT uiMessage, LPCTSTR lpszURL );
				void	SendData( Application::CPeer* pPeer, const char* pData, INT nDataLen );
				void	CloseRequest( Application::CPeer* pPeer );
				void	TimeoutDisconnectRequest( Application::CPeer* pPeer );

				BOOL	StartListen( INT nServicerIndex, INT nPort, INT nListenQueue = 200, u_long hostAddr = INADDR_ANY, short family = AF_INET );
				BOOL	CreateListener( INT nListenIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE );
				BOOL	CreateEnd();

				BOOL	AddSignal( UINT uiMessage, unsigned long wParam = 0, unsigned long lParam = 0 );
				BOOL	AddTimerSignal( INT nTick, UINT uiMessage, unsigned long wParam, unsigned long lParam );

				BOOL	Connect( Application::CPeer* pPeer, INT nServicerIndex, LPCTSTR lpszIP, INT nPort );

				DWORD	GetRttTime( Application::CPeer* pPeer );

				LPCTSTR	GetLocalAddr( Application::CPeer* pPeer )
				{
					return m_Server.GetLocalAddr( pPeer->GetNetworkKey() );
				}

				LPCTSTR	GetPeerAddr( Application::CPeer* pPeer )
				{
					return m_Server.GetPeerAddr( pPeer->GetNetworkKey() );
				}

			public:
				BOOL	RefreshPackets();
			};// class CSessionGate

		}// namespace Gate

	}// namespace Session
}// namespace GamehiSolution