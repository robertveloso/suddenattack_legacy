#include "bdefs.h"
#include "sysudpdriver.h"
#include "sysdebugging.h"
#include "netmgr.h"
#include "dsys.h"
#include "conparse.h"
#include "systimer.h"

#ifndef DE_SERVER_COMPILE
#include "ConsoleCommands.h"
#endif // DE_SERVER_COMPILE

#ifdef _WIN32
#pragma comment(lib, "wsock32.lib")
#endif

#define MURSUM_NET_TRACE

#ifdef _FINAL	// _FINAL 에서는 네트웍 트레이스 멈춤.. by minjin.
	#undef MURSUM_NET_TRACE
#endif	// _FINAL

//=============================================================================
#ifdef MURSUM_NET_TRACE

	static HWND g_hDestWnd = NULL;

	#define IPC_NET_SEND_BYTE	0x00000001
	#define IPC_NET_RECV_BYTE	0x00000002

	typedef struct _tagIPC_HEAD
	{
		DWORD	dwCommand;
	} T_IPC_HEAD, *T_IPC_HEADptr;

	typedef struct _tagIPC_SEND_BYTE : T_IPC_HEAD
	{
		int				nSendByte;
		unsigned char	nAddr1;
		unsigned char	nAddr2;
		unsigned char	nAddr3;
		unsigned char	nAddr4;
	} T_IPC_SEND_BYTE, *T_IPC_SEND_BYTEptr;

	typedef struct _tagIPC_RECV_BYTE : T_IPC_HEAD
	{
		int				nRecvByte;
		unsigned char	nAddr1;
		unsigned char	nAddr2;
		unsigned char	nAddr3;
		unsigned char	nAddr4;
	} T_IPC_RECV_BYTE, *T_IPC_RECV_BYTEptr;

	T_IPC_SEND_BYTE	g_IPC_SendByte;
	T_IPC_RECV_BYTE	g_IPC_RecvByte;
	COPYDATASTRUCT g_CopyDataStruct;

#endif	//#ifdef MURSUM_NET_TRACE
//=============================================================================


//
// Default IP client port.  We try to bind to this first.  If it fails
// and this value is set to something other than zero, we will bind to 
// port zero.
//
extern int32 g_CV_IPClientPort;
extern int32 g_CV_IPClientPortRange;
extern int32 g_CV_IPClientPortMRU;

extern int32 g_CV_UDPDebug;

// IP override.
extern char *g_CV_IP;
extern char *g_CV_BindIP;

// When querying servers, it removes one from the list if it hasn't
// heard from one in this amount of time.
extern float g_CV_IPQueryTimeout;

// Packet loss simulation
extern int32 g_CV_UDPSimulatePacketLoss;
// Packet corruption simulation
extern int32 g_CV_UDPSimulateCorruption;

// Need to know what the bandwidth target for clients is so we can tell the server
// during the connection handshake.
extern int32 g_CV_BandwidthTargetClient;


#ifdef _DEBUG
extern int32	g_CV_INSize;
extern uint32	g_CV_In_UDPSize;
extern uint32	g_CV_In_GSize;
extern uint32	g_CV_In_UGSize;
#endif

uint32	m_nLSTime		= 0;

// [NOVICE] BEGIN ---------------
#include "NFPacketDebug.h"
// [NOVICE] END -----------------


//////////////////////////////////////////////////////////////////////////////
// CUDPConn implementation

CUDPConn::CPacketQueue CUDPConn::s_cPacketTrash;
CUDPConn::CTimeQueue CUDPConn::s_cTimeTrash;
CUDPConn::CFrameQueue CUDPConn::s_cFrameTrash;

CUDPConn::CUDPConn() :
	m_Socket(INVALID_SOCKET),
	m_bPauseGuaranteed(false),
	m_nOutgoingGCount(0),
	m_nOutgoingGSize(0),
	m_nOutgoingUCount(0),
	m_nOutgoingUSize(0),
	m_nOutgoingFrameCode(0),
	m_nIncomingLastFrame(0),
	m_bACKPending(false),
	m_nNumPings(0),
	m_nPingOffset(0),
	m_fCurPing((float)k_nStartPing),
	m_fReportedPing(0.0f),
	m_bWaitingForPingResponse(false),
	m_bFlushingNAKs(false),
	m_nBandwidth(0x7FFFFFFF),
	m_nReportedBandwidth(0x7FFFFFFF),
	m_nMaxBandwidth(0x7FFFFFFF),
	m_nNumOutOfOrderPackets(0),
	m_bFlowControlInitialized(false),
	m_nFlowControlCurPeriodSent(0),
	m_nFlowControlLastPeriodSent(0)
{
	uint32 nCurTime = GetTickCount();
	m_nLastHeartbeatTime = nCurTime;
	m_nLastPingTimeStamp = nCurTime;
	m_nLastSendTime = nCurTime;
	m_nLastRecvTime = nCurTime;
	m_eLastDisconnectReason = DISCONNECTREASON_KICKED;
	AccumulateHistory(m_aOutgoingBandwidthHistory);
	AccumulateHistory(m_aIncomingBandwidthHistory);

#ifdef MURSUM_NET_TRACE
	g_hDestWnd	= FindWindow( NULL, "SA_LTNetTracer" );
	if( g_hDestWnd )
	{
		g_IPC_SendByte.dwCommand	= IPC_NET_SEND_BYTE;
		g_IPC_RecvByte.dwCommand	= IPC_NET_RECV_BYTE;
		
		g_CopyDataStruct.dwData		= 0;
		g_CopyDataStruct.cbData		= sizeof( T_IPC_RECV_BYTE );
	}
#endif
}

CUDPConn::~CUDPConn()
{
	//[MURSUM]
	//ASSERT(m_Node.m_pGNext == m_Node.m_pGPrev); // Should be tied off...
}

uint32 CUDPConn::GetSizeIndicatorSize(uint32 nSize)
{
	if (nSize <= 0x7F)
		return 8;
	uint32 nResult = 12;
	nSize >>= 10;
	while (nSize)
	{
		nResult += 2;
		nSize >>= 1;
	}

	return nResult;
}

void CUDPConn::WriteSizeIndicator(CPacket_Write &cPacket, uint32 nSize)
{
	cPacket.WriteBits(nSize & 0x7F, 7);
	bool bAbove7F = nSize > 0x7F;
	cPacket.Writebool(bAbove7F);
	if (!bAbove7F)
		return;
	nSize >>= 7;
	cPacket.WriteBits(nSize & 7, 3);
	bool bAbove7 = nSize > 7;
	cPacket.Writebool(bAbove7);
	if (!bAbove7)
		return;
	nSize >>= 3;
	while (nSize)
	{
		cPacket.Writebool((nSize & 1) != 0);
		nSize >>= 1;
		cPacket.Writebool(nSize != 0);
	}
}

uint32 CUDPConn::ReadSizeIndicator(CPacket_Read &cPacket)
{
	uint32 nResult;
	nResult = cPacket.ReadBits(7);
	if (!cPacket.Readbool())
		return nResult;
	nResult |= cPacket.ReadBits(3) << 7;
	if (!cPacket.Readbool())
		return nResult;
	uint32 nMask = 1 << 10;
	do
	{
		if (cPacket.Readbool())
			nResult |= nMask;
		nMask <<= 1;
	} while (cPacket.Readbool());
	return nResult;
}

uint32 CUDPConn::GetUDPPacketSize(uint32 nSize)
{
	//- k_nFingerprintBits	: 8
	//- k_nUDPPacketOverhead : 28 * 8
	return (((nSize + k_nFingerprintBits) + 7) & 0xFFFFFFF8) + k_nUDPPacketOverhead;
}

//- 패킷의 유효성(FingerPrint) 체크
uint32 CUDPConn::GetPacketFingerprint(const CPacket_Read &cPacket)
{
	//- k_nFingerprintBits	: 8
	uint32			nResult				= 0;
	uint32			nChecksum			= cPacket.CalcChecksum();
	const uint32	k_nFingerprintMask	= ( 1 << k_nFingerprintBits ) - 1;

	while (nChecksum)
	{
		nResult ^= nChecksum & k_nFingerprintMask;
		nChecksum >>= k_nFingerprintBits;
	}

	return nResult;
}

//- 패킷 큐에다가 넣는거다.
bool CUDPConn::QueuePacket(const CPacket_Read &cPacket, bool bGuaranteed)
{
	//- bGuaranteed 에 따라 Guaranteed 큐 / Unguaranteed 큐에다가 넣는다.
	if (bGuaranteed)
	{
		LT_MEM_TRACK_ALLOC(m_cOutgoingGuaranteedQueue.push_back(cPacket, s_cPacketTrash), LT_MEM_TYPE_NETWORKING);
		++m_nOutgoingGCount;
		m_nOutgoingGSize += cPacket.Size() + GetSizeIndicatorSize(cPacket.Size());
	}
	else
	{
		LT_MEM_TRACK_ALLOC(m_cOutgoingUnguaranteedQueue.push_back(cPacket, s_cPacketTrash), LT_MEM_TYPE_NETWORKING);
		LT_MEM_TRACK_ALLOC(m_cOutgoingUnguaranteedTimeQueue.push_back(GetTickCount(), s_cTimeTrash), LT_MEM_TYPE_NETWORKING);
		++m_nOutgoingUCount;
		m_nOutgoingUSize += cPacket.Size() + GetSizeIndicatorSize(cPacket.Size());
	}

	return true;
}

//- 들어오는 패킷 처리
//- 호출되는 곳 ( 1 곳 )
//- 1. CUDPDriver::Thread_Listen()
CUDPConn::EIncomingPacketResult CUDPConn::HandleIncomingPacket(const CPacket_Read &cPacket)
{
	CWinSync_CSAuto cSerialize(m_cUpdateCS);					//- 크리티컬 섹션용.. 스코프 넘어가믄 자동으로 없어진다

	// Simulate packet loss....
	if (g_CV_UDPSimulatePacketLoss)								//- 패킷 로스가 있을때를 시뮬레이션 한닷
	{
		if ((rand() % 100) < g_CV_UDPSimulatePacketLoss)		//- 대신 랜덤으로
			return eIPR_OK;
	}

	// Track it
	uint32 nCurTime = GetTickCount();
	uint32 nTimeSinceLastRecv = nCurTime - m_nLastRecvTime;		//- 여기서 지난번에 받은 시간을 계산한다. 몬가 할것같다.
	m_nLastRecvTime = nCurTime;

	//- 여기 요것이 알고보니 큐다. 큐에다가 지난번에 받았던 시간을 집어넣은것 뿐.
	m_aIncomingBandwidthHistory.push(CBandwidthPeriod(cPacket.Size(), nTimeSinceLastRecv));
	AccumulateHistory(m_aIncomingBandwidthHistory);				//- 큐에 집어넣은 시간정보에 인덱스 메긴다.

	//- 들어온 패킷을 읽을 준비하고,
	CPacket_Read cReadPacket(cPacket);
	cReadPacket.SeekTo(0);

	// [NOVICE] BEGIN -----------------
#ifdef SA_NETMONITORING
	if ( g_CV_ParseRecv )
	{
		CPacket_Read cDebugPacket(cReadPacket);
		NFPacketDebuger::GetInstance()->NFParseUDPRecv( cDebugPacket );
	}
#endif // SA_NETMONITORING
	// [NOVICE] END -------------------

	//------------
	/*
	// Read the fingerprint
	//- 패킷에 적혀있는 FingerPrint 가져온 다음에,
	uint32 nSentFingerprint = cReadPacket.ReadBits(k_nFingerprintBits);

	// Drop the packet if the fingerprint's wrong
	//- 패킷 내용을 가지고 FingerPrint 를 받아서,
	uint32 nIncomingFingerprint = GetPacketFingerprint(CPacket_Read(cReadPacket, cReadPacket.Tell(), cReadPacket.TellEnd()));

	// 같이 비교해서, 서로 다르면 드롭시켜버린다.
	if (nSentFingerprint != nIncomingFingerprint)
	{
		if (g_CV_UDPDebug)
		{
			dsi_ConsolePrint("UDP: Dropping corrupt incoming packet");
		}
		
		return eIPR_OK;
	}
	*/

	//- 여기까지 오면 패킷은 멀쩡한 놈. 데이터 읽기 시작한다.
	EIncomingPacketResult eResult = eIPR_OK;


	if (cReadPacket.Readbool())						//- 한비트씩 읽어간다.
		eResult = HandleUDP(cReadPacket);			//- 첫번째 비트 플래그는 하트빗이나, 연결종료처리 관련 플래그이다.

	if (cReadPacket.Readbool())
		HandleGuaranteed(cReadPacket, false);		//- 두번째 비트 플래그는 Guaranteed 메세지

	if (cReadPacket.Readbool())
		HandleUnguaranteed(cReadPacket);			//- 세번째 비트 플래그는 Unguaranteed 메세지

#ifdef _DEBUG
	if( g_CV_INSize > 0 )
	{
		static uint32 dwLastUpT = GetTickCount();
		if( GetTickCount()-dwLastUpT > 1000 )
		{
			dsi_ConsolePrint( "UDP: %d", g_CV_In_UDPSize );
			dsi_ConsolePrint( "G  : %d", g_CV_In_GSize );
			dsi_ConsolePrint( "UNG: %d", g_CV_In_UGSize );

			g_CV_In_UDPSize	= 0;
			g_CV_In_GSize = 0;
			g_CV_In_UGSize = 0;
			dwLastUpT = GetTickCount();
		}
	}
#endif

	return eResult;
}

void CUDPConn::ClearFrameHistory(uint32 nFrameCode)
{
	// m_cFrameHistory MUST contain nFrameCode before this function is called..
	while (m_cFrameHistory.front().m_nFrameCode != nFrameCode)
	{
		m_aPacketLossHistory.push(true);
		m_cFrameHistory.pop_front(s_cFrameTrash, s_cPacketTrash);
	}

	ASSERT(!m_cFrameHistory.empty()); // This means the frame code was not in the history
	m_aPacketLossHistory.push(true);
	m_cFrameHistory.pop_front(s_cFrameTrash, s_cPacketTrash);

	// Reset the resend counts
	CFrameQueue::iterator iCurFrame = m_cFrameHistory.begin();
	for (; iCurFrame != m_cFrameHistory.end(); ++iCurFrame)
	{
		iCurFrame->m_nSendCount = LTMIN(iCurFrame->m_nSendCount, k_nGuaranteedBlockCount + 1);
	}
}

bool CUDPConn::RemoveFrameHistory(uint32 nFrameCode, CPacketFrame **pFrame)
{
	CFrameQueue::iterator iCurFrame = m_cFrameHistory.begin();
	for (; iCurFrame != m_cFrameHistory.end(); ++iCurFrame)
	{
		if (iCurFrame->m_nFrameCode == nFrameCode)
		{
			bool bWasAlreadyReceived = iCurFrame->m_bReceivedOO;
			if (!bWasAlreadyReceived)
				m_aPacketLossHistory.push(true);
			iCurFrame->m_bReceivedOO = true;
			*pFrame = &(*iCurFrame);
			return !bWasAlreadyReceived;
		}
	}
	// I don't know what you're talking about...
	return false;
}

void CUDPConn::AddPing(uint32 nTime)
{
	m_aPingHistory[m_nPingOffset] = nTime;
	if (m_nNumPings < k_nPingHistorySize)
		++m_nNumPings;
	m_nPingOffset = (m_nPingOffset + 1) % k_nPingHistorySize;
	m_fCurPing = 0.0f;
	for (uint32 nIndex = 0; nIndex < m_nNumPings; ++nIndex)
		m_fCurPing += (float)m_aPingHistory[nIndex];
	m_fCurPing /= (float)m_nNumPings;
	m_bWaitingForPingResponse = false;
	if (g_CV_UDPDebug > 2)
	{
		dsi_ConsolePrint("UDP: Ping update %3.2f (%d)", m_fCurPing, nTime);
	}
}

bool CUDPConn::ShouldSendHeartbeat()
{
	// If it won't go through, don't bother
	if (IsFlowControlBlocked(k_nUDPCommand_Heartbeat_Size + 32))
		return false;

	// Always send a heartbeat if we've gotten something from them
	if (m_bACKPending)
		return true;

	// Send a heartbeat message whenever we're sending over an empty frame
	return ShouldSendEmptyFrame();
}

bool CUDPConn::ShouldSendEmptyFrame()
{
	uint32 nCurTime = GetTickCount();
	uint32 nTimeSinceLast = nCurTime - m_nLastHeartbeatTime;
	// Don't send them too fast
	if (nTimeSinceLast < k_nPingDelay)
		return false;
	// Don't send them faster than they can respond
	uint32 nNAKMS = (uint32)LTMIN(m_fCurPing, (float)k_nMaxPingNAKTime);
	if (nTimeSinceLast < nNAKMS)
		return false;
	// Make sure they're actually alive over there...
	return true;
}

