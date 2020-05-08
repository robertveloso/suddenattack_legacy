/*
#pragma once

#include "PacketCheckSum.h"
#include "EventQueue.h"

namespace GamehiSolution
{
	namespace Session
	{
		namespace Message
		{
			class CMessage
			{
			public:
				enum	PacketType
				{
					EventType		= 1,	// Event Type

					AcceptType		= 2,	// Accept 되었음을 통지
					ConnectType		= 3,	// Connect 되었음을 통지
					DataType		= 4,	// Recv Data임을 통지
					CloseType		= 5,	// Peer가 Close 시켰을 때 통지
					DisconnectType	= 6,	// 능동적으로 Close되었을 때 통지

					FirstSend		= 7,
					DoCloseType		= 8,	// 능동적으로 Close를 시키라는 통지 : Send Queue에 사용

					HttpRequest		= 9,
					HttpResponse	= 10,
					HttpResponseEnd	= 11,
					HttpError		= 12,
				};

			protected:
				char*			m_Buffer;
				int				m_nBufferSize;
				int				m_nPacketSize;	// 실제 패킷의 크기
				CEvent			m_Event;

				unsigned long	m_dwNetworkKey;	// 패킷을 전달한 object의 키

				PacketType		m_PacketType;	// 패킷의 타입
				CPacketCheckSum::CheckSumType
					m_CheckSumType;	// 패킷 채크섬 타입

				DWORD			m_dwHttpResult;
				char*			m_lpszServerName;
				char*			m_lpszObjectName;
				char*			m_lpszBody;

			public:
				CPacket( void );
				CPacket( const CEvent& event );
				CPacket( PacketType packetType );
				CPacket( unsigned long dwNetworkKey, PacketType packetType );
				CPacket( unsigned long dwNetworkKey, char* pPacketBuffer, int nPacketSize );

				CPacket( unsigned long dwNetworkKey, PacketType packetType, DWORD dwHttpResult );
				~CPacket( void );

			public:
				// 생성을 void로 하였을 경우 내부 버퍼를 생성시키기 위해서 Create 한다.
				virtual bool	Create( int nBufferSize );
				virtual void	Destroy();

				// 패킷 타입을 카피하며, 데이터 타입인 경우 내부 버퍼를 카피한다.
				// 카피를 받는 패킷의 버퍼가 NULL이면, 데이터 타입일 경우 내부 버퍼를 카피할 수 없다.
				void	operator=( CPacket packet );

				// 패킷을 수신할 Peer의 Key또는 그 Key를 포함하는 Packet,
				// 패킷 포인터
				// 패킷의 길이
				void	SetSendPacket( const CPacket& PeerPacket, void* pPacket, int nLen );
				void	SetSendPacket( unsigned long dwPeerNetworkKey, void* pPacket, int nLen );

				void	SetHttpData( unsigned long dwNetworkKey, const char* lpszServerName, const char* lpszObjectName, const char* lpszBody );

				// 패킷 타입을 반환한다.
				PacketType	GetType()
				{
					return m_PacketType;
				}

				// 패킷 타입을 설정한다.
				void		SetType( PacketType packetType )
				{
					m_PacketType = packetType;
				}

				char*		GetBuffer()
				{
					return m_Buffer;
				}

				void		SetHttpData( int nSize, unsigned long dwNetworkKey )
				{
					m_PacketType	= PacketType::HttpResponse;
					m_nPacketSize	= nSize;
					m_dwNetworkKey	= dwNetworkKey;
				}

				int			GetHttpResult()
				{
					return m_dwHttpResult;
				}

				const char* GetServerName()
				{
					return m_lpszServerName;
				}
				const char* GetObjectName()
				{
					return m_lpszObjectName;
				}
				const char* GetBody()
				{
					return m_lpszBody;
				}

				// 패킷의 채크섬 타입을 설정한다.
				void		SetCheckSumType( CPacketCheckSum::CheckSumType checkSumType )
				{
					m_CheckSumType = checkSumType;
				}
				// 패킷의 채크섬 타입을 반환한다.
				CPacketCheckSum::CheckSumType
					GetCheckSumType()
				{
					return m_CheckSumType;
				}


				unsigned long GetNetworkKey()
				{
					return m_dwNetworkKey;
				}

				void		SetNetworkKey( unsigned long dwNetworkKey )
				{
					m_dwNetworkKey = dwNetworkKey;
				}

				int			GetPacketSize()
				{
					return m_nPacketSize;
				}

				CEvent		GetEvent()
				{
					return m_Event;
				}
			};// class CMessage

		}// namespace Message

	}// namespace Session

}// namespace GamehiSolution
*/