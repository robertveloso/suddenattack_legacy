#pragma once

#include <Windows.h>

#include "..\..\DataStructure\InnerObject.h"

namespace GamehiSolution
{
	namespace Application
	{
		class CPeer
		{
		public:
			CPeer			( void )
			{
				m_InnerObjectForPing.Init( this );
			}
			virtual ~CPeer	( void ){}

		private:
			BOOL		m_bCloseWait;		// CloseWait 상태 여부
			BOOL		m_bRequestWait;		// Request되었을 경우 반납 대기
			BOOL		m_bAliveManaged;	// Alive 관리 대상 여부
			INT			m_nServicerIndex;	// Peer의 서비서 인덱스
			INT			m_nRequestCount;	// Request한 갯수
			Common::DataStructure::CInnerObject<CPeer>
						m_InnerObjectForPing;

			unsigned long	m_lpNetworkKey;		// 네트워크 키

			char*		m_pAssembleBuffer;		// Assemble 버퍼
			int			m_nAssemble;

			BOOL		m_bBeginConnect;

		public:
			void		Create( int nMaxPacketSize )
			{
				m_pAssembleBuffer	= new char[nMaxPacketSize];
				m_nAssemble			= 0;
			}

			void		Destroy()
			{
				delete[] m_pAssembleBuffer;
			}

			char*		GetAssembleBuffer()
			{
				return m_pAssembleBuffer;
			}

			int			GetAssembleSize()
			{
				return m_nAssemble;
			}
			void		SetAssembleSize( int nAssembleSize )
			{
				m_nAssemble = nAssembleSize;
			}

			virtual void	Init()
			{
				m_bCloseWait		= FALSE;
				m_nRequestCount		= 0;
				m_bRequestWait		= FALSE;

				m_bBeginConnect		= FALSE;
			}

			void		SetBeginConnect( BOOL bBegin )
			{
				m_bBeginConnect		= bBegin;
			}
			BOOL		IsBeginConnect()
			{
				return m_bBeginConnect;
			}

			BOOL	IsAliveManaged()
			{
				return m_bAliveManaged;
			}
			void	SetAliveManaged( BOOL bAliveManaged )
			{
				m_bAliveManaged = bAliveManaged;
			}

			BOOL	IsCloseWait()
			{
				return m_bCloseWait;
			}

			VOID	SetCloseWait()
			{
				m_bCloseWait = TRUE;
			}

			VOID		SetNetworkKey( unsigned long lpNetworkKey )
			{
				m_lpNetworkKey = lpNetworkKey;
			}
			unsigned long	GetNetworkKey()
			{
				return m_lpNetworkKey;
			}

			Common::DataStructure::CInnerObject<CPeer>*
						GetInnerObjectForPing()
			{
				return &m_InnerObjectForPing;
			}

			VOID		SetServicerIndex( INT nIndex )
			{
				m_nServicerIndex = nIndex;
			}
			INT			GetServicerIndex()
			{
				return m_nServicerIndex;
			}

			VOID		IncrementRequestCount()
			{
				m_nRequestCount++;
			}
			VOID		DecrementRequestCount()
			{
				m_nRequestCount--;
			}
			BOOL		IsRequested()
			{
				return( m_nRequestCount > 0 );
			}

			BOOL		IsRequestWait()
			{
				return m_bRequestWait;
			}
			void		SetRequestWait()
			{
				m_bRequestWait = TRUE;
			}

		};// class CSessionGate

	}// namespace Session
}// namespace GamehiSolution