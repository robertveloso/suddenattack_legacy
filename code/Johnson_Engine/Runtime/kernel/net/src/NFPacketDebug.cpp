#include "bdefs.h"

#include "conparse.h"
#include "netmgr.h"
#include "packetdefs.h"
#include "packet.h"

#ifndef DE_SERVER_COMPILE
#include "ConsoleCommands.h"
#endif // DE_SERVER_COMPILE

#include "NFPacketDebug.h"


NFPacketDebuger* NFPacketDebuger::m_pInstance = NULL;

NFPacketDebuger::NFPacketDebuger()
{
	m_nPlayerNum = 0;

	m_hReportWnd = NULL;
	
	m_pReceivedFromServerMsgInfo = NULL;
	m_pReceivedFromClientMsgInfo = NULL;

	m_pSentMsgInfo = NULL;

	m_pMessageInfo = NULL;

	InitializePacketInfo();
	InitializeMessageInfo();

	m_hReportWnd = ::FindWindow( NULL, "NetworkMonitor" );
}

NFPacketDebuger::~NFPacketDebuger()
{
	if ( NULL != m_pReceivedFromServerMsgInfo )
	{
		delete [] m_pReceivedFromServerMsgInfo;
		m_pReceivedFromServerMsgInfo = NULL;
	}

	if ( NULL != m_pReceivedFromClientMsgInfo )
	{
		delete [] m_pReceivedFromClientMsgInfo;
		m_pReceivedFromClientMsgInfo = NULL;
	}

	if ( NULL != m_pSentMsgInfo )
	{
		delete [] m_pSentMsgInfo;
		m_pSentMsgInfo = NULL;
	}

	if ( NULL != m_pMessageInfo )
	{
		delete [] m_pMessageInfo;
		m_pMessageInfo = NULL;
	}
}

void NFPacketDebuger::Output( char* szMessage )
{
	if ( g_CV_ParseRecv & NF_PARSE_OUTPUT_CONSOLE || g_CV_ParseSend & NF_PARSE_OUTPUT_CONSOLE )
	{
		dsi_ConsolePrint( szMessage );
	}
}

void NFPacketDebuger::AddPlayer()
{
	Output( "@@ NFPacketDebuger : AddPlayer() Called" );
	m_nPlayerNum++;
}

void NFPacketDebuger::RemovePlayer()
{
	Output( "@@ NFPacketDebuger : RemovePlayer() Called" );
	if ( m_nPlayerNum > 0 )
	{
		m_nPlayerNum--;
	}
	else
	{
		m_nPlayerNum = 0;
	}
}

void NFPacketDebuger::NFParseUDPRecv( const CPacket_Read& cPacket )
{
	if ( g_CV_SavePacketStaistics )
	{
		SaveStatistics();
		g_CV_SavePacketStaistics = LTFALSE;
	}

	CPacket_Read cNFParseRecv(cPacket);
	
	char szNFParseResult[8192] = {0,};

	cNFParseRecv.ReadBits(8); // CUDPConn::k_nFingerprintBits
	
	ConcatenateDetail( szNFParseResult, "------------------------- NF ParseRecv (UDP LAYER) -------------------------\n", NF_PARSE_UDP_LAYER );
	ConcatenateDetail( szNFParseResult, "-- FingerPrint Bits are Parsed..", NF_PARSE_UDP_LAYER );
	ConcatenateDetail( szNFParseResult, "-- UDP Handle Packet : ", NF_PARSE_UDP_LAYER );
	
	if ( cNFParseRecv.Readbool() )
	{
		uint32 nCommand1 = cNFParseRecv.ReadBits(1); // CUDPConn::k_nUDPCommandBits
		
		switch ( nCommand1 )
		{
		case 0 : // CUDPConn::k_nUDPCommand_Heartbeat
			{
				ConcatenateDetail( szNFParseResult, " Heartbeat Packet", NF_PARSE_UDP_LAYER );

				cNFParseRecv.ReadBits(13); // CUDPConn::k_nFrameBits;
				
				if ( cNFParseRecv.Readbool() )
				{
					for ( int nDummyCnt = 0; nDummyCnt < 8 /* k_nMaxOutOfOrderPackets */; nDummyCnt++ )
					{
						cNFParseRecv.Readbool();
					}
				}
			}
			break;
			
		case 1: // CDUPConn::k_nUDPCommand_Disconnect
			{
				ConcatenateDetail( szNFParseResult, " Disconnect Packet", NF_PARSE_UDP_LAYER );
				ConcatenateDetail( szNFParseResult, " - Reason Num : ", NF_PARSE_UDP_LAYER );
				
				char szBuf[8]			= {0,};
				int	 nDesconnectReason	= cNFParseRecv.Readuint8();
				itoa( nDesconnectReason, szBuf, 10 );
				
				ConcatenateDetail( szNFParseResult, szBuf, NF_PARSE_UDP_LAYER );
			}
			break;

		default:
			{
				ConcatenateDetail( szNFParseResult, " Invalid Packet Command Detected in HandleUDP !!!!!!!!!!", NF_PARSE_UDP_LAYER );
			}
			break;
		}
	}
	else
	{
		ConcatenateDetail( szNFParseResult, " Not Detected ", NF_PARSE_UDP_LAYER );
	}
	
	ConcatenateDetail( szNFParseResult, "\n", NF_PARSE_UDP_LAYER );

	if ( 0 != lstrlen( szNFParseResult ) )
	{
		Output( szNFParseResult );
	}
}