bool CUDPConn::ShouldSendPacket(bool bOnlyBandwidth)
{
	// If we're flushing NAKs, deal with it ASAP
	if (bOnlyBandwidth && m_bFlushingNAKs)
		return true;

	if (ShouldSendHeartbeat())
		return true;

	uint32 nCurTime = GetTickCount();
	uint32 nTimeSinceLast = nCurTime - m_nLastHeartbeatTime;

	// Only send a packet if we're not using too much of our bandwidth
	uint32 nOutgoingTotal = m_aOutgoingBandwidthHistory.end()->m_nSize;
	uint32 nOutgoingTime = m_aOutgoingBandwidthHistory.end()->m_nTime + nTimeSinceLast;
	float fOutgoingBaud = ((float)nOutgoingTotal / (float)nOutgoingTime) * 1000.0f;
	const float k_fTargetOutgoingPercentage = 0.9f;

	if (fOutgoingBaud > ((float)GetBandwidth() * k_fTargetOutgoingPercentage))
		return false;

	// If we're only worried about overflowing bandwidth, say it's OK to send partials...
	if (bOnlyBandwidth)
		return true;

	// Would it be OK to send a partial packet?
	if ((nTimeSinceLast > k_nHeartbeatDelay) && 
		(m_cOutgoingUnguaranteedQueue.empty() && m_cOutgoingGuaranteedQueue.empty()))
		return true;

	/// Find out if we've got a full packet
	uint32 nHeartbeatSize = 1 + k_nFrameBits;

	// Guaranteed message size
	uint32 nGuaranteedSize = 0;
	if (!m_cOutgoingGuaranteedQueue.empty())
		nGuaranteedSize = m_nOutgoingGSize + k_nFrameBits + 1 + m_nOutgoingGCount;

	// Total message size
	uint32 nMessageSize = 3 + nHeartbeatSize + nGuaranteedSize + m_nOutgoingUSize;

	return (nMessageSize >= k_nTargetPacketSize);
}

//- 패킷으로부터, Heartbeat 과, Disconnect 정보를 가져온다.
//- 호출되는 곳 ( 총 1 곳 )
//- 1. CUDPConn::HandleIncomingPacket
CUDPConn::EIncomingPacketResult CUDPConn::HandleUDP(CPacket_Read &cPacket)
{
#ifdef _DEBUG
	g_CV_In_UDPSize	+= cPacket.Size();
#endif

	uint32 nCommand = cPacket.ReadBits(k_nUDPCommandBits);

	switch (nCommand)
	{
		case k_nUDPCommand_Heartbeat :									//- 하트빗 체크일때는, 
		{
			uint32 nSuccessfulFrame = cPacket.ReadBits(k_nFrameBits);	//- 패킷에 적혀있는 프레임을 읽어와서 (요놈은 계속 증가한다)

			// Check for dropped guaranteed packets
			CFrameQueue::iterator iCurFrame = m_cFrameHistory.begin();

			for (; iCurFrame != m_cFrameHistory.end(); ++iCurFrame)		//- 프레임 정보 쌓여있는곳 훑은다.
			{
				if (iCurFrame->m_nFrameCode == nSuccessfulFrame)
				{
					break;
				}
			}

			if (g_CV_UDPDebug > 1)										//- 마지막까지 훑었는데도 프레임 값이 일치하는게 없으면,
			{															//- 콘솔창에 하트빗 보여줄 때, (OLD) 라고 같이 표시한다.
				dsi_ConsolePrint("UDP: Received heartbeat %d %s", nSuccessfulFrame, iCurFrame == m_cFrameHistory.end() ? "(OLD)" : "");
			}

			uint32 nCurTime = GetTickCount();

			// Do we even know what frame they're talking about?
			if (iCurFrame != m_cFrameHistory.end())						//- 프레임 히스토리 안에 있는 상태라면,
			{
				// Update the ping
				AddPing(nCurTime - iCurFrame->m_nFirstSent);			//- 핑 큐에다가 걸린 시간 업데이트 하고,
				
				// Ok, they're ACKing something we sent..  Forget everything before that.
				ClearFrameHistory(nSuccessfulFrame);					//- 이번 그 프레임 이하는 날린다.
			}

			bool bOOACK = false;

			// Clear any out of order packets they've already got		//- 프레임 정보 지운다는데, 확실히 모르겠다 -_-;;
			if (cPacket.Readbool())
			{
				uint32 nCurFrame = (nSuccessfulFrame + 2) & k_nFrameMask;

				//- k_nMaxOutOfOrderPackets : 8
				for (uint32 nOOACK = 0; nOOACK < k_nMaxOutOfOrderPackets; ++nOOACK, ++nCurFrame)
				{
					if (!cPacket.Readbool())		continue;

					CPacketFrame *pFrame;
					bool bFirstReceipt = RemoveFrameHistory(nCurFrame, &pFrame);

					if (!bFirstReceipt)				continue;

					// Update the ping
					AddPing(nCurTime - pFrame->m_nFirstSent);
					bOOACK = true;
				}
			}

			if (bOOACK)													//- 훔.. 여기도 잘 모르겠다 -_-;;
			{
				CFrameQueue::iterator iCurFrame = m_cFrameHistory.begin();
				for (; iCurFrame != m_cFrameHistory.end(); ++iCurFrame)
				{
					if (iCurFrame->m_bReceivedOO)	continue;

					// NAK the first packet, since it was skipped
					iCurFrame->m_nLastSent = LTMIN(iCurFrame->m_nLastSent, GetTickCount() - LTMAX((uint32)m_fCurPing, k_nHeartbeatDelay));
					m_bACKPending = true;
					break;
				}
			}

			break;
		}
		case k_nUDPCommand_Disconnect :
		{
			//- 끊긴 이유를 멤버변수에다가 넣어놓는다.
			//- GetLastDisconnectReason( ) 하면, 그 끊긴 이유가 리턴되는거다.
			m_eLastDisconnectReason = ( EDisconnectReason )cPacket.Readuint8( );
			return eIPR_Disconnect;
		}
	}

	return eIPR_OK;
}

void CUDPConn::SaveOutOfOrderPacket(CPacket_Read &cPacket, uint32 nFrameCode)
{
	// Sorry, we're full..
	if (m_nNumOutOfOrderPackets >= k_nMaxOutOfOrderPackets)
		return;

	// Filter duplicates and find an open slot
	uint32 nOpenSlot = k_nMaxOutOfOrderPackets;
	for (uint32 nDuplicateSlot = 0; nDuplicateSlot < k_nMaxOutOfOrderPackets; ++nDuplicateSlot)
	{
		COutOfOrderPacket &cCurOOP = m_aOutOfOrderPackets[nDuplicateSlot];
		if (cCurOOP.m_bInUse)
		{
			if (cCurOOP.m_nFrameCode == nFrameCode)
				return;
		}
		else if (nOpenSlot == k_nMaxOutOfOrderPackets)
		{
			nOpenSlot = nDuplicateSlot;
		}
	}

	ASSERT(nOpenSlot < k_nMaxOutOfOrderPackets);

	// Save it
	m_aOutOfOrderPackets[nOpenSlot].m_bInUse = true;
	m_aOutOfOrderPackets[nOpenSlot].m_cPacket = cPacket;
	m_aOutOfOrderPackets[nOpenSlot].m_nFrameCode = nFrameCode;

	if (g_CV_UDPDebug > 1)
	{
		dsi_ConsolePrint("UDP: Saving out of order packet (%d)", nFrameCode);
	}
	
	// Count it
	++m_nNumOutOfOrderPackets;

	// Let them know we got an out of order packet...
	m_bACKPending = true;
}

// 자자.. Guaranteed 메세지일때 여기서 처리한다.
// 호출되는 곳 ( 총 1곳 )
// 1. CUDPConn::HandleIncomingPacket
// 요놈은 첨에 CUDPConn::HandleIncomingPacket 에서 호출될때는, bOutOfOrder 가 false 로 해서 넘어온다.
void CUDPConn::HandleGuaranteed(CPacket_Read &cPacket, bool bOutOfOrder)
{
#ifdef _DEBUG
	g_CV_In_GSize	+= cPacket.Size();
#endif
	
	uint32 nStartPos = cPacket.Tell();

	// Who are we looking at again?
	uint32 nFrameCode = cPacket.ReadBits(k_nFrameBits);

	// Is this an out-of-order packet?
	uint32 nNextFrame = (m_nIncomingLastFrame + 1) & k_nFrameMask;
	bool bGuaranteedOK = nNextFrame == nFrameCode;

	if (bGuaranteedOK)
	{
		// Remember the last OK frame we got
		m_nIncomingLastFrame = nNextFrame;
		
		// Remember to tell them that we got it
		m_bACKPending = true;
	}

	// Is the last sub-packet continued in the next packet?
	bool bLastContinued = cPacket.Readbool();
	bool bLastPacket = false;
	bool bFullPartial = false;
	uint32 nNumSubPackets = 0;

	// Space for queueing up what's in the guaranteed message so we can handle short messages as dropped
	CPacket_Read cPartialStartPacket;
	CPacket_Read cPartialEndPacket;
	CPacketQueue cIncomingQueue;
	do
	{
		// Get the sub-packet out
		uint32 nSubPacketSize = ReadSizeIndicator(cPacket);
		if (!nSubPacketSize)
		{
			bLastPacket = !cPacket.Readbool();
			if (!bLastPacket)
				continue;
			else
				break;
		}
		++nNumSubPackets;
		CPacket_Read cSubPacket(cPacket, cPacket.Tell(), nSubPacketSize);

		// Handle a short packet by backing out of the processing
		if (cPacket.TellEnd() < nSubPacketSize)
		{
			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Short guaranteed message received!");
			}
			cIncomingQueue.clear(s_cPacketTrash);
			return;
		}

		cPacket.Seek(nSubPacketSize);

		bLastPacket = !cPacket.Readbool();

		// Only really get the data out of this packet if it's sequential
		if (bGuaranteedOK)
		{
			// Append to the incomplete packet if we got a partial last packet
			// or if it's continued in the next packet
			if (!m_cIncomingIncompletePacket.Empty() && cPartialStartPacket.Empty())
			{
				cPartialStartPacket = cSubPacket;
				bFullPartial = (bLastPacket && bLastContinued);
			}
			else if (bLastPacket && bLastContinued)
			{
				cPartialEndPacket = cSubPacket;
			}
			// Queue the sub-packet
			else
			{
				LT_MEM_TRACK_ALLOC(cIncomingQueue.push_back(cSubPacket, s_cPacketTrash), LT_MEM_TYPE_NETWORKING);
			}
		}
	} while (!bLastPacket);

	// Write out the results
	if (bGuaranteedOK)
	{
		// Append to the partial packet from last frame
		if (!cPartialStartPacket.Empty())
		{
			m_cIncomingIncompletePacket.WritePacket(cPartialStartPacket);
		}

		// Add the previous partial to the queue first
		if (!bFullPartial && !m_cIncomingIncompletePacket.Empty())
		{
			LT_MEM_TRACK_ALLOC(m_cIncomingQueue.push_back(CPacket_Read(m_cIncomingIncompletePacket), s_cPacketTrash), LT_MEM_TYPE_NETWORKING);
		}

		// Splice in the rest of the queue
		m_cIncomingQueue.splice_back(cIncomingQueue);
		
		// Save the partial packet at the end if needed
		if (!cPartialEndPacket.Empty())
		{
			m_cIncomingIncompletePacket.WritePacket(cPartialEndPacket);
		}
	}

	if (g_CV_UDPDebug > 1)
	{
		dsi_ConsolePrint("UDP: Received guaranteed packet (%d/%d) %d sub %s %s", 
			nFrameCode, nNextFrame, nNumSubPackets,
			bGuaranteedOK ? "" : "(NAK)", 
			bLastContinued ? "(Cont)" : "");
	}

	//- 패킷이 미스됐을 경우에는 프레임 체크해서 연속된 범위 내에 있는지 확인하고 나서
	//- 연속된 범위 내에 있으면, 따로 저장해놓는다.
	if (!bGuaranteedOK)
	{
		// Was this a near-miss?
		if (((nFrameCode - nNextFrame) & k_nFrameMask) <= k_nMaxOutOfOrderPackets)
		{
			// Save this packet for later...
			SaveOutOfOrderPacket(CPacket_Read(cPacket, nStartPos, cPacket.Tell() - nStartPos), nFrameCode);
		}
	}
	// Flush out any following out of order packets...
	else if ((!bOutOfOrder) && (m_nNumOutOfOrderPackets))
	{
		bool bHandledPacket;
		uint32 nCurFrameCode = (nFrameCode + 1) & k_nFrameMask;

		do
		{
			bHandledPacket = false;
			for (uint32 nCurSlot = 0; !bHandledPacket && nCurSlot < k_nMaxOutOfOrderPackets; ++nCurSlot)
			{
				COutOfOrderPacket &cCurOOP = m_aOutOfOrderPackets[nCurSlot];
				if ((!cCurOOP.m_bInUse) ||
					(nCurFrameCode != cCurOOP.m_nFrameCode))
				{
					continue;
				}
				if (g_CV_UDPDebug > 1)
				{
					dsi_ConsolePrint("UDP: Handle out of order packet (%d)", nCurFrameCode);
				}
				// Process it...
				cCurOOP.m_cPacket.SeekTo(0);
				HandleGuaranteed(cCurOOP.m_cPacket, true);
				// Count it
				cCurOOP.m_bInUse = false;
				--m_nNumOutOfOrderPackets;
				// Look for the next one
				nCurFrameCode = (nCurFrameCode + 1) & k_nFrameMask;
				// Remember we sent one
				bHandledPacket = true;
			}
		} while (bHandledPacket && m_nNumOutOfOrderPackets);
	}
}

//- Unguaranteed 메세지 처리하는곳
//- 호출되는 곳 ( 1곳 )
//- 1. CUDPConn::HandleIncomingPacket
void CUDPConn::HandleUnguaranteed(CPacket_Read &cPacket)
{
#ifdef _DEBUG
	g_CV_In_UGSize	+= cPacket.Size();
#endif
	
	CPacketQueue cIncomingQueue;			//- 이 패킷큐는 여기서 잠깐 만들어서,
											//- 들어온 cPacket 에 대해서 큐에다 싹 넣은다음에
											//- m_cIncomingQueue 에다가 모두 넣어버린다.
	uint32 nNumSubPackets = 0;
	while (!cPacket.EOP())
	{
		// Read the next sub-packet
		uint32 nSubPacketSize = ReadSizeIndicator(cPacket);

		if (!nSubPacketSize)	break;
		
		++nNumSubPackets;
		CPacket_Read cSubPacket(cPacket, cPacket.Tell(), nSubPacketSize);
		
		// Handle a short packet by backing out of the processing
		if (cPacket.TellEnd() < nSubPacketSize)
		{
			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Short un-guaranteed message received!");
			}
			cIncomingQueue.clear(s_cPacketTrash);
			return;
		}
		
		cPacket.Seek(nSubPacketSize);

		// Queue the sub-packet
		LT_MEM_TRACK_ALLOC(cIncomingQueue.push_back(cSubPacket, s_cPacketTrash), LT_MEM_TYPE_NETWORKING);

		if ( g_CV_UDPDebug )
		{
			dsi_ConsolePrint( "## UDP: Received Unguaranteed packet %d / %d \n", nNumSubPackets, nSubPacketSize );
		}		
	}

	// If we didn't abort out, this is a good packet, and we're kosher.
	m_cIncomingQueue.splice_back(cIncomingQueue);
}

void CUDPConn::WriteHeartbeat(CPacket_Write &cPacket)
{
	cPacket.WriteBits(k_nUDPCommand_Heartbeat, k_nUDPCommandBits);
	cPacket.WriteBits(m_nIncomingLastFrame, k_nFrameBits);
	m_nLastHeartbeatTime = GetTickCount();
	if (g_CV_UDPDebug > 1)
	{
		dsi_ConsolePrint("UDP: Sending heartbeat %d %s", m_nIncomingLastFrame, (m_bACKPending) ? "(ACK)" : "");
	}
	// Write the out of order packet info
	cPacket.Writebool(m_nNumOutOfOrderPackets != 0);
	uint32 nFlags = 0;
	ASSERT(k_nMaxOutOfOrderPackets <= 32);
	for (uint32 nCurSlot = 0; nCurSlot < k_nMaxOutOfOrderPackets; ++nCurSlot)
	{
		COutOfOrderPacket &cCurSlot = m_aOutOfOrderPackets[nCurSlot];
		if (!cCurSlot.m_bInUse)
			continue;
		uint32 nFrameOffset = (cCurSlot.m_nFrameCode - (m_nIncomingLastFrame + 2)) & k_nFrameMask;
		if (nFrameOffset > k_nMaxOutOfOrderPackets)
		{
			ASSERT(!"Invalid out of order packet encountered.");
			// Remove it from the queue....  Maybe it'll work itself out later.  Hopefully.
			cCurSlot.m_bInUse = false;
			continue;
		}
		nFlags |= (1 << nFrameOffset);
	}
	if (m_nNumOutOfOrderPackets)
		cPacket.WriteBits(nFlags, k_nMaxOutOfOrderPackets);

	// ACK has been sent
	m_bACKPending = false;
}

