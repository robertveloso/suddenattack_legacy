#include "StdAfx.h"

#include "NJeanGate.h"
#include "CasualPacket.h"

#include "Des.h"

#include <time.h>

#define USING_USER_NO


using namespace CasualGame;
using namespace Application;
using namespace Session::Message;
using namespace Session::Manage;
using namespace Session::Gate;
using namespace Common::DataStructure;

CNJeanGate::CNJeanGate()
:	m_aRoom	( NULL	)
{
//	CLog::SetOutputLevel( CLog::LogLevel::None, CLog::LogLevel::Always, CLog::LogLevel::Always );
//	CLog::OpenFile( "c:\\jolyosorry_log.txt" );
}

CNJeanGate::~CNJeanGate()
{
//	CLog::CloseFile();
}

BOOL CNJeanGate::Create( INT nMaxPacketSize, INT nMaxClients, INT nMaxResponseBufferSize, INT nMaxRoomUser, INT nMaxChannels, INT nPort )
{
	INT		nTickTime;				// Tick ¹ß»ý Å¸ÀÓ (ms ´ÜÀ§)
	INT		nMaxTick;				// µî·ÏÇÒ Å¸ÀÌ¸ÓÀÇ ÃÖ´ë Tick (¹Ýµå½Ã Max°ªÀ» ³ÖÁö ¾Ê¾Æµµ µÈ´Ù. ´Ù¸¸ ÀÌ °ªÀ» ³Ñ´Â ºñÀ²¸¸Å­ÀÇ ¿À¹öÇìµå°¡ ÀÖ´Ù.
	INT		nTimerSignalQueueSize;	// Å¸ÀÌ¸Ó°¡ ÇÑ¹ø ¹ß»ý½ÃÅ°´Â ½Ã±×³ÎÀÇ °³¼ö : Create¿¡¼­ ¼±¾ðÇÑ ÃÖ´ë ½Ã±×³ÎÀÇ °³¼ö¸¦ ³Ñ¾î¼­´Â ¾ÈµÈ´Ù.
	INT		nAliveTickError;		// Alive Ã³¸®¸¦ À§ÇÑ Timeout Tick°ªÀÇ ¿ÀÂ÷ Çã¿ë ¹üÀ§
	INT		nAliveTimeoutCount;		// nAliveTimeoutCount * nAliveTickError = Timeout Tick

	nTickTime				= 1000;
	nMaxTick				= 10;
	nTimerSignalQueueSize	= 500;
	nAliveTickError			= 3;
	nAliveTimeoutCount		= 10;		// 30ÃÊ ´ÜÀ§·Î Ping Ã¼Å©
	m_nPort	= nPort;


//	INT		nMaxPacketSize			= 8192;

	CSessionManager::NetworkType
			networkType				= CSessionManager::NetworkType::TCP_IOCP;
	CSessionManager::QueryType
			queryType				= CSessionManager::QueryType::HTTP;
	INT		nRecvQueueSize			= 1000;
	INT		nSendQueueSize			= 3000;
	INT		nQueryQueueSize			= 1000;
	INT		nSignalQueueSize		= 50000;
	INT		nNumOfListener			= 1;
	INT		nPacketSize				= 256;

	BOOL bRet = CSessionGate::Create( nTickTime, nMaxTick, nTimerSignalQueueSize, nAliveTickError, nAliveTimeoutCount, m_nAliveManaged, networkType, queryType, nRecvQueueSize, nSendQueueSize, nQueryQueueSize, nSignalQueueSize,
									  nNumOfListener, nMaxClients, nPacketSize,	nMaxResponseBufferSize );
	if( !bRet ) return FALSE;

	//
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù. : n°³ÀÇ Listener¸¦ Create ½ÃÄÑÁØ µÚ¿¡ CreateEnd¸¦ È£ÃâÇÑ´Ù.
	//


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ÀÛ¼ºÀÚ : °­ÁøÇù
	// ÀÛ¼ºÀÏ : 2004³â 5¿ù 21ÀÏ
	// ³»  ¿ë : Client Listener »ý¼º
	//		1. NJean¿Í ¿¬°áµÇ´Â ³×Æ®¿öÅ©´Â 1°³ÀÇ ListenÀÌ¸ç ServicerIndex::ClientÀÇ ÀÎµ¦½º¸¦ °¡Áø´Ù.
	//

	INT		nServicerIndex;			// ¿¬°áµÇ´Â ¼­ºñ¼­ ÀÎµ¦½º
	INT		nNumOfCommSockets;		// ÇØ´ç ¼­ºñ¼­¸¦ ÅëÇØ ¿¬°áµÉ ¼ö ÀÖ´Â Å¬¶óÀÌ¾ðÆ®ÀÇ °³¼ö
	INT		nAcceptPoolSize;		// ÇØ´ç ¼­ºñ¼­ÀÇ Accept Pool Å©±â

	// Listener »ý¼º
	nServicerIndex		= ServicerIndex::Client;
	nNumOfCommSockets	= nMaxClients;
	nAcceptPoolSize		= 4;
	bRet = CreateListener( nServicerIndex, nNumOfCommSockets, nAcceptPoolSize );
	if( !bRet ) return FALSE;

	// ÇØ´ç ListenerÀÇ ClientµéÀÇ ¸Þ¸ð¸® Ç® »ý¼º
	bRet = m_UserPool.Create( nNumOfCommSockets );
	CUser** paUser = new CUser*[nNumOfCommSockets];
	for( INT i = 0; i < nNumOfCommSockets; i++ )
	{
		paUser[i]				= m_UserPool.Alloc();
		paUser[i]->Create		( nMaxPacketSize );
		paUser[i]->SetPocket	( NewPocket( PocketType::User ) );
	}
	for( INT i = 0; i < nNumOfCommSockets; i++ )
	{
		m_UserPool.Free( paUser[i] );
	}
	delete[] paUser;

	if( !bRet ) return FALSE;

	m_nNumOfClients		= nNumOfCommSockets;
	m_nAliveManaged		= nNumOfCommSockets;

	// Listener »ý¼ºÀÇ ³¡
	bRet = CreateEnd();
	if( !bRet ) return FALSE;
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////




	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ÀÛ¼ºÀÚ : °­ÁøÇù
	// ÀÛ¼ºÀÏ : 2004³â 5¿ù 22ÀÏ
	// ³»  ¿ë : Room¿¡ °ü·ÃµÈ °´Ã¼ ÃÊ±âÈ­
	//
	m_nMaxRoom			= m_nNumOfClients;
	m_nMaxRoomUser		= nMaxRoomUser;

	m_baTmpRoomInnerIndex	= new BOOL[m_nMaxRoomUser];

	m_InnerManagerUnusedRoom	.Init();
	m_InnerManagerUsedRoom		.Init();


	// ÀüÃ¼ RoomÀ» ÇÑ¹ø¿¡ Ã£±â À§ÇÑ InnerIndex µî·Ï
	m_aRoom = new CRoom[m_nMaxRoom];
	for( INT i = 0; i < m_nMaxRoom; i++ )
	{
		m_aRoom[i].SetRoomIndex	( i );
		m_aRoom[i].SetPocket	( NewPocket( PocketType::Room ) );
	}


	//
	// Ã¤³Î »ý¼º : ÀÓ½Ã·Î Ã¤³Î 2°³, Ã¤³Î´ç Room 20°³·Î ÇÑ´Ù. »ç¿ëÀÚ 200À¸·Î ÇÑ´Ù.
	//
	INT		nRooms;	// Ã¤³Î´ç ¹æÀÇ °³¼ö
	INT		nUsers; // Ã¤³Î´ç »ç¿ëÀÚ °³¼ö
	string	strChannelName;

	m_nMaxChannel	= nMaxChannels + 1;		// 0 ÓÃ×÷Ã»ÓÐ½øÈëchannelµÄ½ÇÉ«µÄ¹ÜÀí

	m_aChannel = new CChannel[m_nMaxChannel];
	for( INT i = 1; i < m_nMaxChannel; i++ )
	{
		// ÇÏÀ§ Å¬·¡½º¿¡¼­ nUsers, nRooms, strChannelNameÀ» °¡Á®¿Â´Ù.
		InitChannel( i, nUsers, nRooms, strChannelName );

		m_aChannel[i].Init( nUsers, nRooms, strChannelName, i );

		m_aChannel[i].SetPocket	( NewPocket( PocketType::Channel ) );

		m_aRoom[i-1].Init( i );
		m_aRoom[i-1].Open( nUsers, "´ë±â½Ç" );
		m_aChannel[i].SetWaitRoom( &m_aRoom[i-1] );
	}
	m_aChannel[0].Init( m_nNumOfClients, 0, "Ã»½øchannel", 0 );

//	for( INT i = m_nMaxChannel - 1; i < m_nMaxRoom; i++ )
	for( INT i = m_nMaxRoom - 1; i >= m_nMaxChannel - 1; i-- )//£¿£¿£¿ ÎªÊ²Ã´²»ÊÇm_nMaxRoom
	{
		m_InnerManagerUnusedRoom.Add( m_aRoom[i].GetInnerObjectForGame() );
	}


	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////


	// ÆÐÅ¶µé »ý¼º
	m_pCasualPackets	= new TLibPackets;
	
	

	return TRUE;
}

BOOL CNJeanGate::Start()
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ÀÛ¼ºÀÚ : °­ÁøÇù
	// ÀÛ¼ºÀÏ : 2004³â 5¿ù 21ÀÏ
	// ³»  ¿ë : Client Listener Accept ½ÃÀÛ
	//
	INT		nServicerIndex;			// ¿¬°áµÇ´Â ¼­ºñ¼­ ÀÎµ¦½º

	nServicerIndex		= ServicerIndex::Client;
	StartListen( nServicerIndex, m_nPort );
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	INT		nAliveTickError;		// Alive Ã³¸®¸¦ À§ÇÑ Timeout Tick°ªÀÇ ¿ÀÂ÷ Çã¿ë ¹üÀ§

	nAliveTickError			= 3;
	
	// Start´Â blocking ÇÔ¼öÀÌ´Ù.
	// Start´Â ¸ðµç ¼­¹öÀÇ ¸Þ½ÃÁö¸¦ Ã³¸®ÇÏ´Â ºÎºÐÀ» ´ã°í ÀÖ´Ù.
	// ¸¸ÀÏ return FALSE°¡ »ý±ä´Ù¸é, Start ÃÊ±âÈ­ ½ÇÆÐÀÎ °æ¿ìÀÌ´Ù.
	return CSessionGate::Start( nAliveTickError );
}

VOID CNJeanGate::Destroy()
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ÀÛ¼ºÀÚ : °­ÁøÇù
	// ÀÛ¼ºÀÏ : 2004³â 5¿ù 21ÀÏ
	// ³»  ¿ë : User PoolÀ» Á¦°ÅÇÑ´Ù.
	//
	m_UserPool.Destroy();
	/*
	if( m_aRoom )
	{
		for( INT i = 0; i < m_nMaxRoom; i++ )
		{
			m_aRoom[i].Destroy();
		}
		delete[] m_aRoom;
	}
	*/
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	CSessionGate::Destroy();
}


CPeer* CNJeanGate::AllocPeer( INT nServicerIndex )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.

	CPeer* pPeer;

	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		if( m_UserPool.GetAllocatedSize() < m_nNumOfClients )
			pPeer	= m_UserPool.Alloc();
		else
			pPeer	= NULL;
		break;

	default :
		pPeer	= NULL;
		break;
	}

	return pPeer;
}

VOID CNJeanGate::FreePeer( CPeer* pPeer, INT nServicerIndex )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.

	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		m_UserPool.Free( (CUser*)pPeer );
		break;

	default :
		break;
	}
}

VOID CNJeanGate::InitListenerPacketType(
		INT									nServicerIndex,
		CPacketCheckSum::CheckSumType&		out_CheckSumType,
		CPacketAssembler::PacketUnitType&	out_PacketUnitType )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.

	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		out_CheckSumType	= CPacketCheckSum::CheckSumType::TwoByteZoroOne;
		out_PacketUnitType	= CPacketAssembler::PacketUnitType::Length;
		break;

	default :
		break;
	}
}

VOID CNJeanGate::InitLengthFieldOfPacket(
		INT									nServicerIndex,
		INT&								out_nLengthFieldPos,
		CPacketAssembler::LengthSizeType&	out_lengthSizeType,
		CPacketAssembler::LengthType&		out_lengthType,
		BOOL&								out_bLittleEndian )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.

	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		out_nLengthFieldPos		= CPacket::GetLengthPos();
		out_lengthSizeType		= CPacketAssembler::LengthSizeType::TwoByte;
		out_lengthType			= CPacketAssembler::LengthType::Total;
		out_bLittleEndian		= TRUE;
		break;

	default :
		break;
	}
}

BOOL CNJeanGate::IsAliveManageObject( INT nServicerIndex )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.

	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		return TRUE;

	default :
		return FALSE;
	}
}



