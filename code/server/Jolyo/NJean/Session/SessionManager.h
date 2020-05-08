/*****************************************************************************************************************
	CSessionManager

	ÀÛ¼ºÀÚ : °­ÁøÇù
	ÀÛ¼ºÀÏ : 2004³â 3¿ù 4 ~ 5ÀÏ

	¼³  ¸í :
		IOCP¸¦ ÀÌ¿ëÇÑ Server¸¦ »ó¼Ó
		OnReceive ÀÌº¥Æ®¸¦ ¸Ş½ÃÁö ´ÜÀ§°¡ ¾Æ´Ñ, ÁöÁ¤ÇÑ ÆĞÅ¶ ´ÜÀ§·Î È£Ãâ
	»ç  ¿ë :
		- CIocp
		1. Create¸¦ »ç¿ëÇÏ¿© Iocp Server¸¦ »ı¼ºÇÑ´Ù.
		2. Bind¿Í ListenÀ» ÇÑ´Ù. ÀÌ °ªÀº ±âº» ¼ÒÄÏ ÇÁ·Î±×·¥°ú µ¿ÀÏÇÏ´Ù.
		3. Start¸¦ ÅëÇØ¼­ Iocp Server¸¦ ¼öÇà½ÃÅ²´Ù.
		4. Iocp Server´Â ¼ÒÄÏ ÇÚµé·ÎºÎÅÍ OnAccept, OnReceive, OnSend, OnClose ÀÌº¥Æ®¸¦ ¹ß»ı½ÃÅ²´Ù.
		5. Send¸¦ ÅëÇØ¼­ Iocp Server¿¡ ¿¬°áµÇ¾î ÀÖ´Â Å¬¶óÀÌ¾ğÆ®¿¡°Ô ¸Ş½ÃÁö¸¦ Àü´ŞÇÑ´Ù.
		6. CloseSocketÀ» ÅëÇØ¼­ Iocp Server¿¡ ¿¬°áµÇ¾î ÀÖ´Â Æ¯Á¤ ¼ÒÄÏÀ» ´İ´Â´Ù.
		7. ¸ğµÎ »ç¿ëÇÑ µÚ¿¡´Â Destroy¸¦ ÅëÇØ ³»ºÎ ¸®¼Ò½º¸¦ Á¦°Å¸¦ ÇÑ´Ù.

		- CSessionManager
		1. CIocpÀÇ 3¹ø Start¸¦ ÇÏ±âÀü¿¡ ÆĞÅ¶¿¡ °ü·ÃµÈ ÃÊ±âÈ­¸¦ ÇØÁØ´Ù.
*****************************************************************************************************************/

#pragma once

//#include <Windows.h>
#ifndef CLIENT_MODULE
	#include "..\Network\TcpIocp.h"
#else
	#include "..\Network\TcpAsyncSelect.h"
#endif
#include "..\Network\Network.h"

#include "..\..\DataStructure\MemoryPoolSafty.h"

#include "PacketAssembler.h"
#include "MessageQueue.h"
#include "Timer.h"
#include "SendPacket.h"

#include "PacketBuffer.h"
#include "QueryBuffer.h"

#include "..\Query\IQueryHandler.h"

#ifndef CLIENT_MODULE
	#include "..\Query\AsyncInet.h"
	#include "../Query/SyncInet.h"
#endif


namespace Session{		namespace Manage{

	class CSessionManager : public ITimerHandler, Query::IQueryHandler, Network::ITcpNetworkHandler
	{
		// »ı¼º/¼Ò¸êÀÚ
	public:
		CSessionManager();
		~CSessionManager(void);

	public:
		enum	NetworkType			//ÍøÂçµ×²ãµÄ·½Ê½
		{
			TCP_IOCP			= 0,
			TCP_ASYNCSELECT		= 1,
		};
		enum	QueryType			//Query·½Ê½
		{
			HTTP		= 1,
			ADO,
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
			ULONG_PTR		pSessionOwner;
		};

		struct	TagPackets
		{
			CPacketPingAck	PingAck;
			CPacketAck		Ack;
		};

	private:
		struct DbArgument
		{
			CSessionManager * pThis;
			INT nIndex;
		};


		TagPackets*		m_pPackets;

		Common::DataStructure::CMemoryPoolSafty<Message::CPacketAssembler>
									m_PacketAssemblerPool;		// ·¢ËÍÏûÏ¢µÄ»º³å³Ø
		INT							m_nNumOfPacketAssembler;	// ·şÎñÆ÷µÄ³ĞÔØÉè¼Æ,


		//		CRecvPacketQueue		m_RecvPacketQueue;	// Recv ÆĞÅ¶¸¦ Á÷·ÄÈ­ ½ÃÅ°´Â Å¥
		Message::CMessageQueue		m_RecvMessageQueue;		// Recv ÆĞÅ¶¸¦ Á÷·ÄÈ­ ½ÃÅ°´Â Å¥