void CUDPConn::WriteGuaranteed(CPacket_Write &cPacket, bool bForceEmpty)
{
	// Jump out if we don't have any guaranteed info, or if guaranteed messaging is paused while fixing lost packets
	if (m_bPauseGuaranteed || (m_cOutgoingGuaranteedQueue.empty() && !bForceEmpty) || 
		IsFlowControlBlocked(GetUDPPacketSize(cPacket.Size())))
	{
		cPacket.Writebool(false);
		return;
	}

	CPacket_Write cTempPacket;
	bool bContinued = false;
	bool bFirst = true;

	uint32 nBiggestPacketSize = LTMIN(k_nTargetPacketSize, m_nFlowControlBitsPerPeriod / 2);

	uint32 nNumSubPackets = 0;
	while (!m_cOutgoingGuaranteedQueue.empty() && !bContinued)
	{
		CPacket_Read cNextPacket = m_cOutgoingGuaranteedQueue.front();

		// Figure out how much space we're going to use
		uint32 nNextSize = cNextPacket.Size() + GetSizeIndicatorSize(cNextPacket.Size());
		uint32 nNewPacketSize = GetUDPPacketSize(cPacket.Size() + cTempPacket.Size() + nNextSize);

		bool bTooBig = (nNewPacketSize > nBiggestPacketSize);

		// Are we sending too much?
		if ((!bTooBig && IsFlowControlBlocked(nNewPacketSize)) ||
			(bTooBig && IsFlowControlBlocked(nBiggestPacketSize)))
		{
			// Don't send empties when we're flow control blocked
			bForceEmpty = false;
			break;
		}

		if (bTooBig)
		{
			// Don't split small packets..
			if (cNextPacket.Size() < k_nMinPartialSize)
			{
				break;
			}
			// How much would we go over?
			uint32 nOverflow = nNewPacketSize - nBiggestPacketSize;
			if (cNextPacket.Size() < nOverflow)
			{
				// Abnormal packet split encountered
				break;
			}
			// Leave that much for next time around...
			uint32 nPartialSize = cNextPacket.Size() - nOverflow;
			// Split the packet
			cNextPacket = CPacket_Read(cNextPacket, 0, nPartialSize);
			CPacket_Read &cFrontPacket = m_cOutgoingGuaranteedQueue.front();
			cFrontPacket = CPacket_Read(cFrontPacket, nPartialSize, nOverflow);

			// Continue in the next packet
			bContinued = true;
		}

		// Write the more packets indicator
		if (!bFirst)
			cTempPacket.Writebool(true);
		else
		{
			LT_MEM_TRACK_ALLOC(m_cFrameHistory.push_back(CPacketFrame(), s_cFrameTrash), LT_MEM_TYPE_NETWORKING);
		}
		bFirst = false;

		// Write the sub-packet
		WriteSizeIndicator(cTempPacket, cNextPacket.Size());
		cTempPacket.WritePacket(cNextPacket);
		++nNumSubPackets;
		m_nOutgoingGSize -= cNextPacket.Size();
		if (!bContinued)
		{
			m_nOutgoingGSize -= GetSizeIndicatorSize(m_cOutgoingGuaranteedQueue.front().Size());
			--m_nOutgoingGCount;
			m_cOutgoingGuaranteedQueue.pop_front(s_cPacketTrash);
		}
		LT_MEM_TRACK_ALLOC(m_cFrameHistory.back().m_cPackets.push_back(cNextPacket, s_cPacketTrash), LT_MEM_TYPE_NETWORKING);
	}

	// Jump out if it's an empty packet and we're not being forced to send one...
	if (cTempPacket.Empty() && !bForceEmpty)
	{
		cPacket.Writebool(false);
		return;
	}

	// Count this as a ping...
	if (!m_bWaitingForPingResponse)
	{
		m_nLastPingTimeStamp = GetTickCount();
		m_bWaitingForPingResponse = true;
	}

	// Write the guaranteed content indicator
	cPacket.Writebool(true);

	// Add an entry to the frame history
	if (bFirst)
	{
		LT_MEM_TRACK_ALLOC(m_cFrameHistory.push_back(CPacketFrame(), s_cFrameTrash), LT_MEM_TYPE_NETWORKING);
	}
	m_cFrameHistory.back().m_nFirstSent = GetTickCount();
	m_cFrameHistory.back().m_nLastSent = m_cFrameHistory.back().m_nFirstSent + LTMIN(LTMAX((uint32)m_fCurPing, k_nHeartbeatDelay) * k_nPingFirstNAKMultiplier, k_nMaxPingNAKTime);
	m_cFrameHistory.back().m_bReceivedOO = false;

	// Write the frame code
	m_nOutgoingFrameCode = (m_nOutgoingFrameCode + 1) & k_nFrameMask;
	cPacket.WriteBits(m_nOutgoingFrameCode, k_nFrameBits);
	m_cFrameHistory.back().m_nFrameCode = m_nOutgoingFrameCode;
	// It hasn't been re-sent yet...
	m_cFrameHistory.back().m_nSendCount = 0;
	m_cFrameHistory.back().m_bContinued = bContinued;

	if (g_CV_UDPDebug > 1)
	{
		dsi_ConsolePrint("UDP: Sent Guaranteed %d %d sub %s %s", 
			m_nOutgoingFrameCode, nNumSubPackets,
			cTempPacket.Empty() ? "(Empty)" : "",
			bContinued ? "(Cont)" : "");

	}

	// Write an empty packet if we were forced
	if (cTempPacket.Empty())
	{
		WriteSizeIndicator(cTempPacket, 0);
	}

	// Remember how big it was
	m_cFrameHistory.back().m_nFrameSize = cTempPacket.Size();

	// Was the last sub-packet a partial?
	cPacket.Writebool(bContinued);
	// Write the sub-packets..
	ASSERT(!cTempPacket.Empty());
	cPacket.WritePacket(CPacket_Read(cTempPacket));
	// Write the no more packets indicator
	cPacket.Writebool(false);
}

void CUDPConn::WriteUnguaranteed(CPacket_Write &cPacket)
{
	// Dump any unguaranteed stuff that's taking too long to send
	uint32 nDropDelay = LTMAX((uint32)m_fCurPing, k_nPingDelay) * k_nUnguaranteedDropDelay;
	uint32 nCurTime = GetTickCount();
	while (!m_cOutgoingUnguaranteedTimeQueue.empty())
	{
		uint32 nCurDelay = nCurTime - m_cOutgoingUnguaranteedTimeQueue.front();
		if (nCurDelay < nDropDelay)
			break;
		m_cOutgoingUnguaranteedTimeQueue.pop_front(s_cTimeTrash);
		// Count it
		CPacket_Read &cFrontPacket = m_cOutgoingUnguaranteedQueue.front();
		m_nOutgoingUSize -= cFrontPacket.Size() + GetSizeIndicatorSize(cFrontPacket.Size());
		--m_nOutgoingUCount;
		m_cOutgoingUnguaranteedQueue.pop_front(s_cPacketTrash);
	}
	
	// Write the unguaranteed portion
	if (m_cOutgoingUnguaranteedQueue.empty() || IsFlowControlBlocked(GetUDPPacketSize(cPacket.Size())))
	{
		// Note : This will always be read in as a 0 since it's past the end of packet.  
		// So if we're byte aligned at this point, it doesn't have to go over the net. 
		// Cool, huh?
		return;
	}

	uint32 nSendSize = 0;
	uint32 nNumSubPackets = 0;
	bool bFirst = true;

	while (!m_cOutgoingUnguaranteedQueue.empty())
	{
		CPacket_Read &cNextPacket = m_cOutgoingUnguaranteedQueue.front();
		uint32 nNextSize = cNextPacket.Size() + GetSizeIndicatorSize(cNextPacket.Size());

		// Jump out if that's too much..
		if (IsFlowControlBlocked(GetUDPPacketSize(cPacket.Size() + ((nNextSize * 3) / 2))))
			break;

		bool bTooBig = ((cPacket.Size() + nNextSize) > k_nTargetPacketSize);
		// Don't append big unguaranteed data to packets that already have stuff in them
		if (bTooBig && (cPacket.Size() > 2))
		{
			// Note : Depending on bandwidth usage, we may want to throw the
			// unguaranteed data away instead of putting it into another message
			// NYI ?
			break;
		}

		// Write the unguaranteed info indicator
		if (bFirst)
			cPacket.Writebool(true);

		// Write the sub-packet
		++nNumSubPackets;
		WriteSizeIndicator(cPacket, cNextPacket.Size());
		cPacket.WritePacket(cNextPacket);

		// Count it
		m_nOutgoingUSize -= nNextSize;
		nSendSize += nNextSize;
		--m_nOutgoingUCount;

		// Throw it away
		m_cOutgoingUnguaranteedTimeQueue.pop_front(s_cTimeTrash);
		m_cOutgoingUnguaranteedQueue.pop_front(s_cPacketTrash);

		bFirst = false;
	}

	if ((g_CV_UDPDebug > 1) && nSendSize)
	{
		dsi_ConsolePrint("UDP: Sending unguaranteed (%d packets, %d)", nNumSubPackets, nSendSize);
	}
}

void CUDPConn::ReSendFrame(CPacketFrame &cFrame)
{
	if (g_CV_UDPDebug)
	{
		dsi_ConsolePrint("UDP: Resent frame %d", cFrame.m_nFrameCode);
	}

	CPacket_Write cPacket;
	// Allow heartbeats to go along for the ride, since they can get lost otherwise
	if (ShouldSendHeartbeat())
	{
		cPacket.Writebool(true);
		WriteHeartbeat(cPacket);
		// Make sure we don't pause the ping updates
		if (!m_bWaitingForPingResponse)
		{
			m_nLastPingTimeStamp = GetTickCount();
			m_bWaitingForPingResponse = true;
		}
	}
	else
	{
		cPacket.Writebool(false);
	}

	// Guaranteed packets
	cPacket.Writebool(true);
	cPacket.WriteBits(cFrame.m_nFrameCode, k_nFrameBits);
	cPacket.Writebool(cFrame.m_bContinued);
	CPacketQueue::const_iterator iCurPacket = cFrame.m_cPackets.begin();
	for (; iCurPacket != cFrame.m_cPackets.end(); ++iCurPacket)
	{
		WriteSizeIndicator(cPacket, iCurPacket->Size());
		cPacket.WritePacket(*iCurPacket);
		cPacket.Writebool(iCurPacket != cFrame.m_cPackets.back_i());
	}
	// Handle an empty frame correctly
	if (cFrame.m_cPackets.empty())
	{
		WriteSizeIndicator(cPacket, 0);
		cPacket.Writebool(false);
	}

	// Send over some unguaranteed stuff while we're at it.. (if we can...)
	WriteUnguaranteed(cPacket);

	// Update the packet loss count by writing a NAK
	m_aPacketLossHistory.push(false);

	SendPacket(CPacket_Read(cPacket));
}

void CUDPConn::ReSendLostPackets()
{
	bool bWasPaused = m_bPauseGuaranteed;
	m_bPauseGuaranteed = m_bFlushingNAKs;

	uint32 nCurTime = GetTickCount();
	uint32 nNAKMS = LTMIN(LTMAX((uint32)m_fCurPing, k_nHeartbeatDelay) * k_nPingNAKMultiplier, k_nMaxPingNAKTime);

	// Strip OO received packets from the front of the frame history
	while (!m_cFrameHistory.empty() && m_cFrameHistory.front().m_bReceivedOO)
		m_cFrameHistory.pop_front(s_cFrameTrash, s_cPacketTrash);

	// Find out if we should pause...
	bool bResetGuaranteed = false;
	uint32 nWaitingCount = 0;
	CFrameQueue::iterator iCurFrame = m_cFrameHistory.begin();
	for (; iCurFrame != m_cFrameHistory.end(); ++iCurFrame)
	{
		++nWaitingCount;
		if (iCurFrame->m_bReceivedOO)
			continue;
		bResetGuaranteed |= (iCurFrame->m_nSendCount > k_nGuaranteedTrickleCount);
	}
	if (bResetGuaranteed)
	{
		if (!m_bFlushingNAKs)
			m_nNAKFlushStartTime = nCurTime;
		m_bFlushingNAKs = true;
		// Re-send the first frame with a reset message
		CPacketFrame &cFirstFrame = m_cFrameHistory.front();
		cFirstFrame.m_nSendCount = k_nGuaranteedTrickleCount + 1;
		uint32 nDiff = nCurTime - LTMIN(cFirstFrame.m_nLastSent, nCurTime);
		if ((nDiff >= LTMAX(nNAKMS, k_nTrickleNAKDelay)) && !IsFlowControlBlocked(GetUDPPacketSize(cFirstFrame.m_nFrameSize + k_nUDPCommand_Heartbeat_Size + 32)))
		{
			ReSendFrame(cFirstFrame);
			cFirstFrame.m_nLastSent = nCurTime;
			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Trickling on frame %d", cFirstFrame.m_nFrameCode);
			}
		}
		return;
	}
	else
	{
		m_bFlushingNAKs = false;
	}

	if (nWaitingCount > k_nGuaranteedOverflowCount)
	{
		m_bPauseGuaranteed = true;
	}

	uint32 nPacketOverhead = GetUDPPacketSize(k_nUDPCommand_Heartbeat_Size + 32);
	iCurFrame = m_cFrameHistory.begin();
	for (; iCurFrame != m_cFrameHistory.end(); ++iCurFrame)
	{
		if (iCurFrame->m_bReceivedOO)
			continue;
		m_bPauseGuaranteed |= (iCurFrame->m_nSendCount > k_nGuaranteedBlockCount);
		// Should we send a NAK yet?
		uint32 nDiff = nCurTime - LTMIN(iCurFrame->m_nLastSent, nCurTime);
		if (nDiff >= nNAKMS)
		{
			uint32 nNAKSize = iCurFrame->m_nFrameSize;
			if (IsFlowControlBlocked(nPacketOverhead + nNAKSize))
				continue;
			ReSendFrame(*iCurFrame);
			// Take up some more of the outgoing bandwidth so we don't get crowded out
			//UpdateOutgoingFlowControl(iCurFrame->m_nFrameSize / 2, nCurTime);
			iCurFrame->m_nLastSent = nCurTime;
			++(iCurFrame->m_nSendCount);
		}
	}

	if (g_CV_UDPDebug && m_bPauseGuaranteed && !bWasPaused)
	{
		dsi_ConsolePrint("UDP: Guaranteed messages paused");
	}
	else if (g_CV_UDPDebug && !m_bPauseGuaranteed && bWasPaused)
	{
		dsi_ConsolePrint("UDP: Guaranteed messages un-paused");
	}
}

void CUDPConn::SendDisconnectMessage( EDisconnectReason eDisconnectReason )
{
	CPacket_Write cDisconnect;

	cDisconnect.Writebool(true); // UDP message
	cDisconnect.WriteBits(k_nUDPCommand_Disconnect, k_nUDPCommandBits); // Disconnect
	cDisconnect.Writeuint8( eDisconnectReason );
	
	// No guaranteed data
	// No unguaranteed data

	CPacket_Read cMessage(cDisconnect);

	for(uint32 i = 0; i < 4; i++)
	{
		if (g_CV_UDPDebug > 2)
		{
			dsi_ConsolePrint("UDP: Sending disconnect");
		}
		// Make sure they don't get blocked...
		if (i != 0)
			Sleep(k_nDisconnectSleep);
		SendPacket(cMessage);
	}
}

uint32 CUDPConn::GetTimeSinceLastCommunication()
{
	return GetTickCount() - m_nLastRecvTime;
}