VOID CNJeanGate::OnSignal( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.
	switch( uiMessage )
	{
	case CCasualCommon::CasualSignalMessage::ChangeCaptineSignal :
		SignalProcessChangeCaptine( wParam, lParam );
		break;

	case CCasualCommon::CasualSignalMessage::CompulseExitSignal :
		SignalProcessCompulseExit( wParam, lParam );
		break;

	default:
		OnCasualSignal( uiMessage, wParam, lParam );
	}
}

VOID CNJeanGate::OnResponse( CPeer* pPeer, UINT uiMessage, BOOL bSuccess, _CommandPtr& pCmd )
{
	OnCasualResponse( static_cast<CUser*>( pPeer ), uiMessage, bSuccess, pCmd );
}


VOID CNJeanGate::OnResponse( CPeer* pPeer, UINT uiMessage, DWORD dwError, INT nResult, CHAR* pData, INT nDataSize )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.
	switch( uiMessage )
	{
	case CCasualCommon::CasualQueryMessage::LoginQuery :
		OnResponseLoginQuery( static_cast<CUser*>( pPeer ), dwError, nResult, pData, nDataSize );
		break;

		/*
	case CCasualCommon::CasualQueryMessage::TestQuery :
		memcpy( m_pCasualPackets->TestReply.Data, pData, nDataSize );
		m_pCasualPackets->TestReply.Data[nDataSize] = NULL;
		m_pCasualPackets->TestReply.SetSize( nDataSize + 1 );
		SendData( pPeer, (CHAR*)&m_pCasualPackets->TestReply, m_pCasualPackets->TestReply.GetLength() );
		break;
		*/

	default :
		OnCasualResponse( static_cast<CUser*>( pPeer ), uiMessage, dwError, nResult, pData, nDataSize );
	}
}

VOID CNJeanGate::OnResponseLoginQuery( CUser* pUser, DWORD dwError, INT nResult, CHAR* pData, INT nDataSize )
{
	LoginReply( pUser, TRUE );
}


VOID CNJeanGate::OnAccept( CPeer* pPeer, INT nServicerIndex )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.
	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		break;

	default:
		return;
	}

	CUser* pUser = (CUser*) pPeer;

	pUser->SetLogined		( FALSE	);		// ·Î±×ÀÎ ÁßÀÌ ¾Æ´Ô
	pUser->SetService		( FALSE	);		// ¼­ºñ½º ÁßÀÌ ¾Æ´Ô
	pUser->SetMapped		( FALSE );		// ¸Ê¿¡ µî·ÏµÇÁö ¾ÊÀ½
	pUser->SetRoomIndex		( -1	);		// ¹æ¿¡ Âü¿©ÁßÀÌ ¾Æ´Ô
	pUser->SetChannelIndex	( -1	);		// Ã¤³Î¿¡ Âü¿©ÁßÀÌ ¾Æ´Ô

	OnCasualAccept( pUser );
}

VOID CNJeanGate::OnConnect( CPeer* pPeer, INT nServicerIndex )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.
	OnCasualConnect( (CUser*)pPeer );
}

VOID CNJeanGate::OnClose( CPeer* pPeer, INT nServicerIndex )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.
	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		break;

	default:
		return;
	}

	CUser* pUser = (CUser*) pPeer;

	OnCasualClose( pUser );

	//
	// »ç¿ëÀÚ°¡ ¼­ºñ½º ÁßÀÌ ¾Æ´Ï¸é ¹æ¿¡ ÀÔÀåÇÏÁöµµ, ·Î±×ÀÎ ¼¼¼ÇÀÌ µî·ÏµÇÁöµµ ¾Ê´Â´Ù.
	//
	if( !pUser->IsService() || pUser->GetChannelIndex() < 0 )
		return;

	//
	// »ç¿ëÀÚ°¡ ´ë±âÃ¤³Î¿¡ ÀÖÀ¸¸é
	//
	if( pUser->GetChannelIndex() == 0 )
	{
		m_aChannel[0].Leave( pUser );
		if( pUser->IsMapped() )
		{
#ifdef USING_USER_NO
            m_UserMap.EraseFromUserNo( pUser );
#else
			m_UserMap.EraseFromID( pUser );
#endif
			pUser->SetMapped( FALSE );
		}
		return;
	}


	//
	// Ã¤³Î¿¡ ÀÔÀåÁßÀÎ °æ¿ì
	//

	// ÅðÀåÇÏ´Â »ç¿ëÀÚ°¡ ¼ÓÇÑ ¹æ°ú Ã¤³ÎÀÇ Ref¸¦ °¡Á®¿Â´Ù.
	CRoom*		pRoom		= &m_aRoom[pUser->GetRoomIndex()];
	CChannel*	pChannel	= &m_aChannel[pUser->GetChannelIndex()];

	// Ã¤³Î¿¡¼­ ³ª°£´Ù.
	pChannel->Leave( pUser );

	// ÀÏ¹Ý ¹æ¿¡ Join ÁßÀÎ °æ¿ì
	if( !pChannel->IsWaitRoom( pRoom ) )
	{
		// ÅðÀåÇÏ´Â ¹æ¿¡ ³²¾Æ ÀÖ´Â »ç¿ëÀÚµé¿¡°Ô ÅðÀå Á¤º¸¸¦ Àü´Þ
		m_pCasualPackets->RemoveRoomUser.SetProperty(
			pUser->GetID().c_str(),
			static_cast<INT>( pUser->GetID().length() )
			);

		SendToRoom( pRoom, pUser, &m_pCasualPackets->RemoveRoomUser, m_pCasualPackets->RemoveRoomUser.GetLength() );

		// ¹æ¿¡¼­ ³ª°£´Ù. ÀÌ ¶§, ¹æÀÌ ºñ¾î ÀÖÀ¸¸é Unused °´Ã¼¿¡ ¹ÝÈ¯ÇÑ´Ù.
		if( !pRoom->Leave( pUser ) )
		{
			int a = 3;
		}
		pRoom->FreeEmptyInnerIndex( pUser->GetRoomInnerIndex() );

		if( pRoom->IsEmpty() )
		{
			m_InnerManagerUsedRoom		.Remove	( pRoom->GetInnerObjectForGame() );
//			m_InnerManagerUnusedRoom	.Push	( pRoom->GetInnerObjectForGame() );

			// ´ë±â½ÇÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô ¹æÀÌ ´ÝÇûÀ½À» Àü´ÞÇÑ´Ù.
			m_pCasualPackets->RemoveRoom.SetProperty(
				pRoom->GetRoomIndex()
				);

			SendToRoom( pChannel->GetWaitRoom(), NULL, &m_pCasualPackets->RemoveRoom, m_pCasualPackets->RemoveRoom.GetLength() );

			// Ã¤³Î¿¡¼­ ÇØ´ç ¹æÀ» Á¦°ÅÇÑ´Ù.
			pChannel->RemoveRoom( pRoom );
		}
		else
		{
			// ´ë±â½ÇÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô ¹æÁ¤º¸ º¯°æÀ» Àü´ÞÇÑ´Ù.
			m_pCasualPackets->RoomInfoChange.SetProperty(
				pRoom->GetRoomIndex(),
				pRoom->GetMaxUser(),
				pRoom->GetNumOfUsers(),
				pRoom->IsPassword(),
				pRoom->GetOption()
				);

			SendToRoom( pChannel->GetWaitRoom(), NULL, &m_pCasualPackets->RoomInfoChange, m_pCasualPackets->RoomInfoChange.GetLength() );

			// ÀÚ½ÅÀÌ ¹æÀåÀÌ¾úÀ¸¸é ¹æÀåÀ» »õ·Î »Ì±âÀ§ÇÑ ½Ã±×³ÎÀ» º¸³½´Ù.
			if( pUser->IsCaptine() && !pRoom->IsEmpty() )
			{
				// ¹æÀåÀ» »õ·Î À§ÀÓÇÒÁö °áÁ¤ 
				SHORT	nResult = 0;
				if( IsCorrectCondition( nResult, pUser, PacketCondition::ChangeCaptine, reinterpret_cast<ULONG_PTR>( pRoom ) ) )
				{
					// ¹æÀåÀ» »õ·Î »Ì±âÀ§ÇÑ ½Ã±×³ÎÀ» º¸³½´Ù.
					AddSignal( CCasualCommon::CasualSignalMessage::ChangeCaptineSignal, pUser->GetRoomIndex(), 0 );
				}
				else
				{
					// ÇöÀç ¹æÀÇ ¸ðµç »ç¿ëÀÚ¸¦ °­Åð½ÃÅ²´Ù.
					CUser* pRoomUser = pRoom->GetFirstUser();
					while( pRoomUser )
					{
						// °­Åð ½Ã±×³ÎÀ» º¸³½´Ù.
						AddSignal(
							CCasualCommon::CasualSignalMessage::CompulseExitSignal,							// °­Åð¸Þ½ÃÁö
							reinterpret_cast<ULONG_PTR>( pRoomUser ),										// °­ÅðµÇ´Â »ç¿ëÀÚ
							static_cast<ULONG_PTR>( CCasualPacketCompulsionExited::Type::CloseRoom )	// °­Åð Å¸ÀÔ
							);
						pRoomUser = pRoom->GetNextUser();
					}
				}
			}

			if( pUser->IsCaptine() )
				pUser->SetCaptine( FALSE );
		}

	}
	// ´ë±â½Ç¿¡ Join ÁßÀÎ °æ¿ì
	else
	{
		// ´ë±â½Ç¿¡¼­ ³ª°£´Ù.
		pRoom->Leave( pUser );

		// ´ë±â½Ç¿¡ ³²¾Æ ÀÖ´Â »ç¿ëÀÚµé¿¡°Ô ÅðÀå Á¤º¸¸¦ Àü´Þ
		m_pCasualPackets->RemoveUser.SetProperty(
			pUser->GetID().c_str(),
			static_cast<INT>( pUser->GetID().length() )
			);

		SendToRoom(
			pRoom,
			NULL,
			&m_pCasualPackets->RemoveUser,
			static_cast<INT>( m_pCasualPackets->RemoveUser.GetLength() ) );
	}

	if( pUser->IsMapped() )
	{
#ifdef USING_USER_NO
		m_UserMap.EraseFromUserNo( pUser );
#else
		m_UserMap.EraseFromID( pUser );
#endif
		pUser->SetMapped( FALSE );
	}
}