		// SendPacketQueue´Â µû·Î ¸¸µé¾î¾ß ÇÑ´Ù. 
		Message::CMessageQueue		m_SendMessageQueue;		// Send ÆĞÅ¶¸¦ Á÷·ÄÈ­ ½ÃÅ°´Â Å¥

		// Query Queue
		Message::CMessageQueue		m_QueryMessageQueue;	// httpÏûÏ¢
		// Response Query Queue
		Message::CMessageQueue		m_ResponseMessageQueue;	// Response Ã»ÓĞÓÃµ½

		Message::CMessageQueue		m_DbReqeustMessageQueue;	// Db Request ¸Ş½ÃÁö Å¥
		Message::CMessageQueue		m_DbResponseMessageQueue;	// Db Response ¸Ş½ÃÁö Å¥
		Message::CMessageQueue		m_DbConnMessageQueue;		// Db Connection ¸Ş½ÃÁö Å¥

		Message::CMessage			m_SendMessage;			// Dequeue½Ã Send¸¦ À§ÇÑ ÀÓ½Ã ÆĞÅ¶
		Message::CMessage			m_SendManagePacket;		// Manage PakcetÀ» À§ÇÑ ÀÓ½Ã Send ÆĞÅ¶
		Message::CMessage			m_HttpMessage;			// Http¸¦ À§ÇÑ ÀÓ½Ã ÆĞÅ¶
		int							m_nPacketSize;

		CRITICAL_SECTION			m_csSend;				// Send µ¿±âÈ­¸¦ À§ÇÑ °´Ã¼
		CRITICAL_SECTION			m_csHttp;				// Http µ¿±âÈ­¸¦ À§ÇÑ °´Ã¼

		Common::DataStructure::CMemoryPoolSafty<CSendPacket>
			m_SendPacketPool;		// ±¸ÓÃSendÄÚ´æ³Ø

		Common::DataStructure::CListEx<CPacketBuffer*>*
			m_pPacketBufferList;	//µ±Ç°socket¶ÔÓ¦µÄĞèÒª·¢ËÍµÄÒ»×éÏûÏ¢°ü

		Common::DataStructure::CMemoryPoolSafty< Common::DataStructure::CListEx<CPacketBuffer*> >
									m_PacketBufferListPool;		// ÆĞÅ¶ ¹öÆÛ ¸®½ºÆ® Ç®
		ULONG_PTR					m_lpRecentNetworkKey;

		Common::DataStructure::CMemoryPoolSafty<CPacketBuffer>
									m_RecvPacketBufferPool;		// Recv ÆĞÅ¶ ¹öÆÛ Ç®
		Common::DataStructure::CMemoryPoolSafty<CPacketBuffer>
									m_SendPacketBufferPool;		// Send »º³å

		Common::DataStructure::CMemoryPoolSafty<CDbObject>
									m_DbObjectPool;
		CDbConnObject*				m_paDbConn;
		struct TagDbConn
		{
			_bstr_t		clsServer;
			_bstr_t		clsID;
			_bstr_t		clsPass;
		}m_dbConnInfo;

		CTimer						m_Timer;

		Network::INetwork*			m_pNetwork;

		BOOL						m_bFirstRecvMsg;
		Message::CMessage			m_OldMessage;


//		int		m_nDebug;
//		int		m_nDebugAssem;

#ifndef CLIENT_MODULE
		Query::HTTP::CAsyncInet		m_InetHttp;
		Query::HTTP::CSyncInet		m_SyncInet1;
		Query::HTTP::CSyncInet		m_SyncInet2;
#endif

		// Operations
	public:
		// ÆĞÅ¶À» Àü¼ÛÇÑ´Ù.
		VOID	SendMessage( Message::CMessage& message );
		VOID	SendMessage( ULONG_PTR pNetworkKey, const CHAR* pData, INT nDataSize, DWORD dwHandle );
		VOID	SendMessageFlush();

		// ÆĞÅ¶À» ¼ö½ÅÇÑ´Ù. // Blocking
		void	RecvMessage( Message::CMessage& outMessage );

		BOOL	IsEmptyRecvMessage()
		{
			return m_RecvMessageQueue.IsEmpty();
		}

		// Å¸ÀÌ¸Ó¸¦ µ¿ÀÛ½ÃÅ²´Ù.
		void		StartTimer( int nTickTime, int nMaxTickCount, int nSize );

		// Å¸ÀÌ¸Ó¸¦ Ãß°¡ÇÑ´Ù.
		ULONG_PTR	AddTimer( int nTickCount, const Message::CSignal& signal );

		// ÀÌº¥Æ®¸¦ Ãß°¡ÇÑ´Ù.
		void	AddSignal( const Message::CSignal& signal );

		// Accept¹× ConnectÀÇ ÃÊ±â ¼³Á¤À» ¸¶Ä¡¸é Receive¸¦ ¼ö½ÅÇÏ±â À§ÇØ¼­ ÀÌº¥Æ®¸¦ È£ÃâÇÑ´Ù.
		void	SetReceiveEvent( ULONG_PTR lpNetworkKey );

