#pragma once

//#include <map>

#include "..\NJean\Session\SessionGate.h"
#include "..\DataStructure\MemoryPool.h"
#include "User.h"
#include "Room.h"
#include "Channel.h"

#include "UserMap.h"

namespace CasualGame
{
	using namespace Session;
	class CNJeanGate : public Session::Gate::CSessionGate //½ÇÉ«ºÍroomµÄ¹ÜÀí£¬ºÍÏûÏ¢µÄ·Ö·¢
	{
	public:
		CNJeanGate();
		virtual ~CNJeanGate();

	private:
		enum	ServicerIndex
		{
			Client	= 0,
		};

	private:
		Common::DataStructure::CMemoryPool<CUser>	m_UserPool;					// »ç¿ëÀÚ °´Ã¼µéÀÇ ¸Þ¸ð¸® Ç®
		Common::DataStructure::CInnerManager<CRoom>	m_InnerManagerUnusedRoom;	// »ç¿ëÇÏÁö ¾Ê´Â ¹æ ¸®½ºÆ®
		Common::DataStructure::CInnerManager<CRoom>	m_InnerManagerUsedRoom;		// »ç¿ëÁßÀÎ ¹æ ¸®½ºÆ®
		
		CRoom*					m_aRoom;					// Room ¹è¿­
		CChannel*				m_aChannel;					// Ã¤³Î ¹è¿­
		INT						m_nAliveManaged;			// Å¸ÀÓ¾Æ¿ô Ã³¸®¸¦ ¹ÞÀ» °´Ã¼ÀÇ °¹¼ö

		BOOL*					m_baTmpRoomInnerIndex;		// m_nMaxRoomUserÅ©±â¸¸Å­ »ý¼º

		//////////////////////////////////////////////////////////////
		// SesstionGateÀÇ Method¸¦ ¼û±ä´Ù.

		virtual BOOL	Create( Manage::CSessionManager::NetworkType networkType, Manage::CSessionManager::QueryType queryType, INT nRecvQueueSize, INT nSendQueueSize, INT nQueryQueueSize, INT nSignalQueueSize, INT nNumOfListener, INT nNumOfConnectSockets, INT nPacketSize, INT nResponseBuffer, HWND hWindow, UINT uiMessage )
		{ return FALSE; }

		VOID			CloseRequest( Application::CPeer* pPeer )
		{
			CSessionGate::CloseRequest( pPeer );	
		}
		VOID			RequestHttp( Application::CPeer* pPeer, UINT uiMessage, LPCTSTR lpszURL )
		{
			CSessionGate::RequestHttp( pPeer, uiMessage, lpszURL );
		}
		VOID			RequestHttp( Application::CPeer* pPeer, UINT uiMessage, LPCTSTR lpszAddr, LPCTSTR lpszObjectName, LPCTSTR lpszBody )
		{
			CSessionGate::RequestHttp( pPeer, uiMessage, lpszAddr, lpszObjectName, lpszBody );
		}
		VOID			SendData( Application::CPeer* pPeer, const char* pData, INT nDataLength )
		{
			CSessionGate::SendData( pPeer, pData, nDataLength );
		}


		///////////////////////////////////////////////////////////////
		// Implementation

		virtual VOID		OnSignal	( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam );
		virtual VOID		OnResponse	( Application::CPeer* pPeer, UINT uiMessage, DWORD dwError, INT nResult, CHAR* pData, INT nDataSize );
		virtual VOID		OnResponse	( Application::CPeer* pPeer, UINT uiMessage, BOOL bSuccess, _CommandPtr& pCmd );
		virtual VOID		OnAccept	( Application::CPeer* pPeer, INT nServicerIndex );
		virtual VOID		OnConnect	( Application::CPeer* pPeer, INT nServicerIndex );
		virtual VOID		OnClose		( Application::CPeer* pPeer, INT nServicerIndex );
		virtual VOID		OnData		( Application::CPeer* pPeer, INT nServicerIndex, CHAR* pPacket, INT nPacketLength );
		virtual VOID		OnTimeout	( Application::CPeer* pPeer );

		virtual Application::CPeer*
							AllocPeer	( INT nServicerIndex );
		virtual VOID		FreePeer	( Application::CPeer* pPeer, INT nServicerIndex );