bool CUDPConn::FlushOutgoingQueues()
{
	// Send the guaranteed packets we need to send again...
	ReSendLostPackets();

	// Check for sending a heartbeat
	bool bSendHeartbeat  = ShouldSendHeartbeat();
	bool bSendEmptyFrame = ShouldSendEmptyFrame() && !m_bPauseGuaranteed;

	// Jump out if there's nothing to do
	if ((m_bPauseGuaranteed ||
		(m_nOutgoingGCount == 0) &&
		(m_nOutgoingUCount == 0)) &&
		!bSendHeartbeat &&
		!bSendEmptyFrame)
		return true;

	CPacket_Write cOutgoingPacket;

	do
	{
		bool bWasACKPending = m_bACKPending;

		// Send a heartbeat
		cOutgoingPacket.Writebool(bSendHeartbeat);
		if (bSendHeartbeat)
			WriteHeartbeat(cOutgoingPacket);

		// Send the guaranteed info
		WriteGuaranteed(cOutgoingPacket, bSendEmptyFrame);

		// Send the unguaranteed info
		WriteUnguaranteed(cOutgoingPacket);

		// Make sure something wacky didn't happen...
		if (cOutgoingPacket.Empty())
		{
			ASSERT(!"Unable to build outgoing UDP packet!");
			break;
		}

		// Don't send empty packets
		if (cOutgoingPacket.Size() <= 3)
		{
			return true;
		}

		else if (g_CV_UDPDebug > 2)
		{
			dsi_ConsolePrint("UDP: Sending built frame (%d)", cOutgoingPacket.Size());
		}

		// Send it..
		uint32 nPacketSize = GetUDPPacketSize(cOutgoingPacket.Size());
		// If it's going to get dropped, try again later
		if (bWasACKPending && !m_bACKPending && IsFlowControlBlocked(nPacketSize))
		{
			m_bACKPending = true;
			return true;
		}

		bool bResult = SendPacket(CPacket_Read(cOutgoingPacket));
		if (!bResult)
		{
			return false;
		}

		// Jump out if we're waiting for guaranteed stuff to clear
		if (m_bPauseGuaranteed)
		{
			return true;
		}

		// Try not to overload the poor outgoing bandwidth
		if (IsFlowControlBlocked(k_nUDPPacketOverhead + 32))
		{
			return true;
		}

		// Don't send a heartbeat next time through...
		bSendHeartbeat = false;
		bSendEmptyFrame = false;
	} while ((m_nOutgoingGCount + m_nOutgoingUCount) != 0);

	// It would be nice to know why this is asserting -- NYI
	// ASSERT((m_nOutgoingGSize + m_nOutgoingUSize) == 0);

	m_nOutgoingGSize = 0;
	m_nOutgoingUSize = 0;

	return true;
}

void CUDPConn::UpdatePing()
{
	if (!m_bWaitingForPingResponse)
	{
		m_fReportedPing = m_fCurPing; 
		return;
	}

	uint32 nTimeSinceLastPing = GetTickCount() - m_nLastPingTimeStamp;
	m_fReportedPing	= LTMAX(m_fCurPing, (float)nTimeSinceLastPing);
}

void CUDPConn::UpdateBandwidth()
{
	uint32 nCurTime = GetTickCount();
	uint32 nTimeSinceLastIncoming = nCurTime - m_nLastRecvTime;
	if (nTimeSinceLastIncoming >= k_nBandwidthTrackingMaxPeriod)
	{
		m_aIncomingBandwidthHistory.push(CBandwidthPeriod(0, nTimeSinceLastIncoming));
		AccumulateHistory(m_aIncomingBandwidthHistory);
		m_nLastRecvTime = nCurTime;
	}
}

void CUDPConn::Update(bool bFrameUpdate) 
{
	CWinSync_CSAuto cSerialize(m_cUpdateCS);

	UpdateOutgoingFlowControl(0, GetTickCount());

	FlushOutgoingQueues();
	UpdatePing();
	UpdateBandwidth();
}

CPacket_Read CUDPConn::GetPacket()
{
	CWinSync_CSAuto cSerialize(m_cUpdateCS);

	if (m_cIncomingQueue.empty())
		return CPacket_Read();

	CPacket_Read cResult = m_cIncomingQueue.front();
	m_cIncomingQueue.pop_front(s_cPacketTrash);
	return cResult;
}

bool CUDPConn::IsInTrouble()
{
	uint32 nCurTime = GetTickCount();
	// Act like everything's OK if we're not waiting for anything from them
	if (!m_bWaitingForPingResponse)
		return false;
	// We're in trouble if we're not getting a response on our latest ping...
	uint32 nTimeSinceLastPing = GetTickCount() - m_nLastPingTimeStamp;
	return (nTimeSinceLastPing > k_nTroubleDelay);
}

int32 CUDPConn::GetAvailableBandwidth(float fTime) const
{
	// If we're backed up, pause
	if (m_bFlushingNAKs)
		return 0;

	uint32 nCurTime = GetTickCount();

	// How much bandwidth is available over that time?
	int32 nAvailableBandwidth = (int32)((float)GetBandwidth() * fTime);

	// Figure in flow control
	const_cast<CUDPConn*>(this)->UpdateOutgoingFlowControl(0, nCurTime);
	uint32 nFutureTime = nCurTime + (uint32)(fTime * 1000.0f);
	uint32 nTotalFlowControlTime = (nFutureTime - m_nFlowControlPeriodTime);
	uint32 nFlowControlBits = (nTotalFlowControlTime * m_nFlowControlBitsPerPeriod) / k_nFlowControlPeriod;
	uint32 nCurFlowControlBits = m_nFlowControlLastPeriodSent + m_nFlowControlCurPeriodSent;
	if (nFlowControlBits > nCurFlowControlBits)
	{
		// Restrict the available bandwidth to how much the flow control will let us have...
		nFlowControlBits -= nCurFlowControlBits;
		nAvailableBandwidth = LTMIN(nFlowControlBits, nAvailableBandwidth);
	}
	else
	{
		return 0;
	}

	// Start with the guaranteed queue
	int32 nReservedBandwidth = (int32)m_nOutgoingGSize;
	// Add on a guaranteed header for each packet that'll be broken up into
	uint32 nGuaranteedPackets = LTMAX(m_nOutgoingGSize / k_nTargetPacketSize, 1);
	nReservedBandwidth += (int32)(nGuaranteedPackets * (/*k_nUDPPacketOverhead + frame overhead */ k_nFrameBits + 1));
	// Mix in the waiting unguaranteed...
	nReservedBandwidth += (int32)m_nOutgoingUSize;
	// Add pings over that time period
	uint32 nPings = LTMAX((uint32)(fTime * (1000.0f / (float)k_nPingDelay)), 1);
	nReservedBandwidth += (int32)(nPings * (k_nFrameBits + k_nUDPCommandBits));

	if (nAvailableBandwidth < nReservedBandwidth)
		return 0;

	// They can have whatever's left over
	return nAvailableBandwidth - nReservedBandwidth;
}

uint32 CUDPConn::GetBandwidth() const
{
	return m_nReportedBandwidth;
}

bool CUDPConn::SetBandwidth(uint32 nBPS)
{
	if (m_nReportedBandwidth == nBPS)
		return true;
	m_nReportedBandwidth = nBPS;
	m_nBandwidth = LTMIN(m_nMaxBandwidth, (m_nReportedBandwidth * k_nBandwidthUsageTarget) / 100);
	ResetFlowControl();
	return true;
}

uint32 CUDPConn::GetOutgoingBandwidthUsage() const
{
	float fOutgoingSize = (float)m_aOutgoingBandwidthHistory.end()->m_nSize;
	float fOutgoingTime = (float)m_aOutgoingBandwidthHistory.end()->m_nTime;
	if (fOutgoingTime == 0.0f)
		return 0;
	float fResult = (fOutgoingSize / fOutgoingTime) * 1000.0f;
	return (uint32)fResult;
}

uint32 CUDPConn::GetIncomingBandwidthUsage() const
{
	float fIncomingSize = (float)m_aIncomingBandwidthHistory.end()->m_nSize;
	float fIncomingTime = (float)m_aIncomingBandwidthHistory.end()->m_nTime;
	if (fIncomingTime == 0.0f)
		return 0;
	float fResult = (fIncomingSize / fIncomingTime) * 1000.0f;
	return (uint32)fResult;
}

uint32 CUDPConn::GetTransportOverhead() const
{
	// NYI - This really should include packet loss and variable header lengths and that kind of stuff
	return 0;
}

float CUDPConn::GetPacketLoss() const
{
	uint32 nACKCount = 0;
	uint32 nNAKCount = 0;
	TPacketLossHistory::const_iterator iCurPacket = m_aPacketLossHistory.begin();
	for (; iCurPacket != m_aPacketLossHistory.end(); ++iCurPacket)
	{
		++((*iCurPacket) ? nACKCount : nNAKCount);
	}
	if (!nACKCount)
		return (nNAKCount) ? 1.0f : 0.0f;
	return (float)nNAKCount / (float)(nNAKCount + nACKCount);
}

bool CUDPConn::SendPacket(const CPacket_Read &cPacket)
{
	uint32 nCurTime = GetTickCount();
	uint32 nTimeSinceLastSend = nCurTime - m_nLastSendTime;
	m_nLastSendTime = nCurTime;

	uint32 nRealPacketSize = GetUDPPacketSize(cPacket.Size());

	m_aOutgoingBandwidthHistory.push(CBandwidthPeriod(nRealPacketSize, nTimeSinceLastSend));
	AccumulateHistory(m_aOutgoingBandwidthHistory);

	UpdateOutgoingFlowControl(0, nCurTime);

	// Drop packets if we're blocked..
	bool bBlocked = IsFlowControlBlocked(nRealPacketSize);

	if (bBlocked)
	{
		if (g_CV_UDPDebug > 1)
		{
			dsi_ConsolePrint("UDP: Dropping packet %d", nRealPacketSize);
		}
		return true;
	}

	UpdateOutgoingFlowControl(nRealPacketSize, nCurTime);


	return CUDPDriver::SendTo(m_Socket, cPacket, &m_RemoteAddr);


	// Re-package with a fingerprint
	/*
	CPacket_Write cFingerprintPacket;
	uint32 nFingerprint = GetPacketFingerprint(cPacket);
	cFingerprintPacket.WriteBits(nFingerprint, k_nFingerprintBits);
	cFingerprintPacket.WritePacket(cPacket);
	return CUDPDriver::SendTo(m_Socket, CPacket_Read(cFingerprintPacket), &m_RemoteAddr);
	*/
}

//- 넘어오는 cHistory 에다가 순서를 매긴다 !!
//- 호출되는 곳 ( 총 5곳 )
//- 1. CUDPConn::CUDPConn() 에서만 두번 (IncomingPacket, OutgoingPacket)
//- 2. CUDPConn::EIncomingPacketResult CUDPConn::HandleIncomingPacket(const CPacket_Read &cPacket)
//- 3. CUDPConn::UpdateBandwidth()
//- 4. CUDPConn::SendPacket(const CPacket_Read &cPacket)
void CUDPConn::AccumulateHistory(TBandwidthHistory &cHistory)
{
	cHistory.end()->m_nSize = 0;
	cHistory.end()->m_nTime = 0;

	TBandwidthHistory::const_iterator iCurPeriod = cHistory.begin();

	for (; iCurPeriod != cHistory.end(); ++iCurPeriod)
	{
		*cHistory.end() += *iCurPeriod;
	}
}

void CUDPConn::ResetFlowControl()
{
	const uint32 k_nRawBitsPerPeriod = (m_nBandwidth * k_nFlowControlPeriod) / 1000;
	m_nFlowControlBitsPerPeriod = k_nRawBitsPerPeriod;
	if (g_CV_UDPDebug > 1)
	{
		dsi_ConsolePrint("UDP: Reset flow control (%d / %d)", m_nFlowControlBitsPerPeriod, m_nBandwidth);
	}
	m_nFlowControlCurPeriodSent = LTMIN(m_nFlowControlCurPeriodSent, m_nFlowControlBitsPerPeriod);
	m_nFlowControlLastPeriodSent = LTMIN(m_nFlowControlLastPeriodSent, m_nFlowControlBitsPerPeriod);
}

void CUDPConn::UpdateOutgoingFlowControl(uint32 nPacketSize, uint32 nCurTime)
{
	uint32 nOriginalPacketSize = nPacketSize;
	uint32 nTimeOffset = (nCurTime < m_nFlowControlPeriodTime) ? 0 : nCurTime - m_nFlowControlPeriodTime;

	// Are we just starting? (or is our time messed up..)
	if (!m_bFlowControlInitialized)
	{
		m_bFlowControlInitialized = true;

		ResetFlowControl();
		m_nFlowControlPeriodTime = nCurTime;
	}
	// Are we starting a new period?
	else if (nTimeOffset >= k_nFlowControlMinPeriod)
	{
		// Move the current period into last period
		uint32 nPeriodTime = nCurTime - m_nFlowControlPeriodTime;
		uint32 nFlowBits = (m_nBandwidth * nPeriodTime) / 1000;
		if (m_nFlowControlCurPeriodSent >= nFlowBits)
		{
			m_nFlowControlLastPeriodSent += nFlowBits;
			m_nFlowControlCurPeriodSent -= nFlowBits;
		}
		else
		{
			m_nFlowControlCurPeriodSent = 0;
		}
		
		if (m_nFlowControlLastPeriodSent >= nFlowBits)
		{
			m_nFlowControlLastPeriodSent -= nFlowBits;
		}
		else
		{
			m_nFlowControlLastPeriodSent = 0;
		}
		
		// You are now here.
		m_nFlowControlPeriodTime = nCurTime;

		if ((g_CV_UDPDebug > 2) && (!nOriginalPacketSize))
		{
			dsi_ConsolePrint("UDP: Flow control period (%d/%d of %d)", m_nFlowControlLastPeriodSent, m_nFlowControlCurPeriodSent, m_nFlowControlBitsPerPeriod);
		}
	}

	if (nPacketSize)
	{
		// Put whatever we can into last period
		if (m_nFlowControlLastPeriodSent < m_nFlowControlBitsPerPeriod)
		{
			uint32 nUnderflowBits = m_nFlowControlBitsPerPeriod - m_nFlowControlLastPeriodSent;
			if (nUnderflowBits >= nPacketSize)
			{
				m_nFlowControlLastPeriodSent += nPacketSize;
				nPacketSize = 0;
			}
			else
			{
				m_nFlowControlLastPeriodSent = m_nFlowControlBitsPerPeriod;
				nPacketSize -= nUnderflowBits;
			}
		}

		// Put the rest into the current period
		m_nFlowControlCurPeriodSent += nPacketSize;
	}

	if ((g_CV_UDPDebug > 2) && nOriginalPacketSize)
	{
		dsi_ConsolePrint("UDP: Flow control update (%d/%d of %d, %d)", m_nFlowControlLastPeriodSent, m_nFlowControlCurPeriodSent, m_nFlowControlBitsPerPeriod, nOriginalPacketSize);
	}
}

bool CUDPConn::IsFlowControlBlocked(uint32 nPacketSize) const
{
	const_cast<CUDPConn*>(this)->UpdateOutgoingFlowControl(0, GetTickCount());
	bool bBlocked = (m_nFlowControlLastPeriodSent + m_nFlowControlCurPeriodSent + nPacketSize) > (m_nFlowControlBitsPerPeriod * 2);
	if ((g_CV_UDPDebug > 2) && bBlocked)
	{
		dsi_ConsolePrint("UDP: Flow control blocked (%d/%d of %d, %d)", m_nFlowControlLastPeriodSent, m_nFlowControlCurPeriodSent, m_nFlowControlBitsPerPeriod, nPacketSize);
	}

	return bBlocked;
}

// ----------------------------------------------------------------- //
// Internal helpers.
// ----------------------------------------------------------------- //

// Returns a string representing the last TCP/IP error.
char* udp_GetLastError()
{
	int lastError, i;

	lastError = WSAGetLastError();
	for(i=0; i < NUM_UDPERRORSTRINGS; i++)
	{
		if(g_UDPErrorStrings[i].m_ErrorCode == lastError)
			return g_UDPErrorStrings[i].m_ErrorString;
	}

	return "<unknown>";
}


// Builds a socket address from the string.
bool udp_BuildSockaddrFromString(const char *pCmdData, sockaddr_in *pSockInfo)
{
	hostent *pHostName;
	unsigned char *pAddrList;
	u_long longAddr, curPart, i;
	u_short port;
	char ip[256];
	
	char *pTest = strchr(pCmdData, ':');
	if(pTest)
	{
		memcpy(ip, pCmdData, pTest-pCmdData);
		ip[pTest-pCmdData] = 0;
		port = (u_short)atoi(pTest+1);
	}
	else
	{
		port = DEFAULT_LISTENPORT;
		SAFE_STRCPY(ip, pCmdData);
	}

	if(ip[0] >= '0' && ip[0] <= '9')
	{
		// Numeric IP address.
		longAddr = ntohl(inet_addr(ip));
	}
	else
	{
		pHostName = gethostbyname(ip);
		if(!pHostName)
		{
			DebugOut("gethostbyname() returned LTNULL in udp_BuildSockaddrFromString\n");
			return false;
		}

		pAddrList = (unsigned char*)pHostName->h_addr_list[0];
		longAddr = 0;
		for(i=0; i < 4; i++)
		{
			curPart = pAddrList[i];
			curPart <<= (3-i) * 8;
			longAddr |= curPart;
		}
	}

	memset(pSockInfo, 0, sizeof(*pSockInfo));
	pSockInfo->sin_family = AF_INET;
	pSockInfo->sin_addr.s_addr = htonl(longAddr);
	pSockInfo->sin_port = (u_short)htons(port);

	return true;
}


