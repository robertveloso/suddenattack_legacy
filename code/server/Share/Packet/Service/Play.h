#pragma once

#ifdef _SERVER

	#include "../../../Jolyo/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Character.h"
	#include "../../LogicObject/Player/Result.h"
	#include "../../LogicObject/Player/Stock.h"
	#include "../../LogicObject/Room/Option.h"

#elif _TESTCLIENT

	#include "../../../../SAClient/Network/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Character.h"
	#include "../../LogicObject/Player/Result.h"
	#include "../../LogicObject/Player/Stock.h"
	#include "../../LogicObject/Room/Option.h"

#else

	#include "../../Lib_Network/NJean/Application/Packet.h"
	#include "../../Lib_LogicObject/Player/Type.h"
	#include "../../Lib_LogicObject/Player/Character.h"
	#include "../../Lib_LogicObject/Player/Result.h"
	#include "../../Lib_LogicObject/Player/Stock.h"
	#include "../../Lib_LogicObject/Room/Option.h"

#endif

namespace Packet {			namespace Service {			namespace Play {

enum						{ s_nPlayCode = 400 };

struct NN_Ready
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 0 };

	int						m_nIndex;
	bool					m_bReady;
};

struct NN_Inventory
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 1 };

	int						m_nIndex;
	bool					m_bInventory;
};

struct SN_State
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 2 };

	int						m_nIndex;
	unsigned char			m_byState;
	unsigned char			m_byGrade;
};

struct CN_Start
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 3 };
};

struct SN_StartFail
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 4 };

	struct E_RESULT {		enum ENUM {
		ALONE				= 1,
		NO_HOST,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct SS_Start
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 5 };
};

struct SS_StartFail
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 6 };
};

struct SN_PlayReady
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 7 };
};

struct SN_Host
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 8 };
};

struct CN_Host
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 9 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	unsigned short			m_nPort;
};

struct CN_Cancel
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 10 };
};

struct CN_ReturnBuff
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 11 };
};

struct SN_ReturnBuff
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 12 };

	typedef					::LogicObject::Room::Option
							OPTION;

	unsigned char			m_byPlayerCount;
	unsigned char			m_byTeam;
	OPTION					m_stOption;
};

//070810 add
struct CQ_ViewLobby
	:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 13 };
};

struct SA_ViewLobby
	:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 14 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	short					m_nCount;
};

struct SN_LobbyPlayer
	:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 15 };

	typedef					::LogicObject::Player::Type
		TYPE;

	void					SetNick( const char * i_strNick )
	{
		int i;
		for( i = 0; i < TYPE::s_nUserNick; i++ )
		{
			m_strNick[ i ]	= i_strNick[ i ];
			if( m_strNick[ i ] == '\0' )
			{
				break;
			}
		}
		m_strNick[ TYPE::s_nUserNick - 1 ] = '\0';

		this->SetLength( sizeof( SN_LobbyPlayer ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	unsigned char			m_byGrade;
	char					m_strNick[ TYPE::s_nUserNick ];

};

struct CN_Invite
	:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 16 };

	typedef					::LogicObject::Player::Type
		TYPE;

	void					SetNick( const char * i_strNick )
	{
		int i;
		for( i = 0; i < TYPE::s_nUserNick; i++ )
		{
			m_strNick[ i ]	= i_strNick[ i ];
			if( m_strNick[ i ] == '\0' )
			{
				break;
			}
		}
		m_strNick[ TYPE::s_nUserNick - 1 ] = '\0';

		this->SetLength( sizeof( CN_Invite ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	char					m_strNick[ TYPE::s_nUserNick ];

};

struct SN_Invite
	:	public					CPacket
{
	enum					{ s_nCode = s_nPlayCode + 17 };

	typedef					::LogicObject::Player::Type
		TYPE;

	void					Set( const char * i_strNick, const char * i_strPass )
	{
		int i;
		for( i = 0; i < TYPE::s_nUserNick; i++ )
		{
			m_strBuff[ i ]	= i_strNick[ i ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_byNickLength		= i + 1;

		for( i = m_byNickLength; i < 41; i++ )
		{
			m_strBuff[ i ]= i_strPass[ i - m_byNickLength ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_strBuff[ i ] = '\0';

		this->SetLength( sizeof( SN_Invite ) - ( TYPE::s_nUserNick + 41 - 1 - i ) );
	}
	const char *			GetNick() { return m_strBuff; }
	const char *			GetPass() { return & m_strBuff[ m_byNickLength ]; }

	int						m_nRoomIndex;

private:
	unsigned char			m_byNickLength;
	char					m_strBuff[ TYPE::s_nUserNick + 41 ];
};


} /* Play */				} /* Service */				} /* Packet */