		virtual VOID		InitListenerPacketType( 
			INT											nServicerIndex,
			Message::CPacketCheckSum::CheckSumType&		out_CheckSumType,
			Message::CPacketAssembler::PacketUnitType&	out_PacketUnitType );

		virtual VOID		InitLengthFieldOfPacket(
			INT											nServicerIndex,
			INT&										out_nLengthFieldPos,
			Message::CPacketAssembler::LengthSizeType&	out_lengthSizeType,
			Message::CPacketAssembler::LengthType&		out_lengthType,
			BOOL&										out_bLittleEndian );

		virtual BOOL		IsAliveManageObject( INT nServicerIndex );


		////////////////////////////////////////////////////////////////
		// Operations

		/////////////////////////////////////////////////////////////////////////////////////////////
		// ½Ã±×³Î Ã³¸®
		VOID		SignalProcessChangeCaptine			( ULONG_PTR wParam, ULONG_PTR lParam );
		VOID		SignalProcessCompulseExit			( ULONG_PTR wParam, ULONG_PTR lParam );
		VOID		SignalProcessCompulseDisconnect		( ULONG_PTR wParam, ULONG_PTR lParam );


		/////////////////////////////////////////////////////////////////////////////////////////////
		// ÆÐÅ¶ Ã³¸®

		// ·Î±×ÀÎ ¿äÃ» Ã³¸®
		VOID		PacketProcessLoginRequest			( CUser* pUser, CCasualPacketLoginRequest* pCasualPacket );

		// ¾ÏÈ£È­µÈ ·Î±×ÀÎ ¿äÃ» Ã³¸®
		VOID		PacketProcessSecLoginRequest		( CUser* pUser, CCasualPacketSecLoginRequest* pCasualPacket );

		// Ã¤³Î ÀÔÀå ¿äÃ» Ã³¸®
		VOID		PacketProcessJoinChannelRequest		( CUser* pUser, CCasualPacketJoinChannelRequest* pCasualPacket );

		// Ã¤³Î ÅðÀå ¿äÃ» Ã³¸®
		VOID		PacketProcessLeaveChannelRequest	( CUser* pUser, CCasualPacketLeaveChannelRequest* pCausalPacket );

		// ¹æ °³¼³ ¿äÃ» Ã³¸®
		VOID		PacketProcessOpenRoomRequest		( CUser* pUser, CCasualPacketOpenRoomRequest* pCasualPacket );

		// ¹æ Âü¿© ¿äÃ» Ã³¸®
		VOID		PacketProcessJoinRoomRequest		( CUser* pUser, CCasualPacketJoinRoomRequest* pCasualPacket );

		// ¹æ¿¡¼­ ³ª°¡±â ¿äÃ» Ã³¸®
		VOID		PacketProcessLeaveRoomRequest		( CUser* pUser, CCasualPacketLeaveRoomRequest* pCasualPacket );

		// ¹æ¿¡ ÀÚµ¿ ÀÔÀå ¿äÃ» Ã³¸®
		VOID		PacketProcessAutoJoinRoomRequest	( CUser* pUser, CCasualPacketAutoJoinRoomRequest* pCasualPacket );

		// »ç¿ëÀÚ Ã£±â ¿äÃ» Ã³¸®
		VOID		PacketProcessFindRequest			( CUser* pUser, CCasualPacketFindRequest* pCasualPacket );

		// Ã¤ÆÃ ¸Þ½ÃÁö
		VOID		PacketProcessChat					( CUser* pUser, CCasualPacketChat* pCasualPacket );

		// ÂÊÁö
		VOID		PacketProcessMemo					( CUser* pUser, CCasualPacketMemo* pCasualPacket );

		// ÃÊ´ëÇÏ±â
		VOID		PacketProcessInvite					( CUser* pUser, CCasualPacketInvite* pCasualPacket );

		// °­Åð ¿äÃ»
		VOID		PacketProcessCompulseExitRequest	( CUser* pUser, CCasualPacketCompulsionExitRequest* pCasualPacket );

		// P2P¿¡¼­ ¹æÀåÀÌ µÉ ¼ö ÀÖ´ÂÁö¸¦ ¼³Á¤ÇÑ´Ù.
		VOID		InspectP2PCaptine				( CUser* pUser, LPCTSTR strIP );


	protected:
		////////////////////////////////////////////////////////////////////////////////////
		// Types

