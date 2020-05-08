#pragma once

#ifdef _SERVER

	#include "../../../Jolyo/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Room/Type.h"
	#include "../../LogicObject/Room/Option.h"

#elif _TESTCLIENT

	#include "../../../../SAClient/Network/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Room/Type.h"
	#include "../../LogicObject/Room/Option.h"

#else

	#include "../../Lib_Network/NJean/Application/Packet.h"
	#include "../../Lib_LogicObject/Player/Type.h"
	#include "../../Lib_LogicObject/Room/Type.h"
	#include "../../Lib_LogicObject/Room/Option.h"

#endif

namespace Packet {			namespace Service {			namespace Room {

enum						{ s_nRoomCode = 600 };

struct SN_Enter
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 0 };

	typedef					::LogicObject::Room::Option
							OPTION;
	typedef					::LogicObject::Room::Type
							RTYPE;
	typedef					::LogicObject::Player::Type
							PTYPE;

	void					Set( const char * i_strNick, const char * i_strName )
	{
		int i;
		for( i = 0; i < PTYPE::s_nUserNick; i++ )
		{
			m_strBuff[ i ]	= i_strNick[ i ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_byNickLength		= i + 1;

		for( i = m_byNickLength; i < RTYPE::s_nName; i++ )
		{
			m_strBuff[ i ]= i_strName[ i - m_byNickLength ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_strBuff[ i ] = '\0';

		this->SetLength( sizeof( SN_Enter ) - ( PTYPE::s_nUserNick + RTYPE::s_nName - 1 - i ) );
	}
	const char *			GetNick() { return m_strBuff; }
	const char *			GetName() { return & m_strBuff[ m_byNickLength ]; }

	short					m_nRoomIndex;						// Room의 번호
	unsigned char			m_byUsers;							// 현재 입장된 사용자의 수
//	unsigned int			m_dwRTT;							// 방장의 RTT

	OPTION					m_stOption;

	unsigned char			m_byNetLoss;
	
private:
	unsigned char			m_byNickLength;
	char					m_strBuff[ PTYPE::s_nUserNick + RTYPE::s_nName ];

//	char					m_strName[ RTYPE::s_nName ];		// 방 이름
//	char					m_strNick[ PTYPE::s_nUserNick ];
};

struct SN_Leave
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 1 };

	short					m_nRoomIndex;		// Room의 번호

};

struct SN_AddUser
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 2 };

	short					m_nRoomIndex;						// Room의 번호
};

struct SN_RemoveUser
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 3 };

	short					m_nRoomIndex;						// Room의 번호
};

struct SN_Play
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 4 };

	short					m_nRoomIndex;						// Room의 번호
};

struct SN_Buff
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 5 };

	short					m_nRoomIndex;						// Room의 번호
};

struct CQ_Join
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 6 };
	typedef					::LogicObject::Room::Type
							TYPE;

	unsigned short			m_nRoomIndex;
	char					m_strPass[ TYPE::s_nPass + 1 ];
};

struct SA_Join
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 7 };

	typedef					::LogicObject::Room::Option
							OPTION;

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		NOT_EXIST,
		FULL,
		WRONG_PASS,
		PLAYING,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	
	unsigned short			m_nRoomIndex;
	unsigned char			m_byPlayerCount;
	unsigned char			m_byTeam;
	OPTION					m_stOption;
};

struct CQ_QuickJoin
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 8 };
};

struct SA_QuickJoin
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 9 };

	typedef					::LogicObject::Room::Option
							OPTION;

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FULL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	
	unsigned short			m_nRoomIndex;
	unsigned char			m_byPlayerCount;
	unsigned char			m_byTeam;
	OPTION					m_stOption;
};

struct CQ_Create
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 10 };

	typedef					::LogicObject::Room::Option
							OPTION;
	typedef					::LogicObject::Room::Type
							TYPE;

	char					m_strPass[ TYPE::s_nPass ];
	char					m_strName[ TYPE::s_nName ];
	OPTION					m_stOption;
};

struct SA_Create
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 11 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	unsigned char			m_byTeam;
	int						m_nIndex;
};

struct CQ_Away
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 12 };
};

struct SA_Away
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 13 };

	short					m_nRoomCount;
	short					m_nUserCount;
};

struct NN_ChangeOption
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 14 };

	typedef					::LogicObject::Room::Option
							OPTION;
	typedef					::LogicObject::Room::Type	
							TYPE;

	int						m_nRoomIndex;
	OPTION					m_stOption;
	char					m_strPass[ TYPE::s_nPass ];

	void					Optimize()
	{
		if( m_stOption.IsPassword() == false )
		{
			this->SetLength( sizeof( NN_ChangeOption ) - ( TYPE::s_nPass ) );
		}
	}
};

