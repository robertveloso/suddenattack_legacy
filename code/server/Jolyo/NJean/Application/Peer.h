#pragma once

#include <Windows.h>

#include "..\..\DataStructure\InnerObject.h"

namespace Application
{
	class CPeer //角色状态
	{
	public:
		CPeer			( void )
		{
			m_InnerObjectForPing.Init( this );
		}
		virtual ~CPeer	( void ){}

	private:
		BOOL		m_bCloseWait;		// 是否为等候退出状态
		BOOL		m_bRequestWait;		// 
		BOOL		m_bAliveManaged;	// Alive 包府 措惑 咯何
		INT			m_nServicerIndex;	// Peer狼 辑厚辑 牢郸胶
		INT			m_nRequestCount;	// 请求退出的次数，若只要发送一次请求就认为是
		Common::DataStructure::CInnerObject<CPeer>
					m_InnerObjectForPing;	//放入CSessionGate：：m_AliveManager中管理

		ULONG_PTR	m_lpNetworkKey;		// 匙飘况农 虐
		DWORD		m_dwHandle;

		char*		m_pAssembleBuffer;		// Assemble 滚欺
		int			m_nAssemble;

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

		VOID		SetNetworkKey( ULONG_PTR lpNetworkKey, DWORD dwHandle )
		{
			m_lpNetworkKey	= lpNetworkKey;
			m_dwHandle		= dwHandle;
		}
		ULONG_PTR	GetNetworkKey()
		{
			return m_lpNetworkKey;
		}
		DWORD		GetHandle()
		{
			return m_dwHandle;
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