void NFPacketDebuger::AddPacketInfo( uint8 nDir, uint32 nID, const char* szMessageIDName, const char* szMessageHandledFnName )
{
	switch ( nDir )
	{
	case NETMGR_TRAVELDIR_SERVER2CLIENT:
		{
			if ( m_pReceivedFromServerMsgInfo )
			{
				lstrcpy( m_pReceivedFromServerMsgInfo[nID].szMessageIDName, szMessageIDName );
				lstrcpy( m_pReceivedFromServerMsgInfo[nID].szMessageHandledFnName, szMessageHandledFnName );
			}
		}
		break;

	case NETMGR_TRAVELDIR_CLIENT2SERVER:
		{
			if ( m_pReceivedFromClientMsgInfo )
			{
				lstrcpy( m_pReceivedFromClientMsgInfo[nID].szMessageIDName, szMessageIDName );
				lstrcpy( m_pReceivedFromClientMsgInfo[nID].szMessageHandledFnName, szMessageHandledFnName );
			}
		}
		break;

	case 0:
		{
			if ( m_pSentMsgInfo )
			{
				lstrcpy( m_pSentMsgInfo[nID].szMessageIDName, szMessageIDName );
				lstrcpy( m_pSentMsgInfo[nID].szMessageHandledFnName, szMessageHandledFnName );
			}
		}
		break;

	default:
		{

		}
		break;
	}
}

void NFPacketDebuger::NFParseRecv( uint8 nTravelDir, const CPacket_Read& cPacket )
{
	if ( g_CV_SavePacketStaistics )
	{
		SaveStatistics();
		g_CV_SavePacketStaistics = LTFALSE;
	}
	
	if ( !g_CV_ParseRecv )
		return;
	
	if ( NULL == m_pReceivedFromServerMsgInfo )
	{
		Output( "### [S] NF Receive Packet Parser is not Initialized Properly. ###" );
		return;
	}

	if ( NULL == m_pReceivedFromClientMsgInfo )
	{
		Output( "### [C] NF Receive Packet Parser is not Initialized Properly. ###" );
		return;
	}

	char szNFParseResult[8192] = {0,};

	ConcatenateDetail( szNFParseResult, "------------------------- NF ParseRecv (RCV MESSAGE) -------------------------\n", NF_PARSE_RECV );
	ConcatenateDetail( szNFParseResult, "-> Received Msg Parsing : ", NF_PARSE_RECV );
	
	CPacket_Read cNFParseRecv(cPacket);
	
	int nPacketCmd = cNFParseRecv.Readuint8();

	switch ( nTravelDir )
	{
	case NETMGR_TRAVELDIR_SERVER2CLIENT:
		{
			if ( nPacketCmd > NF_MAX_SERVER_PACKET_CNT )
			{
				ConcatenateDetail( szNFParseResult, "[S2C] Exceeded Packet Command Detected ( ID : ", NF_PARSE_RECV );
				
				char szBuf[8]		= {0,};
				int	 nUnknownMsgID	= nPacketCmd;
				itoa( nUnknownMsgID, szBuf, 10 );
				
				ConcatenateDetail( szNFParseResult, szBuf, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " )\n", NF_PARSE_RECV );

				break;
			}

			if ( 0 != lstrlen( m_pReceivedFromServerMsgInfo[nPacketCmd].szMessageIDName ) )
			{
				ConcatenateDetail( szNFParseResult, "[", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, m_pReceivedFromServerMsgInfo[nPacketCmd].szMessageIDName, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, "]", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " Message Handled to ", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, "<", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, m_pReceivedFromServerMsgInfo[nPacketCmd].szMessageHandledFnName, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, ">", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " Func. \n", NF_PARSE_RECV );
			}
			else
			{
				ConcatenateDetail( szNFParseResult, "[S2C] Unknown Message Handled ( ID : ", NF_PARSE_RECV );
				
				char szBuf[8]		= {0,};
				int	 nUnknownMsgID	= nPacketCmd;
				itoa( nUnknownMsgID, szBuf, 10 );
				
				ConcatenateDetail( szNFParseResult, szBuf, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " )\n", NF_PARSE_RECV );
			}
			
			m_pReceivedFromServerMsgInfo[nPacketCmd].nHandledCnt++;
		}
		break;

	case NETMGR_TRAVELDIR_CLIENT2SERVER :
		{
			if ( nPacketCmd > NF_MAX_CLIENT_PACKET_CNT )
			{
				ConcatenateDetail( szNFParseResult, "[C2S] Exceeded Packet Command Detected ( ID : ", NF_PARSE_RECV );
				
				char szBuf[8]		= {0,};
				int	 nUnknownMsgID	= nPacketCmd;
				itoa( nUnknownMsgID, szBuf, 10 );
				
				ConcatenateDetail( szNFParseResult, szBuf, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " )\n", NF_PARSE_RECV );

				break;
			}
			
			if ( 0 != lstrlen( m_pReceivedFromClientMsgInfo[nPacketCmd].szMessageIDName ) )
			{
				ConcatenateDetail( szNFParseResult, "[", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, m_pReceivedFromClientMsgInfo[nPacketCmd].szMessageIDName, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, "]", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " Message Handled to ", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, "<", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, m_pReceivedFromClientMsgInfo[nPacketCmd].szMessageHandledFnName, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, ">", NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " Func. \n", NF_PARSE_RECV );
			}
			else
			{
				ConcatenateDetail( szNFParseResult, "[C2S] Unknown Message Handled ( ID : ", NF_PARSE_RECV );
				
				char szBuf[8]		= {0,};
				int	 nUnknownMsgID	= nPacketCmd;
				itoa( nUnknownMsgID, szBuf, 10 );
				
				ConcatenateDetail( szNFParseResult, szBuf, NF_PARSE_RECV );
				ConcatenateDetail( szNFParseResult, " )\n", NF_PARSE_RECV );
			}
			
			m_pReceivedFromClientMsgInfo[nPacketCmd].nHandledCnt++;

			switch ( nPacketCmd )
			{
				
			case CMSG_HELLO :			ParseDetail_CMSG_HELLO( cNFParseRecv );				break;
			case CMSG_GOODBYE :			ParseDetail_CMSG_GOODBYE( cNFParseRecv );			break;
			case CMSG_UPDATE :			ParseDetail_CMSG_UPDATE( cNFParseRecv );			break;
			case CMSG_SOUNDUPDATE :		ParseDetail_CMSG_SOUNDUPDATE( cNFParseRecv );		break;
			case CMSG_CONNECTSTAGE :	ParseDetail_CMSG_CONNECTSTAGE( cNFParseRecv );		break;
			case CMSG_COMMANDSTRING :	ParseDetail_CMSG_COMMANDSTRING( cNFParseRecv );		break;
			case CMSG_MESSAGE :			ParseDetail_CMSG_MESSAGE( cNFParseRecv );			break;
			case CMSG_TEST :			ParseDetail_CMSG_TEST( cNFParseRecv );				break;

			}
		}
		break;

	default:
		{

		}
		break;
	}
	
	if ( 0 != lstrlen( szNFParseResult ) )
	{
		Output( szNFParseResult );
	}
}

