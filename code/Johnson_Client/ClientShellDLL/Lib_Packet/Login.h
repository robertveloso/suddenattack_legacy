#pragma once

#ifdef _SERVER

	#include "../../Jolyo/NJean/Application/Packet.h"
	#include "../LogicObject/Player/Type.h"
	#include "../LogicObject/Player/character.h"
	#include "../LogicObject/Player/Result.h"
	#include "../LogicObject/Player/Stock.h"

#elif _TESTCLIENT

	#include "../../../SAClient/Network/NJean/Application/Packet.h"
	#include "../LogicObject/Player/Type.h"
	#include "../LogicObject/Player/character.h"
	#include "../LogicObject/Player/Result.h"
	#include "../LogicObject/Player/Stock.h"

#else

	#include "../Lib_Network/NJean/Application/Packet.h"
	#include "../Lib_LogicObject/Player/Type.h"
	#include "../Lib_LogicObject/Player/character.h"
	#include "../Lib_LogicObject/Player/Result.h"
	#include "../Lib_LogicObject/Player/Stock.h"

#endif

namespace Packet {			namespace Login {

enum						{ s_nLoginCode	= 100 };

struct CQ_Login
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 0, };

	typedef					::LogicObject::Player::Type
							TYPE;

	void					Set( const char * i_strIP, const char * i_strUserNo, const char * i_strMemo )
	{
		int i;
		for( i = 0; i < TYPE::s_nIP; i++ )
		{
			m_strIP[ i ]	= i_strIP[ i ];
			if( m_strIP[ i ] == '\0' )
			{
				break;
			}
		}
		m_strIP[ TYPE::s_nIP - 1 ] = '\0';

		for( i = 0; i < TYPE::s_nUserNo; i++ )
		{
			m_strUserNo[ i ]= i_strUserNo[ i ];
			if( m_strUserNo[ i ] == '\0' )
			{
				break;
			}
		}
		m_strUserNo[ TYPE::s_nUserNo - 1 ] = '\0';

		for( i = 0; i < 600 + 1; i++ )
		{
			m_strMemo[ i ]= i_strMemo[ i ];
			if( m_strMemo[ i ] == '\0' )
			{
				break;
			}
		}
		m_strMemo[ 600 ] = '\0';

		this->SetLength( sizeof( CQ_Login ) - ( 601 - 1 - i ) );
	}

	const char *			GetIP() { return m_strIP; }
	const char *			GetUserNo() { return m_strUserNo; }
	const char *			GetMemo() { return m_strMemo; }

private:
	char					m_strIP[ TYPE::s_nIP ];
	char					m_strUserNo[ TYPE::s_nUserNo ];
	char					m_strMemo[ 601 ];

};

struct SA_Login
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 1, };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,

		FAIL_LOGINED,
		FAIL_LOGINED_DB,
		FAIL_WRONG_ID,
		FAIL_DB,
		FAIL_ADO,
		FAIL_QUERY,
		FAIL_DB_ERROR,

		FAIL_NETMARBLE_TIMEOUT,		// 로그인 시간이 만료되었습니다. 넷마블 웹페이지에서 다시 로그인해주시기 바랍니다
		FAIL_NETMARBLE_DEC,			// 넷마블 복호화 인증 실패
		FAIL_NETMARBLE_WRONG_IP,	// 넷마블 아이피 실패

		FAIL_BAD_PLAYER,			// 불량 사용자

	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	int						m_nIndex;

	char					m_strText[ 201 ];

	void					Set( const char * i_strText )
	{
		int i;
		for( i = 0; i < 201; i++ )
		{
			m_strText[ i ]= i_strText[ i ];
			if( m_strText[ i ] == '\0' )
			{
				break;
			}
		}
		m_strText[ 201 - 1 ] = '\0';

		this->SetLength( sizeof( SA_Login ) - ( 201 - 1 - i ) );
	}
};