		// ÆĞÅ¶ Assembler¸¦ ÃÊ±âÈ­ÇÑ´Ù.
		// ¿¬°áµÇ´Â Sessionº°·Î ÆĞÅ¶ Å¸ÀÔÀÌ ´Ù¸¦ °æ¿ì¿¡´Â OnAccept, OnConnect ½Ã¿¡ ¾Æ·¡ ÇÔ¼ö¸¦ È£ÃâÇÑ´Ù.
		void	InitPacketType( 
			ULONG_PTR							lpCompletionKey,
			Message::CPacketCheckSum::CheckSumType		checkSumType,
			Message::CPacketAssembler::PacketUnitType	packetUnitType );
		void	InitLengthFieldOfPacket(
			ULONG_PTR lpCompletionKey,
			int nLengthFieldPos,
			Message::CPacketAssembler::LengthSizeType lengthSizeType,
			Message::CPacketAssembler::LengthType lengthType = Message::CPacketAssembler::LengthType::Total,
			BOOL bLittleEndian = TRUE );

		// ÀçÁ¤ÀÇ : CIocpServer
	public:
		// ³»ºÎ ¸®¼Ò½º¸¦ »ı¼ºÇÑ´Ù.
		BOOL	Create( NetworkType networkType, QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nEventQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize = 256, HWND hWindow = NULL, UINT uiMessage = 0 );
		BOOL	CreateListener( INT nListenIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE );
		BOOL	CreateEnd();


		INT		CreateDatabase( INT nDbConnectionCount, LPCTSTR lpszConnection );

		CDbObject*	AllocDbObject();
		void		FreeDbObject( CDbObject* pDb );

		// ³»ºÎ ¸®¼Ò½º¸¦ Á¦°ÅÇÑ´Ù.
		void	Destroy();

		// ÇØ´ç Å°ÀÇ Service ÀÎµ¦½º¸¦ °¡Á®¿Â´Ù.
		INT		GetServiceIndex( ULONG_PTR lpCompletionKey );
		void	SetOwnerKey( ULONG_PTR lpNetworkKey, ULONG_PTR lpOwnerKey )
		{
			m_pNetwork->SetOwnerKey( lpNetworkKey, lpOwnerKey );
		}
		ULONG_PTR	getOwnerKey( ULONG_PTR lpNetworkKey )
		{
			return m_pNetwork->GetOwnerKey( lpNetworkKey );
		}

		BOOL	Start( INT nServicerIndex )
		{
			return m_pNetwork->StartListen( nServicerIndex );
		}

		BOOL	Connect( ULONG_PTR lpOwnerKey, const sockaddr* addr, INT namelen )
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

		DWORD	GetRttTime( ULONG_PTR lpNetworkKey )
		{
			DWORD dwRTT;
			m_pNetwork->GetRttTime( lpNetworkKey, dwRTT );
			return dwRTT;
		}

		LPCTSTR	GetLocalAddr( ULONG_PTR lpNetworkKey )
		{
			return m_pNetwork->GetLocalAddr( lpNetworkKey );
		}

		LPCTSTR	GetPeerAddr( ULONG_PTR lpNetworkKey )
		{
			return m_pNetwork->GetPeerAddr( lpNetworkKey );
		}

		// Implementaion
	private:
		// ITimerHandler
		virtual void	OnTimer( const Message::CSignal& signal );

		// IAsyncInetHandler
		virtual void	OnRecvInet		( ULONG_PTR lpInetKey, ULONG_PTR lpContext );
		virtual void	OnErrorInet		( ULONG_PTR lpContext, DWORD nError );

		// ITcpNetworkHandler
		virtual void	OnAccept	( ULONG_PTR lpNetworkKey );
		virtual void	OnConnect	( ULONG_PTR lpNetworkKey );
		virtual void	OnReceive	( ULONG_PTR lpNetworkKey, char* pRecvBuff, int nReceivedLenth );
		virtual void	OnSend		( ULONG_PTR lpNetworkKey, char* pRecvBuff, int nSenedLenth );
		virtual void	OnClose		( ULONG_PTR lpNetworkKey );

		// connectºÍacceptÊ±×¢²á
		virtual void	OnAllocNetworkObject	( ULONG_PTR lpCompletionKey );
		// ´íÎó»ò¹Ø±ÕÊ±µ÷ÓÃ
		virtual void	OnFreeNetworkObject		( ULONG_PTR lpCompletionKey );
		//·¢ËÍÏß³Ì»òpinµÄ¼°Ê±ÏûÏ¢Ê±µ÷ÓÃ
		void			SendPacket	( Session::Message::CMessage& message );
		void			SendPacket	( ULONG_PTR lpNetworkKey, CPacketBuffer* pPacketBuffer );

	private:
		static	DWORD __stdcall SendThreadStartingPoint(PVOID pvParam);
		static	DWORD __stdcall QueryThreadStartingPoint1(PVOID pvParam);
		static	DWORD __stdcall QueryThreadStartingPoint2(PVOID pvParam);

		static	DWORD __stdcall DbThreadStartingPoint(PVOID pvParam);
	};// class CSessionManager

}/* namespace Manage */		}/* namespace Session */