VOID CNJeanGate::OnData( CPeer* pPeer, INT nServicerIndex, CHAR* pPacket, INT nPacketLength )
{
	// ¿©±â¿¡ ¼Ò½º ÄÚµå¸¦ Ãß°¡ÇÑ´Ù.
	CPacket* pCasualPacket = (CPacket*) pPacket;
	CUser* pUser;
	pUser = static_cast<CUser*>( pPeer );

	switch( nServicerIndex )
	{
	case ServicerIndex::Client :
		switch( pCasualPacket->GetCommand() )
		{
		case CCasualCommon::CasualPacketCommand::LoginRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketLoginRequest* pLoginRequest;
			pLoginRequest = (CCasualPacketLoginRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pLoginRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessLoginRequest( pUser, pLoginRequest );
			break;

		case CCasualCommon::CasualPacketCommand::SecLoginRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketSecLoginRequest* pSecLoginRequest;
			pSecLoginRequest = (CCasualPacketSecLoginRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pSecLoginRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessSecLoginRequest( pUser, pSecLoginRequest );
			break;

		case CCasualCommon::CasualPacketCommand::JoinChannelRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketJoinChannelRequest* pJoinChannelRequest;
			pJoinChannelRequest = (CCasualPacketJoinChannelRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pJoinChannelRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessJoinChannelRequest( pUser, pJoinChannelRequest );
			break;

		case CCasualCommon::CasualPacketCommand::LeaveChannelRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketLeaveChannelRequest* pLeaveChannelRequest;
			pLeaveChannelRequest = (CCasualPacketLeaveChannelRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pLeaveChannelRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessLeaveChannelRequest( pUser, pLeaveChannelRequest );
			break;

		case CCasualCommon::CasualPacketCommand::OpenRoomRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketOpenRoomRequest* pOpenRoomRequest;
			pOpenRoomRequest = (CCasualPacketOpenRoomRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pOpenRoomRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessOpenRoomRequest( pUser, pOpenRoomRequest );
			break;

		case CCasualCommon::CasualPacketCommand::JoinRoomRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketJoinRoomRequest* pJoinRoomRequest;
			pJoinRoomRequest = (CCasualPacketJoinRoomRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pJoinRoomRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessJoinRoomRequest( pUser, pJoinRoomRequest );
			break;

		case CCasualCommon::CasualPacketCommand::LeaveRoomRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketLeaveRoomRequest* pLeaveRoomRequest;
			pLeaveRoomRequest = (CCasualPacketLeaveRoomRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pLeaveRoomRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessLeaveRoomRequest( pUser, pLeaveRoomRequest );
			break;

		case CCasualCommon::CasualPacketCommand::AutoJoinRoomRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketAutoJoinRoomRequest* pAutoJoinRoomRequest;
			pAutoJoinRoomRequest = (CCasualPacketAutoJoinRoomRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pAutoJoinRoomRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessAutoJoinRoomRequest( pUser, pAutoJoinRoomRequest );
			break;

		case CCasualCommon::CasualPacketCommand::FindRequest :

			::OutputDebugString( "FindRequest\n" );

			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketFindRequest* pFindRequest;
			pFindRequest = (CCasualPacketFindRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pFindRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessFindRequest( pUser, pFindRequest );
			break;

		case CCasualCommon::CasualPacketCommand::Chat :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketChat* pChat;
			pChat = (CCasualPacketChat*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pChat->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessChat( pUser, pChat );
			break;

		case CCasualCommon::CasualPacketCommand::Memo :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketMemo* pMemo;
			pMemo = (CCasualPacketMemo*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pMemo->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessMemo( pUser, pMemo );
			break;

		case CCasualCommon::CasualPacketCommand::Invite :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketInvite* pInvite;
			pInvite = (CCasualPacketInvite*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pInvite->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessInvite( pUser, pInvite );
			break;

		case CCasualCommon::CasualPacketCommand::CompulseExitRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketCompulsionExitRequest* pCompulseExitRequest;
			pCompulseExitRequest = (CCasualPacketCompulsionExitRequest*) pCasualPacket;

			// ½ÇÁ¦ ÆÐÅ¶ ±æÀÌ¿Í ¸Â´ÂÁö °Ë»ç
			if( nPacketLength != pCompulseExitRequest->GetRealLength() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
			if( !pUser->IsService() )
			{
				CloseRequest( pPeer );
				break;
			}

			// ÆÐÅ¶ Ã³¸®
			PacketProcessCompulseExitRequest( pUser, pCompulseExitRequest );
			break;

			/*
		case CCasualCommon::CasualPacketCommand::TestRequest :
			// ½ÇÁ¦ ÆÐÅ¶À¸·Î Ä³½ºÆÃ
			CCasualPacketTestRequest* pTestRequest;
			pTestRequest = (CCasualPacketTestRequest*) pCasualPacket;

			RequestHttp(
				pUser,
				CCasualCommon::CasualQueryMessage::TestQuery,
				"http://daum.net/"
				);
			break;
			*/

		default :
			OnCasualData( pUser, pCasualPacket, nPacketLength );
//			CloseRequest( pPeer );
			break;
		}
		break;

	default :
		// ¼­ºñ½º ÁßÀÎÁö °Ë»ç
		if( !pUser->IsService() )
		{
			//Ã»ÓÐµÇÂ½³É¹¦
			CloseRequest( pPeer );
			break;
		}
		OnCasualData( static_cast<CUser*>( pPeer ), reinterpret_cast<CPacket*>( pPacket ), nPacketLength );

		break;
	}
}

VOID CNJeanGate::OnTimeout( CPeer* pPeer )
{
	if( OnCasualTimeout( static_cast<CUser*>( pPeer ) ) )
		TimeoutDisconnectRequest( pPeer );
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Util Fuctions

// ¹æÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
CRoom* CNJeanGate::GetRoomRef( CUser* pUser)
{
	if( pUser->GetChannelIndex() <= 0 || pUser->GetChannelIndex() >= m_nMaxChannel )
	{
		return NULL;
	}

	return &m_aRoom[ pUser->GetRoomIndex() ];
}

// ¹æÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
CRoom* CNJeanGate::GetRoomRef( INT nRoomIndex )
{
	if( nRoomIndex < 0 || nRoomIndex >= m_nMaxRoom )
		return NULL;

	return &m_aRoom[ nRoomIndex ];
}

// Ã¤³ÎÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
CChannel* CNJeanGate::GetChannelRef( CUser* pUser )
{
	if( pUser->GetChannelIndex() <= 0 || pUser->GetChannelIndex() >= m_nMaxChannel )
	{
		return NULL;
	}

	return &m_aChannel	[ pUser->GetChannelIndex() ];
}

// Ã¤³ÎÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
CChannel* CNJeanGate::GetChannelRef( INT nChannelIndex )
{
	if( nChannelIndex <= 0 || nChannelIndex >= m_nMaxChannel )
	{
		return NULL;
	}

	return &m_aChannel	[ nChannelIndex	];
}

// ÇØ´ç ¹æÀÌ ´ë±â½ÇÀÎÁö¸¦ ¹ÝÈ¯ÇÑ´Ù.
BOOL CNJeanGate::IsWaitRoom ( CRoom* pRoom )
{
	CChannel* pChannel;
	if( pRoom->GetChannelIndex() <= 0 || pRoom->GetChannelIndex() >= m_nMaxChannel )
	{
		return FALSE;
	}
	
	pChannel	= &m_aChannel[ pRoom->GetChannelIndex() ];

	return pChannel->IsWaitRoom( pRoom );
}

// ÇØ´ç ¹æÀÌ ´ë±â½ÇÀÎÁö¸¦ ¹ÝÈ¯ÇÑ´Ù.
BOOL CNJeanGate::IsWaitRoom ( INT nRoomIndex )
{
	CRoom*		pRoom;
	CChannel*	pChannel;

	if( nRoomIndex < 0 || nRoomIndex >= m_nMaxRoom )
		return FALSE;
	pRoom		= &m_aRoom[ nRoomIndex ];

	if( pRoom->GetChannelIndex() <= 0 || pRoom->GetChannelIndex() >= m_nMaxChannel )
	{
		return FALSE;
	}
	pChannel	= &m_aChannel[ pRoom->GetChannelIndex() ];

	return pChannel->IsWaitRoom( pRoom );
}

// ÇØ´ç Ã¤³ÎÀÌ ´ë±âÃ¤³ÎÀÎÁö¸¦ ¹ÝÈ¯ÇÑ´Ù.
BOOL CNJeanGate::IsWaitChannel ( CChannel* pChannel )
{
	return pChannel->GetChannelIndex() == 0;
}

BOOL CNJeanGate::IsWaitChannel ( INT nChannel )
{
	return nChannel == 0;
}

// ÇØ´ç ¹æÀÌ ¼ÓÇÑ Ã¤³ÎÀÇ Ref¸¦ ¹ÝÈ¯ÇÑ´Ù.
CChannel* CNJeanGate::GetChannelRefOfRoom ( CRoom* pRoom )
{
	if( pRoom->GetChannelIndex() <= 0 || pRoom->GetChannelIndex() >= m_nMaxChannel )
	{
		return NULL;
	}
	
	return &m_aChannel[ pRoom->GetChannelIndex() ];
}

CChannel* CNJeanGate::GetChannelRefOfRoom ( INT nRoomIndex )
{
	CRoom*		pRoom;

	if( nRoomIndex < 0 || nRoomIndex >= m_nMaxRoom )
		return FALSE;
	pRoom		= &m_aRoom[ nRoomIndex ];

	if( pRoom->GetChannelIndex() <= 0 || pRoom->GetChannelIndex() >= m_nMaxChannel )
	{
		return FALSE;
	}
	
	return &m_aChannel[ pRoom->GetChannelIndex() ];
}

VOID CNJeanGate::UrlParsing(CHAR* pStr)
{
	const INT STR_MAX = CPacket::PredefineLength::GetQuerySize;

	CHAR str[STR_MAX];
	::memcpy( str, pStr, STR_MAX );
	INT nLen = static_cast<INT>( strlen( str ) );
	INT nCount = 0;
	for ( INT i = 0 ; i < nLen ; i++, nCount++ )
	{
		if ( i >= STR_MAX ) { pStr[nCount] = 0; return; }

		CHAR ch;
		if ( str[i] == '%' )
		{
			CHAR temp[2] = { str[i+1], str[i+2] };
			ch = HexToChar(temp);
			i += 2;
		}
		else if ( str[i] == '+' ) 
		{
			ch = ' ';
		}
		else
		{
			ch = str[i];
		}
		pStr[nCount] = ch;
		if ( ch == 13 )
		{
			nCount++;
			pStr[nCount] = 10;
		}
	}
	pStr[nCount] = 0;
}

CHAR CNJeanGate::HexToChar(CHAR* pStr)
{
	CHAR Hex[2];
	Hex[0] = pStr[0];
	Hex[1] = pStr[1];
	if ( Hex[0] > 57 ) 
		Hex[0] = Hex[0] - 87;
	else
		Hex[0] = Hex[0] - 48;

	if ( Hex[1] > 57 )
		Hex[1] = Hex[1] - 87;
	else
		Hex[1] = Hex[1] - 48;

	UCHAR Dec;
	Dec = (Hex[0]<<4) + Hex[1];

	return Dec;
}


// ÇØ´ç ¹æ¿¡ ¸Þ½ÃÁö¸¦ Áß°èÇÑ´Ù.
VOID CNJeanGate::SendToRoom( CRoom* pRoom, const CUser* pExeptUser, const CPacket* pPacket, INT nPacketSize )
{
	CUser* pUser = pRoom->GetFirstUser();
	while( pUser )
	{
		if( pUser != pExeptUser )
			SendData( pUser, reinterpret_cast<const CHAR*>(pPacket), nPacketSize );
		pUser = pRoom->GetNextUser();
	}
}

// P2P¿¡¼­ ¹æÀåÀÌ µÉ ¼ö ÀÖ´ÂÁö¸¦ ¼³Á¤ÇÑ´Ù.
inline VOID CNJeanGate::InspectP2PCaptine( CUser* pUser, LPCTSTR strIP )
{
	if( ::strcmp( GetPeerAddr( pUser ), strIP ) == 0 )
		pUser->SetP2PCaptine( TRUE );
	else
		pUser->SetP2PCaptine( FALSE );
}

// ¼­¹ö ³»ÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
VOID CNJeanGate::SendToAllUser( const CPacket* pPacket, INT nPacketSize )
{
	CUser* pUser;

	// 0¹ø ´ë±â Ã¤³ÎÀÇ »ç¿ëÀÚ¿¡°Ô´Â ¸Þ½ÃÁö¸¦ º¸³»Áö ¾Ê´Â´Ù.
	for( INT i = 1; i < m_nMaxChannel; i++ )
	{
		pUser = m_aChannel[i].GetFirstUser();
		while( pUser )
		{
			SendData( pUser, reinterpret_cast<const CHAR*>(pPacket), nPacketSize );
			pUser = m_aChannel[i].GetNextUser();
		}
	}
}

// Ã¤³Î ³»ÀÇ ´ë±â½Ç¿¡ ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
VOID CNJeanGate::SendToWaitRoom( CChannel* pChannel, const CUser* pExeptUser, const CPacket* pPacket, INT nPacketSize )
{
	CRoom* pWaitRoom = pChannel->GetWaitRoom();
	SendToRoom( pWaitRoom, pExeptUser, pPacket, nPacketSize );
}

// Ã¤³Î ³»ÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
VOID CNJeanGate::SendToAllUser( CChannel* pChannel, const CPacket* pPacket, INT nPacketSize )
{
	CUser* pUser;

	pUser = pChannel->GetFirstUser();
	while( pUser )
	{
		SendData( pUser, reinterpret_cast<const CHAR*>(pPacket), nPacketSize );
		pUser = pChannel->GetNextUser();
	}
}

// ¼­¹ö ³»ÀÇ ¸ðµç ¿­¸° ¹æ¿¡ ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
VOID CNJeanGate::SendToAllRoomUser( const CPacket* pPacket, INT nPacketSize )
{
	CRoom*	pRoom;
	CUser*	pUser;

	// 0¹ø ´ë±â Ã¤³ÎÀÇ »ç¿ëÀÚ¿¡°Ô´Â ¸Þ½ÃÁö¸¦ º¸³»Áö ¾Ê´Â´Ù.
	for( INT i = 1; i < m_nMaxChannel; i++ )
	{
		pRoom = m_aChannel[i].GetFirstRoom();
		while( pRoom )
		{
			pUser = pRoom->GetFirstUser();
			while( pUser )
			{
				SendData( pUser, reinterpret_cast<const CHAR*>(pPacket), nPacketSize );
				pUser = pRoom->GetNextUser();
			}
			pRoom = m_aChannel[i].GetNextRoom();
		}
	}
}

// Ã¤³Î ³»ÀÇ ¸ðµç ¿­¸° ¹æ¿¡ ¸Þ½ÃÁö¸¦ Àü´ÞÇÑ´Ù.
VOID CNJeanGate::SendToAllRoomUser( CChannel* pChannel, const CPacket* pPacket, INT nPacketSize )
{
	CRoom*	pRoom;
	CUser*	pUser;

	pRoom = pChannel->GetFirstRoom();
	while( pRoom )
	{
		pUser = pRoom->GetFirstUser();
		while( pUser )
		{
			SendData( pUser, reinterpret_cast<const CHAR*>(pPacket), nPacketSize );
			pUser = pRoom->GetNextUser();
		}
		pRoom = pChannel->GetNextRoom();
	}
}

// »ç¿ëÀÚÀÇ À§Ä¡¸¦ ¹ÝÈ¯ÇÑ´Ù.
VOID CNJeanGate::GetUserPlace( CUser* pUser, CChannel* &out_pChannel, CRoom* &out_pRoom, BOOL &out_bWaitRoom )
{
	if( pUser->GetChannelIndex() <= 0 || pUser->GetChannelIndex() >= m_nMaxChannel )
	{
		out_pChannel	= NULL;
		out_pRoom		= NULL;
		out_bWaitRoom	= FALSE;
		return;
	}
	out_pChannel	= &m_aChannel	[ pUser->GetChannelIndex()	];
	out_pRoom		= &m_aRoom		[ pUser->GetRoomIndex()		];
	out_bWaitRoom	= out_pChannel->IsWaitRoom( out_pRoom );
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Response Ã³¸®
inline VOID CNJeanGate::LoginReply( CUser* pUser, BOOL bLogined )
{
	if( !bLogined )
	{
		// Å¬¶óÀÌ¾ðÆ®¿¡ ·Î±×ÀÎ ½ÇÆÐ¸¦ Åëº¸ÈÄ ¿¬°á ÇØÁ¦
		m_pCasualPackets->LoginReply.SetProperty( CCasualPacketLoginReply::LoginResult::Fail );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->LoginReply ), m_pCasualPackets->LoginReply.GetLength() );

		CloseRequest( pUser );
		return;
	}

//	pUser->SetUserProperty( lpszUserProperty );

	CUser* pFindUser;

#ifdef USING_USER_NO
	pFindUser = m_UserMap.FindFromUserNo( pUser->GetUserNo() );

#ifdef __TEST__
	if( pFindUser )
	{
		pFindUser = m_UserMap.FindFromID( pUser->GetID() );
	}
#endif

#else
	pFindUser = m_UserMap.FindFromID( pUser->GetID() );
#endif
		
	// ¿¬°áµÈ ¼¼¼ÇÀÌ ¾ø´Â °æ¿ì
	if( !pFindUser )
	{
		// ¸Ê¿¡ µî·Ï
		m_UserMap.Insert( pUser->GetUserNo(), pUser->GetID(), pUser->GetNick(), pUser );

		// ·Î±×ÀÎ ¿©ºÎ µî·Ï
		pUser->SetLogined( TRUE );

		// ¼­ºñ½º °¡´É µî·Ï
		pUser->SetService( TRUE );

		// ¸Ê¿¡ µî·ÏµÊ
		pUser->SetMapped( TRUE );

		// ±âº» Ã¤³Î¿¡ µî·Ï
		m_aChannel[0].Join( pUser );
		pUser->SetChannelIndex( 0 );

		// Å¬¶óÀÌ¾ðÆ®¿¡ ·Î±×ÀÎ ¼º°øÀ» Åëº¸
		m_pCasualPackets->LoginReply.SetProperty( CCasualPacketLoginReply::LoginResult::Success );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->LoginReply ), m_pCasualPackets->LoginReply.GetLength() );

		OnLoginSuccess( pUser );
	}
	else
	{
		// ·Î±×ÀÎ ¿©ºÎ µî·Ï
		pUser->SetLogined( TRUE );

		// Å¬¶óÀÌ¾ðÆ®¿¡ ´Ù¸¥°÷¿¡¼­ ¿¬°áÁßÀ» Åëº¸
		m_pCasualPackets->LoginReply.SetProperty( CCasualPacketLoginReply::LoginResult::Joined );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->LoginReply ), m_pCasualPackets->LoginReply.GetLength() );
	}
}

VOID CNJeanGate::LoginQueryRequest( CUser* pUser, LPCTSTR strLoginQuery )
{
	/*
	for( int i = 0; i < 10; i++ )
	{

		RequestHttp(
			pUser,
			111,
			strLoginQuery
			);
	}
	*/

	RequestHttp(
		pUser,
		CCasualCommon::CasualQueryMessage::LoginQuery,
		strLoginQuery
		);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// ½Ã±×³Î Ã³¸®

inline VOID CNJeanGate::SignalProcessChangeCaptine( ULONG_PTR wParam, ULONG_PTR lParam )
{
	INT nRoomIndex		= (INT) wParam;

	if( nRoomIndex < 0 || nRoomIndex >= m_nMaxRoom )
		return;

	CRoom* pRoom = &m_aRoom[nRoomIndex];

	if( pRoom->IsEmpty() )
		return;

	CUser* pCaptine;
	pCaptine = pRoom->GetFirstUser();
	while( pCaptine )
	{
		if( CanBeCaptine( pCaptine ) )
		{
			pCaptine->SetCaptine( TRUE );
			break;
		}
		pCaptine = pRoom->GetNextUser();
	}
	pRoom->SetCaptine( pCaptine );

	if( !pCaptine )
	{
		OnNoneCaptine( pRoom );
		return;
	}

	OnChangeCaptine( pCaptine, pRoom );
}

VOID CNJeanGate::OnChangeCaptine( CUser* pCaptine, CRoom* pRoom )
{
	// ÇöÀç ¹æÀÇ »ç¶÷µé¿¡°Ô ¹æÀå º¯°æ ÆÐÅ¶À» Àü¼ÛÇÑ´Ù.
	m_pCasualPackets->RoomCaptineChange.SetProperty(
		pCaptine->GetID().c_str(),
		static_cast<INT>( pCaptine->GetID().length() )
		);
	SendToRoom( pRoom, NULL, &m_pCasualPackets->RoomCaptineChange, m_pCasualPackets->RoomCaptineChange.GetLength() );
}

inline VOID CNJeanGate::SignalProcessCompulseExit( ULONG_PTR wParam, ULONG_PTR lParam )
{
	CUser*		pUser;		// ÅðÀåÇÏ´Â »ç¿ëÀÚ
	CRoom*		pRoom;		// »ç¿ëÀÚ°¡ ¼ÓÇß´ø ¹æ
	CChannel*	pChannel;	// »ç¿ëÀÚ°¡ ¼ÓÇß´ø Ã¤³Î
	
	CCasualPacketCompulsionExited::Type
				type;	// °­Á¦ ÅðÀå Å¸ÀÔ : ÀÏ¹Ý ÅðÀåµµ °°Àº ½Ã±×³Î·Î Ã³¸® ÇÏ¹Ç·Î lParamÀ¸·Î ±¸º°À» ÇÑ´Ù.

	pUser	= reinterpret_cast	<CUser*>								( wParam );
	type	= static_cast		<CCasualPacketCompulsionExited::Type>	( lParam );

	if( pUser->GetRoomIndex() < 0 || pUser->GetChannelIndex() <= 0 )
	{
		return;
	}

	pRoom		= &m_aRoom		[ pUser->GetRoomIndex()		];
	pChannel	= &m_aChannel	[ pUser->GetChannelIndex()	];

	if( pChannel->IsWaitRoom( pRoom ) )
		return;

	OnLeaveRoom( pUser, pRoom, type );


	// °­Á¦ ÅðÀå ´çÇÔÀ» ÅëÁöÇÑ´Ù.
	if( type != CCasualPacketCompulsionExited::Type::Mine )
	{
		m_pCasualPackets->CompulsionExited.SetProperty(
			type );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->CompulsionExited ), m_pCasualPackets->CompulsionExited.GetLength() );
	}


	// ¹æ¿¡¼­ ³ª°£´Ù. ÀÌ ¶§, ¹æÀÌ ºñ¾î ÀÖÀ¸¸é Unused °´Ã¼¿¡ ¹ÝÈ¯ÇÑ´Ù.
	if( !pRoom->Leave( pUser ) )
	{
		int a = 3;
	}

	pRoom->FreeEmptyInnerIndex( pUser->GetRoomInnerIndex() );
	if( pRoom->IsEmpty() )
	{
		m_InnerManagerUsedRoom		.Remove	( pRoom->GetInnerObjectForGame() );
//		m_InnerManagerUnusedRoom	.Push	( pRoom->GetInnerObjectForGame() );

		// Ã¤³Î¿¡¼­ ÇØ´ç ¹æÀ» Á¦°ÅÇÑ´Ù.
		pChannel->RemoveRoom( pRoom );
	}
	else
	{
		// ÀÚ½ÅÀÌ ¹æÀåÀÌ¾úÀ¸¸é
		if( pUser->IsCaptine() )
		{
			// ¹æÀåÀ» »õ·Î À§ÀÓÇÒÁö °áÁ¤ 
			SHORT nResult = 0;
			if( IsCorrectCondition( nResult, pUser, PacketCondition::ChangeCaptine, reinterpret_cast<ULONG_PTR>( pRoom ) ) )
			{
				// ¹æÀåÀ» »õ·Î »Ì±âÀ§ÇÑ ½Ã±×³ÎÀ» º¸³½´Ù.
				AddSignal( CCasualCommon::CasualSignalMessage::ChangeCaptineSignal, pUser->GetRoomIndex(), 0 );
			}
			else
			{
				// ÇöÀç ¹æÀÇ ¸ðµç »ç¿ëÀÚ¸¦ °­Åð½ÃÅ²´Ù.
				CUser* pRoomUser = pRoom->GetFirstUser();
				while( pRoomUser )
				{
					// °­Åð ½Ã±×³ÎÀ» º¸³½´Ù.
					AddSignal(
						CCasualCommon::CasualSignalMessage::CompulseExitSignal,							// °­Åð¸Þ½ÃÁö
						reinterpret_cast<ULONG_PTR>( pRoomUser ),										// °­ÅðµÇ´Â »ç¿ëÀÚ
						static_cast<ULONG_PTR>( CCasualPacketCompulsionExited::Type::CloseRoom )	// °­Åð Å¸ÀÔ
						);
					pRoomUser = pRoom->GetNextUser();
				}
			}
		}
	}
	pUser->SetCaptine( FALSE );

	// ´ë±â½Ç¿¡ ÀÔÀåÀ» ÇÑ´Ù.
	CRoom* pWaitRoom = pChannel->GetWaitRoom();
	pWaitRoom->Join( pUser );
	pUser->SetRoomIndex( pWaitRoom->GetRoomIndex() );

	// ¹æ¿¡¼­ ³ª°£´Ù.
	m_pCasualPackets->LeaveRoomReply.SetProperty( CCasualPacketLeaveRoomReply::Result::Success );
	SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->LeaveRoomReply ), m_pCasualPackets->LeaveRoomReply.GetLength() );

	// Ã¤³Î ÀÔÀåÀ» Åëº¸ÇÑ´Ù. : ´ë±â½Ç ÀÔÀå°ú µ¿ÀÏÇÏ°Ô Ãë±Þ
	m_pCasualPackets->JoinChannelReply.SetProperty(
		CCasualPacketJoinChannelReply::Result::Success,
		pWaitRoom->GetNumOfUsers() - 1,
		pChannel->GetNumOfOpenRooms()
		);
	SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinChannelReply ), m_pCasualPackets->JoinChannelReply.GetLength() );



	//
	// Step 6 : ¹æ ¸®½ºÆ®¸¦ Àü´Þ : Step 4,5¿Í º¯°æ
	//

	CRoom* pOpenRoom = pChannel->GetFirstRoom();
	while( pOpenRoom )
	{
		m_pCasualPackets->RoomInfo.SetProperty(	pOpenRoom->GetRoomIndex(),
												pOpenRoom->GetMaxUser(),
												pOpenRoom->GetNumOfUsers(),
												pOpenRoom->GetRoomName().c_str(),
												static_cast<INT>( pOpenRoom->GetRoomName().length() ),
												pOpenRoom->IsPassword(),
												pOpenRoom->GetOption(),
												0 );

		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->RoomInfo ), m_pCasualPackets->RoomInfo.GetLength() );

		pOpenRoom = pChannel->GetNextRoom();
	}



	//
	// Step 4 : ¹æÀÇ »ç¿ëÀÚµé¿¡°Ô »ç¿ëÀÚ ÅðÀåÀ» Àü´Þ
	//
	m_pCasualPackets->RemoveRoomUser.SetProperty(
		pUser->GetID().c_str(),
		static_cast<INT>( pUser->GetID().length() )
		);
	SendToRoom( pRoom, NULL, &m_pCasualPackets->RemoveRoomUser, m_pCasualPackets->RemoveRoomUser.GetLength() );


	//
	// Step 5 : ´ë±â½ÇÀÇ »ç¿ëÀÚµé°úÀÇ Á¤º¸ ±³È¯
	//

	// ¹æÀÌ ºñ¾îÀÖÀ¸¸é, ¹æÀÌ Á¦°ÅµÇ¾úÀ½À» Åëº¸
	if( pRoom->IsEmpty() )
	{
		m_pCasualPackets->RemoveRoom.SetProperty(
			pRoom->GetRoomIndex()
			);

		// ±âÁ¸ ´ë±â½Ç »ç¿ëÀÚ¿¡°Ô´Â ¹æ Á¤º¸ º¯°æÀ» Àü´Þ
		SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->RemoveRoom, m_pCasualPackets->RemoveRoom.GetLength() );

		m_pCasualPackets->UserInfo.SetProperty(
			pUser->GetGrade(),	
			pUser->GetID().c_str(),
			static_cast<INT>( pUser->GetID().length() ),
			pUser->GetNick().c_str(),
			static_cast<INT>( pUser->GetNick().length() )
			);

		// ±âÁ¸ ´ë±â½Ç »ç¿ëÀÚ¿¡°Ô´Â ÇöÀç »ç¿ëÀÚÀÇ ´ë±â½Ç ÀÔÀåÀ» Àü´Þ
		SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->UserInfo, m_pCasualPackets->UserInfo.GetLength() );

		CUser* pWaitUser;
		pWaitUser = pWaitRoom->GetFirstUser();
		while( pWaitUser )
		{
			if( pWaitUser != pUser )
			{
				// ÇöÀç »ç¿ëÀÚ¿¡°Ô´Â ±âÁ¸ ´ë±â½Ç »ç¿ëÀÚÀÇ Á¤º¸¸¦ Àü´Þ
				m_pCasualPackets->UserInfo.SetProperty(
					pWaitUser->GetGrade(),
					pWaitUser->GetID().c_str(),
					static_cast<INT>( pWaitUser->GetID().length() ),
					pWaitUser->GetNick().c_str(),
					static_cast<INT>( pWaitUser->GetNick().length() )
					);
				SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->UserInfo ), m_pCasualPackets->UserInfo.GetLength() );
			}
			pWaitUser = pWaitRoom->GetNextUser();
		}// while( pWaitUser )

	}// if( pRoom->IsEmpty() )

	// ¹æÀÌ ¿­·ÁÀÖÀ¸¸é, ¹æÁ¤º¸°¡ º¯°æµÇ¾úÀ½À» Åëº¸
	else
	{
		m_pCasualPackets->RoomInfoChange.SetProperty(
			pRoom->GetRoomIndex(),
			pRoom->GetMaxUser(),
			pRoom->GetNumOfUsers(),
			pRoom->IsPassword(),
			pRoom->GetOption()
			);

		// ±âÁ¸ ´ë±â½Ç »ç¿ëÀÚ¿¡°Ô´Â ¹æ Á¤º¸ º¯°æÀ» Àü´Þ
		SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->RoomInfoChange, m_pCasualPackets->RoomInfoChange.GetLength() );

		m_pCasualPackets->UserInfo.SetProperty(
			pUser->GetGrade(),
			pUser->GetID().c_str(),
			static_cast<INT>( pUser->GetID().length() ),
			pUser->GetNick().c_str(),
			static_cast<INT>( pUser->GetNick().length() )
			);

		// ±âÁ¸ ´ë±â½Ç »ç¿ëÀÚ¿¡°Ô´Â ÇöÀç »ç¿ëÀÚÀÇ ´ë±â½Ç ÀÔÀåÀ» Àü´Þ
		SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->UserInfo, m_pCasualPackets->UserInfo.GetLength() );

		CUser* pWaitUser;
		pWaitUser = pWaitRoom->GetFirstUser();
		while( pWaitUser )
		{
			if( pWaitUser != pUser )
			{
				// ÇöÀç »ç¿ëÀÚ¿¡°Ô´Â ±âÁ¸ ´ë±â½Ç »ç¿ëÀÚÀÇ Á¤º¸¸¦ Àü´Þ
				m_pCasualPackets->UserInfo.SetProperty(
					pWaitUser->GetGrade(),
					pWaitUser->GetID().c_str(),
					static_cast<INT>( pWaitUser->GetID().length() ),
					pWaitUser->GetNick().c_str(),
					static_cast<INT>( pWaitUser->GetNick().length() )
					);
				SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->UserInfo ), m_pCasualPackets->UserInfo.GetLength() );
			}
			pWaitUser = pWaitRoom->GetNextUser();
		}
	}


}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// ÆÐÅ¶ Ã³¸®
//

