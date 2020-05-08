#pragma once

#ifdef _SERVER

	#include "../../Jolyo/NJean/Application/Packet.h"
	#include "../LogicObject/Player/Type.h"
	#include "../LogicObject/Player/Result.h"
	#include "../LogicObject/Player/Stock.h"

#elif _TESTCLIENT

	#include "../../../SAClient/Network/NJean/Application/Packet.h"
	#include "../LogicObject/Player/Type.h"
	#include "../LogicObject/Player/Result.h"
	#include "../LogicObject/Player/Stock.h"

#elif _MANAGERCLIENT

	#include "../../../SA_Manager/Network/NJean/Application/Packet.h"
	#include "../LogicObject/Player/Type.h"
	#include "../LogicObject/Player/Result.h"
	#include "../LogicObject/Player/Stock.h"

#else

	#include "../Lib_Network/NJean/Application/Packet.h"
	#include "../Lib_LogicObject/Player/Type.h"
	#include "../Lib_LogicObject/Player/Result.h"
	#include "../Lib_LogicObject/Player/Stock.h"

#endif

namespace Packet {			namespace Manager {

enum						{ s_nManagerCode	= 1000 };

struct CN_Noti
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 0, };

	struct E_TYPE {			enum ENUM {
		ALL					= 0,
		LOBBY,
		BUFF,
		GAME,
		PERSON,
	}; /* ENUM */			}; /* E_TYPE */

	typedef					::LogicObject::Player::Type
							TYPE;

	void					SetNoti( const char * i_strNoti )
	{
		int i;
		for( i = 0; i < 200 + 1; i++ )
		{
			m_strNoti[ i ]	= i_strNoti[ i ];
			if( m_strNoti[ i ] == '\0' )
			{
				break;
			}
		}
		m_strNoti[ 200 ] = '\0';

		this->SetLength( sizeof( CN_Noti ) - ( 200 - i ) );
	}
	void					SetNick( const char * i_strNick )
	{
		int i;
		for( i = 0; i < TYPE::s_nUserNick + 1; i++ )
		{
			m_strNick[ i ]	= i_strNick[ i ];
			if( m_strNick[ i ] == '\0' )
			{
				break;
			}
		}
		m_strNick[ TYPE::s_nUserNick ] = '\0';
	}

	unsigned char			m_byType;
	char					m_strNick[ TYPE::s_nUserNick + 1 ];
	char					m_strNoti[ 200  + 1 ];

};

struct SN_Notice
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 1, };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_NONE_USER,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct SN_Noti
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 2, };

	void					SetNoti( const char * i_strNoti )
	{
		int i;
		for( i = 0; i < 200 + 1; i++ )
		{
			m_strNoti[ i ]	= i_strNoti[ i ];
			if( m_strNoti[ i ] == '\0' )
			{
				break;
			}
		}
		m_strNoti[ 200 ] = '\0';

		this->SetLength( sizeof( SN_Noti ) - ( 200 - i ) );
	}

	const char *			GetNoti()
	{
		return m_strNoti;
	}

private:
	char					m_strNoti[ 200  + 1 ];
};

struct CQ_Kick
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 3, };

	struct E_TYPE {			enum ENUM {
		Nick				= 0,
		UserNo,
	}; /* ENUM */			}; /* E_TYPE */

	void					SetName( const char * i_strName )
	{
		int i;
		for( i = 0; i < 256 + 1; i++ )
		{
			m_strName[ i ]	= i_strName[ i ];
			if( m_strName[ i ] == '\0' )
			{
				break;
			}
		}
		m_strName[ 256 ] = '\0';

		this->SetLength( sizeof( CQ_Kick ) - ( 256 - i ) );
	}

	unsigned char			m_byType;
	char					m_strName[ 256 + 1 ];
};

struct SA_Kick
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 4, };

	bool					m_bSuccess;
};

struct CQ_Finish
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 5, };

	void					SetNoti( const char * i_strNoti )
	{
		int i;
		for( i = 0; i < 200 + 1; i++ )
		{
			m_strNoti[ i ]	= i_strNoti[ i ];
			if( m_strNoti[ i ] == '\0' )
			{
				break;
			}
		}
		m_strNoti[ 200 ] = '\0';

		this->SetLength( sizeof( CQ_Finish ) - ( 200 - i ) );
	}

	unsigned char			m_byMin;
	char					m_strNoti[ 200 + 1 ];
};

struct SA_Finish
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 6, };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_REGIST,
		FAIL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct CQ_Login
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 7, };

	void					Set( const char * i_strID, const char * i_strPass )
	{
		int i;
		for( i = 0; i < 20 + 1; i++ )
		{
			m_strID[ i ]	= i_strID[ i ];
			if( m_strID[ i ] == '\0' )
			{
				break;
			}
		}
		m_strID[ 20 ] = '\0';

		for( i = 0; i < 20 + 1; i++ )
		{
			m_strPass[ i ]	= i_strPass[ i ];
			if( m_strPass[ i ] == '\0' )
			{
				break;
			}
		}
		m_strPass[ 20 ] = '\0';
	}

	char					m_strID[ 21 ];
	char					m_strPass[ 21 ];
};

struct SA_Login
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 8, };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_ID,
		FAIL_PASS,
		FAIL_IP,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct CN_Monitor
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 9, };

	struct E_TYPE {			enum ENUM {
		CONNECTIONS			= 0,
		OPEN_ROOMS,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byType;
};

struct SN_MonitorConnections
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 10, };

	unsigned char			m_byChannelIndex;
	unsigned short			m_nConnections;
};

struct SN_MonitorOpenRooms
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 11, };

	unsigned char			m_byChannelIndex;
	unsigned short			m_nRooms;
};

struct CQ_EventLog
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 12, };

	bool					m_bLowQuery;
	bool					m_bNetwork;
	bool					m_bQuery;
	bool					m_bState;
	bool					m_bGame;
};

struct SA_EventLog
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 13, };

	bool					m_bLowQuery;
	bool					m_bNetwork;
	bool					m_bQuery;
	bool					m_bState;
	bool					m_bGame;
};

struct CQ_ResetEventLog
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 14, };
};

struct SA_ResetEventLog
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 15, };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct CQ_Reload
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 16, };

	struct E_TYPE {			enum ENUM {
		RESULT_TABLE		= 0,
		CONFIG,
	}; /* ENUM */			}; /* E_TYPE */

	unsigned char			m_byType;
};

struct SA_Reload
:	public					CPacket
{
	enum					{ s_nCode = s_nManagerCode + 17, };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};


} /* Manager */				} /* Packet */
