#pragma once

#include "..\NJean\Application\Peer.h"
#include "..\DataStructure\InnerObject.h"
#include "CasualPacket.h"	// ID의 길이를 가져오기 위해 사용

namespace GamehiSolution
{
	namespace CasualGame
	{
		using namespace CasualPacket;
		class CServerPeer : public Application::CPeer
		{
			/*
		public:
			enum	SessionStatus
			{
				Ready	= 0,	// 접속된 상태
				Login	= 1,	// 로그인된 상태 : 다중 세션인 경우에 Login 상태가 되며, Unique한 경우에 Service 상태가 된다.
				Service	= 2,	// 서비스 가능 상태

				// 흐름
				// Ready -> Login -> Service	: 인증처리중 다중 세션이 된 경우
				// Ready -> Service				: Unique한 인증이 된 경우
			};

		private:
			long		m_lIdd;
			char		m_strID[ CCasualPacketInfo::StringLength::UserID + 1 ];		// NULL 포함 + 1

			BOOL		m_bLogined;		// 인증 성공 여부
			BOOL		m_bService;		// 서비스 가능 상태 여부

			int			m_nRoomIndex;	// 사용자가 속한 Room의 Index

			Common::DataStructure::CInnerObject<CServerPeer>
					m_InnerObjectForRoom;	// Room 안에 속하기 위한 Inner Object
		public:
			CServerPeer();
			virtual ~CServerPeer();

		public:
			Common::DataStructure::CInnerObject<CServerPeer>*
				GetInnerObjectForRoom()
			{
				return &m_InnerObjectForRoom;
			}

			long		GetIdd()
			{
				return m_lIdd;
			}
			void		SetIdd( long lIdd )
			{
				m_lIdd = lIdd;
			}

			LPCTSTR		GetID()
			{
				return m_strID;
			}
			void		SetID( LPCTSTR strID, int nLen )
			{
				if( nLen > CCasualPacketInfo::StringLength::UserID )
					nLen = CCasualPacketInfo::StringLength::UserID;

				::memcpy( m_strID, strID, nLen );
				m_strID[nLen] = NULL;
			}

			BOOL		IsLogined()
			{
				return m_bLogined;
			}
			void		SetLogined( BOOL bLogined )
			{
				m_bLogined = bLogined;
			}

			BOOL		IsService()
			{
				return m_bService;
			}
			void		SetService( BOOL bService )
			{
				m_bService = bService;
			}

			int		GetRoomIndex()
			{
				return m_nRoomIndex;
			}
			void	SetRoomIndex( int nRoomIndex )
			{
				m_nRoomIndex = nRoomIndex;
			}

			*/
		};// class CServerPeer

	}// namespace CasualGame

}// namespace GamehiSolution