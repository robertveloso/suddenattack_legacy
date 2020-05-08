#include "StdAfx.h"
/*
#include ".\packet.h"

using namespace Network;
CPacket::CPacket(void)
:	m_Buffer		(NULL)
,	m_nBufferSize	(0)
,	m_nPacketSize	(0)
{
}

CPacket::CPacket( const CEvent& event )
:	m_PacketType	( PacketType::EventType )
{
	m_Event = event;
}

CPacket::CPacket( PacketType packetType )
:	m_PacketType	( packetType )
{
}

CPacket::CPacket( unsigned long dwNetworkKey, PacketType packetType )
:	m_PacketType	( packetType	)
,	m_Buffer		( NULL			)
,	m_nBufferSize	( 0				)
,	m_nPacketSize	( 0				)
,	m_dwNetworkKey	( dwNetworkKey	)
{
}

CPacket::CPacket( unsigned long dwNetworkKey, PacketType packetType, DWORD dwHttpResult )
:	m_PacketType	( packetType	)
,	m_Buffer		( NULL			)
,	m_nBufferSize	( 0				)
,	m_nPacketSize	( 0				)
,	m_dwNetworkKey	( dwNetworkKey	)
,	m_dwHttpResult	( dwHttpResult	)
{
}

CPacket::CPacket( unsigned long dwNetworkKey, char* pPacketBuffer, int nPacketSize )
:	m_PacketType	( PacketType::DataType	)
,	m_Buffer		( pPacketBuffer			)
,	m_nBufferSize	( nPacketSize			)
,	m_nPacketSize	( nPacketSize			)
,	m_dwNetworkKey	( dwNetworkKey			)
{
}

CPacket::~CPacket(void)
{
}

void CPacket::operator=( CPacket packet )
{
	m_PacketType	= packet.m_PacketType;
	m_dwNetworkKey	= packet.m_dwNetworkKey;

	switch( m_PacketType )
	{
	case PacketType::DataType :
	case PacketType::HttpRequest :
	case PacketType::HttpResponse :
		if( m_Buffer == NULL )
			return;
		::memcpy( m_Buffer, packet.m_Buffer, packet.m_nPacketSize );
		m_nPacketSize		= packet.m_nPacketSize;

		m_lpszServerName	= &m_Buffer[0];
		m_lpszObjectName	= &m_Buffer[50];
		m_lpszBody			= &m_Buffer[100];
		break;

	case PacketType::HttpResponseEnd :
	case PacketType::HttpError :
		m_dwHttpResult	= packet.m_dwHttpResult;
		break;

	case PacketType::EventType :
		m_Event = packet.m_Event;
		break;

	default :
		break;
	}
}

// 패킷의 체크섬 타입
// 패킷을 수신할 Peer의 Key를 포함하는 Packet,
// 패킷 포인터
// 패킷의 길이
void CPacket::SetSendPacket( const CPacket& PeerPacket, void* pPacket, int nLen )
{
	m_dwNetworkKey	= PeerPacket.m_dwNetworkKey;
	::memcpy( m_Buffer, pPacket, nLen );

	// 패킷에 Check Sum을 삽입한다.
	CPacketCheckSum::SetCheckSum( m_CheckSumType, (char*)m_Buffer, nLen );

	m_nPacketSize = nLen + CPacketCheckSum::GetCheckSumSize( m_CheckSumType );
}

// 패킷의 체크섬 타입
// 패킷을 수신할 Peer의 Key
// 패킷 포인터
// 패킷의 길이
void CPacket::SetSendPacket( unsigned long dwPeerNetworkKey, void* pPacket, int nLen )
{
	m_dwNetworkKey = dwPeerNetworkKey;
	::memcpy( m_Buffer, pPacket, nLen );

	char* test = (char*)pPacket;
	if( test[0] != nLen )
	{
		int a = 3;
	}

	// 패킷에 Check Sum을 삽입한다.
	CPacketCheckSum::SetCheckSum( m_CheckSumType, (char*)m_Buffer, nLen );

	m_nPacketSize = nLen + CPacketCheckSum::GetCheckSumSize( m_CheckSumType );
}


void CPacket::SetHttpData( unsigned long dwNetworkKey, const char* lpszServerName, const char* lpszObjectName, const char* lpszBody )
{
	m_dwNetworkKey		= dwNetworkKey;
	m_lpszServerName	= &m_Buffer[0];
	m_lpszObjectName	= &m_Buffer[50];
	m_lpszBody			= &m_Buffer[100];

	int nServerName	= ::strlen( lpszServerName	);
	int nObjectName	= ::strlen( lpszObjectName	);
	int nBody		= ::strlen( lpszBody		);

	::memcpy( m_lpszServerName,	lpszServerName,	nServerName	);
	::memcpy( m_lpszObjectName,	lpszObjectName,	nObjectName	);
	::memcpy( m_lpszBody,		lpszBody,		nBody		);

	m_lpszServerName	[nServerName]	= NULL;
	m_lpszObjectName	[nObjectName]	= NULL;
	m_lpszBody			[nBody]			= NULL;

	m_nPacketSize	= 100 + nBody + 1;
}

bool CPacket::Create( int nBufferSize )
{
	m_Buffer = new char[nBufferSize];
	if( m_Buffer == NULL )
		return false;

	m_nBufferSize = nBufferSize;
	return true;
}

void CPacket::Destroy()
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