void udp_SetupLocalSockaddr(sockaddr_in *pSockInfo, u_short portNum)
{
	memset(pSockInfo, 0, sizeof(*pSockInfo));
	pSockInfo->sin_family = AF_INET;
	pSockInfo->sin_port = htons(portNum);

	if(g_CV_BindIP)
	{
		pSockInfo->sin_addr.s_addr = inet_addr(g_CV_BindIP);
	}
	else
	{
		pSockInfo->sin_addr.s_addr = INADDR_ANY;
	}
}

int32 udp_GetMaxPossiblePort()
{
  	uint32 maxPossiblePort = ((1 << (sizeof(((sockaddr_in *)0)->sin_port) * 8)) - 1);
	if (maxPossiblePort == 0)
		return -1;
	return maxPossiblePort;
} // udp_GetMaxPossiblePort

static void udp_SaveIPClientPortMRU()  
{
	#ifndef DE_SERVER_COMPILE
	//
	// Saving of the MRU client port is relevant for clients and 
	// synoptic servers only!
	//
	// Note that the MRU will NOT be saved to the configuration file
	// if the configuration file is read-only.
	//
	char tmp [ 32 ];
	LTSNPrintF(tmp, sizeof(tmp), "%d", (int)g_CV_IPClientPortMRU);
	static char sVar [] = "IPClientPortMRU";
	cc_SetConsoleVariable(&g_ClientConsoleState, sVar, tmp);
	#endif // DE_SERVER_COMPILE
} // udp_SaveIPClientPortMRU

static int32 udp_GetFirstClientPort()
{
	//
	// return the first client port to try to bind to in the port 
	// range.  returns zero if client port is specified.
	// 
	if (g_CV_IPClientPort)
	{

		int32 port = g_CV_IPClientPort;

		if (g_CV_IPClientPortRange > 1 &&
			g_CV_IPClientPortMRU != -1)
		{
			int32 maxPort = (g_CV_IPClientPort + (g_CV_IPClientPortRange - 1));

			uint32 maxPossiblePort = udp_GetMaxPossiblePort();
			if (maxPort > (int32)maxPossiblePort)
				maxPort = (int32)maxPossiblePort;

			if (g_CV_IPClientPortMRU)
			{
				port = (g_CV_IPClientPortMRU + 1);
				if (port > maxPort)
					port = g_CV_IPClientPort; // wrap to beginning
				else
				{
					if (port < g_CV_IPClientPort)
						port = g_CV_IPClientPort; // lock into range
				}
			}
			else
			{
				// Start with a random-ish port.
				srand(time_GetMSTime());
				int32 randOffset =  (rand() % g_CV_IPClientPortRange);
				port = (g_CV_IPClientPort + randOffset);
			}
			g_CV_IPClientPortMRU = port;
			//
			// Attempt to save the MRU value in the client's configuration file.
			//
			udp_SaveIPClientPortMRU();
		}
		return port;
	}
	return 0; // no default client port specified
} // udp_GetFirstClientPort

static int32 udp_GetNextClientPort(int32& bindAttemptCount)
{
	ASSERT(g_CV_IPClientPort != 0); // else shouldn't be calling this function!
	if (bindAttemptCount < g_CV_IPClientPortRange)
	{
		int32 maxPort = (g_CV_IPClientPort + (g_CV_IPClientPortRange - 1));

		//
		// See if we can use the MRU value...
		//
		if (g_CV_IPClientPortMRU != -1)
		{
			ASSERT(g_CV_IPClientPortMRU >= g_CV_IPClientPort); // this should be guaranteed by udp_GetFirstClientPort
			ASSERT(g_CV_IPClientPortMRU <= maxPort);	// this should also be guaranteed by udp_GetFirstClientPort	

			if (++g_CV_IPClientPortMRU > maxPort)
				g_CV_IPClientPortMRU = g_CV_IPClientPort; // wrap

			udp_SaveIPClientPortMRU(); // save it

			return g_CV_IPClientPortMRU; // the next port to try...
		}

		//
		// MRU is explicitly turned off, go sequentially through the range 
		// (we started with IPClientPort)
		//
		int32 port = (g_CV_IPClientPort + bindAttemptCount);
		if (port <= maxPort)
			return port;
	}
	return 0; // try the INADDR_ANY port
} // udp_GetNextClientPort

// Binds to the specified port so we can send/receive on it.
// portNum is in host byte order.  If pFinalAddr is non-LTNULL, then
// it is filled in with the final address used.
SOCKET udp_BindToPort(sockaddr_in *pAddr, sockaddr_in *pFinalAddr)
{
	SOCKET ret;
	int status;
	socklen_t nameBufSize;
	unsigned long val; // third arg to Winsock ioctlsocket
	
	// Create a socket to send and receive through.
	ret = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(ret == SOCKET_ERROR)
	{
		DebugOut("socket() returned SOCKET_ERROR.  Last error: %s\n", udp_GetLastError());
		return INVALID_SOCKET;
	}

	// Nonblocking please..
	val = 1;
	status = ioctlsocket(ret, FIONBIO, &val);

	if (status != 0) 
	{
		dsi_ConsolePrint("ioctlsocket FIONBIO (nonblocking) returned %d", status);
		closesocket(ret);
		return INVALID_SOCKET;
	}

	// reuse the address if necessary
	int bReuseAddr = 1; // Winsock "BOOL" must be sizeof(int)
	status = setsockopt(ret, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuseAddr, sizeof(bReuseAddr));
	if (status != 0) 
	{
		dsi_ConsolePrint("setsockopt SO_REUSEADDR returned %d", status);
		closesocket(ret);
		return INVALID_SOCKET;
	}

	// bind to it!
	status = bind(ret, (sockaddr*)pAddr, sizeof(*pAddr));
	if(status == 0)
	{
		if(pFinalAddr)
		{
			nameBufSize = sizeof(*pFinalAddr);
			getsockname(ret, (sockaddr*)pFinalAddr, &nameBufSize);
		}
		
		return ret;
	}
	else
	{
		DebugOut("bind() returned %d.  Last error: %s.\n", status, udp_GetLastError());
		closesocket(ret);
		return INVALID_SOCKET;
	}
}


// Tries to receive incoming data for the socket.  Returns true if data was successfully received
bool udp_RecvFromSocket(SOCKET theSocket, CPacket_Read *pPacket, sockaddr_in *pSender, int *pResultStatus = 0)
{
	const uint32 k_nMaxUDPPacketSize = 8192;
	uint8 aRecvBuffer[k_nMaxUDPPacketSize];

	// Get the data.
	socklen_t fromSize = sizeof(sockaddr_in);
	int status = recvfrom(theSocket, (char*)aRecvBuffer, k_nMaxUDPPacketSize, 0, (struct sockaddr*)pSender, &fromSize);
	if (pResultStatus)
		*pResultStatus = status;

	if (status == SOCKET_ERROR)
	{
		if (pResultStatus)
			*pResultStatus = WSAGetLastError();

		if (WSAGetLastError() != EWOULDBLOCK)
		{
			if(g_CV_UDPDebug > 1)
			{
				dsi_ConsolePrint("UDP: recvfrom returned error %d (max packet size %d)", (int)WSAGetLastError(), k_nMaxUDPPacketSize);
			}
		}

		return false;
	}

	if (status == 0)
	{
		if (g_CV_UDPDebug > 1)
		{
			dsi_ConsolePrint("UDP: recvfrom received a zero-length message");
		}
		return false;
	}
	
	// Dump it into a packet
	CPacket_Write cIncomingPacket;
	cIncomingPacket.WriteData(aRecvBuffer, status * 8);
	*pPacket = CPacket_Read(cIncomingPacket);


#ifdef MURSUM_NET_TRACE
	
	if( !g_hDestWnd ) return true;

	g_IPC_RecvByte.nRecvByte	= status;
	g_IPC_RecvByte.nAddr1		= pSender->sin_addr.S_un.S_un_b.s_b1;
	g_IPC_RecvByte.nAddr2		= pSender->sin_addr.S_un.S_un_b.s_b2;
	g_IPC_RecvByte.nAddr3		= pSender->sin_addr.S_un.S_un_b.s_b3;
	g_IPC_RecvByte.nAddr4		= pSender->sin_addr.S_un.S_un_b.s_b4;

	g_CopyDataStruct.lpData = (char*)&g_IPC_RecvByte;	

	::SendMessage( g_hDestWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&g_CopyDataStruct );

#endif

	return true;
}


// ----------------------------------------------------------------- //
// CUDPDriver code.
// ----------------------------------------------------------------- //

static const char *g_pUDPServiceName = "Internet TCP/IP";

CUDPDriver::CUDPDriver()
{
	m_bHosting = false;
	m_bWSAInitted = false;
	m_Socket = INVALID_SOCKET;
	m_SessionName = LTNULL;
	m_bPassword = false;
	m_nGameType = 0;
	m_nMaxConnections = 0;
	m_QuerySocket = INVALID_SOCKET;
	SAFE_STRCPY(m_Name, "internet");
	m_DriverFlags = NETDRIVER_TCPIP;
	m_nCurPingID = 0;
	memset(&m_cGUID, 0, sizeof(m_cGUID));

	m_LocalConn	= INVALID_SOCKET;
}


CUDPDriver::~CUDPDriver()
{
	Term();
}


bool CUDPDriver::Init()
{
	return true;
}


void CUDPDriver::Term2(bool bFullShutdown)
{
	// Close down any connections.
	m_cCS_Connections.Enter();
	MDeleteAndRemoveElements(m_Connections);
	m_cCS_Connections.Leave();

	// Kill the listen socket.
	if(m_Socket != INVALID_SOCKET)
	{
		StopThread_Listen();
	}

	// Get rid of the query stuff.
	EndQuery();

	if(m_SessionName)
	{
		delete [] m_SessionName;
		m_SessionName = LTNULL;
	}

	m_bPassword = false;
	m_nGameType = 0;

	if(bFullShutdown && INVALID_SOCKET != m_LocalConn)
	{
		closesocket(m_LocalConn);
		m_LocalConn	= INVALID_SOCKET;
	}

	// [NOVICE] BEGIN ------------------------------
#ifdef SA_NETMONITORING
	NFPacketDebuger::GetInstance()->Destroy();
#endif //SA_NETMONITORING
	// [NOVICE] END --------------------------------
}


void CUDPDriver::Term()
{
	m_cCS_Connections.Enter();

	// Flush our queues
	FlushInternalQueues();

	MPOS pos;
	CUDPConn *pConn;

	// Tell all our connections we're going away.
	for(pos=m_Connections; pos; )
	{
		pConn = m_Connections.GetNext(pos);

		pConn->SendDisconnectMessage( DISCONNECTREASON_SHUTDOWN );
	}

	m_cCS_Connections.Leave();

	Term2(false);
}


bool CUDPDriver::SendTo(SOCKET theSocket, const CPacket_Read &cPacket, sockaddr_in *pSendTo)
{
	int status;

	CPacket_Read cReadPacket(cPacket);
	cReadPacket.SeekTo(0);
	int nDataLen = (cReadPacket.Size() + 7) / 8;
	uint8 *aSendBuffer = (uint8 *)alloca(nDataLen);
	cReadPacket.ReadData(aSendBuffer, nDataLen * 8);

	if (g_CV_UDPSimulateCorruption)
	{
		if ((rand() % 100) < g_CV_UDPSimulateCorruption)
		{
			// Screw over the packet...
			uint32 nNumCorruptions = rand() % 10 + 1;
			while (nNumCorruptions--)
			{
				aSendBuffer[rand() % nDataLen] = (uint8)rand();
			}
		}
	}

	status = sendto(theSocket, (char*)aSendBuffer, nDataLen,
		0, (sockaddr*)pSendTo, sizeof(*pSendTo));

#ifdef MURSUM_NET_TRACE
	
	if( !g_hDestWnd ) return status != SOCKET_ERROR;

	g_IPC_SendByte.nSendByte	= status;
	g_IPC_SendByte.nAddr1		= pSendTo->sin_addr.S_un.S_un_b.s_b1;
	g_IPC_SendByte.nAddr2		= pSendTo->sin_addr.S_un.S_un_b.s_b2;
	g_IPC_SendByte.nAddr3		= pSendTo->sin_addr.S_un.S_un_b.s_b3;
	g_IPC_SendByte.nAddr4		= pSendTo->sin_addr.S_un.S_un_b.s_b4;

	g_CopyDataStruct.lpData = (char*)&g_IPC_SendByte;	

	::SendMessage( g_hDestWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&g_CopyDataStruct );

#endif

	return status != SOCKET_ERROR;
}


CUDPConn* CUDPDriver::FindConnByAddr(sockaddr_in *pAddr)
{
	CWinSync_CSAuto cConnProtect(m_cCS_Connections);

	MPOS pos;
	CUDPConn *pConn;

	for(pos=m_Connections; pos; )
	{
		pConn = m_Connections.GetNext(pos);
		
		if(pConn->m_RemoteAddr.sin_port == pAddr->sin_port &&
			pConn->m_RemoteAddr.sin_addr.s_addr == pAddr->sin_addr.s_addr)
		{
			return pConn;
		}
	}
	return LTNULL;
}

LTRESULT CUDPDriver::GetServiceList(NetService* &pListHead)
{
	NetService *pRet;

	LT_MEM_TRACK_ALLOC(pRet = new NetService,LT_MEM_TYPE_NETWORKING);
	if(!pRet)
		return LT_ERROR;

	m_DummyService.m_pDriver = this;
	pRet->m_handle = (HNETSERVICE)&m_DummyService;
	pRet->m_dwFlags = NETSERVICE_TCPIP;
	memset(&pRet->m_guidService, 0, sizeof(pRet->m_guidService));
	SAFE_STRCPY(pRet->m_sName, g_pUDPServiceName);
	
	pRet->m_pNext = pListHead;
	pListHead = pRet;
	return LT_OK;
}


static int FindAddrInList(sockaddr_in *pTest, CMoArray<CUDPQuery> &sessions)
{
	uint32 i;
	
	for(i=0; i < sessions; i++)
	{
		if ((pTest->sin_port == sessions[i].m_Addr.sin_port) &&
			(pTest->sin_addr.S_un.S_addr == sessions[i].m_Addr.sin_addr.S_un.S_addr))
			return (int)i;
	}
	return -1;
}


LTRESULT CUDPDriver::StartQuery(const char *pInfo)
{
	ConParse parse;
	int bTrue, ret;
	u_short port;

	// Clear our old list of sessions.
	EndQuery();

	int32 bindPort = udp_GetFirstClientPort();
	int32 bindAttempts = 0;
	
	while (m_QuerySocket == INVALID_SOCKET)
	{
		udp_SetupLocalSockaddr(&m_QueryAddr, (u_short)bindPort);
		m_QuerySocket = udp_BindToPort(&m_QueryAddr, &m_QueryAddr);
		//
		if (m_QuerySocket == INVALID_SOCKET)
		{
			if (bindPort == 0)
			{
				if(g_CV_UDPDebug)
				{
					dsi_ConsolePrint("UDP: Error: StartQuery bind to port 0 failed!");
				}
				RETURN_ERROR_PARAM(2, CUDPDriver::StartQuery, LT_ERROR, "Can't bind to port");
			}

			if(g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: StartQuery: bind to port %d failed.  Trying next port.", (int)bindPort);
			}
			bindPort = udp_GetNextClientPort(++bindAttempts); // last port to try is always port 0
		}
	}

	// Parse out the list of addresses to query.
	parse.Init(pInfo);
	while(parse.Parse())
	{
		if(parse.m_nArgs > 0)
		{
			CUDPQuery tempQuery;
			
			if(parse.m_Args[0][0] == '*')
			{
				// Broadcast.. get the port.
				char *pTest = strchr(parse.m_Args[0], ':');
				if(pTest)
				{
					port = (u_short)atoi(pTest+1);
				}
				else
				{
					port = DEFAULT_LISTENPORT;
				}

				tempQuery.m_Addr.sin_family = AF_INET;
				tempQuery.m_Addr.sin_port = htons(port);
				tempQuery.m_Addr.sin_addr.s_addr = 0xFFFFFFFF;
				tempQuery.m_CurQueryNum = BROADCAST_QUERYNUM;
				tempQuery.m_nQueriesRemaining = 1;
				m_Queries.Append(tempQuery);
			}
			else
			{			
				if(udp_BuildSockaddrFromString(parse.m_Args[0], &tempQuery.m_Addr))
				{
					tempQuery.m_nQueriesRemaining = (uint32)(QUERY_TIME / QUERY_SEND_INTERVAL) + 1;
					m_Queries.Append(tempQuery);
				}
			}
		}
	}


	// Set it up so our broadcast works.
	bTrue = true;
	ret = setsockopt(m_QuerySocket, SOL_SOCKET, SO_BROADCAST, (char*)&bTrue, sizeof(bTrue));

	m_LastQuerySendTime = time_GetTime() - (QUERY_SEND_INTERVAL*2.0f);
	UpdateQuery();
	return LT_OK;
}