VOID CNJeanGate::OnPacketLogin( CUser* pUser, LPCTSTR strUserNo, LPCTSTR strID, LPCTSTR strNick )
{
	pUser->SetUserInfo( strUserNo,
						strID,
						strNick	);

	pUser->SetCaptine( FALSE );
//	pUser->SetP2PCaptine( FALSE );
	pUser->SetGameCaptine( TRUE );

	if( pUser->GetIP().compare( GetPeerAddr( pUser ) ) == 0 ) //£¿£¿£¿ÄÑµÀ»¹ÄÜ²»Ò»ÑùÂð£¿
		pUser->SetP2PCaptine( TRUE );
	else
		pUser->SetP2PCaptine( FALSE );

//	pUser->SetP2PCaptine( TRUE );
}

// ·Î±×ÀÎ ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessLoginRequest( CUser* pUser, CCasualPacketLoginRequest* pCasualPacket )
{
	pUser->SetAddress( pCasualPacket->GetIpAddr(), pCasualPacket->GetPort() );

	
	OnPacketLogin(
		pUser,
		pCasualPacket->GetUserNo(),
		pCasualPacket->GetID(),
		pCasualPacket->GetNick()
		);
}

inline VOID CNJeanGate::PacketProcessSecLoginRequest( CUser* pUser, CCasualPacketSecLoginRequest* pCasualPacket )
{
	pUser->SetAddress( pCasualPacket->GetIpAddr(), pCasualPacket->GetPort() );

	// ¾ÏÈ£È­ Ç®±â Ã³¸® Ãß°¡ ¿¹Á¤

	// ¾ÏÈ£È­ : È²¼ø±¸¾¾ ¼Ò½º Ä«ÇÇ
	CUserSCDApp cDataEn;

	char	szResult[4096];
	char	szOutput[4096];
	char	szKey[100];

	// szKey ¼³Á¤ÇÏ±â : ÇöÀç ³¯Â¥ : ex) 20020101
	__time64_t ltime;
	struct tm *today;
	::_time64( &ltime );
	today = ::_localtime64( &ltime );
	::strftime( szKey, 100, "%Y%m%d", today );

	const char*	szInput	= pCasualPacket->GetSecData();
	int			nLen	= pCasualPacket->GetSecSize();

	::memset(szResult , 0 , 4096);

	CUserSCDApp::Decoder( (unsigned char *)szInput , (unsigned char *)szResult , nLen );

	long nBytesRead = cDataEn.uToDedes( (unsigned char *)szResult, (unsigned char *)szKey
									  , (unsigned char *)szOutput, nLen / 2 , 2 ) ;

	szOutput[nBytesRead] = NULL;

	OnPacketSecLogin( pUser, szOutput, pCasualPacket->GetRealLength(), pCasualPacket->GetSessionKey(), CCasualCommon::CasualStringLength::SessionKey  );
}

