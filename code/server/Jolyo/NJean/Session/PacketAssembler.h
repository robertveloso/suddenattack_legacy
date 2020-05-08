/*****************************************************************************************************************
	CPacketAssembler

	작성자 : 강진협
	작성일 : 2004년 3월 4일

	설  명 :
		내부 버퍼를 사용하여, 네트워크 메시지들을 패킷단위로 재조립하여 반환해준다.
		내부 버퍼의 크기는 최대 패킷 크기의 2배가 되어야 한다.
		Length Field의 크기는 1, 2, 4Byte 세종류만 가질 수 있다.
	사  용 :
		1. Create를 통해서 내부 버퍼를 생성한다.
		2. Init~~ 을 통해서 패킷의 구조를 설정한다.
		3. 처음 사용할 때마다 Reset을 해준다.
		4. 메시지를 수신하면 Assemble을 통해서 내부 버퍼로 메시지들을 쌓는다.
		5. 1번 Assemble하면 GetCompletePacket을 통해 패킷이 반환될 때까지 반복하여, 조립된 패킷을 받는다.
		6. 4, 5번을 반복하면서 수행한다.
		7. Destroy를 통해서 내부 버퍼를 제거한다.

	* Length Base Packet Assembler만 구현되어 있다.
	* 추후에 Charector Base Packet Assembler로 구현해야 한다.
*****************************************************************************************************************/

#pragma once

#include <Windows.H>
#include "PacketCheckSum.h"
#include "SendPacket.h"
#include "..\..\DataStructure\List.h"
#include "..\..\DataStructure\ListEx.h"
#include "PacketBuffer.h"

namespace Session
{
	namespace Message
	{
		class CPacketAssembler
		{
		public:
			CPacketAssembler(void);
			~CPacketAssembler(void);

		public:
			// 패킷 단위
			enum	PacketUnitType
			{
				Length		= 1,	// 길이로 구분
				Charector	= 2		// 종결 문자로 구분
			};

			// Length Type
			enum	LengthType
			{
				Total			= 1,	// 패킷 전체의 길이
				After			= 2,	// Length 필드 다음부터의 길이
				IncludeAfter	= 3,	// 패킷 전체의 길이에서 Length 필드의 앞을 제외한 길이
			};

			// Length Size Type
			enum	LengthSizeType
			{
				OneByte			= 1,
				TwoByte			= 2,
				FourByte		= 4,
			};

			// Attributes
		private:
			// 길이 필드의 정보
			struct TagLengthField
			{
				int		nPos;				// Length Field의 위치
				int		nSize;				// Length Field의 길이
				BOOL	bLittleEndian;		// Length Field가 Little Endian인지 여부
			};

			struct	TagAssembleData
			{
				BOOL		bFirst;			// Assemble의 첫번째 데이타인지 여부
				char*		pDataBuffer;	// Assemble중인 데이타 버퍼의 포인터
				int			nLength;		// Assemble한 데이타의 총 길이
				long int	nPacketLength;	// Assemble할 패킷의 길이
				char		cFirstChar;		// Assemble한 패킷 반환시에 데이타가 뒤에 붙어 있을 경우
				// 패킷의 마지막에 NULL로 치환되는 부분, 다음 데이타의 첫번째 char
			};

			char*			m_aBuffer;			// 동적 배열로 할당받는 내부 버퍼
			int				m_nBufferSize;		// 내부 버퍼의 크기

			char*			m_aSendBuffer;		// Send Buffer
			int				m_nSendBufferSize;

			BOOL			m_bCloseWait;		// 종료 대기 상태 : Send List를 모두 제거할때 사용한다.

			DWORD			m_dwHandle;
			static DWORD	g_dwHandle;			//close珂++，杰鹿悧角顆寧돨


			BOOL			m_bFree;			// Free 시킬지 여부
			BOOL			m_bOver;


			TagAssembleData	m_AssembleData;		// Assemble 데이타 정보

			PacketUnitType	m_PacketUnitType;	// 패킷을 조립할 단위

			CPacketCheckSum::CheckSumType		// 패킷 체크섬 타입
							m_PacketCheckSumType;

			TagLengthField	m_LengthFieldInfo;	// 길이 필드의 정보
			char			m_cTerminatedChar;	// 종결자 문자

			Manage::CSendPacket*	m_pSendPacketHead;
			Manage::CSendPacket*	m_pSendPacketRear;

			Common::DataStructure::CListEx< Manage::CPacketBuffer* >
								m_lstPacketBuffer;
			Common::DataStructure::CList< Manage::CPacketBuffer* >
								m_lstFirstPacketBuffer;	// 우선적으로 보낼 패킷 버퍼 리스트

