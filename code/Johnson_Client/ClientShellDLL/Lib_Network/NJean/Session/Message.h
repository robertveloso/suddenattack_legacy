#pragma once

#include "PacketCheckSum.h"
#include "SignalQueue.h"

#include "PacketBuffer.h"
#include "QueryBuffer.h"

namespace GamehiSolution
{
	namespace Session
	{
		namespace Message
		{
			class CMessage
			{
			public:
				enum	MessageType
				{
					SignalType		= 1,	// Event Type

					AcceptType		= 2,	// Accept 되었음을 통지
					ConnectType		= 3,	// Connect 되었음을 통지
					BeginConnectType= 4,
					DataType		= 5,	// Recv Data임을 통지
					CloseType		= 6,	// Peer가 Close 시켰을 때 통지
					DisconnectType	= 7,	// 능동적으로 Close되었을 때 통지

					FirstSend		= 8,
					DoCloseType		= 9,	// 능동적으로 Close를 시키라는 통지 : Send Queue에 사용
					DoShutdownType	= 10,	// 능동적으로 Close를 시키라는 통지 : Send Queue에 사용

					HttpRequest		= 11,
					HttpResponse	= 12,
					HttpResponseEnd	= 13,
					HttpError		= 14,
				};

			protected:
				CHAR*			m_Buffer;
				INT				m_nBufferSize;
				INT				m_nPacketSize;	// 실제 패킷의 크기
				CSignal			m_Signal;

				unsigned long		m_lpNetworkKey;	// 패킷을 전달한 object의 키
//				DWORD			m_dwSendHandle;	// Send 시의 NetworkKey의 handle

				MessageType		m_MessageType;	// 메시지의 타입
				CPacketCheckSum::CheckSumType
					m_CheckSumType;	// 패킷 채크섬 타입

				DWORD			m_dwHttpResult;
				CHAR*			m_lpszServerName;
				CHAR*			m_lpszObjectName;
				CHAR*			m_lpszBody;

				Manage::CPacketBuffer*		m_pPacketBuffer;
				Manage::CRequestObject*		m_pRqeustObject;

			public:
				CMessage( VOID );
				CMessage( const CSignal& signal );
				CMessage( MessageType packetType );
				CMessage( unsigned long lpNetworkKey, MessageType packetType );
				CMessage( unsigned long lpNetworkKey, CHAR* pPacketBuffer, INT nPacketSize );

				CMessage( unsigned long lpNetworkKey, MessageType packetType, DWORD dwHttpResult );



				CMessage( MessageType messageType, unsigned long lpNetworkKey, Manage::CPacketBuffer* pPacketBuffer )
				{
					m_MessageType	= messageType;
					m_lpNetworkKey	= lpNetworkKey;
					m_pPacketBuffer	= pPacketBuffer;
				}
				CMessage( MessageType messageType, Manage::CRequestObject* pRqeustObject )
				{
					m_MessageType	= messageType;
					m_pRqeustObject	= pRqeustObject;
				}
				CMessage( MessageType messageType, Manage::CRequestObject* pRqeustObject, Manage::CPacketBuffer* pPacketBuffer )
				{
					m_MessageType	= messageType;
					m_pRqeustObject	= pRqeustObject;
					m_pPacketBuffer	= pPacketBuffer;
				}

				virtual ~CMessage( VOID );

			public:
				// 생성을 VOID로 하였을 경우 내부 버퍼를 생성시키기 위해서 Create 한다.
//				virtual bool	Create( INT nBufferSize );
//				virtual VOID	Destroy();

				Manage::CPacketBuffer*
								GetPacketBuffer()
				{
					return m_pPacketBuffer;
				}
				Manage::CRequestObject*
								GetRequestObject()
				{
					return m_pRqeustObject;
				}
				Manage::CQueryBuffer*
								GetQueryBuffer()
				{
					return m_pRqeustObject->GetQueryBuffer();
				}

				virtual bool	Create( INT nBufferSize ){return TRUE;}
				virtual VOID	Destroy(){}

				// 패킷 타입을 카피하며, 데이터 타입인 경우 내부 버퍼를 카피한다.
				// 카피를 받는 패킷의 버퍼가 NULL이면, 데이터 타입일 경우 내부 버퍼를 카피할 수 없다.
				VOID	operator=( CMessage message );

				// 패킷을 수신할 Peer의 Key또는 그 Key를 포함하는 Packet,
				// 패킷 포인터
				// 패킷의 길이
				VOID	SetSendPacket( const CMessage& PeerPacket, VOID* pPacket, INT nLen );
				VOID	SetSendPacket( unsigned long lpPeerNetworkKey, VOID* pPacket, INT nLen );

				VOID	SetHttpData( unsigned long lpNetworkKey, const CHAR* lpszServerName, const CHAR* lpszObjectName, const CHAR* lpszBody );

//				VOID	SetSendHandle( DWORD dwSendHandle )
//				{
//					m_dwSendHandle = dwSendHandle;
//				}
//				DWORD	GetSendHandle()
//				{
//					return m_dwSendHandle;
//				}

				// 패킷 타입을 반환한다.
				MessageType	GetType()
				{
					return m_MessageType;
				}

				// 패킷 타입을 설정한다.
				VOID		SetType( MessageType messageType )
				{
					m_MessageType = messageType;
				}

				CHAR*		GetBuffer()
				{
					return m_pPacketBuffer->GetData();
				}

				VOID		SetHttpData( INT nSize, unsigned long lpNetworkKey )
				{
					m_MessageType		= MessageType::HttpResponse;
					m_nPacketSize		= nSize;
					m_lpNetworkKey		= lpNetworkKey;
				}

				INT			GetHttpResult()
				{
					return m_dwHttpResult;
				}

				const CHAR* GetServerName()
				{
					return m_lpszServerName;
				}
				const CHAR* GetObjectName()
				{
					return m_lpszObjectName;
				}
				const CHAR* GetBody()
				{
					return m_lpszBody;
				}

				// 패킷의 채크섬 타입을 설정한다.
				VOID		SetCheckSumType( CPacketCheckSum::CheckSumType checkSumType )
				{
					m_CheckSumType = checkSumType;
				}
				// 패킷의 채크섬 타입을 반환한다.
				CPacketCheckSum::CheckSumType
					GetCheckSumType()
				{
					return m_CheckSumType;
				}


				unsigned long	GetNetworkKey()
				{
					return m_lpNetworkKey;
				}

				VOID		SetNetworkKey( unsigned long lpNetworkKey )
				{
					m_lpNetworkKey = lpNetworkKey;
				}

				INT			GetPacketSize()
				{
					return m_pPacketBuffer->GetDataSize();
				}

				CSignal		GetSignal()
				{
					return m_Signal;
				}
			};// class CMessage

		}// namespace Message

	}// namespace Session

}// namespace GamehiSolution