// Ã¤³Î ÀÔÀå ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessJoinChannelRequest( CUser* pUser, CCasualPacketJoinChannelRequest* pCasualPacket )
{
	CRoom*		pWaitRoom;	// »ç¿ëÀÚ°¡ ¼ÓÇÒ Ã¤³ÎÀÇ ´ë±â½Ç
	CChannel*	pChannel;	// »ç¿ëÀÚ°¡ ¼ÓÇÒ Ã¤³Î

	//
	// Stpe 1 : ¿¹¿Ü °Ë»ç
	//

	// Ã¤³Î ¹üÀ§°¡ ³Ñ´Â °æ¿ì
	if( pCasualPacket->GetChannelIndex() <= 0 ||
		pCasualPacket->GetChannelIndex() >= m_nMaxChannel )
	{
		m_pCasualPackets->JoinChannelReply.SetProperty(
			CCasualPacketJoinChannelReply::Result::FailOutOfChannel
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinChannelReply ), m_pCasualPackets->JoinChannelReply.GetLength() );
		return;
	}
	
	// ÀÌ¹Ì ´Ù¸¥ Ã¤³Î¿¡ ¼ÓÇÑ °æ¿ì
	if( pUser->GetChannelIndex() > 0 )
	{
		m_pCasualPackets->JoinChannelReply.SetProperty(
			CCasualPacketJoinChannelReply::Result::FailExist
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinChannelReply ), m_pCasualPackets->JoinChannelReply.GetLength() );
		return;
	}
	pChannel	= &m_aChannel[ pCasualPacket->GetChannelIndex() ];

	SHORT nResult = CCasualPacketJoinChannelReply::Result::FailETC;
	if( !IsCorrectCondition( nResult, pUser, PacketCondition::JoinChannel, reinterpret_cast<ULONG_PTR>( pChannel ) ) )
	{
		//µ±Ç°½ÇÉ«ÄÜ·ñµÇÂ½µ½¸Ãchannnel
		m_pCasualPackets->JoinChannelReply.SetProperty(
//			CCasualPacketJoinChannelReply::Result::FailETC
			nResult
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinChannelReply ), m_pCasualPackets->JoinChannelReply.GetLength() );
		return;
	}


	//
	// Step 2 : Ã¤³Î¿¡ ÀÔÀå
	//

	// ´Óm_aChannel[0]ÖÐÍË³ö£¬LoginReply³É¹¦Ê±£¬½«½ÇÉ«¼ÓÈëÆäÖÐ
	m_aChannel[0].Leave( pUser );

	// ÇØ´ç Ã¤³Î¿¡ ÀÔÀå
	if( !pChannel->Join( pUser ) )
	{
		m_aChannel[0].Join( pUser );

		m_pCasualPackets->JoinChannelReply.SetProperty(
			CCasualPacketJoinChannelReply::Result::FailFull
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinChannelReply ), m_pCasualPackets->JoinChannelReply.GetLength() );
		return;
	}
	pUser->SetChannelIndex( pChannel->GetChannelIndex() );


	//
	// Step 3 : ´ë±â½Ç¿¡ ÀÔÀå
	//

	pWaitRoom	= pChannel->GetWaitRoom();

	pWaitRoom->Join( pUser );
	pUser->SetRoomIndex( pWaitRoom->GetRoomIndex() );



	//
	// Step 4 : ´ë±â½Ç¾ÈÀÇ »ç¿ëÀÚµéÀÇ Á¤º¸ ±³È¯
	//

	// ÀÔÀåÇÑ »ç¿ëÀÚ¿¡°Ô ÀÔÀå ¼º°øÀ» Àü´Þ
	m_pCasualPackets->JoinChannelReply.SetProperty(
		CCasualPacketJoinChannelReply::Result::Success,
		pWaitRoom->GetNumOfUsers() - 1,		// ÀÚ±â ÀÚ½ÅÀº ÀüÃ¼ »ç¿ëÀÚÀÇ ¼ö¿¡ Æ÷ÇÔÇÏÁö ¾Ê´Â´Ù.
		pChannel->GetNumOfOpenRooms()
		);
	SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinChannelReply ), m_pCasualPackets->JoinChannelReply.GetLength() );


	//
	// Step 5 : ¹æÀÇ Á¤º¸¸¦ Àü´ÞÇÑ´Ù.
	//

	CRoom* pOpenRoom = pChannel->GetFirstRoom();
	while( pOpenRoom )
	{		
		m_pCasualPackets->RoomInfo.SetProperty(
			pOpenRoom->GetRoomIndex(),
			pOpenRoom->GetMaxUser(),
			pOpenRoom->GetNumOfUsers(),
			pOpenRoom->GetRoomName().c_str(),
			static_cast<INT>( pOpenRoom->GetRoomName().length() ),
			pOpenRoom->IsPassword(),
			pOpenRoom->GetOption()
			);

		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->RoomInfo ), m_pCasualPackets->RoomInfo.GetLength() );

		pOpenRoom = pChannel->GetNextRoom();
	}


	//
	// Step 4 : 5¿Í º¯°æµÇ¾úÀ½
	//

	// ´ë±â½Ç¾ÈÀÇ »ç¿ëÀÚ¿¡°Ô ÀÔÀåÇÑ »ç¿ëÀÚÀÇ Á¤º¸¸¦ Àü´ÞÇÒ ÆÐÅ¶
	m_pCasualPackets->UserInfo.SetProperty(
		pUser->GetGrade(),
		pUser->GetID().c_str(),
		static_cast<INT>( pUser->GetID().length() ),
		pUser->GetNick().c_str(),
		static_cast<INT>( pUser->GetNick().length() )
		);

	// Room¾ÈÀÇ »ç¿ëÀÚ¿¡°Ô ÀÔÀåÇÑ »ç¿ëÀÚÀÇ Á¤º¸¸¦ Àü´Þ
	SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->UserInfo, m_pCasualPackets->UserInfo.GetLength() );

	CUser* pRoomUser = pWaitRoom->GetFirstUser();
	while( pRoomUser )
	{
		if( pRoomUser != pUser )
		{
			// ÀÔÀåÇÑ »ç¿ëÀÚ¿¡°Ô Room¾ÈÀÇ »ç¿ëÀÚ Á¤º¸¸¦ Àü´Þ
			m_pCasualPackets->UserInfo.SetProperty(
				pRoomUser->GetGrade(),
				pRoomUser->GetID().c_str(),
				static_cast<INT>( pRoomUser->GetID().length() ),
				pRoomUser->GetNick().c_str(),
				static_cast<INT>( pRoomUser->GetNick().length() )
				);
			SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->UserInfo ), m_pCasualPackets->UserInfo.GetLength() );
		}

		pRoomUser = pWaitRoom->GetNextUser();
	}




	// ¹æÀåÀÌ ¾Æ´ÔÀ» ¼³Á¤ÇÑ´Ù.
	pUser->SetCaptine( FALSE );
}