LTRESULT CUDPDriver::UpdateQuery()
{
	CUDPQuery *pQuery;
	uint32 i;
	
	if (m_QuerySocket == INVALID_SOCKET)
	{
		RETURN_ERROR(1, UpdateQuery, LT_NOTINITIALIZED);
	}

	// Send out a request?
	if ((time_GetTime() - m_LastQuerySendTime) >= QUERY_SEND_INTERVAL)
	{
		// Send out requests again.
		for (i=0; i < m_Queries; i++)
		{
			pQuery = &m_Queries[i];

			if (!pQuery->m_nQueriesRemaining)
				continue;

			--(pQuery->m_nQueriesRemaining);

			CPacket_Write cQuery;
			cQuery.Writeuint32(UNCONNECTED_DATA_TOKEN);
			cQuery.WriteBits(UNCONNECTED_MSG_QUERY, UNCONNECTED_MSG_BITS);
			cQuery.WriteType(m_pNetMgr->m_guidApp);
			
			uint8 nQueryNum;
			if(pQuery->m_CurQueryNum == BROADCAST_QUERYNUM)
			{
				nQueryNum = 0xFF;
			}
			else
			{	
				nQueryNum = (uint8)(pQuery->m_CurQueryNum % MAX_UDP_QUERY_TIMES);
				++pQuery->m_CurQueryNum;
				pQuery->m_QueryTimes[nQueryNum] = time_GetTime();
			}
			cQuery.Writeuint8(nQueryNum);
			
			SendTo(m_QuerySocket, CPacket_Read(cQuery), &m_Queries[i].m_Addr);
		}

		m_LastQuerySendTime = time_GetTime();
	}


	// Check for responses.
	CPacket_Read cIncomingPacket;
	sockaddr_in senderAddr;
	while (udp_RecvFromSocket(m_QuerySocket, &cIncomingPacket, &senderAddr))
	{
		if (cIncomingPacket.EOP())
		{
			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Zero-length UDP packet discarded in UpdateQuery");
			}
			continue;
		}

		// Is this a valid response?
		uint32 nPacketToken = cIncomingPacket.Readuint32();
		if (nPacketToken != UNCONNECTED_DATA_TOKEN)
			continue;

		uint32 nMsgID = cIncomingPacket.ReadBits(UNCONNECTED_MSG_BITS);
		if (nMsgID != UNCONNECTED_MSG_QUERY_RESPONSE)
			continue;

		// Check the version
		bool bGoodVersion = cIncomingPacket.Readbool();
		if (!bGoodVersion)
		{
			// Note : This should really notify the caller somehow...
			// But if we return an invalid version code, it'll cancel the whole query.
			continue;
		}

		int iQuery = FindAddrInList(&senderAddr, m_Queries);
		if (iQuery == -1)
		{
			CUDPQuery newQuery;
			memset(&newQuery, 0, sizeof(newQuery));
			newQuery.m_Addr = senderAddr;
			if (m_Queries.Append(newQuery))
			{
				iQuery = (int)m_Queries.LastI();
			}
		}

		if (iQuery == -1)
			continue;

		pQuery = &m_Queries[iQuery];

		// Update the name.
		if (pQuery->m_pName)
		{
			delete pQuery->m_pName;
		}

		uint32 nNameLen = cIncomingPacket.PeekString(0, 0) + 1;
		LT_MEM_TRACK_ALLOC(pQuery->m_pName = new char[nNameLen],LT_MEM_TYPE_NETWORKING);
		cIncomingPacket.ReadString(pQuery->m_pName, nNameLen);

		// Update ping?
		uint32 nQueryNum = cIncomingPacket.Readuint8();
		if (nQueryNum < MAX_UDP_QUERY_TIMES && nQueryNum != BROADCAST_QUERYNUM)
		{
			if (pQuery->m_QueryTimes[nQueryNum] == 0.0f)
			{
				pQuery->m_Ping = 0.0f;
			}
			else
			{
				pQuery->m_Ping = time_GetTime() - pQuery->m_QueryTimes[nQueryNum];
			}
		}

		// Password?
		pQuery->m_bPassword = cIncomingPacket.Readbool();

		// GameType?
		pQuery->m_nGameType = cIncomingPacket.Readuint8();

		// We've gotten a response from this one.  Stop bothering them.
		pQuery->m_nQueriesRemaining = 0;
	}

	return LT_OK;
}


LTRESULT CUDPDriver::GetQueryResults(NetSession* &pListHead)
{
	uint32 i;
	CUDPQuery *pQuery;
	CUDPSession *pSession;

	pListHead = LTNULL;

	// Copy in the list for them.
	for(i=0; i < m_Queries; i++)
	{
		pQuery = &m_Queries[i];

		if(pQuery->m_pName)
		{
			LT_MEM_TRACK_ALLOC(pSession = new CUDPSession(this),LT_MEM_TYPE_NETWORKING);
			if(pSession)
			{
				pSession->m_Addr = pQuery->m_Addr;
				SAFE_STRCPY(pSession->m_sName, pQuery->m_pName);
				pSession->m_Ping = (uint32)(pQuery->m_Ping * 1000.0f);
				
				// Fill in its unique guid.
				memset(&pSession->m_guidInst, 0, sizeof(pSession->m_guidInst));
				pSession->m_guidInst.guid.a = (uint32)pQuery;

				memset(&pSession->m_guidApp, 0, sizeof(pSession->m_guidApp));

				// Fill in the host info.
				LTSNPrintF(pSession->m_HostIP, MAX_HOSTIP_LEN, "%d.%d.%d.%d", EXPAND_BASEADDR(pQuery->m_Addr));
				pSession->m_HostPort = ntohs(pSession->m_Addr.sin_port);

				pSession->m_bHasPassword = pQuery->m_bPassword;
				pSession->m_nGameType = pQuery->m_nGameType;

				pSession->m_pNext = pListHead;
				pListHead = pSession;
			}
		}
	}

	return LT_OK;
}


LTRESULT CUDPDriver::EndQuery()
{
	if(m_QuerySocket != INVALID_SOCKET)
	{
		closesocket(m_QuerySocket);
		m_QuerySocket = INVALID_SOCKET;
	}

	m_Queries.Term();
	return LT_OK;
}


LTRESULT CUDPDriver::GetSessionList(NetSession* &pListHead, const char *pInfo)
{
	LTRESULT dResult;
	float startTime;

	pListHead = LTNULL;
	if((dResult = StartQuery(pInfo)) != LT_OK)
		return dResult;

	startTime = time_GetTime();
	while((time_GetTime() - startTime) < QUERY_TIME)
	{
		if((dResult = UpdateQuery()) != LT_OK)
			return dResult;
	}

	GetQueryResults(pListHead);
	EndQuery();
	return LT_OK;
}


LTRESULT CUDPDriver::GetSessionName(char* sName, uint32 dwBufferSize)
{
	CWinSync_CSAuto cProtection(m_cCS_SessionData);

	sName[0] = 0;
	if(m_SessionName)
	{
		LTStrCpy(sName, m_SessionName, dwBufferSize);
	}
	return LT_OK;
}


LTRESULT CUDPDriver::SetSessionName(const char* sName)
{
	CWinSync_CSAuto cProtection(m_cCS_SessionData);
	
	if(m_SessionName && strlen(m_SessionName) >= strlen(sName))
	{
		// Avoid the allocation/deallocation if possible.
		strcpy(m_SessionName, sName);
		return LT_OK;
	}
	else
	{
		delete [] m_SessionName;
		LT_MEM_TRACK_ALLOC(m_SessionName = new char[strlen(sName)+1],LT_MEM_TYPE_NETWORKING);
		if(m_SessionName)
		{
			strcpy(m_SessionName, sName);
			return LT_OK;
		}
		else
		{
			return LT_ERROR;
		}
	}
}

LTRESULT CUDPDriver::GetMaxConnections(uint32 &nMaxConnections)
{
	CWinSync_CSAuto cProtection(m_cCS_SessionData);

	nMaxConnections = m_nMaxConnections;
	return LT_OK;
}

void CUDPDriver::FlushInternalQueues()
{
	FlushDisconnectQueue();
	FlushConnectQueue();
	FlushUnknownMessageQueue();
}

void CUDPDriver::FlushDisconnectQueue()
{
	CWinSync_CSAuto cConnProtect(m_cCS_Connections);

	// Disconnect any un-believers
	CWinSync_CSAuto cDisconnectProtect(m_cCS_DisconnectQueue);

	while (!m_cDisconnectQueue.empty())
	{
		CDisconnectRequest &cRequest = m_cDisconnectQueue.front();
		// Make sure it's still a valid connection...
		bool bValidConnection = false;
		MPOS pCurPos = m_Connections.GetHeadPosition();
		while (pCurPos)
		{
			CUDPConn *pCurConn = m_Connections.GetNext(pCurPos);
			if (pCurConn == cRequest.m_pConnection)
			{
				bValidConnection = true;
				break;
			}
		}
		if (bValidConnection)
		{
			Disconnect(cRequest.m_pConnection, cRequest.m_eReason, false);
		}
		m_cDisconnectQueue.pop_front();
	}
}

void CUDPDriver::FlushConnectQueue()
{
	if( false == m_cConnectList.empty() )
	{
		// Connect anyone new
		CWinSync_CSAuto cConnectProtect(m_cCS_ConnectQueue);

		for( CON_LIST_ITR itr=m_cConnectList.begin() ; itr!=m_cConnectList.end() ; ++itr )
		{
			CConnectRequest* pRequest = (CConnectRequest*)(*itr);

			if (!m_pNetMgr->NewConnectionNotify(pRequest->m_pConnection))
			{
				// Um..  Whoops.  Give 'em the boot.
				Disconnect(pRequest->m_pConnection, DISCONNECTREASON_KICKED);
			}

			delete pRequest;
		}

		m_cConnectList.clear();
	}

	/*
	while (!m_cConnectQueue.empty())
	{
		CConnectRequest &cRequest = m_cConnectQueue.front();

		// Tell the NetMgr they're here
		dsi_ConsolePrint( "[UDP_DRV] New Connection Notify  0x%08x", cRequest.m_pConnection );
		if (!m_pNetMgr->NewConnectionNotify(cRequest.m_pConnection))
		{
			// Um..  Whoops.  Give 'em the boot.
			Disconnect(cRequest.m_pConnection, DISCONNECTREASON_KICKED);
		}
		
		m_cConnectQueue.pop_front();
	}
	*/
}

void CUDPDriver::FlushUnknownMessageQueue()
{
	// Send any unknown messages that got queued
	CWinSync_CSAuto CUnknownMessageProtect(m_cCS_UnknownMessages);

	// Jump out if there's no handler
	if (!m_pNetMgr->m_pHandler)
	{
		m_cUnknownMessages.clear();
		return;
	}

	while (!m_cUnknownMessages.empty())
	{
		CUnknownMessage &cMsg = m_cUnknownMessages.front();

		uint8 senderIP[4];
		uint16 senderPort;
		senderIP[0] = INADDR_B1(cMsg.m_cSender);
		senderIP[1] = INADDR_B2(cMsg.m_cSender);
		senderIP[2] = INADDR_B3(cMsg.m_cSender);
		senderIP[3] = INADDR_B4(cMsg.m_cSender);
		senderPort = ntohs(cMsg.m_cSender.sin_port);
		m_pNetMgr->m_pHandler->HandleUnknownPacket(cMsg.m_cPacket, senderIP, senderPort);

		m_cUnknownMessages.pop_front();
	}
}

void CUDPDriver::Update()
{
	CWinSync_CSAuto cConnProtect(m_cCS_Connections);

	FlushDisconnectQueue();

	FlushConnectQueue();

	FlushUnknownMessageQueue();

	// Update the connections
	MPOS pCurPos = m_Connections.GetHeadPosition();
	while (pCurPos)
	{
		MPOS pNextPos = pCurPos;
		CUDPConn *pCurConn = m_Connections.GetNext(pNextPos);
		pCurConn->Update(true);
		// Kick 'em if they get out of line
		/*
		if (pCurConn->IsInTrouble())
		{
			Disconnect(pCurConn, DISCONNECTREASON_DEAD, true);
		}
		*/
		pCurPos = pNextPos;
	}
}


void CUDPDriver::Disconnect(CBaseConn *id, EDisconnectReason reason, bool bSendMessage)
{
	CWinSync_CSAuto cConnProtect(m_cCS_Connections);

	CUDPConn *pConn;
	
	if (!id)
		return;

	pConn = (CUDPConn*)id;

	if(g_CV_UDPDebug)
	{
		dsi_ConsolePrint("UDP: CUDPDriver::Disconnect (%d) on %d.%d.%d.%d:%d",
			reason, EXPAND_ADDR(pConn->m_RemoteAddr));
	}

	m_pNetMgr->DisconnectNotify(id, reason);
	if (bSendMessage)
		pConn->SendDisconnectMessage( reason );

	m_Connections.RemoveAt(&pConn->m_Node);

	//[MURSUM] ConnectList에 존재하면 리스트에서 삭제하여 잘못된 포인터 참조 방지.

	if( false == m_cConnectList.empty() )
	{
		for( CON_LIST_ITR itr=m_cConnectList.begin() ; itr!=m_cConnectList.end() ; )
		{
			CConnectRequest* pRequest = (CConnectRequest*)(*itr);

			if( pRequest->m_pConnection == pConn )
			{
				delete pRequest;
				itr = m_cConnectList.erase(itr);
			}
			else
			{
				++itr;
			}	
		}
	}

	delete pConn;
}

LTRESULT CUDPDriver::HostSession(NetHost* pHost,SOCKET Sock)
{
	Term2(false);
	m_Socket		= Sock;
	if( INVALID_SOCKET != m_LocalConn ) 
		closesocket(m_LocalConn);
	m_LocalConn		= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_nLocalPort	= pHost->m_Port;
	
	// Start listening...
	StartThread_Listen();

	m_cCS_SessionData.Enter();
	m_bHosting = true;
	m_nMaxConnections = pHost->m_dwMaxConnections;
	SetSessionName(pHost->m_sName);
	SetHasPassword(pHost->m_bHasPassword);
	SetGameType(pHost->m_nGameType);
	m_cCS_SessionData.Leave();

	return LT_OK;
}


LTRESULT CUDPDriver::JoinSession(NetSession *pNetSession)
{
	CUDPSession *pSession;

	pSession = (CUDPSession*)pNetSession;
	ASSERT(pSession->m_pDriver == this); // Make sure we're the right driver.
	return ReallyJoinSession(0, &pSession->m_Addr);
}

LTRESULT CUDPDriver::StartCheckConeType( SOCKET Sock, const char* lpszIP1, uint16 nPort1,
										 const char* lpszIP2, uint16 nPort2, const char* lpszID, uint8 nIDLen )
{
	Term2(false);

	m_nConeType			= 0x00000008;
	m_nLocalIDLen		= nIDLen * 8;
	m_nGameServerPort1	= nPort1;
	m_nGameServerPort2	= nPort2;

	memset( m_szGameServerIP1, 0, 16 );
	if( lpszIP1 ) strcpy( m_szGameServerIP1, lpszIP1 );
	memset( m_szGameServerIP2, 0, 16 );
	if( lpszIP2 ) strcpy( m_szGameServerIP2, lpszIP2 );

	memset( m_szLocalID, 0, 256 );
	strcpy( m_szLocalID, lpszID );

	m_Socket = Sock;
		
	StartThread_CheckCone();

	return LT_OK;
}

void CUDPDriver::StopCheckConeType()
{
	StopThread_CheckCone();
	m_Socket	= INVALID_SOCKET;
}