		// Æ÷ÄÏ Å¸ÀÔ
		enum	PocketType
		{
			User,			// »ç¿ëÀÚ Æ÷ÄÏ
			Room,			// ¹æ Æ÷ÄÏ
			Channel,		// Ã¤³Î Æ÷ÄÏ
		};

		// ÆÐÅ¶ ÄÁµð¼Ç
		enum	PacketCondition
		{
			JoinChannel,		// Ã¤³Î¿¡ ÀÔÀå
			OpenRoom,			// ¹æ °³¼³
			JoinRoom,			// ¹æ¿¡ ÀÔÀå
			AutoJoinRoom,		// ¹æ¿¡ ÀÚµ¿ Âü¿©
			LeaveRoom,			// ¹æ¿¡¼­ ÅðÀå
			ChangeCaptine,		// ¹æÀåÀ» À§ÀÓÇÒÁöÀÇ °Ë»ç
			CompulseExit,		// °­Åð½ÃÅ³ ¼ö ÀÖ´ÂÁö ¿©ºÎ¸¦ °Ë»ç
		};

		////////////////////////////////////////////////////////////////////////////////////
		// Attributes

		CUserMap				m_UserMap;					// »ç¿ëÀÚµéÀ» °Ë»öÇÏ±â À§ÇÑ ¸Ê

		INT						m_nMaxRoomUser;				// ÀÏ¹Ý Room¿¡ µé¾î°¥ ¼ö ÀÖ´Â ÃÖ´ë »ç¿ëÀÚ ¼ö
		INT						m_nMaxRoom;					// °³¼³ÇÒ ¼ö ÀÖ´Â ÃÖ´ë RoomÀÇ ¼ö
		INT						m_nNumOfClients;			// ÃÖ´ë Á¢¼Ó °¡´ÉÀÚÀÇ ¼ö
		INT						m_nMaxChannel;				// Ã¤³ÎÀÇ ÃÖ´ë °³¼ö

		INT						m_nPort;					// Æ÷Æ® ¹øÈ£

		TLibPackets*				m_pCasualPackets;			// Peerµé¿¡°Ô Àü¼ÛÇÒ ÆÐÅ¶µé


		////////////////////////////////////////////////////////////////////////////////////
		// Operations

		// »ç¿ëÀÚÀÇ Á¢¼ÓÀ» Á¾·á½ÃÅ²´Ù.
		VOID			CloseUser( CUser* pUser )
		{
			CSessionGate::CloseRequest( pUser );
		}

		// Http Äõ¸®¸¦ ¿äÃ»ÇÑ´Ù. : Get ¹æ½Ä
		VOID			Request( CUser* pUser, UINT uiMessage, LPCTSTR lpszURL )
		{
			CSessionGate::RequestHttp( pUser, uiMessage, lpszURL );
		}
		// Http Äõ¸®¸¦ ¿äÃ»ÇÑ´Ù. : Get ¹æ½Ä
		VOID			Request( UINT uiMessage, LPCTSTR lpszURL )
		{
			CSessionGate::RequestHttp( uiMessage, lpszURL );
		}

		// Http Äõ¸®¸¦ ¿äÃ»ÇÑ´Ù. : Post ¹æ½Ä
		VOID			Request( CUser* pUser, UINT uiMessage, LPCTSTR lpszAddr, LPCTSTR lpszObjectName, LPCTSTR lpszBody )
		{
			CSessionGate::RequestHttp( pUser, uiMessage, lpszAddr, lpszObjectName, lpszBody );
		}

		// ÆÐÅ¶À» Àü¼ÛÇÑ´Ù.
		VOID			SendPacket( CUser* pUser, const CPacket* pPacket, INT nPacketLength )
		{
			CSessionGate::SendData( pUser, reinterpret_cast<const char*>(pPacket), nPacketLength );
		}

		// ¹æÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
		CRoom*		GetRoomRef						( CUser*	pUser		);
		CRoom*		GetRoomRef						( INT		nRoomIndex	);

		// Ã¤³ÎÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
		CChannel*	GetChannelRef					( CUser*	pUser		);
		CChannel*	GetChannelRef					( INT		nChannelIndex );

		// ÇØ´ç ¹æÀÌ ´ë±â½ÇÀÎÁö¸¦ ¹ÝÈ¯ÇÑ´Ù.
		BOOL		IsWaitRoom						( CRoom*	pRoom		);
		BOOL		IsWaitRoom						( INT		nRoomIndex	);