			// SendIndex는 1이상이며, 0은 Ping 등의 우선 처리 패킷이 사용한다.
			DWORD			m_dwSendIndex;			// Send할 Index : 1이상이어야 하며, 이 값보다 같거나 작은 패킷을 전송시킨다.
			DWORD			m_dwAllocSendIndex;		// 할당할 Send Index : 1이상이어야 하며, 할당시마다 1씩 증가시킨다.

			CRITICAL_SECTION		m_cs;

		public:
			int						m_nQueueingCount;

			// Operations
		public:
			/*/
			DWORD		GetSendIndex()
			{
				return m_dwSendIndex;
			}
			/*/
			BOOL		IsSending( DWORD dwSendIndex )
			{
				return m_dwSendIndex >= dwSendIndex;
			}
			VOID		AddSendIndex()
			{
				m_dwSendIndex++;
			}
			DWORD		AllocSendIndex()
			{
				DWORD dwIndex;
//					::EnterCriticalSection( &m_cs );

				dwIndex = m_dwAllocSendIndex;
				m_dwAllocSendIndex++;

//					::LeaveCriticalSection( &m_cs );
				return dwIndex;
			}

			// 내부 버퍼를 생성한다.
			BOOL		Create( int nBufferSize, INT nSendPacketListSize/* = 500*/ );
			// 내부 버퍼를 제거한다.
			void		Destroy();

			// 조립할 패킷의 타입을 설정한다.
			void		Init(
				CPacketCheckSum::CheckSumType	checkSumType,
				PacketUnitType					packetUnitType = PacketUnitType::Length );
			// 길이 기반 패킷에서 Length 필드를 설정한다.
			void		InitLengthField(
				int				nLengthPos,
				LengthSizeType	lengthSizeType,
				LengthType		lengthType = LengthType::Total,
				BOOL			bLittleEndian = true );
			// 문자 기반 패킷에서 종료 문자를 설정한다.
			void		InitTerminatedCharector( char cTerminatedChar );

			// 조립을 한다. 맨일 내부 버퍼의 크기를 초과하게되면 false를 반환한다.
			BOOL		Assemble( char* buffer, int nBufferSize );
			// 조립된 하나의 완전한 패킷을 반환한다.
			void*		GetCompletePacket( int& nOutPacketSize, int& nOutResult );

			// Assemble 작업을 Reset한다.
			void		Reset();

			Common::DataStructure::CListEx< Manage::CPacketBuffer* >*
						GetPacketBufferList()
			{
				return	&m_lstPacketBuffer;
			}
			Common::DataStructure::CList< Manage::CPacketBuffer* >*
						GetFirstPacketBufferList()
			{
				return	&m_lstFirstPacketBuffer;
			}

			void		AddCheckSum( const char* pData, int nDataLength );
			void		SetCheckSum( CHAR* pData, INT nDataLength, INT& out_nDataLength );

			char*					GetSendBuffer();
			int						GetSendSize();
			BOOL					IsEmptySendList();
			Manage::CSendPacket*	RemoveFromSendList( int nSendingLength );
			void					AddToSendList( Manage::CSendPacket* pSendPacket, BOOL bFirst = FALSE );
			Manage::CSendPacket*	FreeSendList();

			BOOL		IsCreated()
			{
				return m_aBuffer != NULL;
			}

			BOOL		IsFree()
			{
				return m_bFree;
			}
			VOID		SetFree()
			{
				m_bFree = TRUE;
			}

			BOOL		IsOverlapped()
			{
				return m_bOver;
			}
			VOID		SetOverlapped( BOOL bOver )
			{
				m_bOver = bOver;
			}

			VOID		ResetHandle()
			{
				m_dwHandle = g_dwHandle++;
			}

			DWORD		GetHandle()
			{
				return m_dwHandle;
			}

			BOOL					IsCloseWait()
			{
				return m_bCloseWait;
			}
			void					ResetCloseWait()
			{
				m_bCloseWait = FALSE;
			}

			void	Lock()
			{
				EnterCriticalSection( &m_cs );
			}
			void	Unlock()
			{
				LeaveCriticalSection( &m_cs );
			}

		private:
			// 길이 기반일 때의 패킷을 반환한다.
			void*	GetCompletePacketLengthType( int& nOutPacketSize, int& nOutResult );
			// 문자 기반일 때의 패킷을 반환한다.
			void*	GetCompletePacketCharType( int& nOutPacketSize, int& nOutResult );
		};// class CPacketAssembler

	}// namespace Message

}// namespace Session