// Ã¤³Î ÅðÀå ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessLeaveChannelRequest( CUser* pUser, CCasualPacketLeaveChannelRequest* pCausalPacket )
{
	CRoom*		pRoom;		// »ç¿ëÀÚ°¡ ÇöÀç ¼ÓÇÑ ¹æ
	CRoom*		pWaitRoom;	// »ç¿ëÀÚ°¡ ¼ÓÇÒ Ã¤³ÎÀÇ ´ë±â½Ç
	CChannel*	pChannel;	// »ç¿ëÀÚ°¡ ¼ÓÇÒ Ã¤³Î

	//
	// Stpe 1 : ¿¹¿Ü °Ë»ç
	//

	// Ã¤³Î¿¡ ¾ø´Â °æ¿ì
	if( pUser->GetChannelIndex() <= 0 ||
		pUser->GetChannelIndex() >= m_nMaxChannel )
	{
		return;
	}

	pChannel	= &m_aChannel[pUser->GetChannelIndex()];
	pWaitRoom	= pChannel->GetWaitRoom();
	pRoom		= &m_aRoom[pUser->GetRoomIndex()];

	// ´ë±â½Ç¿¡ ÀÖÁö ¾ÊÀº °æ¿ì
	if( !pChannel->IsWaitRoom( pRoom ) )
		return;


	//
	// Step 2 : Ã¤³Î¿¡¼­ ³ª°£´Ù.
	//
	pChannel->Leave( pUser );
	CCasualPacketLeaveChannelReply	leaveChannelPacket;
	SendData( pUser, reinterpret_cast<const CHAR*>( &leaveChannelPacket ), leaveChannelPacket.GetLength() );

	// ´ë±â Ã¤³Î¿¡ ÀÔÀåÇÑ´Ù.
	m_aChannel[0].Join( pUser );
	pUser->SetChannelIndex( 0 );


	// ´ë±â½Ç¿¡¼­ ³ª°£´Ù.
	pWaitRoom->Leave( pUser );

	// ´ë±â½Ç¿¡ ³²¾Æ ÀÖ´Â »ç¿ëÀÚµé¿¡°Ô ÅðÀå Á¤º¸¸¦ Àü´Þ
	m_pCasualPackets->RemoveUser.SetProperty(
		pUser->GetID().c_str(),
		static_cast<INT>( pUser->GetID().length() )
		);

	SendToRoom( pWaitRoom, NULL, &m_pCasualPackets->RemoveUser, m_pCasualPackets->RemoveUser.GetLength() );
}

