#pragma once

#include "SessionManager.h"

#include "AliveManager.h"
#include "..\..\DataStructure\InnerManager.h"
#include "..\Application\Peer.h"
#include "KillTimerMap.h"
//#include "QueryBuffer.h"

#include "HttpResponser.h"

#include "..\..\..\SuddenAttack\ServerInfo.h"		// added by minjin.

namespace Session
{
	namespace Gate
	{
		class CSessionGate //网络消息、http消息、数据库消息的响应和处理
		{
		public:
			CSessionGate(void);
			~CSessionGate(void);

		protected:
			Manage::CSessionManager	m_Server;
			Message::CMessage		m_MessageHttp;			// 获取的http消息包，RequestHttp调用，目前没有用到
			Message::CMessage		m_MessageData;			// 发送消息包，目前也没有意义
			Message::CMessage		m_MessageRecv;			// 接受消息包
			CHttpResponser			m_HttpResponser;		// http响应消息包
			INT						m_nPacketSize;			// Packet狼 农扁

		private:
			CAliveManager<Application::CPeer>
									m_AliveManager;

			Common::DataStructure::CMemoryPoolSafty< Manage::CRequestObject >
									m_RequestObjectPool;
			Common::DataStructure::CMemoryPoolSafty< Manage::CQueryBuffer >
									m_QeuryBufferPool;			// Query 滚欺 钱

			CKillTimerMap			m_KillTimerMap;

			Application::CPeer		m_QueryPeer;

		public:
			virtual BOOL	Create( INT nTickTime, INT nMaxTick, INT nTimerEventQueueSize, INT nAliveTickError, INT nAliveTimeoutCount, INT nNumOfAliveManaged, Manage::CSessionManager::NetworkType networkType, Manage::CSessionManager::QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nSignalQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize, INT nResponseBuffer, HWND hWindow = NULL, UINT uiMessage = 0 );
			virtual BOOL	Start( INT nAliveTickError );
			virtual void	Destroy(){}

			//INT	CreateDatabase( LP_DB_INFO pDBInfo )
			//{
			//	return CreateDatabase(	pDBInfo->nAdoConn,
			//							pDBInfo->szDataSource,
			//							pDBInfo->szCatalog,
			//							pDBInfo->szAccID,
			//							pDBInfo->szAccPW );
			//}

			INT	CreateDatabase( INT nDbConnectionCount, LPCTSTR lpszConnection )
			{
				return m_Server.CreateDatabase( nDbConnectionCount, lpszConnection );
			}

//				void			FireTimer( int nTick );
			void			FirePacket( DWORD wParam, DWORD lParam );

			// Do Implementation
		protected:
			virtual void	OnSignal	( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam ){}
			virtual void	OnResponse	( Application::CPeer* pPeer, UINT uiMessage, DWORD dwError, INT nResult, char* pData, int nDataSize ){}
			virtual VOID	OnResponse	( Application::CPeer* pPeer, Manage::CDbObject * i_pDbObject ){}
			virtual void	OnAccept	( Application::CPeer* pPeer, INT nServicerIndex ){}
			virtual void	OnConnect	( Application::CPeer* pPeer, INT nServicerIndex ){}
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

			void	RequestADO( Application::CPeer* pPeer, UINT uiMessage, Manage::CDbObject* pDbObject, void * i_pArg );
			Manage::CDbObject*	AllocDbObject()
			{
				return m_Server.AllocDbObject();
			}
			void		FreeDbObject( Manage::CDbObject* pDb )
			{
				m_Server.FreeDbObject( pDb );
			}

			void	RequestHttp( Application::CPeer* pPeer, UINT uiMessage, LPCTSTR lpszAddr, LPCTSTR lpszObjectName, LPCTSTR lpszBody );
			void	RequestHttp( Application::CPeer* pPeer, UINT uiMessage, LPCTSTR lpszURL );
			void	RequestHttp( UINT uiMessage, LPCTSTR lpszURL );
			void	SendData( Application::CPeer* pPeer, const char* pData, INT nDataLen );
			void	CloseRequest( Application::CPeer* pPeer );
			void	TimeoutDisconnectRequest( Application::CPeer* pPeer );

			BOOL	StartListen( INT nServicerIndex, INT nPort, INT nListenQueue = 200, u_long hostAddr = INADDR_ANY, short family = AF_INET );
			BOOL	CreateListener( INT nListenIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE );
			BOOL	CreateEnd();

			BOOL		AddSignal( UINT uiMessage, ULONG_PTR wParam = 0, ULONG_PTR lParam = 0 );
			ULONG_PTR	AddTimerSignal( INT nTick, UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam );
			VOID		KillTimer( ULONG_PTR lpTimerKey );

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