		// ÇØ´ç Ã¤³ÎÀÌ ´ë±âÃ¤³ÎÀÎÁö¸¦ ¹ÝÈ¯ÇÑ´Ù.
		BOOL		IsWaitChannel					( CChannel*	pChannel	);
		BOOL		IsWaitChannel					( INT		nChannel	);

		// ÇØ´ç ¹æÀÌ ¼ÓÇÑ Ã¤³ÎÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
		CChannel*	GetChannelRefOfRoom				( CRoom*	pRoom		);
		CChannel*	GetChannelRefOfRoom				( INT		nRoomIndex	);

		// ÇØ´ç ¹æ¿¡ ¸Þ½ÃÁö¸¦ Áß°èÇÑ´Ù.
		VOID		SendToRoom						( CRoom* pRoom, const CUser* pExeptUser, const CPacket* pPacket, INT nPacketSize );

		// ¼­¹ö ³»ÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
		VOID		SendToAllUser					( const CPacket* pPacket, INT nPacketSize );

		// Ã¤³Î ³»ÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
		VOID		SendToAllUser					( CChannel* pChannel, const CPacket* pPacket, INT nPacketSize );

		// ¼­¹ö ³»ÀÇ ¸ðµç ¿­¸° ¹æ¿¡ ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
		VOID		SendToAllRoomUser				( const CPacket* pPacket, INT nPacketSize );

		// Ã¤³Î ³»ÀÇ ¸ðµç ¿­¸° ¹æ¿¡ ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
		VOID		SendToAllRoomUser				( CChannel* pChannel, const CPacket* pPacket, INT nPacketSize );

		// Ã¤³Î ³»ÀÇ ´ë±â½Ç¿¡ ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
		VOID		SendToWaitRoom					( CChannel* pChannel, const CUser* pExeptUser, const CPacket* pPacket, INT nPacketSize );

		// »ç¿ëÀÚÀÇ À§Ä¡¸¦ ¹ÝÈ¯ÇÑ´Ù.
		VOID		GetUserPlace					( CUser* pUser, CChannel* &out_pChannel, CRoom* &out_pRoom, BOOL &out_bWaitRoom );

		// ·Î±×ÀÎ ÀÀ´äÀ» Àü¼ÛÇÑ´Ù.
		VOID		LoginReply						( CUser* pUser, BOOL bLogined );

		// ·Î±×ÀÎ Äõ¸®¸¦ Àü¼ÛÇÑ´Ù.
		VOID		LoginQueryRequest				( CUser* pUser, LPCTSTR strLoginQuery );

		INT			GetUserCount					()
		{
			return m_UserPool.GetAllocatedSize();
		}

		// XMLÀ» ÆÄ½ÌÇÑ´Ù. : ¼º¿¬
		VOID		UrlParsing						( CHAR* pStr );
		CHAR		HexToChar						( CHAR* pStr );



		////////////////////////////////////////////////////////////////////////////////////
		// Operations : ÇÏÀ§Å¬·¡½º¿¡¼­ ÀçÁ¤ÀÇ ÇÏ¿© »ç¿ë

		virtual VOID		OnLoginSuccess			( CUser* pUser );
		virtual VOID		OnOpenRoomReply			( CUser* pUser, CRoom* pRoom );
		virtual VOID		OnJoinReply				( CUser* pUser, CRoom* pRoom );
		virtual VOID		OnAutoJoinReply			( CUser* pUser, CRoom* pRoom );
		virtual VOID		OnLeaveRoom				( CUser* pUser, CRoom* pRoom, CCasualPacketCompulsionExited::Type ExitType ){}
		virtual VOID		OnChangeCaptine( CUser* pCaptineUser, CRoom* pRoom );
		
		// PocketÀ» »ý¼ºÇÑ´Ù.
		virtual CPocket*	NewPocket( PocketType type )	{ return NULL; }

		// PocketÀ» Á¦°ÅÇÑ´Ù.
		virtual VOID		DeletePocket( PocketType type, CPocket* pPocket ){}

		// Ã¤³ÎÀ» ÃÊ±âÈ­ ÇÑ´Ù.
		virtual VOID		InitChannel( INT nChannelIndex, INT& out_nMaxUsers, INT& out_nMaxRooms, string& out_strChannelName ){}