struct SN_Nick
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 2, };

	struct E_RESULT {		enum ENUM {
		FAIL_ADO			= 1,
		FAIL_DB1,
		FAIL_DB2,
		FAIL_DB3,
		FAIL_LOGINED,
		EMPTY,
		EXIST,
		BAD_NAME,
		RESET,				// 특정 이유로 초기화
		
	}; /* ENUM */			}; /* E_RESULT */

	typedef					::LogicObject::Player::Type
							TYPE;

	unsigned char			m_byResult;
	int						m_nIndex;

	//char					m_strText[ 201 ];

	//void					Set( const char * i_strText )
	//{
	//	int i;
	//	for( i = 0; i < 201; i++ )
	//	{
	//		m_strText[ i ]= i_strText[ i ];
	//		if( m_strText[ i ] == '\0' )
	//		{
	//			break;
	//		}
	//	}
	//	m_strText[ 201 - 1 ] = '\0';

	//	this->SetLength( sizeof( SN_Nick ) - ( 201 - 1 - i ) );
	//}


	void					Set( const char * strNick, const char * strText )
	{
		int i;
		for( i = 0; i < TYPE::s_nUserNick; i++ )
		{
			m_strBuff[ i ]	= strNick[ i ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_byNickLen		= i + 1;

		for( i = m_byNickLen; i < 201; i++ )
		{
			m_strBuff[ i ]	= strText[ i - m_byNickLen ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_strBuff[ i ] = '\0';

		this->SetLength( sizeof( SN_Nick ) - ( TYPE::s_nUserNick + 201 - 1 - i ) );
	}
	const char *			GetNick()	{ return m_strBuff; }
	const char *			GetText()	{ return & m_strBuff[ m_byNickLen ]; }

private:
	unsigned char			m_byNickLen;
	char					m_strBuff[ TYPE::s_nUserNick + 201 ];
};

struct CN_Nick
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 3, };

	typedef					::LogicObject::Player::Type
							TYPE;

	void					Set( const char * i_strNick )
	{
		int i;
		for( i = 0; i < TYPE::s_nUserNick; i++ )
		{
			m_strNick[ i ]= i_strNick[ i ];
			if( m_strNick[ i ] == '\0' )
			{
				break;
			}
		}
		m_strNick[ TYPE::s_nUserNick - 1 ] = '\0';

		this->SetLength( sizeof( CN_Nick ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	const char *			GetNick() { return m_strNick; }

private:
	char					m_strNick[ TYPE::s_nUserNick ];
};

struct SN_Character
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 4, };

	struct E_RESULT {		enum ENUM {
		FAIL_ADO			= 1,
		FAIL_DB1,
		FAIL_DB2,
		FAIL_DB3,
		EMPTY,
		
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	int						m_nIndex;
};

struct CN_Character
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 5, };

	typedef					::LogicObject::Player::Character
							CHARACTER;

	CHARACTER				stRed;
	CHARACTER				stBlue;
};

struct SN_MyProperty
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 6, };
	
	typedef					::LogicObject::Player::Character
							CHARACTER;
	typedef					::LogicObject::Player::Stock
							STOCK;
	typedef					::LogicObject::Player::Result
							RESULT;

	void					Set( const char * strServerName, const char * strID )
	{
		int i;
		for( i = 0; i < 21; i++ )
		{
			m_strBuff[ i ]	= strServerName[ i ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_byServerLen		= i + 1;

		for( i = m_byServerLen; i < 61; i++ )
		{
			m_strBuff[ i ]	= strID[ i - m_byServerLen ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_strBuff[ i ] = '\0';

		this->SetLength( sizeof( SN_MyProperty ) - ( 21 + 41 - 1 - i ) );
	}
	const char *			GetServerName()	{ return m_strBuff; }
	const char *			GetID()			{ return & m_strBuff[ m_byServerLen ]; }

	CHARACTER				m_stRed;
	CHARACTER				m_stBlue;
	RESULT					m_stResult;
	STOCK					m_stStock;
	unsigned char			m_byAge;
	unsigned char			m_bRealName;

private:
	unsigned char			m_byServerLen;
	char					m_strBuff[ 21 + 41 ];
};

struct SN_GameResultFail
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 7, };

	struct E_RESULT {		enum ENUM {
		FAIL_ADO			= 0,
		FAIL_DB1,
		FAIL_DB2,
		FAIL_DB3,
		FAIL_DB4,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};


struct CQ_LoginNetmarble
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 8, };

	typedef					::LogicObject::Player::Type
							TYPE;

	void					Set( const char * i_strIP, const char * i_strCpCookie, const char * i_strMemo )
	{
		int i;
		for( i = 0; i < TYPE::s_nIP; i++ )
		{
			m_strIP[ i ]	= i_strIP[ i ];
			if( m_strIP[ i ] == '\0' )
			{
				break;
			}
		}
		m_strIP[ TYPE::s_nIP - 1 ] = '\0';

		for( i = 0; i < 1024 * 2; i++ )
		{
			m_strBuff[ i ]	= i_strCpCookie[ i ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_byCpCookie		= i + 1;

		for( i = m_byCpCookie; i < 601; i++ )
		{
			m_strBuff[ i ]= i_strMemo[ i - m_byCpCookie ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_strBuff[ i ] = '\0';

		this->SetLength( sizeof( CQ_LoginNetmarble ) - ( 2048 + 601 - 1 - i ) );
	}

	const char *			GetIP() { return m_strIP; }
	const char *			GetCpCookie() { return m_strBuff; }
	const char *			GetMemo() { return & m_strBuff[ m_byCpCookie ]; }

private:
	char					m_strIP[ TYPE::s_nIP ];
	unsigned char			m_byCpCookie;
	char					m_strBuff[ 1024 * 2 + 601 ];
};

struct SN_Ranking
:	public					CPacket
{
	enum					{ s_nCode = s_nLoginCode + 9, };

	void					SetText( const char * i_strText )
	{
		int i;
		for( i = 0; i < 257; i++ )
		{
			m_strText[ i ]	= i_strText[ i ];
			if( m_strText[ i ] == '\0' )
			{
				break;
			}
		}
		m_strText[ i ] = '\0';

		this->SetLength( sizeof( SN_Ranking ) - ( 257 - 1 - i ) );
	}

	char					m_strText[ 257 ];

};

} /* Login */				} /* Packet */
