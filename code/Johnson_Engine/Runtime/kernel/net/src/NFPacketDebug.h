// [NOVICE] BEGIN ---------------
#ifndef __NOVICE_IN_ENGINE_PACKET_DEBUG_HEADER__
#define __NOVICE_IN_ENGINE_PACKET_DEBUG_HEADER__

#define NF_PARSE_OUTPUT_CONSOLE	1<<0		// 1
#define NF_PARSE_UDP_LAYER		1<<1		// 2
#define NF_PARSE_GUARANTEED		1<<2		// 4
#define NF_PARSE_UNGUARANTEED	1<<3		// 8
#define NF_PARSE_RECV			1<<4		// 16
#define NF_PARSE_SEND			1<<5		// 32

#define NF_MAX_SERVER_PACKET_CNT	32
#define NF_MAX_CLIENT_PACKET_CNT	32

#define NF_MAX_MESSAGE_CNT			256


#define SA_NETMONITORING

//////////////////////////////////////////////////////////////////////////
// Final 에서도 네트웍 모니터링 하려면 #undef 하는거를 주석처리 하면 된다
#ifdef _FINAL
	#undef SA_NETMONITORING
#endif

//////////////////////////////////////////////////////////////////////////


extern uint32	g_CV_ParseRecv;
extern uint32	g_CV_ParseSend;
extern uint32	g_CV_SavePacketStaistics;

class NFPacketDebuger
{
public:
	NFPacketDebuger();
	~NFPacketDebuger();

	enum EN_MESSAGE_TYPE
	{
		EMT_NORMAL				= 0,
		EMT_SFX					= 1,
		EMT_UNGUARANTEED		= 2,
		EMT_YOURID				= 3,
		EMT_LOADWORLD			= 4,
		EMT_UNLOADWORLD			= 5,
		EMT_UPDATE				= 6,
		EMT_PACKETGROUP			= 7,
		EMT_CONSOLEVAR			= 8,
		EMT_SKYDEF				= 9,
		EMT_GLOBALLIGHT			= 10,
		EMT_CLIENTOBJECTID		= 11,
		EMT_PRELOADLIST			= 12,
		EMT_NETPROTOCOLVERSION	= 13,
		EMT_THREADLOAD			= 14,
		EMT_UNLOAD				= 15,
		EMT_CHANGE_CHILDMODEL	= 16,
		EMT_TOTAL			// 항상 맨 마지막에 위치해야함!
	};

	enum EN_RECV_MESSAGE_TYPE
	{
		ERMT_GOODBYE			= 0,
		ERMT_UPDATE				= 1,
		ERMT_SOUNDUPDATE		= 2,
		ERMT_COMMANDSTRING		= 3,
		ERMT_MESSAGE			= 4,
		ERMT_CONNECTSTAGE		= 5,
		ERMT_HELLO				= 6,
		ERMT_TEST				= 7,
		ERMT_TOTAL			// 항상 맨 마지막에 위치해야함!
	};

	typedef struct _PacketInfo
	{
		char szMessageIDName[64];
		char szMessageHandledFnName[64];
		uint32 nHandledCnt;
	} PacketInfo, *LPPacketInfo;

	typedef struct _MessageInfo
	{
		DWORD	dwMessageType;
		DWORD	dwMessageID;
		DWORD	dwMessageSize;
		BOOL	bSend;

		_MessageInfo()
		{
			dwMessageType	= 0;
			dwMessageID		= 0;
			dwMessageSize	= 0;
			bSend			= 0;
		}
	} MessageInfo, *LPMessageInfo;



	static NFPacketDebuger* GetInstance()
	{
		if ( NULL == m_pInstance )
		{
			m_pInstance = new NFPacketDebuger();
		}

		return m_pInstance;
	}