// ¹æ °³¼³ ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessOpenRoomRequest( CUser* pUser, CCasualPacketOpenRoomRequest* pCasualPacket )
{
	CChannel*	pChannel;	// »ç¿ëÀÚ°¡ ¼ÓÇÑ Ã¤³Î
	CRoom*		pWaitRoom;	// »ç¿ëÀÚ°¡ ¼ÓÇÑ ´ë±â½Ç

	//
	// Step 1 : ¿¹¿Ü °Ë»ç
	//

	// ¹æÀÇ ÃÖ´ë Âü¿©ÀÚ°¡, ½ÇÁ¦ ¹æÀÇ »çÀÌÁîº¸´Ù Å« °æ¿ì
	if( pCasualPacket->GetMaxUsers() > m_nMaxRoomUser )
	{
		m_pCasualPackets->OpenRoomReply.SetProperty(
			CCasualPacketOpenRoomReply::Result::FailOverUsers,
			-1 );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
		return;
	}

	// Ã¤³Î¿¡ ¼ÓÇÏÁö ¾ÊÀº °æ¿ì
	if( pUser->GetChannelIndex() <= 0 ||
		pUser->GetChannelIndex() >= m_nMaxChannel )
	{
		m_pCasualPackets->OpenRoomReply.SetProperty(
			CCasualPacketOpenRoomReply::Result::FailOutOfChannel,
			-1 );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
		return;
	}
	
	pChannel	= &m_aChannel	[ pUser->GetChannelIndex()	];
	pWaitRoom	= &m_aRoom		[ pUser->GetRoomIndex()		];

	// ´ë±â½Ç¿¡ ¾ø´Â °æ¿ì
	if( !pChannel->IsWaitRoom( pWaitRoom ) )
	{
		m_pCasualPackets->OpenRoomReply.SetProperty(
			CCasualPacketOpenRoomReply::Result::FailExist,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
		return;
	}

	SHORT nResult = CCasualPacketOpenRoomReply::Result::FailFull;
	if( !IsCorrectCondition( nResult, pUser, PacketCondition::OpenRoom ) )
	{
		m_pCasualPackets->OpenRoomReply.SetProperty(
//			CCasualPacketOpenRoomReply::Result::FailFull,
			nResult,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
		return;
	}

	//
	// Step 2 : »õ Room ÇÒ´ç
	//

	BOOL		bRet;
	CRoom*		pNewRoom;

	pChannel	= &m_aChannel[ pUser->GetChannelIndex() ];

	bRet = m_InnerManagerUnusedRoom.Pop( pNewRoom );

	// ÇÒ´ç¿¡ ½ÇÆÐ¸¦ ÇÏ¸é
	if( !bRet )
	{
		m_pCasualPackets->OpenRoomReply.SetProperty(
			CCasualPacketOpenRoomReply::Result::FailFull,
			-1 );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
		return;
	}
	pNewRoom->Init( pChannel->GetChannelIndex() );

	// Ã¤³Î¿¡ °³¼³¿¡ °³¼³µÈ ¹æÀ» Ãß°¡ÇÑ´Ù.
	bRet = pChannel->AddRoom( pNewRoom );

	// Ã¤³Î¿¡ ¹æÃß°¡°¡ ½ÇÆÐ¸¦ ÇÏ¸é...
	if( !bRet )
	{
		m_InnerManagerUnusedRoom.Add( pNewRoom->GetInnerObjectForGame() );

		m_pCasualPackets->OpenRoomReply.SetProperty(
			CCasualPacketOpenRoomReply::Result::FailFull,
			-1 );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
		return;
	}

	// ÇÒ´ç¿¡ ¼º°øÇÑ RoomÀ» m_InnerManagerUsedRoom¿¡ »ðÀÔÇÏ°í, RoomÁ¤º¸¸¦ ¼³Á¤ÇÑ´Ù.
	m_InnerManagerUsedRoom.Add( pNewRoom->GetInnerObjectForGame() );
	pNewRoom->Open(
		pCasualPacket->GetMaxUsers(),
		pCasualPacket->GetRoomName()
		);



	//
	// Step 3 : ´ë±â½Ç »ç¿ëÀÚµé¿¡°Ô ÇöÀç »ç¿ëÀÚ°¡ ´ë±â½Ç¿¡¼­ ³ª°¬À½À» Åëº¸
	//

	// ´ë±â½Ç¿¡¼­ ³ª°£´Ù.
	pWaitRoom->Leave( pUser );

	// ´ë±â½Ç¿¡ ³²¾Æ ÀÖ´Â »ç¿ëÀÚµé¿¡°Ô ÅðÀå Á¤º¸¸¦ Àü´Þ
	m_pCasualPackets->RemoveUser.SetProperty(
		pUser->GetID().c_str(),
		static_cast<INT>( pUser->GetID().length() )
		);

	SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->RemoveUser, m_pCasualPackets->RemoveUser.GetLength() );



	//
	// Step 4 : ´ë±â½Ç »ç¿ëÀÚµé¿¡°Ô »õ·Î ¹æ °³¼³À» Åëº¸
	//

	// OpenÇÑ Room¿¡ ÀÔÀåÀ» ÇÑ´Ù.
	if( !pNewRoom->Join( pUser ) )
	{
		// Àý´ë·Î ÀÌÂÊ¿¡ ¿Ã ¼ö´Â ¾ø´Ù.
		return;
	}
	pUser->SetRoomIndex( pNewRoom->GetRoomIndex() );

	// ¹æÀ» °³¼³ÇÑ ÀÚ´Â 0¹øÂ° ÀÎµ¦½º¸¦ °¡Áú °ÍÀÌ´Ù.
	pUser->SetRoomInnerIndex( pNewRoom->AllocEmptyInnerIndex() );

	// ¹æÀåÀ¸·Î ¼³Á¤ÇÑ´Ù.
	pUser->SetCaptine( TRUE );
	pNewRoom->SetCaptine( pUser );

	// ¹æ¿¡ ÆÐ½º¿öµå ¼³Á¤ À¯¹«¸¦ ¼³Á¤ÇÑ´Ù.
	pNewRoom->SetPassword( pCasualPacket->IsPassword(), pCasualPacket->GetPassword() );
	
	// ¿É¼ÇÀ» ¼³Á¤ÇÑ´Ù.
	for( INT i = 0; i < CCasualCommon::CasualStringLength::RoomOption; i++ )
		pNewRoom->SetOption( i, pCasualPacket->GetOption( i ) );

	pNewRoom->SetRTT( GetRttTime( pUser ) );

	OnOpenRoomReply( pUser, pNewRoom );
	/*
	// Open ¼º°øÀ» ÅëÁöÇÑ´Ù.
	m_pCasualPackets->OpenRoomReply.SetProperty(
		CCasualPacketOpenRoomReply::Result::Success,
		pNewRoom->GetRoomIndex() );
	SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
	*/


	// ´ë±â½ÇÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô »ý¼ºµÈ RoomÁ¤º¸¸¦ Àü´ÞÇÑ´Ù.
	
	m_pCasualPackets->RoomInfo.SetProperty(
		pNewRoom->GetRoomIndex(),
		pNewRoom->GetMaxUser(),
		pNewRoom->GetNumOfUsers(),
		pNewRoom->GetRoomName().c_str(),
		static_cast<INT>( pNewRoom->GetRoomName().length() ),
		pNewRoom->IsPassword(),
		pNewRoom->GetOption()
		);

	SendToRoom( pWaitRoom, NULL, &m_pCasualPackets->RoomInfo, m_pCasualPackets->RoomInfo.GetLength() );

	// ¹æÀå º¯°æ ½Ã±×³ÎÀ» ÅëÇØ¼­ ¹æÀåÀ» º¯°æÇÑ´Ù.
//	AddSignal( CCasualCommon::CasualSignalMessage::ChangeCaptineSignal, (DWORD) pUser->GetRoomIndex(), 0 );
}

// ¹æ Âü¿© ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessJoinRoomRequest( CUser* pUser, CCasualPacketJoinRoomRequest* pCasualPacket )
{
	CChannel*	pChannel;	// »ç¿ëÀÚ°¡ ¼ÓÇÑ Ã¤³Î
	CRoom*		pWaitRoom;	// »ç¿ëÀÚ°¡ ¼ÓÇÑ ´ë±â½Ç
	CRoom*		pRoom;		// »ç¿ëÀÚ°¡ ÀÔÀåÇÏ°íÀÚ ÇÏ´Â ¹æ

	//
	// Step 1 : ¿¹¿Ü °Ë»ç
	//

	// Ã¤³Î¿¡ ¼ÓÇÏÁö ¾ÊÀº °æ¿ì
	if( pUser->GetChannelIndex() <= 0 ||
		pUser->GetChannelIndex() >= m_nMaxChannel )
	{
		m_pCasualPackets->JoinRoomReply.SetProperty(
			CCasualPacketJoinRoomReply::Result::FailOutOfChannel,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
		return;
	}
	
	pChannel	= &m_aChannel	[ pUser->GetChannelIndex()	];
	pWaitRoom	= &m_aRoom		[ pUser->GetRoomIndex()		];

	// ´ë±â½Ç¿¡ ¾ø´Â °æ¿ì
	if( !pChannel->IsWaitRoom( pWaitRoom ) )
	{
		m_pCasualPackets->JoinRoomReply.SetProperty(
			CCasualPacketJoinRoomReply::Result::FailExist,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
		return;
	}

	// Room Index°¡ ¿µ¿ª ¹ÛÀÎ °æ¿ì
	if( pCasualPacket->GetRoomIndex() < 0 ||
		pCasualPacket->GetRoomIndex() >= m_nMaxRoom )
	{
		m_pCasualPackets->JoinRoomReply.SetProperty(
			CCasualPacketJoinRoomReply::Result::FailOutOfRoom,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
		return;
	}
	pRoom		= &m_aRoom		[pCasualPacket->GetRoomIndex()];

	// RoomÀÌ ´Ù¸¥ Ã¤³Î¿¡ Á¸ÀçÇÏ´Â °æ¿ì
	if( pRoom->GetChannelIndex() != pUser->GetChannelIndex() )
	{
		m_pCasualPackets->JoinRoomReply.SetProperty(
			CCasualPacketJoinRoomReply::Result::FailOutOfRoom,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
		return;
	}

	// »ç¿ëÀÚ°¡ ÀÔÀåÇÏ·Á´Â ¼ø°£¿¡ ¹æÀÌ ´ÝÈù °æ¿ì
	if( pRoom->IsEmpty() )
	{
		m_pCasualPackets->JoinRoomReply.SetProperty(
			CCasualPacketJoinRoomReply::Result::FailClose,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
		return;
	}


	//
	// Step 2 : ¹æ¿¡ ÀÔÀå
	//
	SHORT nResult = CCasualPacketJoinRoomReply::Result::FailETC;
	if( !IsCorrectCondition( nResult, pUser, PacketCondition::JoinRoom, reinterpret_cast<ULONG_PTR>( pRoom ) ) )
	{
		m_pCasualPackets->JoinRoomReply.SetProperty(
//			CCasualPacketJoinRoomReply::Result::FailETC,
			nResult,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
		return;
	}

	if( pRoom->IsPassword() )
	{
		if( !pRoom->IsCorrectPassword( pCasualPacket->GetPass() ) )
		{
			m_pCasualPackets->JoinRoomReply.SetProperty(
				CCasualPacketJoinRoomReply::Result::FailWrongPass,
				pUser->GetRoomIndex() );
			SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
			return;
		}
	}


	BOOL	bLeaveNotify = FALSE;

	// ´ë±â½Ç¿¡¼­ ³ª°£´Ù.
	pWaitRoom->Leave( pUser );

	// ¹æ¿¡ ÀÔÀåÇÑ´Ù.
	BOOL bRet;
	bRet = pRoom->Join( pUser );

	// ¹æÀÌ ²ËÂù °æ¿ì
	if( !bRet )
	{
		pWaitRoom->Join( pUser );
		pUser->SetRoomIndex( pWaitRoom->GetRoomIndex() );

		m_pCasualPackets->JoinRoomReply.SetProperty(
			CCasualPacketJoinRoomReply::Result::FailFull,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
		return;
	}
	pUser->SetRoomIndex( pCasualPacket->GetRoomIndex() );


	//
	// ¹æ ¾È¿¡¼­ÀÇ ÀÚ½ÅÀÇ index¸¦ ¼³Á¤ÇÑ´Ù.
	//
	pUser->SetRoomInnerIndex( pRoom->AllocEmptyInnerIndex() );


	//
	// Step 3 : ´ë±â½Ç »ç¿ëÀÚµé¿¡°Ô ÇöÀç »ç¿ëÀÚ°¡ ´ë±â½Ç¿¡¼­ ³ª°¬À½À» Åëº¸
	//
	m_pCasualPackets->RemoveUser.SetProperty(
		pUser->GetID().c_str(),
		static_cast<INT>( pUser->GetID().length() )
		);

	SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->RemoveUser, m_pCasualPackets->RemoveUser.GetLength() );


	//
	// Step 4 : ¹æ¾ÈÀÇ »ç¿ëÀÚµéÀÇ Á¤º¸ ±³È¯
	//
	OnJoinReply( pUser, pRoom );

	//
	// Step 5 : ´ë±â½Ç »ç¿ëÀÚ¿¡°Ô ¹æÀÇ Á¤º¸°¡ ¹Ù²î¾úÀ½À» Àü´ÞÇÑ´Ù.
	//

	m_pCasualPackets->RoomInfoChange.SetProperty(
		pRoom->GetRoomIndex(),
		pRoom->GetMaxUser(),
		pRoom->GetNumOfUsers(),
		pRoom->IsPassword(),
		pRoom->GetOption()
		);

	SendToRoom( pWaitRoom, NULL, &m_pCasualPackets->RoomInfoChange, m_pCasualPackets->RoomInfoChange.GetLength() );
}

// ¹æ¿¡¼­ ³ª°¡±â ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessLeaveRoomRequest( CUser* pUser, CCasualPacketLeaveRoomRequest* pCasualPacket )
{
	// ³ª°¥ ¼ö ÀÖ´ÂÁö Á¶°Ç°Ë»ç..
	SHORT nResult = CCasualPacketLeaveRoomReply::Result::Fail;
	if( !IsCorrectCondition( nResult, pUser, PacketCondition::LeaveRoom ) )
	{
		m_pCasualPackets->LeaveRoomReply.SetProperty(
//			CCasualPacketLeaveRoomReply::Result::Fail
			nResult
			);
		SendData(
			pUser,
			reinterpret_cast<const CHAR*>( &m_pCasualPackets->LeaveRoomReply ),
			m_pCasualPackets->LeaveRoomReply.GetLength()
			);
		return;
	}
	else
	{
		m_pCasualPackets->LeaveRoomReply.SetProperty(
			CCasualPacketLeaveRoomReply::Result::Success
			);
		SendData(
			pUser,
			reinterpret_cast<const CHAR*>( &m_pCasualPackets->LeaveRoomReply ),
			m_pCasualPackets->LeaveRoomReply.GetLength()
			);
	}

	// ¹æ¿¡¼­ ³ª°¡´Â °ÍÀº.. ÀÚ½ÅÀ» °­Á¦ ÅðÀå ½ÃÅ°´Â °Í°ú µ¿ÀÏÇÏ´Ù.
	AddSignal(
		CCasualCommon::CasualSignalMessage::CompulseExitSignal,
		reinterpret_cast<ULONG_PTR>( pUser ),
		static_cast<ULONG_PTR>( CCasualPacketCompulsionExited::Type::Mine )	// °­Åð Å¸ÀÔ
		);
}

// ¹æ¿¡ ÀÚµ¿ ÀÔÀå ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessAutoJoinRoomRequest( CUser* pUser, CCasualPacketAutoJoinRoomRequest* pCasualPacket )
{
	CChannel*	pChannel;	// »ç¿ëÀÚ°¡ ¼ÓÇÑ Ã¤³Î
	CRoom*		pWaitRoom;	// »ç¿ëÀÚ°¡ ¼ÓÇÑ ´ë±â½Ç

	//
	// Step 1 : ¿¹¿Ü °Ë»ç
	//

	// Ã¤³Î¿¡ ¼ÓÇÏÁö ¾ÊÀº °æ¿ì
	if( pUser->GetChannelIndex() <= 0 ||
		pUser->GetChannelIndex() >= m_nMaxChannel )
	{
		m_pCasualPackets->AutoJoinRoomReply.SetProperty(
			CCasualPacketAutoJoinRoomReply::Result::FailOutOfChannel,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->AutoJoinRoomReply ), m_pCasualPackets->AutoJoinRoomReply.GetLength() );
		return;
	}
	
	pChannel	= &m_aChannel	[ pUser->GetChannelIndex()	];
	pWaitRoom	= &m_aRoom		[ pUser->GetRoomIndex()		];

	// ´ë±â½Ç¿¡ ¾ø´Â °æ¿ì
	if( !pChannel->IsWaitRoom( pWaitRoom ) )
	{
		m_pCasualPackets->AutoJoinRoomReply.SetProperty(
			CCasualPacketAutoJoinRoomReply::Result::FailExist,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->AutoJoinRoomReply ), m_pCasualPackets->AutoJoinRoomReply.GetLength() );
		return;
	}

	// »ç¿ëÀÚ°¡ ÀÚµ¿ÀÔÀåÀ» ÇÒ ¼ö ¾ø´Â °æ¿ì : ÇÏÀ§ Å¬·¡½º¿¡¼­ Ã³¸®
	SHORT nResult = CCasualPacketAutoJoinRoomReply::Result::FailExist;
	if( !IsCorrectCondition( nResult, pUser, PacketCondition::AutoJoinRoom ) )
	{
		m_pCasualPackets->AutoJoinRoomReply.SetProperty(
//			CCasualPacketJoinRoomReply::Result::FailETC,
			nResult,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->AutoJoinRoomReply ), m_pCasualPackets->AutoJoinRoomReply.GetLength() );
		return;
	}


	CRoom*		pOpenRoom;	// ÀÚµ¿À¸·Î ÀÔÀåÇÒ ¹æ

	//
	// Step 2 : ÀÚµ¿ ÀÔÀå
	//
	CRoom*		pTmpRoom;	// Æ¯Á¤¹æ¿¡ µé¾î°¡Áö ¾Ê±â À§ÇØ »ç¿ëÇÏ´Â º¯¼ö
	pTmpRoom	= pChannel->GetFirstRoom();
	pOpenRoom	= NULL;

	while( pTmpRoom )
	{
		// ¹æ¿¡ ´õ µé¾î°¥ ¼ö ÀÖ´ÂÁö °Ë»ç
		if( pTmpRoom->GetNumOfUsers() < pTmpRoom->GetMaxUser()	&&
			pTmpRoom->GetCaptine() != NULL							&& 	// ¹æÀåÀÌ ¾øÀ¸¸é ¾ÆÁ÷ °³¼³ÁßÀÎ ¹æ
			!pTmpRoom->IsPassword()									// ºñ¹øÀÌ ¾ø´Â ¹æ
			)
		{
			if( !IsCorrectCondition( nResult, pUser, PacketCondition::JoinRoom, reinterpret_cast<ULONG_PTR>( pTmpRoom ) ) )
			{
				pTmpRoom = pChannel->GetNextRoom();
				continue;
			}

			if( pTmpRoom->GetRoomIndex() == pCasualPacket->GetLastRoomIndex() )
			{
				pOpenRoom = pTmpRoom;		// case A??? ÊÇ·ñÓ¦¸ÃÔÚ´Ëbreak£¿Ö±½Ó½øÈëÉÏ´ÎµÄroom
			}
			else
			{
				break;
			}
		}

		pTmpRoom = pChannel->GetNextRoom();
	}

	// Ã£À¸¸é Ã£Àº ¹æÀ» ¼³Á¤ ¾Æ´Ï¸é À§ÀÇ case A¿¡ ÀÇÇØ ¼³Á¤µÈ ¹æÀ¸·Î ÀÔÀå, ¾øÀ¸¸é NULL
	if( pTmpRoom )
		pOpenRoom = pTmpRoom;

	//
	// Step 3 : ÀÔÀå ½ÇÆÐ½Ã ½ÇÆÐ ¸Þ½ÃÁö Àü´Þ
	//
	if( !pOpenRoom )
	{
		// ÀÔÀå ½ÇÆÐ Àü´Þ
		m_pCasualPackets->AutoJoinRoomReply.SetProperty(
			CCasualPacketAutoJoinRoomReply::Result::FailNone
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->AutoJoinRoomReply ), m_pCasualPackets->AutoJoinRoomReply.GetLength() );
		return;
	}
	nResult = CCasualPacketAutoJoinRoomReply::Result::FailExist;
	/*/
	if( !IsCorrectCondition( nResult, pUser, PacketCondition::JoinRoom, reinterpret_cast<ULONG_PTR>( pOpenRoom ) ) )
	{
		m_pCasualPackets->AutoJoinRoomReply.SetProperty(
//			CCasualPacketJoinRoomReply::Result::FailETC,
			nResult,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->AutoJoinRoomReply ), m_pCasualPackets->AutoJoinRoomReply.GetLength() );
		return;
	}
	/*/



	//
	// Step 5 : ´ë±â½Ç »ç¿ëÀÚ¿¡°Ô »ç¿ëÀÚ ÅðÀå Àü´Þ
	//
	pWaitRoom->Leave( pUser );

	m_pCasualPackets->RemoveUser.SetProperty(
		pUser->GetID().c_str(),
		static_cast<INT>( pUser->GetID().length() )
		);
	SendToRoom( pWaitRoom, pUser, &m_pCasualPackets->RemoveUser, m_pCasualPackets->RemoveUser.GetLength() );

	BOOL bRet = pOpenRoom->Join( pUser );

	// ¹æÀÌ ²ËÂù °æ¿ì
	if( !bRet )
	{
		pWaitRoom->Join( pUser );
		pUser->SetRoomIndex( pWaitRoom->GetRoomIndex() );

		m_pCasualPackets->AutoJoinRoomReply.SetProperty(
			CCasualPacketAutoJoinRoomReply::Result::FailFull,
			pUser->GetRoomIndex() );
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->AutoJoinRoomReply ), m_pCasualPackets->AutoJoinRoomReply.GetLength() );
		return;
	}
	pUser->SetRoomIndex( pOpenRoom->GetRoomIndex() );


	//
	// ¹æ ¾È¿¡¼­ÀÇ ÀÚ½ÅÀÇ index¸¦ ¼³Á¤ÇÑ´Ù.
	//
	pUser->SetRoomInnerIndex( pOpenRoom->AllocEmptyInnerIndex() );


	//
	// Step 4 : ÀÔÀå ¼º°ø Àü´Þ
	//
	OnAutoJoinReply( pUser, pOpenRoom );

	//
	// Step 7 : ´ë±â½Ç »ç¿ëÀÚ¿¡°Ô ¹æÀÇ Á¤º¸°¡ ¹Ù²î¾úÀ½À» Àü´ÞÇÑ´Ù.
	//
	m_pCasualPackets->RoomInfoChange.SetProperty(
		pOpenRoom->GetRoomIndex(),
		pOpenRoom->GetMaxUser(),
		pOpenRoom->GetNumOfUsers(),
		pOpenRoom->IsPassword(),
		pOpenRoom->GetOption()
		);
	SendToRoom( pWaitRoom, NULL, &m_pCasualPackets->RoomInfoChange, m_pCasualPackets->RoomInfoChange.GetLength() );
}

// »ç¿ëÀÚ Ã£±â ¿äÃ» Ã³¸®
inline VOID CNJeanGate::PacketProcessFindRequest( CUser* pUser, CCasualPacketFindRequest* pCasualPacket )
{
	CUser* pFindUser = NULL;
	switch( pCasualPacket->GetFindType() )
	{
	case CCasualPacketFindRequest::FindType::ID :
		pFindUser = m_UserMap.FindFromID( pCasualPacket->GetFindUser() );
		break;

	case CCasualPacketFindRequest::FindType::Nick :
		pFindUser = m_UserMap.FindFromNick( pCasualPacket->GetFindUser() );
		break;
	}


	// »ç¿ëÀÚ¸¦ Ã£Áö ¸øÇÑ °æ¿ì
	if( pFindUser == NULL )
	{
		m_pCasualPackets->FindReply.SetProperty(
			CCasualPacketFindReply::Result::Fail
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->FindReply ), m_pCasualPackets->FindReply.GetLength() );
	}

	// »ç¿ëÀÚ¸¦ Ã£Àº °æ¿ì
	else
	{
		// »ç¿ëÀÚ°¡ ´ë±â Ã¤³Î¿¡ ÀÖ´ÂÁö¸¦ °Ë»ç
		if( pFindUser->GetChannelIndex() == 0 )
		{
			m_pCasualPackets->FindReply.SetProperty(
				CCasualPacketFindReply::Result::Success,
				pFindUser->GetChannelIndex(),
				pFindUser->GetRoomIndex(),
				pFindUser->GetID().c_str(),
				static_cast<INT>( pFindUser->GetID().length() ),
				pFindUser->GetNick().c_str(),
				static_cast<INT>( pFindUser->GetNick().length() ),
				TRUE
			);
		}
		else
		{
			CChannel*	pFindUserChannel	= &m_aChannel	[ pFindUser->GetChannelIndex() ];
			CRoom*		pFindUserRoom		= &m_aRoom		[ pFindUser->GetRoomIndex() ];
			BOOL		bWaitRoom;

			if( pFindUserChannel->IsWaitRoom( pFindUserRoom ) )
				bWaitRoom = TRUE;
			else
				bWaitRoom = FALSE;

			m_pCasualPackets->FindReply.SetProperty(
				CCasualPacketFindReply::Result::Success,
				pFindUser->GetChannelIndex(),
				pFindUser->GetRoomIndex(),
				pFindUser->GetID().c_str(),
				static_cast<INT>( pFindUser->GetID().length() ),
				pFindUser->GetNick().c_str(),
				static_cast<INT>( pFindUser->GetNick().length() ),
				bWaitRoom
				);
		}
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->FindReply ), m_pCasualPackets->FindReply.GetLength() );
	}
}

