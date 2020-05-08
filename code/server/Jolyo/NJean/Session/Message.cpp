#include "StdAfx.h"
#include ".\Message.h"

using namespace Session::Message;

CMessage::CMessage(VOID)
:	m_Buffer		(NULL)
,	m_nBufferSize	(0)
,	m_nPacketSize	(0)
{
}

CMessage::CMessage( const CSignal& signal )
:	m_MessageType	( MessageType::SignalType )
{
	m_Signal = signal;
}

CMessage::CMessage( MessageType MessageType )
:	m_MessageType	( MessageType )
{
}

CMessage::CMessage( ULONG_PTR lpNetworkKey, MessageType MessageType )
:	m_MessageType	( MessageType	)
,	m_Buffer		( NULL			)
,	m_nBufferSize	( 0				)
,	m_nPacketSize	( 0				)
,	m_lpNetworkKey( lpNetworkKey)
{
}

CMessage::CMessage( ULONG_PTR lpNetworkKey, MessageType MessageType, DWORD dwHttpResult )
:	m_MessageType	( MessageType	)
,	m_Buffer		( NULL			)
,	m_nBufferSize	( 0				)
,	m_nPacketSize	( 0				)
,	m_lpNetworkKey( lpNetworkKey)
,	m_dwHttpResult	( dwHttpResult	)
{
}

CMessage::CMessage( ULONG_PTR lpNetworkKey, CHAR* pPacketBuffer, INT nPacketSize )
:	m_MessageType	( MessageType::DataType	)
,	m_Buffer		( pPacketBuffer			)
,	m_nBufferSize	( nPacketSize			)
,	m_nPacketSize	( nPacketSize			)
,	m_lpNetworkKey( lpNetworkKey		)
{
}

CMessage::~CMessage(VOID)
{
}

VOID CMessage::operator=( CMessage message )
{
	m_MessageType	= message.m_MessageType;
	m_lpNetworkKey	= message.m_lpNetworkKey;
	m_dwHttpResult	= message.m_dwHttpResult;

	switch( m_MessageType )
	{
	case MessageType::DataType :
	case MessageType::HttpRequest :
	case MessageType::HttpResponse :
//		if( m_Buffer == NULL )
//			return;
//		::memcpy( m_Buffer, message.m_Buffer, message.m_nPacketSize );
		m_pPacketBuffer		= message.m_pPacketBuffer;
		m_pPacketBufferList	= message.m_pPacketBufferList;
		m_lpNetworkKey		= message.m_lpNetworkKey;
		m_nPacketSize		= message.m_nPacketSize;
		m_pRqeustObject		= message.m_pRqeustObject;

		m_lpszServerName	= &m_Buffer[0];
		m_lpszObjectName	= &m_Buffer[50];
		m_lpszBody			= &m_Buffer[100];
		break;
	case MessageType::DbReqeust :
	case MessageType::DbResponse :
		m_pDbObject		= message.m_pDbObject;
		m_pDbConnObject = message.m_pDbConnObject;
		break;

	case MessageType::HttpResponseEnd :
	case MessageType::HttpError :
		m_dwHttpResult	= message.m_dwHttpResult;
		break;

	case MessageType::SignalType :
		m_Signal = message.m_Signal;
		break;

	default :
		break;
	}
}

// 패킷의 체크섬 타입
// 패킷을 수신할 Peer의 Key를 포함하는 Packet,
// 패킷 포인터
// 패킷의 길이
VOID CMessage::SetSendPacket( const CMessage& PeerPacket, VOID* pPacket, INT nLen )
{
	m_lpNetworkKey	= PeerPacket.m_lpNetworkKey;
	::memcpy( m_Buffer, pPacket, nLen );

	m_nPacketSize = nLen;
	return;

	// 패킷에 Check Sum을 삽입한다.
	CPacketCheckSum::SetCheckSum( m_CheckSumType, (CHAR*)m_Buffer, nLen );

	m_nPacketSize = nLen + CPacketCheckSum::GetCheckSumSize( m_CheckSumType );
}

// 패킷의 체크섬 타입
// 패킷을 수신할 Peer의 Key
// 패킷 포인터
// 패킷의 길이
VOID CMessage::SetSendPacket( ULONG_PTR lpPeerNetworkKey, VOID* pPacket, INT nLen )
{
	m_lpNetworkKey = lpPeerNetworkKey;
	::memcpy( m_Buffer, pPacket, nLen );

	m_nPacketSize = nLen;
}


VOID CMessage::SetHttpData( ULONG_PTR lpNetworkKey, const CHAR* lpszServerName, const CHAR* lpszObjectName, const CHAR* lpszBody )
{
	m_lpNetworkKey		= lpNetworkKey;
	m_lpszServerName	= &m_Buffer[0];
	m_lpszObjectName	= &m_Buffer[50];
	m_lpszBody			= &m_Buffer[100];

	INT nServerName	= 0;
	INT nObjectName	= 0;
	if( lpszServerName != NULL )
	{
		nServerName	= (INT)::strlen( lpszServerName	);
		nObjectName	= (INT)::strlen( lpszObjectName	);

		::memcpy( m_lpszServerName,	lpszServerName,	nServerName	);
		::memcpy( m_lpszObjectName,	lpszObjectName,	nObjectName	);
	}
	INT nBody = (INT)::strlen( lpszBody );

	::memcpy( m_lpszBody, lpszBody, nBody );

	m_lpszServerName	[nServerName]	= NULL;
	m_lpszObjectName	[nObjectName]	= NULL;
	m_lpszBody			[nBody]			= NULL;

	m_nPacketSize	= 100 + nBody + 1;
}

/*
BOOL CMessage::Create( INT nBufferSize )
{
	m_Buffer = new CHAR[nBufferSize];
	if( m_Buffer == NULL )
		return FALSE;

	m_nBufferSize = nBufferSize;
	return TRUE;
}

VOID CMessage::Destroy()
{
	if( m_Buffer )
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
		m_nBufferSize = 0;
		m_nPacketSize = 0;
	}
}
*/