	void Destroy()
	{
		if ( NULL != m_pInstance )
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	void NFParseUDPRecv( const CPacket_Read& cPacket );
	void NFParseRecv( uint8 nTravelDir, const CPacket_Read& cPacket );
	void NFParseSend( uint8 nTravelDir, const CPacket_Read& cPacket );

	void AddPlayer();
	void RemovePlayer();

	void SaveStatistics();

	void SendToNetworkMonitor( char* pMessage, int nLength, BOOL bSend = TRUE );
	
protected:
	static NFPacketDebuger* m_pInstance;

	LPPacketInfo m_pReceivedFromServerMsgInfo;
	LPPacketInfo m_pReceivedFromClientMsgInfo;
	LPPacketInfo m_pSentMsgInfo;
	LPPacketInfo m_pMessageInfo;

	uint8 m_nPlayerNum;
	COPYDATASTRUCT m_copyData;

	HWND m_hReportWnd;

	void Output( char* szMessage );

	void ParseDetail_SMSG_UPDATE( const CPacket_Read& cUpdatePacket );
	void ParseDetail_SMSG_MESSAGE( const CPacket_Read& cMessagePacket );
	void ParseDetail_SMSG_INSTANTSPECIALEFFECT( const CPacket_Read& cSFXPacket );
	void ParseDetail_SMSG_UNGUARANTEEDUPDATE( const CPacket_Read& cUnguaranteedPacket );
	void ParseDetail_SMSG_YOURID( const CPacket_Read& cYourIDPacket );
	void ParseDetail_SMSG_LOADWORLD( const CPacket_Read& cLoadWorldPacket );
	void ParseDetail_SMSG_UNLOADWORLD( const CPacket_Read& cUnloadWorldPacket );
	void ParseDetail_SMSG_PACKETGROUP( const CPacket_Read& cPacketGroupPacket );
	void ParseDetail_SMSG_CONSOLEVAR( const CPacket_Read& cConsoleVarPacket );
	void ParseDetail_SMSG_SKYDEF( const CPacket_Read& cSkyDefPacket );
	void ParseDetail_SMSG_GLOBALLIGHT( const CPacket_Read& cGlobalLightPacket );
	void ParseDetail_SMSG_CLIENTOBJECTID( const CPacket_Read& cClientObjectIDPacket );
	void ParseDetail_SMSG_PRELOADLIST( const CPacket_Read& cPreloadListPacket );
	void ParseDetail_SMSG_NETPROTOCOLVERSION( const CPacket_Read& cNetProtocolVerPacket );
	void ParseDetail_SMSG_THREADLOAD( const CPacket_Read& cThreadLoadPacket );
	void ParseDetail_SMSG_UNLOAD( const CPacket_Read& cUnloadPacket );
	void ParseDetail_SMSG_CHANGE_CHILDMODEL( const CPacket_Read& cChangeChildModelPacket );
	  

	void ParseDetail_CMSG_HELLO( const CPacket_Read& cHelloPacket );
	void ParseDetail_CMSG_GOODBYE( const CPacket_Read& cGoodByePacket );
	void ParseDetail_CMSG_UPDATE( const CPacket_Read& cUpdatePacket );
	void ParseDetail_CMSG_SOUNDUPDATE( const CPacket_Read& cSoundUpdatePacket );
	void ParseDetail_CMSG_CONNECTSTAGE( const CPacket_Read& cConnectStagePacket );
	void ParseDetail_CMSG_COMMANDSTRING( const CPacket_Read& cCommandStringPacket );
	void ParseDetail_CMSG_MESSAGE( const CPacket_Read& cMessagePacket );
	void ParseDetail_CMSG_TEST( const CPacket_Read& cTestPacket );

	void InitializePacketInfo();
	void InitializeMessageInfo();
	void AddPacketInfo( uint8 nDir, uint32 nID, const char* szMessageIDName, const char* szMessageHandledFnName );

	inline LPSTR ConcatenateDetail( char* szBuf, const char* szMessage, uint32 nLevel )
	{
		if ( g_CV_ParseRecv & nLevel || g_CV_ParseSend & nLevel )
		{
			return lstrcat( szBuf, szMessage );
		}
		
		return "";
	}

};



#endif //__NOVICE_IN_ENGINE_PACKET_DEBUG_HEADER__
// [NOVICE] END -----------------
