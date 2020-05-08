#pragma once

#include "User.h"
#include "..\DataStructure\InnerManager.h"

namespace CasualGame
{
	class CRoom
	{
	public:
		CRoom();
		virtual ~CRoom();

	public:
		enum	InnerIndexType		// 방 안에서의 Index번호 타입
		{
			User,		// 사용자가 사용중
			Empty,		// 비어있음
			Close,		// 사용할 수 없음
		};

	private:
		enum
		{
			MaxUser	= 16,
		};

		Common::DataStructure::CInnerObject<CRoom>
			m_InnerObjectForGame;
		Common::DataStructure::CInnerObject<CRoom>
			m_InnerObjectForChannel;

		CPocket*	m_pPocket;				// 사용자의 게임별 특화된 클래스

		Common::DataStructure::CInnerManager<CUser>
			m_InnerManagerUser;

		INT			m_nMaxUser;				// 방에 최대로 들어갈 수 있는 사용자 수 : 방 개설자가 지정한다.

		INT			m_nRoomIndex;			// 현재 룸의 인덱스
		INT			m_nChannelIndex;		// 현재 룸의 채널 인덱스

		CUser*		m_pCaptine;				// 방장

		string		m_strRoomName;
		
		string		m_strPassword;			// 방 비밀번호
		BOOL		m_bPassword;			// 비밀번호 유무

		DWORD		m_dwRTT;

		char		m_caOption[CCasualCommon::CasualStringLength::RoomOption];
											// 8바이트 옵션
		
		InnerIndexType	m_aUserIndex[ MaxUser ];
//			INT				m_nUsingIndex;

	public:
		VOID		SetRTT( DWORD dwRTT )
		{
			m_dwRTT = dwRTT;
		}
		DWORD		GetRTT()
		{
			return m_dwRTT;
		}
		void		Init	( INT nChannelIndex );

		void		Open	( INT nMaxUser, LPCTSTR strRoomName );
		void		Open	( INT nMaxUser, LPCTSTR strRoomName, LPCTSTR strPassword );

		// 방에 입장을 한다.
		BOOL		Join	( CUser* pUser );

		// 방에서 나간다. : nIndex(룸안에서의 자신의 번호)
		BOOL		Leave	( CUser* pUser );


		//
		// Get/Set Methods
		//

		// Inner Index를 할당받는다.
		CHAR		AllocEmptyInnerIndex();

		// Inner Index를 제거한다.
		VOID		FreeEmptyInnerIndex( INT nIndex );

		// Inner Index를 사용못하게 한다.
		BOOL		SetToCloseInnerIndex( INT nIndex );

		// Inner Index를 사용하게 한다.
		VOID		ResetToCloseInnerIndex( INT nIndex );

		CPocket*	GetPocket()
		{
			return m_pPocket;
		}
		void		SetPocket( CPocket* pPocket )
		{
			m_pPocket = pPocket;
		}

		INT			GetChannelIndex()
		{
			return m_nChannelIndex;
		}

		// 방장을 가져온다.
		CUser*		GetCaptine()
		{
			return m_pCaptine;
		}
		VOID		SetCaptine( CUser* pCaptine )
		{
			m_pCaptine = pCaptine;
		}

		INT			GetRoomIndex()
		{
			return m_nRoomIndex;
		}
		void		SetRoomIndex( INT nRoomIndex )
		{
			m_nRoomIndex = nRoomIndex;
		}

		INT			GetMaxUser()
		{
			return m_nMaxUser;
		}
		void		SetMaxUser( INT nMaxUser )
		{
			m_nMaxUser = nMaxUser;
		}

		INT			GetNumOfUsers()
		{
			return m_InnerManagerUser.GetNumOfInnerObject();
		}
		
		string&		GetRoomName()
		{
			return m_strRoomName;
		}
		void		SetRoomName( LPCTSTR strRoomName )
		{
			m_strRoomName = strRoomName;
		}

		BOOL		IsPassword()
		{
			return m_bPassword;
		}
		void		SetPassword( BOOL bPassword, LPCTSTR strPassword )
		{
			m_bPassword = bPassword;
			if( bPassword )
				m_strPassword = strPassword;
		}
		string&		GetPassword()
		{
			return m_strPassword;
		}

		BOOL		IsCorrectPassword( LPCTSTR strPassword )
		{
			return m_strPassword.compare( strPassword ) == 0;
		}

		char		GetOption	( INT nIndex )
		{
			if( nIndex < 0 || nIndex >= CCasualCommon::CasualStringLength::RoomOption )
				return -1;

			return m_caOption[nIndex];
		}
		void		SetOption	( INT nIndex, char cOption )
		{
			if( nIndex < 0 || nIndex >= CCasualCommon::CasualStringLength::RoomOption )
				return;

			m_caOption[nIndex] = cOption;
		}

		const char*	GetOption()
		{
			return m_caOption;
		}
		void		SetOption( const char* caOption )
		{
			::memcpy( m_caOption, caOption, CCasualCommon::CasualStringLength::RoomOption );
		}

		// 룸이 비어있는지를 검사한다.
		BOOL		IsEmpty()
		{
			return m_InnerManagerUser.IsEmpty();
		}

		Common::DataStructure::CInnerObject<CRoom>*
			GetInnerObjectForGame()
		{
			return &m_InnerObjectForGame;
		}
		Common::DataStructure::CInnerObject<CRoom>*
			GetInnerObjectForChannel()
		{
			return &m_InnerObjectForChannel;
		}

		// 방의 첫번째 클라이언트 서비서를 가져온다.
		CUser*	GetFirstUser();

		// 방의 다음 클라이언트 서비서를 가져온다.
		CUser*	GetNextUser();
	};// CRoom

}// namespace CasualGame
