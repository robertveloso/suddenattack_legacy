#pragma once

#include "PacketCheckSum.h"
#include "SignalQueue.h"

#include "PacketBuffer.h"
#include "QueryBuffer.h"

#include "../../DataStructure/ListEx.h"


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
				DataType		= 4,	// Recv Data임을 통지
				CloseType		= 5,	// Peer가 Close 시켰을 때 통지
				DisconnectType	= 6,	// 능동적으로 Close되었을 때 통지

				FirstSend		= 7,
				DoCloseType		= 8,	// 능동적으로 Close를 시키라는 통지 : Send Queue에 사용
				DoShutdownType	= 9,	// 능동적으로 Close를 시키라는 통지 : Send Queue에 사용

				HttpRequest		= 10,
				HttpResponse	= 11,
				HttpResponseEnd	= 12,
				HttpError		= 13,


				DbReqeust		= 14,
				DbResponse		= 15,
			};

		protected:
			CHAR*			m_Buffer;
			INT				m_nBufferSize;
			INT				m_nPacketSize;	// 실제 패킷의 크기
	
			CSignal			m_Signal;		//SignalType句口

			ULONG_PTR		m_lpNetworkKey;	// TCP槨COverlappedListenSocket，HTTP槨CRequestObject
	
			Manage::CRequestObject*		m_pRqeustObject;
			
			Manage::CDbObject*			m_pDbObject;		//DbResponse
			Manage::CDbConnObject*		m_pDbConnObject;

			MessageType		m_MessageType;	// 메시지의 타입
			CPacketCheckSum::CheckSumType
							m_CheckSumType;	// 패킷 채크섬 타입

			DWORD			m_dwHttpResult;
			CHAR*			m_lpszServerName;
			CHAR*			m_lpszObjectName;
			CHAR*			m_lpszBody;

//				DWORD			m_dwSending;

			Manage::CPacketBuffer*		m_pPacketBuffer;
			Common::DataStructure::CListEx<Manage::CPacketBuffer*>*	
										m_pPacketBufferList;


		public:
			CMessage( VOID );
			CMessage( const CSignal& signal );
			CMessage( MessageType packetType );
			CMessage( ULONG_PTR lpNetworkKey, MessageType packetType );
			CMessage( ULONG_PTR lpNetworkKey, CHAR* pPacketBuffer, INT nPacketSize );

			CMessage( ULONG_PTR lpNetworkKey, MessageType packetType, DWORD dwHttpResult );



			CMessage( MessageType messageType, ULONG_PTR lpNetworkKey, Common::DataStructure::CListEx<Manage::CPacketBuffer*>* pPacketList )
			{
				m_MessageType		= messageType;
				m_lpNetworkKey		= lpNetworkKey;
				m_pPacketBufferList	= pPacketList;
			}
			CMessage( MessageType messageType, ULONG_PTR lpNetworkKey, Manage::CPacketBuffer* pPacketBuffer )
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


			CMessage( MessageType messageType, Manage::CDbObject* pDbObject )
			{
				m_MessageType	= messageType;
				m_pDbObject		= pDbObject;
			}

			CMessage( Manage::CDbConnObject* pDbConnObject )
			{
				m_MessageType	= CMessage::MessageType::DbReqeust;
				m_pDbConnObject = pDbConnObject;
			}
			
			virtual ~CMessage( VOID );

		public:
			// 생성을 VOID로 하였을 경우 내부 버퍼를 생성시키기 위해서 Create 한다.
//				virtual bool	Create( INT nBufferSize );
//				virtual VOID	Destroy();
			Common::DataStructure::CListEx<Manage::CPacketBuffer*>*
							GetPacketBufferList()
			{
				return m_pPacketBufferList;
			}

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

			Manage::CDbObject*
							GetDbObject()
			{
				return m_pDbObject;
			}
			Manage::CDbConnObject*
							GetDbConnObject()
			{
				return m_pDbConnObject;
			}

			/*
			VOID	SetSendingIndex( DWORD dwIndex )
			{
				m_dwSending = dwIndex;
			}
			DWORD	GetSendingIndex()
			{
				return m_dwSending;
			}
			*/

			virtual bool	Create( INT nBufferSize ){return TRUE;}
			virtual VOID	Destroy(){}

			// 패킷 타입을 카피하며, 데이터 타입인 경우 내부 버퍼를 카피한다.
			// 카피를 받는 패킷의 버퍼가 NULL이면, 데이터 타입일 경우 내부 버퍼를 카피할 수 없다.
			VOID	operator=( CMessage message );

			// 패킷을 수신할 Peer의 Key또는 그 Key를 포함하는 Packet,
			// 패킷 포인터
			// 패킷의 길이
			VOID	SetSendPacket( const CMessage& PeerPacket, VOID* pPacket, INT nLen );
			VOID	SetSendPacket( ULONG_PTR lpPeerNetworkKey, VOID* pPacket, INT nLen );

			VOID	SetHttpData( ULONG_PTR lpNetworkKey, const CHAR* lpszServerName, const CHAR* lpszObjectName, const CHAR* lpszBody );

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

			VOID		SetHttpData( INT nSize, ULONG_PTR lpNetworkKey )
			{
				m_MessageType		= MessageType::HttpResponse;
				m_nPacketSize		= nSize;
				m_lpNetworkKey		= lpNetworkKey;
			}

			INT			GetHttpResult()
			{
				return m_dwHttpResult; //茄셥槨句口관돨ID긍뵀
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


			ULONG_PTR	GetNetworkKey()
			{
				return m_lpNetworkKey;
			}

			VOID		SetNetworkKey( ULONG_PTR lpNetworkKey )
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
