#pragma once

#include "User.h"
#include "Room.h"
#include "..\DataStructure\InnerManager.h"

namespace CasualGame
{
	class CChannel
	{
	public:
		CChannel(void){}
		virtual ~CChannel(void){}

	private:
		CPocket*	m_pPocket;					// 사용자의 게임별 특화된 클래스

		Common::DataStructure::CInnerManager<CUser>
					m_InnerManagerUser;			// 채널의 모든 사용자 리스트

		Common::DataStructure::CInnerManager<CRoom>
					m_InnerManagerRoom;			// 채널의 모든 방 리스트

	protected:
		int			m_nMaxUser;					// 채널에 최대로 들어갈 수 있는 사용자 수
		int			m_nMaxRoom;					// 채널에 개설될 수 있는 최대 방의 개수

		CRoom*		m_pWaitRoom;				// 대기실

		INT			m_nChannelIndex;			// 현재 룸의 인덱스

		string		m_strChannelName;

	public:
		void		Init	( INT nMaxUser, INT nMaxRoom, const string& strChannelName, INT nChannelIndex );

		CPocket*	GetPocket()
		{
			return m_pPocket;
		}
		void		SetPocket( CPocket* pPocket )
		{
			m_pPocket = pPocket;
		}

		// 채널의 인덱스를 가져온다.
		INT			GetChannelIndex()
		{
			return m_nChannelIndex;
		}

		const char*	GetChannelName()
		{
			return m_strChannelName.c_str();
		}

		void		SetWaitRoom	( CRoom* pWaitRoom )
		{
			m_pWaitRoom = pWaitRoom;
		}
		CRoom*		GetWaitRoom	()
		{
			return m_pWaitRoom;
		}
		BOOL		IsWaitRoom	( const CRoom* pRoom )
		{
			return m_pWaitRoom == pRoom;
		}

		BOOL		AddRoom		( CRoom* pRoom );
		BOOL		RemoveRoom	( CRoom* pRoom );

		INT			GetMaxRoom()
		{
			return m_nMaxRoom;
		}
		INT			GetNumOfOpenRooms()
		{
			return m_InnerManagerRoom.GetNumOfInnerObject();
		}
		// 채널의 첫번째 방을 가져온다.
		CRoom*	GetFirstRoom()
		{
			return m_InnerManagerRoom.GetFirstInnerObject();
		}

		// 채널의 다음 방을 가져온다.
		CRoom*	GetNextRoom()
		{
			return m_InnerManagerRoom.GetNextInnerObject();
		}

		// 채널에 입장을 한다.
		BOOL		Join	( CUser* pUser );

		// 채널에서 나간다.
		BOOL		Leave	( CUser* pUser );

		INT			GetMaxUser()
		{
			return m_nMaxUser;
		}
		INT			GetNumOfUsers()
		{
			return m_InnerManagerUser.GetNumOfInnerObject();
		}

		// 채널의 첫번째 클라이언트 서비서를 가져온다.
		CUser*	GetFirstUser()
		{
			return m_InnerManagerUser.GetFirstInnerObject();
		}

		// 채널의 다음 클라이언트 서비서를 가져온다.
		CUser*	GetNextUser()
		{
			return m_InnerManagerUser.GetNextInnerObject();
		}
	};// CRoom

}// namespace CasualGame