struct SN_ChangeOption
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 15 };

	typedef					::LogicObject::Room::Option
							OPTION;

	int						m_nRoomIndex;
	OPTION					m_stOption;
};

struct SN_ChangeOptionFail
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 16 };

	struct E_RESULT {		enum ENUM {
		FAIL_TEAMMAX		= 1,
		FAIL_MODE,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct NN_ChangeCaptain
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 17 };

	int						m_nCaptainIndex;
};

struct SN_ChangeCaptain
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 18 };

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

		this->SetLength( sizeof( SN_ChangeCaptain ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	int						m_nRoomIndex;
	char					m_strNick[ TYPE::s_nUserNick ];
};

//struct SN_ChangeCaptainFail
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nRoomCode + 19 };
//
//	struct E_RESULT {		enum ENUM {
//		NOT_EXIT			= 1,
//	}; /* ENUM */			}; /* E_RESULT */
//
//	unsigned char			m_byResult;
//};

struct NN_Chat
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 20 };

	void					Set( const char * i_strChatMessage )
	{
		int i;
		for( i = 0; i < 81; i++ )
		{
			strChatMessage[ i ]	= i_strChatMessage[ i ];
			if( strChatMessage[ i ] == '\0' )
			{
				break;
			}
		}
		strChatMessage[ 81 - 1 ] = '\0';

		this->SetLength( sizeof( NN_Chat ) - ( 81 - 1 - i ) );
	}

	const char *			GetMessage() { return strChatMessage; }

private:
	char					strChatMessage[ 81 ];
};

struct SS_Init
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 21 };
};

struct SN_JoinPlayFail
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 22 };
};

struct CQ_ChangeCaptain
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 23 };

	int						m_nPlayerIndex;
};

struct SA_ChangeCaptain
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 24 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_NOT_CAPTAIN,						// 방장이 아닌자가 해당 패킷을 보낸 경우
		FAIL_NOT_EXIST,							// 해당 인덱스가 방에 존재하지 않은 경우
		FAIL_CAPTAIN_INDEX,						// 해당 인덱스가 본인의 인덱스인 경우
	}; /* ENUM */			}; /* E_RESULT */
	
	unsigned char			m_byResult;
};

struct NN_ChangeTitle
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 25 };

	typedef					::LogicObject::Room::Type
							TYPE;

	void					Set( const char * i_strTitle )
	{
		int i;
		for( i = 0; i < TYPE::s_nName; i++ )
		{
			m_strTitle[ i ]	= i_strTitle[ i ];
			if( m_strTitle[ i ] == '\0' )
			{
				break;
			}
		}
		m_strTitle[ TYPE::s_nName - 1 ] = '\0';

		this->SetLength( sizeof( NN_ChangeTitle ) - ( TYPE::s_nName - 1 - i ) );
	}

	char					m_strTitle[ TYPE::s_nName ];
};

struct SN_ChangeTitle
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 26 };

	typedef					::LogicObject::Room::Type
							TYPE;

	void					Set( const char * i_strTitle )
	{
		int i;
		for( i = 0; i < TYPE::s_nName; i++ )
		{
			m_strTitle[ i ]	= i_strTitle[ i ];
			if( m_strTitle[ i ] == '\0' )
			{
				break;
			}
		}
		m_strTitle[ TYPE::s_nName - 1 ] = '\0';

		this->SetLength( sizeof( SN_ChangeTitle ) - ( TYPE::s_nName - 1 - i ) );
	}

	int						m_nRoomIndex;
	char					m_strTitle[ TYPE::s_nName ];
};

struct CQ_Info
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 27 };
};

struct SA_Info
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 28 };

	bool					m_bPlanted;
	unsigned char			m_byPlayerCount;
};

struct SN_PlayerInfo
:	public					CPacket
{
	enum					{ s_nCode = s_nRoomCode + 29 };

	typedef					::LogicObject::Player::Type
							TYPE;

	unsigned char			m_byTeam;
	bool					m_bBuff;
	bool					m_bLoading;
	bool					m_bDead;
	bool					m_bSpectator;

	char					m_strNick[ TYPE::s_nUserNick ];

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

		this->SetLength( sizeof( SN_PlayerInfo ) - ( TYPE::s_nUserNick - 1 - i ) );
	}
};

} /* Room */				} /* Service */				} /* Packet */