void CUDPDriver::HandleUnconnectedData(CPacket_Read &cPacket, sockaddr_in *pSender)
{
	// Skip the unconnected data tag
	cPacket.SeekTo(32);

	// Get the sub-command
	uint32 nSubMsg = cPacket.ReadBits(UNCONNECTED_MSG_BITS);

	switch (nSubMsg)
	{
		case UNCONNECTED_MSG_QUERY :
		{
			CPacket_Write cResponse;

			// Make sure it's the right app.
			LTGUID theirGuid;
			cPacket.ReadType(&theirGuid);
			m_cCS_GUID.Enter();
			bool bValidGUID = (memcmp(&theirGuid, &m_cGUID, sizeof(theirGuid)) == 0);
			m_cCS_GUID.Leave();
			if (!bValidGUID)
			{
				cResponse.Writeuint32(UNCONNECTED_DATA_TOKEN);
				cResponse.WriteBits(UNCONNECTED_MSG_QUERY_RESPONSE, UNCONNECTED_MSG_BITS);
				cResponse.Writebool(false); // Wrong version
				SendTo(m_Socket, CPacket_Read(cResponse), pSender);
				return;
			}

			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Received query from %d.%d.%d.%d:%d, sending response", EXPAND_ADDR(*pSender));
			}

			uint32 nQueryNum = cPacket.Readuint8();

			// Build the response.
			cResponse.Writeuint32(UNCONNECTED_DATA_TOKEN);
			cResponse.WriteBits(UNCONNECTED_MSG_QUERY_RESPONSE, UNCONNECTED_MSG_BITS);
			cResponse.Writebool(true); // Correct version
			m_cCS_SessionData.Enter();
			cResponse.WriteString(m_SessionName);
			cResponse.Writeuint8(nQueryNum);
			cResponse.Writebool(m_bPassword);
			cResponse.Writeuint8(m_nGameType);
			m_cCS_SessionData.Leave();

			SendTo(m_Socket, CPacket_Read(cResponse), pSender);
			break;
		}
		case UNCONNECTED_MSG_CONNECT :
		{
			//[MURSUM]===============================
			m_cCS_SessionData.Enter();
			uint32 nMaxConnections = m_nMaxConnections;
			m_cCS_SessionData.Leave();

			if( nMaxConnections <= 0 )
			{
				CPacket_Write cResponse_Write;
				cResponse_Write.Writeuint32(UNCONNECTED_DATA_TOKEN);
				cResponse_Write.WriteBits(UNCONNECTED_MSG_CONNECT_RESPONSE, UNCONNECTED_MSG_BITS);
				cResponse_Write.Writeuint8(UNCONNECTED_MSG_HOST_NOT_READY);
				break;
			}
			//========================================

			bool bNewConnection = true;
			{
				CWinSync_CSAuto cConnProtect(m_cCS_Connections);

				// Ignore queries from connections we already know about
				CUDPConn *pConn = FindConnByAddr(pSender);
				if (pConn)
				{
					// Find out if we should give them the boot first
					if (pConn->GetTimeSinceLastCommunication() < k_nReconnection_Delay)
					{
						bNewConnection = false;
					}
					else
					{
						// Queue up a disconnection and break out..  They'll send another request later
						CWinSync_CSAuto cDisconnectProtect(m_cCS_DisconnectQueue);
						CDisconnectRequest cDisconnect;
						cDisconnect.m_pConnection = pConn;
						cDisconnect.m_eReason = DISCONNECTREASON_DEAD;
						LT_MEM_TRACK_ALLOC(m_cDisconnectQueue.push_back(cDisconnect), LT_MEM_TYPE_NETWORKING);
						break;
					}
				}
			}

			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Received conn request from %d.%d.%d.%d:%d, sending response",
					EXPAND_ADDR(*pSender));
			}

			bool bAllow = true;
			bool bWrongVersion = false;

			//[MURSUM]
			/*
			m_cCS_SessionData.Enter();
			uint32 nMaxConnections = m_nMaxConnections;
			m_cCS_SessionData.Leave();
			*/

			// Check the version number
			LTGUID theirGuid;
			cPacket.ReadType(&theirGuid);
			m_cCS_GUID.Enter();
			bool bValidGUID = (memcmp(&theirGuid, &m_cGUID, sizeof(theirGuid)) == 0);
			m_cCS_GUID.Leave();
			if (!bValidGUID)
			{
				// Wrong app version.. they can't connect.
				bAllow = false;
				bWrongVersion = true;
			}
			else if (m_Connections.GetSize() >= nMaxConnections)
			{
				bAllow = false;
			}
			else if (bNewConnection)
			{
				// Make a new connection
				CUDPConn *pConn;
				LT_MEM_TRACK_ALLOC(pConn = new CUDPConn,LT_MEM_TYPE_NETWORKING);
				pConn->m_RemoteAddr = *pSender;
				pConn->m_Socket = m_Socket;
				pConn->m_pDriver = this;
				ASSERT(cPacket.Peekuint32() > 8000);
				pConn->SetMaxBandwidth(cPacket.Readuint32());
				
				// Add them to our connection list
				m_cCS_Connections.Enter();
				m_Connections.AddHead(pConn, &pConn->m_Node);
				m_cCS_Connections.Leave();

				// Add it to the connection queue
				CWinSync_CSAuto cConnQueueProtection(m_cCS_ConnectQueue);
				
				CConnectRequest* pRequest = new CConnectRequest;
				pRequest->m_pConnection = pConn;
				m_cConnectList.push_back(pRequest);
				
				/*
				CConnectRequest cConnRequest;
				cConnRequest.m_pConnection = pConn;
				dsi_ConsolePrint( "[UDP_DRV] New Connection  0x%08x", pConn );
				LT_MEM_TRACK_ALLOC(m_cConnectQueue.push_back(cConnRequest), LT_MEM_TYPE_NETWORKING);
				*/
			}

			// Send a response
			CPacket_Write cResponse_Write;
			cResponse_Write.Writeuint32(UNCONNECTED_DATA_TOKEN);
			cResponse_Write.WriteBits(UNCONNECTED_MSG_CONNECT_RESPONSE, UNCONNECTED_MSG_BITS);						

			if( bAllow )
			{
				cResponse_Write.Writeuint8(UNCONNECTED_MSG_SUCCESS);
			}
			else
			{
				if( bWrongVersion )
				{
					cResponse_Write.Writeuint8(UNCONNECTED_MSG_WRONG_VERSION);
				}
				else
				{
					cResponse_Write.Writeuint8(UNCONNECTED_MSG_HOST_FULL);
				}
			}
					
			CPacket_Read cResponse(cResponse_Write);
			bool bSendResult = SendTo(m_Socket, cResponse, pSender);
			if (!bSendResult)
			{
				ASSERT(!"Error sending connection response");
				break;
			}
			break;
		}
		case UNCONNECTED_MSG_PING :
		{
			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Received ping request from %d.%d.%d.%d:%d, sending response",
					EXPAND_ADDR(*pSender));
			}

			// Send a response
			CPacket_Write cResponse;

			cResponse.Writeuint32(UNCONNECTED_DATA_TOKEN);
			cResponse.WriteBits(UNCONNECTED_MSG_PING_RESPONSE, UNCONNECTED_MSG_BITS);
			cResponse.WriteBits(cPacket.ReadBits(k_nPing_Bits), k_nPing_Bits);

			SendTo(m_Socket, CPacket_Read(cResponse), pSender);
			break;
		}
		case UNCONNECTED_MSG_PING_RESPONSE :
		{
			if (g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Received ping response from %d.%d.%d.%d:%d",
					EXPAND_ADDR(*pSender));
			}

			uint32 nCurTime = GetTickCount();

			uint32 nPingID = cPacket.ReadBits(k_nPing_Bits);

			// Do a look-up
			CWinSync_CSAuto cProtection(m_cCS_ActivePings);
			TPingMap::iterator iPing = m_cActivePings.find(nPingID);
			if (iPing == m_cActivePings.end())
			{
				// Ignore unknown pings
				break;
			}
			// Finalize the ping
			iPing->second.m_nStatus = PING_STATUS_SUCCESS;
			iPing->second.m_nTimeStamp = nCurTime - iPing->second.m_nTimeStamp;			
			break;
		}
		default :
		{
			if (g_CV_UDPDebug)
				dsi_ConsolePrint("UDP: Got invalid unconnected message %d from %d.%d.%d.%d:%d", nSubMsg, EXPAND_ADDR(*pSender));
			break;
		}
	}
}

//[MURSUM] Cone
void CUDPDriver::HandleCheckConeType( CPacket_Read &cPacket )
{
	cPacket.SeekTo(32);

	// Get the sub-command
	uint32 nConeType = cPacket.Readuint32();
	m_nConeType	|= nConeType;
	dsi_ConsolePrint( "Recv cone : 0x%08x", nConeType );
}

bool CUDPDriver::GetPacket(CPacket_Read *pPacket, CBaseConn **pSender)
{
	CWinSync_CSAuto cConnProtect(m_cCS_Connections);

	// Flush the queues, just in case something got processed since the update
	FlushInternalQueues();

	// Ask the connections for any pending packets
	MPOS pCurPos = m_Connections.GetHeadPosition();
	while (pCurPos)
	{
		CUDPConn *pCurConn = m_Connections.GetNext(pCurPos);
		CPacket_Read cNewPacket = pCurConn->GetPacket();
		if (cNewPacket.Empty())
			continue;
		// Return the packet
		*pPacket = cNewPacket;
		*pSender = pCurConn;
		return true;
	}
	
	return false;
}

/*#################################################################################################
	자.. 패킷 보내는 놈이다. 아주 자세히 봐야한다.
	CUDPDriver 측에서 패킷 보내는건 CUDPConn 큐에 넣는 작업이다.
	실제적으로 CNetMgr 에서 CUDPDriver::SendPacket 을 쓰는것이기 때문에
	이렇게 큐에 들어간 다음에 CUDPConn 쪽에서 기냥 가져다가 쓰는거라고 생각하면 된다.
#################################################################################################*/
bool CUDPDriver::SendPacket(const CPacket_Read &cPacket, CBaseConn *idSendTo, bool bGuaranteed)
{
	CUDPConn *pConn;

	if(!idSendTo || !m_Socket)
		return false;

	CWinSync_CSAuto cConnProtect(m_cCS_Connections);

	pConn = (CUDPConn*)idSendTo;

	// Forgive any past transgressions
	WSASetLastError(0);

	// Queue the packet
	bool bResult = pConn->QueuePacket(cPacket, bGuaranteed);
	if (!bResult)
	{
		switch (WSAGetLastError())
		{
			// if the guy on the other end is gone, kill the RCC
			case ECONNRESET:
				Disconnect(pConn, DISCONNECTREASON_DEAD, true);
				break;
			// if the socket send/output buffer if full we should try again later
			// rather than timing out waiting for the ACK.
			case EWOULDBLOCK:
				return false;
		}
	}

	return bResult;
}


bool CUDPDriver::GetLocalIpAddress(char* sBuffer, uint32 dwBufferSize, uint16 &hostPort)
{
	hostent *pHostName;
	char name[256];
	int status;
	unsigned char *pAddrList;
	u_long longAddr, curPart;
	int iAddrList, i, nAddr;
	sockaddr_in tempInfo;

	if (IsHosting())
	{
		hostPort = ntohs(m_HostAddr.sin_port);
	}
	else
	{
		hostPort = 0xFFFF;
	}


	// If they specified an IP, use that.
	if(g_CV_IP && g_CV_IP[0] != 0)
	{
		LTStrCpy(sBuffer, g_CV_IP, dwBufferSize);
		return true;
	}


	// Setup a sockaddr_in to bind to the socket with.
	status = gethostname(name, sizeof(name));
	if(status != 0)
	{
		DebugOut("gethostname() returned %d\n", status);
		return false;
	}

	pHostName = gethostbyname(name);
	if(!pHostName)
	{
		DebugOut("gethostbyname() returned LTNULL in udp_BuildSockaddrFromString\n");
		return false;
	}

	// If they did a +ip on the command line, use the one they asked for.
	for(nAddr=0; nAddr < 10000; nAddr++)
	{
		if(!pHostName->h_addr_list[nAddr])
			break;
	}

	if(nAddr == 0)
	{
		dsi_ConsolePrint("No IP devices found!");
		return false;
	}

	iAddrList = 0;
	if(g_CV_UDPDebug > 2)
	{
		dsi_ConsolePrint("UDP: ---- %d IP device%s ----", nAddr, nAddr>1?"s":"");
		for(i=0; i < nAddr; i++)
		{
			pAddrList = (unsigned char*)pHostName->h_addr_list[i];
			dsi_ConsolePrint("UDP: %d.%d.%d.%d", pAddrList[0], pAddrList[1], pAddrList[2], pAddrList[3]);
		}
	}

	pAddrList = (unsigned char*)pHostName->h_addr_list[iAddrList];
	longAddr = 0;
	for(i=0; i < 4; i++)
	{
		curPart = pAddrList[i];
		curPart <<= (3-i) * 8;
		longAddr |= curPart;
	}

	tempInfo.sin_addr.s_addr = htonl(longAddr);
	tempInfo.sin_port = m_HostAddr.sin_port;

	// Print it into the buffer.
	LTSNPrintF(sBuffer, dwBufferSize, "%d.%d.%d.%d", EXPAND_BASEADDR(tempInfo));

	if(g_CV_UDPDebug > 2)
	{
		dsi_ConsolePrint("UDP: SetupLocalSockaddr: %d.%d.%d.%d:%d", EXPAND_ADDR(tempInfo));
	}

	return true;
}

LTRESULT CUDPDriver::ReallyJoinSession(SOCKET Sock, sockaddr_in *addr)
{
	uint32 startTime, curTime, lastSendTime;
	sockaddr_in queryAddr, senderAddr, myAddr;

	int status;
	socklen_t size;
	CUDPConn *pConn;
	char addrStr[128];

	// Remember the remote address, then get rid of all our connections and stuff.
	queryAddr = *addr;
	Term2(false);

	SOCKET theSocket = Sock;
	
	CPacket_Write cConnectionPacket_Write;
	cConnectionPacket_Write.Writeuint32(UNCONNECTED_DATA_TOKEN);
	cConnectionPacket_Write.WriteBits(UNCONNECTED_MSG_CONNECT, UNCONNECTED_MSG_BITS);
	cConnectionPacket_Write.WriteType(m_pNetMgr->m_guidApp);
	ASSERT(g_CV_BandwidthTargetClient > 8000);
	cConnectionPacket_Write.Writeuint32(g_CV_BandwidthTargetClient);
	CPacket_Read cConnectionPacket(cConnectionPacket_Write);

	// Send off and wait for a response.
	startTime = GetTickCount();
	lastSendTime = startTime - CONN_SEND_INTERVAL;
	do
	{
		curTime = GetTickCount();

		// Ask if we can join.
		if((curTime - lastSendTime) >= CONN_SEND_INTERVAL)
		{
			if(g_CV_UDPDebug)
			{
				dsi_ConsolePrint("UDP: Sending conn request to %d.%d.%d.%d:%d",
					EXPAND_ADDR(queryAddr));
			}

			SendTo(theSocket, cConnectionPacket, &queryAddr);
			lastSendTime = curTime;
		}

		// See if we've gotten a response yet.
		CPacket_Read cResponsePacket;
		if (!udp_RecvFromSocket(theSocket, &cResponsePacket, &senderAddr))
			continue;

		if (cResponsePacket.Readuint32() != UNCONNECTED_DATA_TOKEN)
		{
			// Note : There's a possibility this may mean we're already connected,
			// and we should be getting heartbeat messages.  That can't be handled
			// Reliably, so the message is get ignored, and the connection will
			// time out from the other end.
			continue;
		}

		uint32 sub = cResponsePacket.ReadBits(UNCONNECTED_MSG_BITS);
		if (sub != UNCONNECTED_MSG_CONNECT_RESPONSE)
		{
			// Something must have gotten delayed or something....
			continue;
		}

		switch( cResponsePacket.Readuint8() )
		{
		case UNCONNECTED_MSG_SUCCESS:
			break;		

		case UNCONNECTED_MSG_WRONG_VERSION:
			RETURN_ERROR_PARAM(2, CUDPDriver::JoinSession, LT_NOTSAMEGUID, addrStr);

		case UNCONNECTED_MSG_HOST_FULL:
			RETURN_ERROR_PARAM(2, CUDPDriver::JoinSession, LT_REJECTED, addrStr);

		default:
		case UNCONNECTED_MSG_HOST_NOT_READY:
			startTime = GetTickCount();
			continue;
		}

		if (g_CV_UDPDebug)
		{
			dsi_ConsolePrint("UDP: Connection to %d.%d.%d.%d:%d accepted",
				EXPAND_ADDR(queryAddr));
			dsi_ConsolePrint("UDP: recvfrom addr: %d.%d.%d.%d:%d",
				EXPAND_ADDR(senderAddr));
		}

		CWinSync_CSAuto cConnProtect(m_cCS_Connections);

		LT_MEM_TRACK_ALLOC(pConn = new CUDPConn,LT_MEM_TYPE_NETWORKING);
		if (!pConn)
			break;
		pConn->m_RemoteAddr = senderAddr;
		pConn->m_Socket = theSocket;
		pConn->m_pDriver = this;
		
		if(m_pNetMgr->NewConnectionNotify(pConn))
		{
			m_Connections.AddHead(pConn, &pConn->m_Node);
			
			m_Socket = theSocket;
			StartThread_Listen();

			if(g_CV_UDPDebug)
			{
				size = sizeof(myAddr);
				status = getsockname(m_Socket, (sockaddr*)&myAddr, &size);
				if(status == 0)
				{
					dsi_ConsolePrint("UDP: getsockname() returned %d.%d.%d.%d:%d", 
						EXPAND_ADDR(myAddr));
				}
				else
				{
					dsi_ConsolePrint("UDP: getsockname() returned error %d", status);
				}
			}

			return LT_OK;
		}
		else
		{
			delete pConn;
		}

	} while ((curTime - startTime) < CONN_WAIT_TIME);
	
	RETURN_ERROR_PARAM(2, CUDPDriver::JoinSession, LT_TIMEOUT, addrStr);
}


