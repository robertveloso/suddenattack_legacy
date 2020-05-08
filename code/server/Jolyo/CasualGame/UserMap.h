#pragma once

#include <map>
#include <string>
#include "User.h"

namespace CasualGame
{
	class CUserMap //CUserµÄmutimap
	{
	public:
		CUserMap(){}
		~CUserMap(){}

	private:
		std::map <string, CUser*>			m_mapUserNo;
		std::multimap <string, CUser*>		m_mapUserID;
		std::multimap <string, CUser*>		m_mapUserNick;

		// id, nick map -> multi map
		std::map <string, CUser*>::iterator			m_IterUserNo;
		std::multimap <string, CUser*>::iterator	m_IterUserID;
		std::multimap <string, CUser*>::iterator	m_IterUserNick;

	public:
		CUser*		FindFromUserNo		( LPCTSTR strUserNo );
		CUser*		FindFromUserNo		( const string& strUserNo );
		CUser*		FindFromID			( LPCTSTR lpszID );
		CUser*		FindFromID			( const string& strID );
		CUser*		FindFromNick		( LPCTSTR lpszNick );
		CUser*		FindFromNick		( const string& strNick );

		void		Insert				( LPCTSTR lpszUserNo, LPCTSTR lpszID, LPCTSTR lpszNick, CUser* pUser );
		void		Insert				( const string& strUserNo, const string& strID, const string& strNick, CUser* pUser );
		void		EraseFromUserNo		( CUser* pUser );
		void		EraseFromID			( CUser* pUser );

	};// class CUserMap

}// namespace CasualGame