		virtual VOID		OnCasualSignal		( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam ){}
		virtual VOID		OnCasualResponse	( CUser* pUser, UINT uiMessage, DWORD dwError, INT nResult, CHAR* pData, INT nDataSize ){}
		virtual VOID		OnCasualAccept		( CUser* pUser ){}
		virtual VOID		OnCasualConnect		( CUser* pUser ){}
		virtual VOID		OnCasualClose		( CUser* pUser ){}
		virtual VOID		OnCasualData		( CUser* pUser, CPacket* pPacket, INT nPacketLength ){}
		virtual BOOL		OnCasualTimeout		( CUser* pUser )
		{
			return TRUE;
		}
		virtual VOID		OnCasualResponse	( CUser* pUser, UINT uiMessage, BOOL bSuccess, _CommandPtr& pCmd ){}

		// ·Î±×ÀÎ Äõ¸® ÀÀ´ä ¹ß»ý½Ã
		virtual VOID	OnResponseLoginQuery	( CUser* pUser, DWORD dwError, INT nResult, CHAR* pData, INT nDataSize );

		// CNJeanGate¿¡¼­ Æ¯Á¤ Á¶°ÇÀ» ÇÏÀ§ Å¬·¡½º¿¡¼­ °áÁ¤ÇÒ ¶§ »ç¿ë
//			virtual BOOL	IsCorrectCondition		( CUser* pUser, PacketCondition condition, ULONG_PTR rParam = NULL, ULONG_PTR lParam = NULL )
//			{
//				return TRUE;
//			}

		virtual BOOL	IsCorrectCondition		( SHORT &out_nResult, CUser* pUser, PacketCondition condition, ULONG_PTR rParam = NULL, ULONG_PTR lParam = NULL )
		{
			return TRUE;
		}

		// ¾ÏÈ£È­ µÇ¾î ½ºÆ®¸µÇüÅÂ·Î Àü´ÞµÈ ·Î±×ÀÎ ÆÐÅ¶ ¼ö½Å
		virtual VOID	OnPacketSecLogin		( CUser* pUser, const char* pSecData, INT nDataSize, const char* pSessionKey, INT nKeyLength ){}

		// ÀÏ¹Ý ·Î±×ÀÎ ÆÐÅ¶ ¼ö½Å
		virtual VOID	OnPacketLogin			( CUser* pUser, LPCTSTR strUserNo, LPCTSTR strID, LPCTSTR strNick );

		// ¹æÀåÀÌ µÉ ¼ö ÀÖ´ÂÁö ¿©ºÎ¸¦ ¹ÝÈ¯
		virtual BOOL	CanBeCaptine			( CUser* pUser )
		{
			return pUser->CanBeGameCaptine() && pUser->CanBeP2PCaptine();
		}

		// ¹æÀåÀÌ ¾ø´Â ¹æÀÌ »ý¼ºµÉ ¶§
		virtual VOID	OnNoneCaptine			( CRoom* pRoom ){}

	public:
		// ¼­¹ö »ý¼º
		// nMaxPacketSize			: ¼ö½ÅÇÒ ÆÐÅ¶µé ÁßÀÇ ÃÖ´ë Å©±â
		// nMaxClients				: ÃÖ´ë·Î Á¢¼Ó Çã¿ëÇÒ Å¬¶óÀÌ¾ðÆ®ÀÇ °³¼ö
		// nMaxResponsebufferSize	: Http ResponseÀÇ ÃÖ´ë Å©±â
		// nMaxRoomUser				: ÇÑ ¹æ¿¡ ÀÔÀåÇÒ ¼ö ÀÖ´Â ÃÖ´ë »ç¿ëÀÚÀÇ °³¼ö : ´ë±â½Ç Á¦¿Ü
		// nMaxChannels				: Ã¤³ÎÀÇ ÃÖ´ë °³¼ö
		// nPort					: Æ÷Æ® ¹øÈ£
		virtual BOOL	Create( INT nMaxPacketSize, INT nMaxClients, INT nMaxResponseBufferSize, INT nMaxRoomUser, INT nMaxChannels, INT nPort );

		// ¼­¹ö ½ÇÇà
		virtual BOOL	Start();

		// ¼­¹ö ¼Ò¸ê
		virtual VOID	Destroy();
	};
}