LTRESULT CUDPDriver::ConnectTCP(const char* sAddress, SOCKET Sock, uint16 nLocalPort )
{
	sockaddr_in addr;

	if( INVALID_SOCKET != m_LocalConn ) closesocket(m_LocalConn);
	m_LocalConn		= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_nLocalPort	= nLocalPort;

#ifdef _DEBUG	//modify by zhoujb 0814. 덤횅땍	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(sAddress);
	addr.sin_port = nLocalPort;
#else
	if(!udp_BuildSockaddrFromString(sAddress, &addr))
	{
		RETURN_ERROR(1, ConnectTCP, LT_CANTBINDTOPORT);
	}
#endif

	return(ReallyJoinSession(Sock,&addr));
}

LTRESULT CUDPDriver::SendTcpIp(const CPacket_Read &cMsg, const char *sAddr, uint32 port)
{
	sockaddr_in addr;

	if (m_Socket == INVALID_SOCKET)
		return LT_NOTINITIALIZED;

	if (!udp_BuildSockaddrFromString(sAddr, &addr))
		return LT_ERROR;

	addr.sin_port = htons((u_short)port);

	// Send away!
	int nDataLen = (cMsg.Size() + 7) / 8;
	uint8 *aSendBuffer = (uint8 *)alloca(nDataLen);
	cMsg.PeekData(aSendBuffer, nDataLen * 8);

	int status = sendto(m_Socket, (char*)aSendBuffer, nDataLen,
		0, (sockaddr*)&addr, sizeof(addr));


#ifdef MURSUM_NET_TRACE
	
	if( !g_hDestWnd )
	{
		if (status == (int)nDataLen)
			return LT_OK;
		else
			return LT_ERROR;
	}

	g_IPC_SendByte.nSendByte	= status;
	g_IPC_SendByte.nAddr1		= addr.sin_addr.S_un.S_un_b.s_b1;
	g_IPC_SendByte.nAddr2		= addr.sin_addr.S_un.S_un_b.s_b2;
	g_IPC_SendByte.nAddr3		= addr.sin_addr.S_un.S_un_b.s_b3;
	g_IPC_SendByte.nAddr4		= addr.sin_addr.S_un.S_un_b.s_b4;

	g_CopyDataStruct.lpData = (char*)&g_IPC_SendByte;	

	::SendMessage( g_hDestWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&g_CopyDataStruct );

#endif
	
	if (status == (int)nDataLen)
		return LT_OK;
	else
		return LT_ERROR;
}

LTRESULT CUDPDriver::StartPing(const char *pAddr, uint16 nPort, uint32 *pPingID)
{
	LTRESULT nSendResult = SendPing(pAddr, nPort, m_nCurPingID);
	if (nSendResult != LT_OK)
		return nSendResult;
	// Add a request entry
	CPingRequest cRequest;
	cRequest.m_nID = m_nCurPingID;
	cRequest.m_nStatus = PING_STATUS_WAITING;
	cRequest.m_nTimeStamp = GetTickCount();
	m_cActivePings[m_nCurPingID] = cRequest;
	*pPingID = m_nCurPingID;
	// Inc the ping ID
	++m_nCurPingID;
	return LT_OK;
}

LTRESULT CUDPDriver::GetPingStatus(uint32 nPingID, uint32 *pStatus, uint32 *pLatency)
{
	TPingMap::iterator iPing = m_cActivePings.find(nPingID);
	if (iPing == m_cActivePings.end())
		return LT_NOTFOUND;
	// Time-out the ping if it's been too long
	if (iPing->second.m_nStatus == PING_STATUS_WAITING)
	{
		uint32 nCurTime = GetTickCount();
		if ((nCurTime - iPing->second.m_nTimeStamp) > k_nPing_Timeout)
		{
			iPing->second.m_nStatus = PING_STATUS_TIMEOUT;
			iPing->second.m_nTimeStamp = k_nPing_Timeout;
		}
	}
	// Return the result
	if (pStatus)
		*pStatus = iPing->second.m_nStatus;
	if (pLatency)
		*pLatency = iPing->second.m_nTimeStamp;
	return LT_OK;
}

LTRESULT CUDPDriver::RemovePing(uint32 nPingID)
{
	TPingMap::iterator iPing = m_cActivePings.find(nPingID);
	if (iPing == m_cActivePings.end())
		return LT_NOTFOUND;
	m_cActivePings.erase(iPing);
	return LT_OK;
}

LTRESULT CUDPDriver::SendPing(const char *pAddr, uint16 nPort, uint32 nID)
{
	sockaddr_in cPingAddr;
	if (!udp_BuildSockaddrFromString(pAddr, &cPingAddr))
		return LT_ERROR;
	cPingAddr.sin_port = htons((u_short)nPort);

	// Make sure we have a socket...
	if (m_Socket == INVALID_SOCKET)
	{
		int32 bindPort = udp_GetFirstClientPort();
		int32 bindAttempts = 0;

		while (m_Socket == INVALID_SOCKET)
		{
			sockaddr_in cAddr;
			udp_SetupLocalSockaddr(&cAddr, (u_short)bindPort);
			m_Socket = udp_BindToPort(&cAddr, &cAddr);
			if (m_Socket == INVALID_SOCKET)
			{
				if (bindPort == 0)
				{
					RETURN_ERROR_PARAM(2, CUDPDriver::SendPing, LT_ERROR, "Can't bind to port");
				}
				bindPort = udp_GetNextClientPort(++bindAttempts); // last port to try is always port 0
			}
		}
		// Make sure we'll get the response...
		StartThread_Listen();
	}

	// Send the ping
	CPacket_Write cPing;
	cPing.Writeuint32(UNCONNECTED_DATA_TOKEN);
	cPing.WriteBits(UNCONNECTED_MSG_PING, UNCONNECTED_MSG_BITS);
	cPing.WriteBits(nID, k_nPing_Bits);
	if (!SendTo(m_Socket, CPacket_Read(cPing), &cPingAddr))
		return LT_ERROR;

	return LT_OK;
}

void CUDPDriver::UpdateGUID(LTGUID &cGUID)
{
	CWinSync_CSAuto cProtection(m_cCS_GUID);
	m_cGUID = cGUID;
}

void CUDPDriver::StartThread_Listen()
{
	ASSERT(m_Socket != INVALID_SOCKET);

	m_nLSTime		= GetTickCount();
	
	unsigned long nThreadID;
	m_hThread = CreateThread(NULL, 0, ThreadBootstrap_Listen, (void *)this, 0, &nThreadID);
	m_cEvent_Thread_Ready.Block();
}

void CUDPDriver::StopThread_Listen()
{
	// Signal the shutdown
	m_hEvent_Thread_Shutdown.Set();

	if( INVALID_SOCKET != m_LocalConn )
	{
		sockaddr_in SendTo;
		memset( &SendTo, 0, sizeof(sockaddr_in) );
		SendTo.sin_family		= AF_INET;
		SendTo.sin_port			= htons( m_nLocalPort );
		SendTo.sin_addr.s_addr	= inet_addr( "127.0.0.1" );

		uint32	nTerm = UNCONNECTED_DATA_TERM;
		char	szBuf[16];
		memcpy( szBuf, &nTerm, sizeof(uint32) );
		sendto( m_LocalConn, szBuf, sizeof(uint32), 0, (sockaddr*)&SendTo, sizeof(sockaddr_in) );
	}

	// Wait for it to shut down
	WaitForSingleObject(m_hThread, INFINITE);
	// Clean up
	CloseHandle(m_hThread);
	m_hThread	= 0;
	m_Socket	= INVALID_SOCKET;
	m_hEvent_Thread_Shutdown.Clear();
}

unsigned long CUDPDriver::ThreadBootstrap_Listen(void *pUserData)
{
	CUDPDriver *pDriver = (CUDPDriver *)pUserData;

	return (unsigned long)pDriver->Thread_Listen();
}

uint32 CUDPDriver::Thread_Listen()
{
	uint32 nResult = 0;

	timeval cTimeout;
	cTimeout.tv_sec = k_nListenThread_Timeout / 1000;
	cTimeout.tv_usec = (k_nListenThread_Timeout % 1000) * 1000;

	// Ok, we're starting now...
	m_cEvent_Thread_Ready.Set();

	// Semi-infinite loop...
	while (1)
	{
		// Read a packet
		CPacket_Read cIncomingPacket;
		sockaddr_in senderAddr;
		int nRecvStatus;
		if (!udp_RecvFromSocket(m_Socket, &cIncomingPacket, &senderAddr, &nRecvStatus))
		{
			// Jump out if the socket's being shut down
			if (m_hEvent_Thread_Shutdown.IsSet())
			{
				break;
			}

			// Go to sleep if there's nothing waiting on the line
			else if (nRecvStatus == EWOULDBLOCK)
			{
				// Reset the read state
				fd_set aReadSet;
				FD_ZERO(&aReadSet);
				FD_SET(m_Socket, &aReadSet);

				// Wait...
				int status = select(m_Socket + 1, &aReadSet, NULL, NULL, &cTimeout);
				// Did we time out?
				if (status == 0)
				{
					// Jump out if we're supposed to shut down...
					if (m_hEvent_Thread_Shutdown.IsSet())
						break;
					// Update the connections so they stay alive
					// Note : This can't use the standard update, because we don't want
					// to flush anything, and we don't want to disconnect any dead connections.
					CWinSync_CSAuto cConnProtection(m_cCS_Connections);
					MPOS pCurPos = m_Connections.GetHeadPosition();
					while (pCurPos)
					{
						CUDPConn *pCurConn = m_Connections.GetNext(pCurPos);
						pCurConn->Update(false);
					}
					continue;
				}
				// Was there an error?
				else if (status == SOCKET_ERROR)
				{
					if (!m_hEvent_Thread_Shutdown.IsSet())
					{
						dsi_ConsolePrint("UDP Socket error %s!  Shutting down listen thread..", udp_GetLastError());
						nResult = 1;
					}
					break;
				}
			}	
			else if (nRecvStatus != ECONNRESET)
			{
				// Return a non-zero result if there was an error
				nResult = 1;
				break;
			}

			// if we're inside this block, we don't want to do the packet parsing...
			continue;
		}

		switch( cIncomingPacket.Peekuint32() )
		{
		case UNCONNECTED_DATA_TOKEN:
			// Parse the unconnected data packet
			HandleUnconnectedData(cIncomingPacket, &senderAddr);
			break;

		case UNCONNECTED_DATA_TERM:
			if (m_hEvent_Thread_Shutdown.IsSet()) return 0;						
			break;

		default: 
			{
				CWinSync_CSAuto cConnProtect(m_cCS_Connections);

				// Look up the sender
				CUDPConn *pConn = FindConnByAddr(&senderAddr);

				if (pConn)
				{
					// Handle the packet
					CUDPConn::EIncomingPacketResult eResult;
					eResult = pConn->HandleIncomingPacket(cIncomingPacket);
					if (eResult == CUDPConn::eIPR_Disconnect)
					{
						// Handle a disconnection the next time we update
						CDisconnectRequest cRequest;
						cRequest.m_pConnection = pConn;
						cRequest.m_eReason = pConn->GetLastDisconnectReason( );

						CWinSync_CSAuto cDisconnectProtect(m_cCS_DisconnectQueue);
						LT_MEM_TRACK_ALLOC(m_cDisconnectQueue.push_back(cRequest), LT_MEM_TYPE_NETWORKING);
					}
					else
					{
						// Give them an update, just to keep things running as smoothly as possible
						pConn->Update(false);
					}
				}
				else
				{
					CUnknownMessage cMsg;
					cMsg.m_cPacket = cIncomingPacket;
					cMsg.m_cSender = senderAddr;
					// Handle an unknown message the next time we update
					CWinSync_CSAuto cUnknownMessageProtect(m_cCS_UnknownMessages);
					LT_MEM_TRACK_ALLOC(m_cUnknownMessages.push_back(cMsg), LT_MEM_TYPE_NETWORKING);
				}
			}
			break;
		}
	}

	return nResult;
}

void CUDPDriver::StartThread_CheckCone()
{
	ASSERT(m_Socket != INVALID_SOCKET);
	unsigned long nThreadID;
	m_hThread = CreateThread(NULL, 0, ThreadBootstrap_CheckCone, (void *)this, 0, &nThreadID);
	m_cEvent_Thread_Ready.Block();
}

void CUDPDriver::StopThread_CheckCone()
{
	m_hEvent_Thread_Shutdown.Set();
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	m_hThread = 0;
	m_hEvent_Thread_Shutdown.Clear();
}

unsigned long CUDPDriver::ThreadBootstrap_CheckCone(void *pUserData)
{
	CUDPDriver *pDriver = (CUDPDriver *)pUserData;
	return (unsigned long)pDriver->Thread_CheckCone();
}

uint32 CUDPDriver::Thread_CheckCone()
{
	uint32		nResult = 0;
	sockaddr_in	serverAddr;
	DWORD		dwCurr, dwLast;
	CPacket_Write cCheckPacket_Write;
	
	memset( &serverAddr, 0, sizeof(sockaddr_in) );
	serverAddr.sin_family		= AF_INET;
			
	cCheckPacket_Write.Writeuint32(UNCONNECTED_CHECK_CONE);
	cCheckPacket_Write.WriteData( m_szLocalID, m_nLocalIDLen );

	m_cEvent_Thread_Ready.Set();
	
	dwLast	= ::GetTickCount();
		
	while (1)
	{
		Sleep(k_nCheckConeSleep);
		// Read a packet
		CPacket_Read cIncomingPacket;
		sockaddr_in senderAddr;
		int nRecvStatus;

		dwCurr	= ::GetTickCount();
		if( dwCurr-dwLast > k_nCheckPacket_Delay )
		{
			dwLast = dwCurr;

			if( strlen( m_szGameServerIP1 ) > 0 )
			{
				serverAddr.sin_addr.s_addr	= inet_addr( m_szGameServerIP1 );
				serverAddr.sin_port			= htons( m_nGameServerPort1 );				

				CPacket_Read cCheckPacket(cCheckPacket_Write);
				SendTo( m_Socket, cCheckPacket, &serverAddr );
			}

			if( strlen( m_szGameServerIP2 ) > 0 )
			{
				serverAddr.sin_addr.s_addr	= inet_addr( m_szGameServerIP2 );
				serverAddr.sin_port			= htons( m_nGameServerPort2 );				

				CPacket_Read cCheckPacket(cCheckPacket_Write);
				SendTo( m_Socket, cCheckPacket, &serverAddr );
			}
		}

		if (!udp_RecvFromSocket(m_Socket, &cIncomingPacket, &senderAddr, &nRecvStatus))
		{
			if (m_hEvent_Thread_Shutdown.IsSet())
			{
				break;
			}
			else if ( nRecvStatus != EWOULDBLOCK && nRecvStatus != ECONNRESET )
			{
				dsi_ConsolePrint( "nRecvStatus = %d", nRecvStatus );
				nResult = 1;
				break;
			}
			
			continue;
		}

		if (cIncomingPacket.Peekuint32() == UNCONNECTED_CHECK_CONE)
		{
			HandleCheckConeType(cIncomingPacket);
		}		
	}

	return nResult;
}

void CUDPDriver::SendDummyPacket( SOCKET Sock, const char* lpszIP, uint16 nPort )
{
	sockaddr_in	DestAddr;
	CPacket_Write cDummyPacket_Write;

	memset( &DestAddr, 0, sizeof(sockaddr_in) );
	DestAddr.sin_family		= AF_INET;			
	cDummyPacket_Write.Writeuint32(UNCONNECTED_DATA_DUMMY);


	DestAddr.sin_addr.s_addr	= inet_addr( lpszIP );
	DestAddr.sin_port			= htons( nPort );	

	CPacket_Read cDummyPacket(cDummyPacket_Write);
	SendTo( Sock, cDummyPacket, &DestAddr );
}