// Ã¤ÆÃ ¸Þ½ÃÁö
inline VOID CNJeanGate::PacketProcessChat( CUser* pUser, CCasualPacketChat* pCasualPacket )
{
	//
	// Step 1 : ¿¹¿Ü °Ë»ç
	//

	INT nRoomIndex = pUser->GetRoomIndex();

	if( nRoomIndex < 0 || pUser->GetChannelIndex() <= 0 )
		return;

	//
	// Step 2 :  ÇöÀç »ç¿ëÀÚ°¡ ¼ÓÇÑ ¹æÀÇ ¸ðµç »ç¿ëÀÚ¿¡°Ô Àü´ÞÇÑ´Ù.
	//
	SendToRoom( &m_aRoom[nRoomIndex], pUser, pCasualPacket, pCasualPacket->GetLength() );
}


// ÂÊÁö
inline VOID CNJeanGate::PacketProcessMemo( CUser* pUser, CCasualPacketMemo* pCasualPacket )
{
	CUser* pFindUser = NULL;
	switch( pCasualPacket->GetFindType() )
	{
	case CCasualPacketMemo::FindType::ID :
		pFindUser = m_UserMap.FindFromID( pCasualPacket->GetReceiver() );
		break;

	case CCasualPacketMemo::FindType::Nick :
		pFindUser = m_UserMap.FindFromNick( pCasualPacket->GetReceiver() );
		break;
	}

	// »ç¿ëÀÚ¸¦ Ã£Áö ¸øÇÑ °æ¿ì
	if( pFindUser == NULL )
	{
		m_pCasualPackets->MemoReply.SetProperty(
			pCasualPacket->GetFindType(),
			CCasualPacketMemoReply::Result::Fail,
			pCasualPacket->GetReceiver(),
			static_cast<SHORT>( ::strlen( pCasualPacket->GetReceiver() ) )
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->MemoReply ), m_pCasualPackets->MemoReply.GetLength() );
	}

	// »ç¿ëÀÚ¸¦ Ã£Àº °æ¿ì
	else
	{
		m_pCasualPackets->MemoReply.SetProperty(
			pCasualPacket->GetFindType(),
			CCasualPacketMemoReply::Result::Success,
			pCasualPacket->GetReceiver(),
			static_cast<SHORT>( ::strlen( pCasualPacket->GetReceiver() ) )
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->MemoReply ), m_pCasualPackets->MemoReply.GetLength() );

		SendData( pFindUser, reinterpret_cast<const CHAR*>( pCasualPacket ), pCasualPacket->GetLength() );
	}
}

// ÃÊ´ëÇÏ±â
inline VOID CNJeanGate::PacketProcessInvite( CUser* pUser, CCasualPacketInvite* pCasualPacket )
{
	//
	// Step 1 : ¿¹¿Ü °Ë»ç
	//
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );

	if( pRoom == NULL || bWaitRoom )
	{
		m_pCasualPackets->InviteReply.SetProperty(
			pCasualPacket->GetFindType(),
			CCasualPacketInviteReply::Result::Fail,
			pCasualPacket->GetReceiver(),
			static_cast<SHORT>( ::strlen( pCasualPacket->GetReceiver() ) )
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->InviteReply ), m_pCasualPackets->InviteReply.GetLength() );
	}

	CUser* pFindUser = NULL;
	switch( pCasualPacket->GetFindType() )
	{
	case CCasualPacketInvite::FindType::ID :
		pFindUser = m_UserMap.FindFromID( pCasualPacket->GetReceiver() );
		break;

	case CCasualPacketInvite::FindType::Nick :
		pFindUser = m_UserMap.FindFromNick( pCasualPacket->GetReceiver() );
		break;
	}

	// »ç¿ëÀÚ¸¦ Ã£Áö ¸øÇÑ °æ¿ì
	if( pFindUser == NULL )
	{
		m_pCasualPackets->InviteReply.SetProperty(
			pCasualPacket->GetFindType(),
			CCasualPacketInviteReply::Result::Fail,
			pCasualPacket->GetReceiver(),
			static_cast<SHORT>( ::strlen( pCasualPacket->GetReceiver() ) )
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->InviteReply ), m_pCasualPackets->InviteReply.GetLength() );
	}

	// »ç¿ëÀÚ¸¦ Ã£Àº °æ¿ì
	else
	{
		m_pCasualPackets->InviteReply.SetProperty(
			pCasualPacket->GetFindType(),
			CCasualPacketInviteReply::Result::Success,
			pCasualPacket->GetReceiver(),
			static_cast<SHORT>( ::strlen( pCasualPacket->GetReceiver() ) )
			);
		SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->InviteReply ), m_pCasualPackets->InviteReply.GetLength() );

		pCasualPacket->SetIndex(
			pUser->GetChannelIndex(),
			pUser->GetRoomIndex()
			);
		if( pRoom->IsPassword() )
		{
			pCasualPacket->SetPass(
				pRoom->GetPassword().c_str(),
				static_cast<INT>( pRoom->GetPassword().length() ) );
		}

		SendData( pFindUser, reinterpret_cast<const CHAR*>( pCasualPacket ), pCasualPacket->GetLength() );
	}
}

inline VOID
CNJeanGate::PacketProcessCompulseExitRequest( CUser* pUser, CCasualPacketCompulsionExitRequest* pCasualPacket )
{
	CUser*		pExitUser;	// °­ÅðµÉ »ç¿ëÀÚ
	CChannel*	pChannel;	// °­ÅðµÉ »ç¿ëÀÚ°¡ ¼ÓÇÑ Ã¤³Î
	CRoom*		pRoom;		// °­ÅðµÉ »ç¿ëÀÚ°¡ ¼ÓÇÑ ¹æ
	BOOL		bWaitRoom;	// pRoomÀÌ ´ë±â½ÇÀÎÁö ¿©ºÎ

	//
	// ¿¹¿Ü °Ë»ç
	//
	pExitUser = m_UserMap.FindFromID( pCasualPacket->GetUserID() );
	if( pExitUser == NULL )
	{
		m_pCasualPackets->CompulseExitReply.SetProperty(
			CCasualPacketCompulsionExitReply::Result::FailNotExist,
			pCasualPacket->GetUserID(),
			static_cast<INT>( strlen( pCasualPacket->GetUserID() ) )
			);
		SendData(
			pUser,
			reinterpret_cast<const CHAR*>( &m_pCasualPackets->CompulseExitReply ),
			m_pCasualPackets->CompulseExitReply.GetLength()
			);
		return;
	}

	GetUserPlace( pExitUser, pChannel, pRoom, bWaitRoom );

	if( !pRoom || bWaitRoom )
	{
		m_pCasualPackets->CompulseExitReply.SetProperty(
			CCasualPacketCompulsionExitReply::Result::FailNotExist,
			pExitUser->GetID().c_str(),
			static_cast<INT>( pExitUser->GetID().length() )
			);
		SendData(
			pUser,
			reinterpret_cast<const CHAR*>( &m_pCasualPackets->CompulseExitReply ),
			m_pCasualPackets->CompulseExitReply.GetLength()
			);
		return;
	}

	SHORT nResult = CCasualPacketCompulsionExitReply::Result::FailETC;
	if( !IsCorrectCondition( nResult, pUser, PacketCondition::CompulseExit, reinterpret_cast<ULONG_PTR>( pExitUser ), reinterpret_cast<ULONG_PTR>( pRoom ) ) )
	{
		m_pCasualPackets->CompulseExitReply.SetProperty(
//			CCasualPacketCompulsionExitReply::Result::FailETC,
			nResult,
			pExitUser->GetID().c_str(),
			static_cast<INT>( pExitUser->GetID().length() )
			);
		SendData(
			pUser,
			reinterpret_cast<const CHAR*>( &m_pCasualPackets->CompulseExitReply ),
			m_pCasualPackets->CompulseExitReply.GetLength()
			);
		return;
	}

	m_pCasualPackets->CompulseExitReply.SetProperty(
		CCasualPacketCompulsionExitReply::Result::Success,
		pExitUser->GetID().c_str(),
		static_cast<INT>( pExitUser->GetID().length() )
		);
	SendData(
		pUser,
		reinterpret_cast<const CHAR*>( &m_pCasualPackets->CompulseExitReply ),
		m_pCasualPackets->CompulseExitReply.GetLength()
		);

	AddSignal(
		CCasualCommon::CasualSignalMessage::CompulseExitSignal,
		reinterpret_cast<ULONG_PTR>( pExitUser ),
		static_cast<ULONG_PTR>( CCasualPacketCompulsionExited::Type::Normal )	// °­Åð Å¸ÀÔ
		);
}

VOID CNJeanGate::OnLoginSuccess( CUser* pUser )
{
}

VOID
CNJeanGate::OnOpenRoomReply( CUser* pUser, CRoom* pRoom )
{
	// Open ¼º°øÀ» ÅëÁöÇÑ´Ù.
	m_pCasualPackets->OpenRoomReply.SetProperty(
		CCasualPacketOpenRoomReply::Result::Success,
		pRoom->GetRoomIndex() );
	SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->OpenRoomReply ), m_pCasualPackets->OpenRoomReply.GetLength() );
}

VOID CNJeanGate::OnJoinReply( CUser* pUser, CRoom* pRoom )
{
	// ÀÔÀåÇÑ »ç¿ëÀÚ¿¡°Ô ÀÔÀå ¼º°øÀ» Àü´Þ
	m_pCasualPackets->JoinRoomReply.SetProperty(
		CCasualPacketJoinRoomReply::Result::Success,
		pUser->GetRoomIndex() );
	SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->JoinRoomReply ), m_pCasualPackets->JoinRoomReply.GetLength() );
}

VOID CNJeanGate::OnAutoJoinReply( CUser* pUser, CRoom* pRoom )
{
	// ÀÔÀåÇÑ »ç¿ëÀÚ¿¡°Ô ÀÔÀå ¼º°øÀ» Àü´Þ
	m_pCasualPackets->AutoJoinRoomReply.SetProperty(
		CCasualPacketAutoJoinRoomReply::Result::Success,
		pUser->GetRoomIndex() );
	SendData( pUser, reinterpret_cast<const CHAR*>( &m_pCasualPackets->AutoJoinRoomReply ), m_pCasualPackets->AutoJoinRoomReply.GetLength() );
}