void NFPacketDebuger::ParseDetail_CMSG_HELLO( const CPacket_Read& cHelloPacket )
{
	CPacket_Read cPacket(cHelloPacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_HELLO;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_CMSG_GOODBYE( const CPacket_Read& cGoodByePacket )
{
	CPacket_Read cPacket(cGoodByePacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_GOODBYE;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_CMSG_UPDATE( const CPacket_Read& cUpdatePacket )
{
	CPacket_Read cPacket(cUpdatePacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_UPDATE;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_CMSG_SOUNDUPDATE( const CPacket_Read& cSoundUpdatePacket )
{
	CPacket_Read cPacket(cSoundUpdatePacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_SOUNDUPDATE;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_CMSG_CONNECTSTAGE( const CPacket_Read& cConnectStagePacket )
{
	CPacket_Read cPacket(cConnectStagePacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_CONNECTSTAGE;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_CMSG_COMMANDSTRING( const CPacket_Read& cCommandStringPacket )
{
	CPacket_Read cPacket(cCommandStringPacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_COMMANDSTRING;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_CMSG_MESSAGE( const CPacket_Read& cMessagePacket )
{
	CPacket_Read cPacket(cMessagePacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_MESSAGE;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_CMSG_TEST( const CPacket_Read& cTestPacket )
{
	CPacket_Read cPacket(cTestPacket);
	
	int nSendCommand = 0;
	
	MessageInfo mi;
	mi.bSend			= FALSE;
	mi.dwMessageType	= ERMT_TEST;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}



void NFPacketDebuger::NFParseSend( uint8 nTravelDir, const CPacket_Read& cPacket )
{
	if ( g_CV_SavePacketStaistics )
	{
		SaveStatistics();
		g_CV_SavePacketStaistics = LTFALSE;
	}
	
	if ( !g_CV_ParseSend )
		return;
	
	if ( NULL == m_pSentMsgInfo )
	{
		Output( "### [U] NF Receive Packet Parser is not Initialized Properly. ###" );
		return;
	}

	char szNFParseResult[8192] = {0,};

	ConcatenateDetail( szNFParseResult, "------------------------- NF ParseSend (SENT MESSAGE) -------------------------\n", NF_PARSE_SEND );
	ConcatenateDetail( szNFParseResult, "-> Sent Msg Parsing : ", NF_PARSE_SEND );
	
	CPacket_Read cNFParseSend(cPacket);
	
	int nPacketCmd = cNFParseSend.Readuint8();

	switch ( nTravelDir )
	{
	case 0:
		{
			if ( nPacketCmd > NF_MAX_SERVER_PACKET_CNT)
			{
				ConcatenateDetail( szNFParseResult, "[S2C] Exceeded Packet Command Detected ( ID : ", NF_PARSE_SEND );
				
				char szBuf[8]		= {0,};
				int	 nUnknownMsgID	= nPacketCmd;
				itoa( nUnknownMsgID, szBuf, 10 );
				
				ConcatenateDetail( szNFParseResult, szBuf, NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, " )\n", NF_PARSE_SEND );

				break;
			}

			if ( 0 != lstrlen( m_pSentMsgInfo[nPacketCmd].szMessageIDName ) )
			{
				ConcatenateDetail( szNFParseResult, "[", NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, m_pSentMsgInfo[nPacketCmd].szMessageIDName, NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, "]", NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, " Message Handled to ", NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, "<", NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, m_pSentMsgInfo[nPacketCmd].szMessageHandledFnName, NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, ">", NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, " Func. \n", NF_PARSE_SEND );
			}
			else
			{
				ConcatenateDetail( szNFParseResult, "[S2C] Unknown Message Handled ( ID : ", NF_PARSE_SEND );
				
				char szBuf[8]		= {0,};
				int	 nUnknownMsgID	= nPacketCmd;
				itoa( nUnknownMsgID, szBuf, 10 );
				
				ConcatenateDetail( szNFParseResult, szBuf, NF_PARSE_SEND );
				ConcatenateDetail( szNFParseResult, " )\n", NF_PARSE_SEND );
			}

			switch ( nPacketCmd )
			{
			case SMSG_UPDATE: 				ParseDetail_SMSG_UPDATE( cNFParseSend );					break;
			case SMSG_MESSAGE:				ParseDetail_SMSG_MESSAGE( cNFParseSend );					break;
			case SMSG_INSTANTSPECIALEFFECT:	ParseDetail_SMSG_INSTANTSPECIALEFFECT( cNFParseSend );		break;
			case SMSG_UNGUARANTEEDUPDATE:	ParseDetail_SMSG_UNGUARANTEEDUPDATE( cNFParseSend );		break;
			case SMSG_YOURID:				ParseDetail_SMSG_YOURID( cNFParseSend );					break;
			case SMSG_LOADWORLD:			ParseDetail_SMSG_LOADWORLD( cNFParseSend );					break;
			case SMSG_UNLOADWORLD:			ParseDetail_SMSG_UNLOADWORLD( cNFParseSend );				break;
			case SMSG_PACKETGROUP:			ParseDetail_SMSG_PACKETGROUP( cNFParseSend );				break;
			case SMSG_CONSOLEVAR:			ParseDetail_SMSG_CONSOLEVAR( cNFParseSend );				break;
			case SMSG_SKYDEF:				ParseDetail_SMSG_SKYDEF( cNFParseSend );					break;
			case SMSG_GLOBALLIGHT:			ParseDetail_SMSG_GLOBALLIGHT( cNFParseSend );				break;
			case SMSG_CLIENTOBJECTID:		ParseDetail_SMSG_CLIENTOBJECTID( cNFParseSend );			break;
			case SMSG_PRELOADLIST:			ParseDetail_SMSG_PRELOADLIST( cNFParseSend );				break;
			case SMSG_NETPROTOCOLVERSION:	ParseDetail_SMSG_NETPROTOCOLVERSION( cNFParseSend );		break;
			case SMSG_THREADLOAD:			ParseDetail_SMSG_THREADLOAD( cNFParseSend );				break;
			case SMSG_UNLOAD:				ParseDetail_SMSG_UNLOAD( cNFParseSend );					break;
			case SMSG_CHANGE_CHILDMODEL:	ParseDetail_SMSG_CHANGE_CHILDMODEL( cNFParseSend );			break;
			}
			
			m_pSentMsgInfo[nPacketCmd].nHandledCnt++;

		}
		break;

	default:
		{

		}
		break;
	}
	
	if ( 0 != lstrlen( szNFParseResult ) )
	{
		Output( szNFParseResult );
	}
}

void NFPacketDebuger::SendToNetworkMonitor( char* pMessage, int nLength, BOOL bSend /*=TRUE*/)
{
	if ( NULL == m_hReportWnd )
	{
		m_hReportWnd = ::FindWindow( NULL, "NetworkMonitor" );
	}
	
	if ( NULL != m_hReportWnd )
	{
		ZeroMemory( &m_copyData, sizeof(COPYDATASTRUCT) );
		m_copyData.cbData = nLength;
		m_copyData.lpData = pMessage;
		
		::SendMessage( m_hReportWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)(&m_copyData) );
	}
}

void NFPacketDebuger::ParseDetail_SMSG_INSTANTSPECIALEFFECT( const CPacket_Read& cSFXPacket )
{
	CPacket_Read cPacket(cSFXPacket);

	int nSendCommand = cPacket.Readuint8();
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_SFX;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();

	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_MESSAGE( const CPacket_Read& cMessagePacket )
{
	CPacket_Read cPacket(cMessagePacket);

	int nSendCommand = cPacket.Readuint8();

	if ( m_pMessageInfo )
	{
		m_pMessageInfo[nSendCommand].nHandledCnt++;
	}
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_NORMAL;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();

	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_UNGUARANTEEDUPDATE( const CPacket_Read& cUnguaranteedPacket )
{
	CPacket_Read cPacket(cUnguaranteedPacket);

	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_UNGUARANTEED;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();

	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_YOURID( const CPacket_Read& cYourIDPacket )
{
	CPacket_Read cPacket(cYourIDPacket);
	
	int nSendCommand = cPacket.Readuint8();
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_YOURID;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_LOADWORLD( const CPacket_Read& cLoadWorldPacket )
{
	CPacket_Read cPacket(cLoadWorldPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_LOADWORLD;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_UNLOADWORLD( const CPacket_Read& cUnloadWorldPacket )
{
	CPacket_Read cPacket(cUnloadWorldPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_UNLOADWORLD;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_PACKETGROUP( const CPacket_Read& cPacketGroupPacket )
{
	CPacket_Read cPacket(cPacketGroupPacket);

	while (!cPacket.EOP())
	{
		uint16 nLength = cPacket.Readuint8();

		if (nLength > cPacket.TellEnd())
		{
			return;
		}
		else if (nLength == 0)
		{
			// (This signals the end of the grouped packets).
			return;
		}

		CPacket_Read cSubPacket(cPacket, cPacket.Tell(), nLength);
		cPacket.Seek(nLength);

		int nSendCommand =cSubPacket.Readuint8();
		
		MessageInfo mi;
		mi.bSend			= TRUE;
		mi.dwMessageType	= EMT_PACKETGROUP;
		mi.dwMessageID		= nSendCommand;
		mi.dwMessageSize	= cPacket.Size();
		
		SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
	}
}

void NFPacketDebuger::ParseDetail_SMSG_CONSOLEVAR( const CPacket_Read& cConsoleVarPacket )
{
	CPacket_Read cPacket(cConsoleVarPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_CONSOLEVAR;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_SKYDEF( const CPacket_Read& cSkyDefPacket )
{
	CPacket_Read cPacket(cSkyDefPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_SKYDEF;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_GLOBALLIGHT( const CPacket_Read& cGlobalLightPacket )
{
	CPacket_Read cPacket(cGlobalLightPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_GLOBALLIGHT;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_CLIENTOBJECTID( const CPacket_Read& cClientObjectIDPacket )
{
	CPacket_Read cPacket(cClientObjectIDPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_CLIENTOBJECTID;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_PRELOADLIST( const CPacket_Read& cPreloadListPacket )
{
	CPacket_Read cPacket(cPreloadListPacket);
	
	int nSendCommand = cPacket.Readuint8();
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_PRELOADLIST;
	mi.dwMessageID		= nSendCommand;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_NETPROTOCOLVERSION( const CPacket_Read& cNetProtocolVerPacket )
{
	CPacket_Read cPacket(cNetProtocolVerPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_NETPROTOCOLVERSION;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_THREADLOAD( const CPacket_Read& cThreadLoadPacket )
{
	CPacket_Read cPacket(cThreadLoadPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_THREADLOAD;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_UNLOAD( const CPacket_Read& cUnloadPacket )
{
	CPacket_Read cPacket(cUnloadPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_UNLOAD;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_CHANGE_CHILDMODEL( const CPacket_Read& cChangeChildModelPacket )
{
	CPacket_Read cPacket(cChangeChildModelPacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_CHANGE_CHILDMODEL;
	mi.dwMessageID		= 0;
	mi.dwMessageSize	= cPacket.Size();
	
	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );
}

void NFPacketDebuger::ParseDetail_SMSG_UPDATE( const CPacket_Read& cUpdatePacket )
{
//    uint16 flags, objectID;
//    uint8 subType;

	CPacket_Read cPacket(cUpdatePacket);
	
	MessageInfo mi;
	mi.bSend			= TRUE;
	mi.dwMessageType	= EMT_UPDATE;
	mi.dwMessageID		= 255;
	mi.dwMessageSize	= cPacket.Size();

/*
    while (!cPacket.EOP()) 
	{
		uint32 nUpdateSize = cPacket.Readuint32();
		uint32 nUpdateStart = cPacket.Tell();
		
		CPacket_Read cSubPacket(cPacket, nUpdateStart, nUpdateSize );				
		flags = cSubPacket.Readuint8();

        // Get more change flags if there are any.
        if (flags & CF_OTHER) 
		{
            flags |= (uint16)cSubPacket.Readuint8() << 8;
        }
        
        // Object identification.
        if (flags) 
		{
			mi.dwMessageID	= 0;
			Output( " Object Packet in SMSG_UPDATE" );
        }
        else 
		{
            subType = cSubPacket.Readuint8();
			
            if (subType == UPDATESUB_PLAYSOUND) 
			{
				mi.dwMessageID	= 1;
				Output( "UPDATESUB_PLAYSOUND in SMSG_UPDATE" );
            }
            else if (subType == UPDATESUB_SOUNDTRACK) 
			{
				mi.dwMessageID	= 2;
				Output( "UPDATESUB_SOUNDTRACK in SMSG_UPDATE" );
            }
            else if (subType == UPDATESUB_OBJECTREMOVES) 
			{
				mi.dwMessageID	= 4;
				Output( "UPDATESUB_OBJECTREMOVES in SMSG_UPDATE" );
            }
            else 
			{
				mi.dwMessageID	= 201;
				Output( "LT_INVALIDSERVERPACKET in SMSG_UPDATE" );
            }
        }
		if (cSubPacket.Tell() != nUpdateStart + nUpdateSize)
		{
			mi.dwMessageID	= 202;
			Output( "LT_INVALIDSERVERPACKET in SMSG_UPDATE End" );
		}

		cPacket.Seek(nUpdateSize+nUpdateStart);
    }*/


	SendToNetworkMonitor( (char*)&mi, sizeof(MessageInfo) );	
}

void NFPacketDebuger::SaveStatistics()
{
	FILE*	fp	= NULL;
	int		n	= 0;

	fp = fopen( "NFPacketStatistics.log", "a" );

	if ( NULL == fp )
	{
		return;
	}
	

	SYSTEMTIME st;
	GetLocalTime( &st );

	fprintf( fp, "-----------------------------------------------------------------------\n" );
	fprintf( fp, "--  Packet Statistics               %4d / %0.2d / %0.2d    %0.2d : %0.2d : %0.2d    -\n", 
													st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond ) ;
	fprintf( fp, "-----------------------------------------------------------------------\n" );

	fprintf( fp, " Current Player Count : %d \n", m_nPlayerNum );
	fprintf( fp, " \n");
	fprintf( fp, " Received ---------------------------------- \n" );
	fprintf( fp, " - From Server\n" );

	for ( n = 0; n < NF_MAX_SERVER_PACKET_CNT; n++ )
	{
		if ( 0 < m_pReceivedFromServerMsgInfo[n].nHandledCnt )
		{
			fprintf( fp, " [%0.3d] %50s : %.5d times\n", n,
													( 0 != lstrlen( m_pReceivedFromServerMsgInfo[n].szMessageIDName ) ) ?
													m_pReceivedFromServerMsgInfo[n].szMessageIDName : "Unknown",
													m_pReceivedFromServerMsgInfo[n].nHandledCnt		);
		}
	}

	fprintf( fp, " - From Client\n" );

	for ( n = 0; n < NF_MAX_CLIENT_PACKET_CNT; n++ )
	{
		if ( 0 < m_pReceivedFromClientMsgInfo[n].nHandledCnt )
		{
			fprintf( fp, " [%0.3d] %50s : %.5d times\n", n,
													( 0 != lstrlen( m_pReceivedFromClientMsgInfo[n].szMessageIDName ) ) ?
													m_pReceivedFromClientMsgInfo[n].szMessageIDName : "Unknown",
													m_pReceivedFromClientMsgInfo[n].nHandledCnt		);
		}
	}

	fprintf( fp, " Sent -------------------------------------- \n" );

	for ( n = 0; n < NF_MAX_SERVER_PACKET_CNT + NF_MAX_CLIENT_PACKET_CNT; n++ )
	{
		if ( 0 < m_pSentMsgInfo[n].nHandledCnt )
		{
			fprintf( fp, " [%0.3d] %50s : %.5d times\n", n,
													( 0 != lstrlen( m_pSentMsgInfo[n].szMessageIDName ) ) ?
													m_pSentMsgInfo[n].szMessageIDName : "Unknown",
													m_pSentMsgInfo[n].nHandledCnt		);
		}
	}

	fprintf( fp, "\n" );
	fprintf( fp, " Message Packets --------------------------- \n" );

	for ( n = 0; n < NF_MAX_MESSAGE_CNT; n++ )
	{
		fprintf( fp, " MessageID[%0.3d] Total : %d\n", n, m_pMessageInfo[n].nHandledCnt );
	}

	fclose( fp );
	fp = NULL;
}


void NFPacketDebuger::InitializePacketInfo()
{
	m_pReceivedFromServerMsgInfo = new PacketInfo[NF_MAX_SERVER_PACKET_CNT];
	ZeroMemory( m_pReceivedFromServerMsgInfo, sizeof(PacketInfo) * NF_MAX_SERVER_PACKET_CNT );
	
	// NETMGR_TRAVELDIR_SERVER2CLIENT : 1
	AddPacketInfo( 1, SMSG_YOURID,					"SMSG_YOURID",					"OnYourIDPacket"				);
    AddPacketInfo( 1, SMSG_LOADWORLD,				"SMSG_LOADWORLD",				"OnLoadWorldPacket"				);
    AddPacketInfo( 1, SMSG_UNLOADWORLD,				"SMSG_UNLOADWORLD",				"OnUnloadWorldPacket"			);
    AddPacketInfo( 1, SMSG_UPDATE,					"SMSG_UPDATE",					"OnUpdatePacket"				);
    AddPacketInfo( 1, SMSG_UNGUARANTEEDUPDATE,		"SMSG_UNGUARANTEEDUPDATE",		"OnUnguaranteedUpdatePacket"	);
    AddPacketInfo( 1, SMSG_MESSAGE,					"SMSG_MESSAGE",					"OnMessagePacket"				);
    AddPacketInfo( 1, SMSG_PACKETGROUP,				"SMSG_PACKETGROUP",				"OnPacketGroupPacket"			);
    AddPacketInfo( 1, SMSG_CONSOLEVAR,				"SMSG_CONSOLEVAR",				"OnConsoleVar"					);
    AddPacketInfo( 1, SMSG_SKYDEF,					"SMSG_SKYDEF",					"OnSkyDef"						);
    AddPacketInfo( 1, SMSG_GLOBALLIGHT,				"SMSG_GLOBALLIGHT",				"OnGlobalLight"					);
    AddPacketInfo( 1, SMSG_INSTANTSPECIALEFFECT,	"SMSG_INSTANTSPECIALEFFECT",	"OnInstantSpecialEffect"		);
    AddPacketInfo( 1, SMSG_CLIENTOBJECTID,			"SMSG_CLIENTOBJECTID",			"OnClientObjectID"				);
    AddPacketInfo( 1, SMSG_PRELOADLIST,				"SMSG_PRELOADLIST",				"OnPreloadListPacket"			);
    AddPacketInfo( 1, SMSG_NETPROTOCOLVERSION,		"SMSG_NETPROTOCOLVERSION",		"OnNetProtocolVersionPacket"	);
    AddPacketInfo( 1, SMSG_THREADLOAD,				"SMSG_THREADLOAD",				"OnThreadLoadPacket"			);
    AddPacketInfo( 1, SMSG_UNLOAD,					"SMSG_UNLOAD",					"OnUnloadPacket"				);
	AddPacketInfo( 1, SMSG_CHANGE_CHILDMODEL,		"SMSG_CHANGE_CHILDMODEL",		"OnChangeChildModel"			);

	//////////////////////////////////////////////////////////////////////////
	
	m_pReceivedFromClientMsgInfo = new PacketInfo[NF_MAX_CLIENT_PACKET_CNT];
	ZeroMemory( m_pReceivedFromClientMsgInfo, sizeof(PacketInfo) * NF_MAX_CLIENT_PACKET_CNT );

	// NETMGR_TRAVELDIR_CLIENT2SERVER : 2
	AddPacketInfo( 2, CMSG_GOODBYE,			"CMSG_GOODBYE",				"OnClientDisconnectPacket"	);
	AddPacketInfo( 2, CMSG_UPDATE,			"CMSG_UPDATE",				"OnClientUpdatePacket"		);
	AddPacketInfo( 2, CMSG_SOUNDUPDATE,		"CMSG_SOUNDUPDATE",			"OnSoundUpdatePacket"		);
	AddPacketInfo( 2, CMSG_COMMANDSTRING,	"CMSG_COMMANDSTRING",		"OnCommandStringPacket"		);
	AddPacketInfo( 2, CMSG_MESSAGE,			"CMSG_MESSAGE",				"OnMessagePacket"			);
	AddPacketInfo( 2, CMSG_CONNECTSTAGE,	"CMSG_CONNECTSTAGE",		"OnConnectStagePacket"		);
	AddPacketInfo( 2, CMSG_HELLO,			"CMSG_HELLO",				"OnHelloPacket"				);

	//////////////////////////////////////////////////////////////////////////
	
	m_pSentMsgInfo = new PacketInfo[NF_MAX_SERVER_PACKET_CNT];
	ZeroMemory( m_pSentMsgInfo, sizeof(PacketInfo) * (NF_MAX_SERVER_PACKET_CNT) );

	AddPacketInfo( 0, SMSG_YOURID,					"SMSG_YOURID",					"OnYourIDPacket"				);
    AddPacketInfo( 0, SMSG_LOADWORLD,				"SMSG_LOADWORLD",				"OnLoadWorldPacket"				);
    AddPacketInfo( 0, SMSG_UNLOADWORLD,				"SMSG_UNLOADWORLD",				"OnUnloadWorldPacket"			);
    AddPacketInfo( 0, SMSG_UPDATE,					"SMSG_UPDATE",					"OnUpdatePacket"				);
    AddPacketInfo( 0, SMSG_UNGUARANTEEDUPDATE,		"SMSG_UNGUARANTEEDUPDATE",		"OnUnguaranteedUpdatePacket"	);
    AddPacketInfo( 0, SMSG_MESSAGE,					"SMSG_MESSAGE",					"OnMessagePacket"				);
    AddPacketInfo( 0, SMSG_PACKETGROUP,				"SMSG_PACKETGROUP",				"OnPacketGroupPacket"			);
    AddPacketInfo( 0, SMSG_CONSOLEVAR,				"SMSG_CONSOLEVAR",				"OnConsoleVar"					);
    AddPacketInfo( 0, SMSG_SKYDEF,					"SMSG_SKYDEF",					"OnSkyDef"						);
    AddPacketInfo( 0, SMSG_GLOBALLIGHT,				"SMSG_GLOBALLIGHT",				"OnGlobalLight"					);
    AddPacketInfo( 0, SMSG_INSTANTSPECIALEFFECT,	"SMSG_INSTANTSPECIALEFFECT",	"OnInstantSpecialEffect"		);
    AddPacketInfo( 0, SMSG_CLIENTOBJECTID,			"SMSG_CLIENTOBJECTID",			"OnClientObjectID"				);
    AddPacketInfo( 0, SMSG_PRELOADLIST,				"SMSG_PRELOADLIST",				"OnPreloadListPacket"			);
    AddPacketInfo( 0, SMSG_NETPROTOCOLVERSION,		"SMSG_NETPROTOCOLVERSION",		"OnNetProtocolVersionPacket"	);
    AddPacketInfo( 0, SMSG_THREADLOAD,				"SMSG_THREADLOAD",				"OnThreadLoadPacket"			);
    AddPacketInfo( 0, SMSG_UNLOAD,					"SMSG_UNLOAD",					"OnUnloadPacket"				);
	AddPacketInfo( 0, SMSG_CHANGE_CHILDMODEL,		"SMSG_CHANGE_CHILDMODEL",		"OnChangeChildModel"			);
/*
	AddPacketInfo( 0, CMSG_GOODBYE,					"CMSG_GOODBYE",					"OnClientDisconnectPacket"		);
	AddPacketInfo( 0, CMSG_UPDATE,					"CMSG_UPDATE",					"OnClientUpdatePacket"			);
	AddPacketInfo( 0, CMSG_SOUNDUPDATE,				"CMSG_SOUNDUPDATE",				"OnSoundUpdatePacket"			);
	AddPacketInfo( 0, CMSG_COMMANDSTRING,			"CMSG_COMMANDSTRING",			"OnCommandStringPacket"			);
	AddPacketInfo( 0, CMSG_MESSAGE,					"CMSG_MESSAGE",					"OnMessagePacket"				);
	AddPacketInfo( 0, CMSG_CONNECTSTAGE,			"CMSG_CONNECTSTAGE",			"OnConnectStagePacket"			);
	AddPacketInfo( 0, CMSG_HELLO,					"CMSG_HELLO",					"OnHelloPacket"					);
*/
}

void NFPacketDebuger::InitializeMessageInfo()
{
	m_pMessageInfo = new PacketInfo[NF_MAX_MESSAGE_CNT];
	ZeroMemory( m_pMessageInfo, sizeof(PacketInfo) * NF_MAX_MESSAGE_CNT );

	/*
	MID_PLAYER_UPDATE, "MID_PLAYER_UPDATE"
	MID_PLAYER_SKILLS, "MID_PLAYER_SKILLS"
	MID_PLAYER_SUMMARY, "MID_PLAYER_SUMMARY"
	MID_PLAYER_INFOCHANGE, "MID_PLAYER_INFOCHANGE"
	MID_PLAYER_SCORE, "MID_PLAYER_SCORE"
	MID_PLAYER_STATE_CHANGE, "MID_PLAYER_STATE_CHANGE"
	MID_PLAYER_MESSAGE, "MID_PLAYER_MESSAGE"
	MID_PLAYER_CHEAT, "MID_PLAYER_CHEAT"
	MID_PLAYER_ORIENTATION, "MID_PLAYER_ORIENTATION"
	MID_PLAYER_INFO, "MID_PLAYER_INFO"
	MID_PLAYER_REMOVED, "MID_PLAYER_REMOVED"
	MID_PLAYER_SCORED, "MID_PLAYER_SCORED"
	MID_PLAYER_RESPAWN, "MID_PLAYER_RESPAWN"
	MID_PLAYER_CANCELREVIVE, "MID_PLAYER_CANCELREVIVE"
	MID_PLAYER_MULTIPLAYER_INIT, "MID_PLAYER_MULTIPLAYER_INIT"
	MID_PLAYER_SINGLEPLAYER_INIT, "MID_PLAYER_SINGLEPLAYER_INIT"
	MID_PLAYER_LOADCLIENT, "MID_PLAYER_LOADCLIENT"
	MID_PLAYER_DAMAGE, "MID_PLAYER_DAMAGE"
	MID_PLAYER_ACTIVATE, "MID_PLAYER_ACTIVATE"
	MID_PLAYER_CLIENTMSG, "MID_PLAYER_CLIENTMSG"
	MID_PLAYER_OVERLAY, "MID_PLAYER_OVERLAY"
	MID_PLAYER_TELEPORT, "MID_PLAYER_TELEPORT"
	MID_PLAYER_TRANSMISSION, "MID_PLAYER_TRANSMISSION"
	MID_MISSION_INFO, "MID_MISSION_INFO
	MID_PLAYER_GHOSTMESSAGE, "MID_PLAYER_GHOSTMESSAGE"
	MID_PLAYER_CREDITS, "MID_PLAYER_CREDITS"
	MID_PLAYER_TEAM, "MID_PLAYER_TEAM"
	MID_PLAYER_CONNECTED, "MID_PLAYER_CONNECTED"
	MID_WEAPON_CHANGE, "MID_WEAPON_CHANGE"
	MID_WEAPON_SOUND, "MID_WEAPON_SOUND"
	MID_WEAPON_SOUND_LOOP, "MID_WEAPON_SOUND_LOOP"
	MID_WEAPON_FIRE, "MID_WEAPON_FIRE"
	MID_WEAPON_RELOAD, "MID_WEAPON_RELOAD"
	MID_COMPILE, "MID_COMPILE"
	MID_LIGHT_CYCLE_INFO, "MID_LIGHT_CYCLE_INFO"
	MID_PERFORMANCE, "MID_PERFORMANCE"
	MID_PROJECTILE, "MID_PROJECTILE"
	MID_CLIENT_LOADED, "MID_CLIENT_LOADED"
	MID_SWITCHINGWORLDSSTATE, "MID_SWITCHINGWORLDSSTATE"
	MID_CLIENT_READY_FOR_AUTOLOAD, "MID_CLIENT_READY_FOR_AUTOLOAD"
	MID_DOOMSDAY_MESSAGE, "MID_DOOMSDAY_MESSAGE"
	MID_PLAYER_TAUNT, "MID_PLAYER_TAUNT"
	MID_MULTIPLAYER_UPDATE, "MID_MULTIPLAYER_UPDATE"
	MID_MULTIPLAYER_OPTIONS, "MID_MULTIPLAYER_OPTIONS"
	MID_TEAM_INFO, "MID_TEAM_INFO"
	MID_TEAM_ADD, "MID_TEAM_ADD"
	MID_MUSIC, "MID_MUSIC"
	MID_START_ENERGY_TRANSFER, "MID_START_ENERGY_TRANSFER"
	MID_STOP_ENERGY_TRANSFER, "MID_STOP_ENERGY_TRANSFER"
	MID_ENERGY_TRANSFER_COMPLETE, "MID_ENERGY_TRANSFER_COMPLETE"
	MID_QUERY_TARGET_PROPERTIES, "MID_QUERY_TARGET_PROPERTIES"
	MID_OBJECT_MESSAGE, "MID_OBJECT_MESSAGE"
	MID_GAME_PAUSE, "MID_GAME_PAUSE"
	MID_GAME_UNPAUSE, "MID_GAME_UNPAUSE"
	MID_GEAR_PICKEDUP, "MID_GEAR_PICKEDUP"
	MID_INTEL_PICKEDUP, "MID_INTEL_PICKEDUP"
	MID_SUBROUTINE_OBTAINED, "MID_SUBROUTINE_OBTAINED"
	MID_ADDITIVE_OBTAINED, "MID_ADDITIVE_OBTAINED"
	MID_PROCEDURAL_OBTAINED, "MID_PROCEDURAL_OBTAINED"
	MID_PRIMITIVE_OBTAINED, "MID_PRIMITIVE_OBTAINED"
	MID_SAVE_GAME, "MID_SAVE_GAME"
	MID_LOAD_GAME, "MID_LOAD_GAME"
	MID_START_GAME, "MID_START_GAME"
	MID_END_GAME, "MID_END_GAME"
	MID_START_LEVEL, "MID_START_LEVEL"
	MID_EXIT_LEVEL, "MID_EXIT_LEVEL"
	MID_SAVE_DATA, "MID_SAVE_DATA"
	MID_RESTART_LEVEL, "MID_RESTART_LEVEL"
	MID_END_LEVEL, "MID_END_LEVEL"
	MID_SERVER_ERROR, "MID_SERVER_ERROR"
	MID_LOAD_FAILED, "MID_LOAD_FAILED"
	MID_OBJECTIVES_DATA, "MID_OBJECTIVES_DATA"
	MID_SCMD, "MID_SCMD"
	MID_MULTIPLAYER_DATA, "MID_MULTIPLAYER_DATA"
	MID_MULTIPLAYER_SERVERDIR, "MID_MULTIPLAYER_SERVERDIR"
	MID_CLIENT_PLAYER_UPDATE, "MID_CLIENT_PLAYER_UPDATE"
	MID_FRAG_SELF, "MID_FRAG_SELF"
	MID_PINGTIMES, "MID_PINGTIMES"
	MID_SHAKE_SCREEN, "MID_SHAKE_SCREEN"
	MID_CHANGE_WORLDPROPERTIES, "MID_CHANGE_WORLDPROPERTIES"
	MID_CLIENTFX_CREATE, "MID_CLIENTFX_CREATE"
	MID_SFX_MESSAGE, "MID_SFX_MESSAGE"
	MID_CONSOLE_TRIGGER, "MID_CONSOLE_TRIGGER"
	MID_CONSOLE_COMMAND, "MID_CONSOLE_COMMAND"
	MID_CONSOLE_CLIENTFX, "MID_CONSOLE_CLIENTFX"
	MID_DECISION, "MID_DECISION"
	MID_DIFFICULTY, "MID_DIFFICULTY"
	MID_GAMETEXMGR, "MID_GAMETEXMGR"
	MID_DISPLAY_METER, "MID_DISPLAY_METER"
	MID_HANDSHAKE, "MID_HANDSHAKE"
	MID_GADGETTARGET, "MID_GADGETTARGET"
	MID_CLEAR_PROGRESSIVE_DAMAGE, "MID_CLEAR_PROGRESSIVE_DAMAGE"
	MID_SEARCH, "MID_SEARCH"
	MID_ADD_PUSHER, "MID_ADD_PUSHER"
	MID_STIMULUS, "MID_STIMULUS"
	MID_RENDER_STIMULUS, "MID_RENDER_STIMULUS"
	MID_ADD_GOAL, "MID_ADD_GOAL"
	MID_REMOVE_GOAL, "MID_REMOVE_GOAL"
	MID_PROJECTILE_FX, "MID_PROJECTILE_FX"
	MID_PLAYER_ATTACH, "MID_PLAYER_ATTACH"
	*/	
}
