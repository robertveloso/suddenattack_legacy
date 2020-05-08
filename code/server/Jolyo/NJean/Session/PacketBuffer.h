/*****************************************************************************************************************
*****************************************************************************************************************/

#pragma once

#include "../Application/Packet.h"

namespace Session{		namespace Manage{
	class CPacketBuffer		//数据包，！！！实际上归于Session::Message中更好
	{
		// 积己 / 家戈磊
	public:
		CPacketBuffer()
			:	m_pData			( m_Buffer)
			,	c_nBufferSize	( CPacket::PredefineLength::BasePacketSize	)
			,	m_nDataSize		( 0		)
			,	m_dwHandle		( NULL	)
		{}
		virtual ~CPacketBuffer(VOID){}

		// Attributes
	private:
		CHAR		m_Buffer[CPacket::PredefineLength::BasePacketSize+2];		// 滚欺狼 角力 皋葛府 康开
		const INT	c_nBufferSize;											// 角力 滚欺狼 农扁

		CHAR*		m_pData;		// 蜡瓤茄 单捞磐狼 器牢磐 : Send狼 版快 漏妨辑 焊郴霸 登搁 捞 蔼捞 崔扼柳促.
		INT			m_nDataSize;	// 单捞磐 农扁

		DWORD		m_dwHandle;		// 捞 菩哦狼 家蜡林狼 勤甸
		DWORD		m_dwSendIndex;	// Send 鉴辑 锅龋

	public:
		VOID		CopyData		( const CHAR* pData, INT nDataSize );
		VOID		CopyData		( const CHAR* pData, INT nDataSize, DWORD dwHandle, DWORD dwSendIndex );

		VOID		ReplaceDataPos	( INT nProcessedSize );

		// Get / Set
		BOOL		IsCreated	()
		{
			return m_Buffer != NULL;
		}
		CHAR*		GetData		()
		{
			return	m_pData;
		}
		VOID		SetDataSize ( INT nDataSize )
		{
			m_nDataSize = nDataSize;
		}
		INT			GetDataSize	()
		{
			return	m_nDataSize;
		}
		INT			GetBufferSize()
		{
			return	c_nBufferSize;
		}
		DWORD		GetHandle	()
		{
			return	m_dwHandle;
		}
		DWORD		GetSendIndex()
		{
			return m_dwSendIndex;
		}
		VOID		SetSendIndex( DWORD dwSendIndex )
		{
			m_dwSendIndex = dwSendIndex;
		}
	};// CPacketBuffer

}/* namespace Manage */		}/* namespace Session */
