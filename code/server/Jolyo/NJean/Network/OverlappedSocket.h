/*****************************************************************************************************************
	COverlappedSocket

	ÀÛ¼ºÀÚ : °­ÁøÇù
	ÀÛ¼ºÀÏ : 2004³â 2¿ù 25~27ÀÏ

	¼³  ¸í :
		IOCP¿¡ µî·ÏµÇ´Â Overlapped Socket Class
		Accept, Recv, Send µîÀÇ ÀÌº¥Æ®¸¦ ¹Þ±â À§ÇØ »ç¿ëµÇ´Â ClassÀÇ base Class
	»ç  ¿ë :
		1. IOCP·ÎºÎÅÍ ¿Ï·á ÀÌº¥Æ®¸¦ ¹ÞÀ¸¸é, COverlappedSocketÀ¸·Î Ref¸¦ ¹Þ´Â´Ù.
		2. COverlappedSocketÀÇ vitual functionÀÎ GetIOType¸¦ ÅëÇØ¼­ ½ÇÁ¦ °´Ã¼ÀÇ Å¸ÀÔÀ» ¹Þ´Â´Ù.
		   µû¶ó¼­ ÀÌ Å¬·¡½º¸¦ »ó¼Ó¹ÞÀº Å¬·¡½º´Â ¹Ýµå½Ã GetIOTypeÀ» ±¸ÇöÇØ¾ß ÇÑ´Ù.
*****************************************************************************************************************/


#pragma once

#include <Winsock2.h>
#include <Windows.h>

namespace Network
{
	namespace TcpIOCP
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// OverlappedSocketÀÇ BaseClass
		class COverlappedSocket
		{
		public:
			COverlappedSocket(void){}
			~COverlappedSocket(void){}

		public:
			enum ErrorCode
			{
				BufferCreateFail		= 1,	// ¹öÆÛ »ý¼º ½ÇÆÐ
				SocketCreateFail		= 2,	// ¼ÒÄÏ »ý¼º ½ÇÆÐ
				SetSockOptFail			= 3,	// ¼ÒÄÏ ¿É¼Ç º¯°æ ½ÇÆÐ
				AcceptExFail			= 4,	// AcceptEx µî·Ï ½ÇÆÐ
				AcceptPoolCreateFail	= 5		// Accept Pool »ý¼º ½ÇÆÐ
			};

			enum IOType
			{
				NoneType		= 0,
				AcceptType		= 1,
				AcceptRecvType	= 2,
				RecvType		= 3,
				SendType		= 4
			};

			// Attributtes
		public:
			SOCKET			m_sSocket;
			COverlappedSocket*
				m_pParentOvSocket;

		protected:
			DWORD			m_dwErrCode;			// ¸¶Áö¸· ¿¡·¯ ÄÚµå

			ULONG_PTR		m_lpdwOwnerKey;			// ÀÌ ¼ÒÄÏÀ» »ç¿ëÇÏ´Â °´Ã¼ÀÇ Å°
			ULONG_PTR		m_lpAssemblerKey;		// ÀÌ ¼ÒÄÏÀ¸·ÎºÎÅÍ Data AssembleÀ» ÇÏ´Â °´Ã¼ÀÇ Å°
			DWORD			m_dwAssemblerHandle;

			int				m_nServiceIndex;		// ¼ÒÄÏÀÇ ¼­ºñ½º ÀÎµ¦½º


			// Operations
		public:
			// IOCP¿¡ µî·ÏÇÒ IO ÇÚµéÀ» ¹ÝÈ¯ÇÑ´Ù.
			HANDLE	GetIOHandle()
			{
				return (HANDLE) m_sSocket;
			}

			// ¸¶Áö¸· ¿¡·¯¸¦ ¹ÝÈ¯ÇÑ´Ù.
			DWORD	GetLastError(void)
			{
				return m_dwErrCode;
			}

			void	SetOwnerKey( ULONG_PTR dwOwnerKey )
			{
				m_lpdwOwnerKey = dwOwnerKey;
			}
			ULONG_PTR	GetOwnerKey()
			{
				return m_lpdwOwnerKey;
			}

			void	SetAssemblerKey( ULONG_PTR lpAssemblerKey, DWORD dwAssemblerHandle )
			{
				m_lpAssemblerKey	= lpAssemblerKey;
				m_dwAssemblerHandle	= dwAssemblerHandle;
			}
			void	GetAssemblerKey( ULONG_PTR& lpAssemblerKey, DWORD& dwAssemblerHandle )
			{
				lpAssemblerKey		= m_lpAssemblerKey;
				dwAssemblerHandle	= m_dwAssemblerHandle;
			}

			int		GetServiceIndex()//CreateListenerÖÐÉèÖÃ,¶Ô·þÎñÆ÷À´Ëµ,×Ü¶¨ÒåÎªServicerIndex::Client(1)
			{
				return m_nServiceIndex;
			}
			void	SetServiceIndex( int nServiceIndex )
			{
				m_nServiceIndex = nServiceIndex;
			}

			// Overroadable
		public:
			// IOTypeÀ» ¹ÝÈ¯ÇÑ´Ù.
			virtual IOType GetIOType( LPOVERLAPPED lpOv )
			{
				return IOType::NoneType;
			}

			virtual void	Init()
			{
				m_pParentOvSocket = NULL;
			}
		};// class COverlappedSocket

	}// namespace TcpIOCP
}// namespace Network
