#pragma once

#ifdef _SERVER

	#include "../../../Jolyo/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Character.h"
	#include "../../LogicObject/Player/Result.h"
	#include "../../LogicObject/Player/Stock.h"

#elif _TESTCLIENT

	#include "../../../../SAClient/Network/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Character.h"
	#include "../../LogicObject/Player/Result.h"
	#include "../../LogicObject/Player/Stock.h"

#else

	#include "../../Lib_Network/NJean/Application/Packet.h"
	#include "../../Lib_LogicObject/Player/Type.h"
	#include "../../Lib_LogicObject/Player/Character.h"
	#include "../../Lib_LogicObject/Player/Result.h"
	#include "../../Lib_LogicObject/Player/Stock.h"

#endif

namespace Packet {			namespace Service {			namespace Player {

enum						{ s_nPlayerCode = 500 };

struct SN_Enter
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 0 };

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

		this->SetLength( sizeof( SN_Enter ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	int						m_nIndex;
	unsigned char			m_byGrade;
	unsigned char			m_byTeam;
	unsigned char			m_byState;
	bool					m_bCaptain;

	char					m_strNick[ TYPE::s_nUserNick ];

};

struct SN_Leave
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 1 };

	int						m_nPlayerIndex;

};

struct SN_EnterLobby
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 2 };

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

		this->SetLength( sizeof( SN_EnterLobby ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	int						m_nIndex;
	unsigned char			m_byGrade;
	unsigned char			m_byTeam;
	unsigned char			m_byState;
	bool					m_bCaptain;
	char					m_strNick[ TYPE::s_nUserNick ];

};

struct SN_LeaveLobby
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 3 };

	int						m_nPlayerIndex;

};

struct NN_ChangeTeam
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 4 };

	int						m_nIndex;
	unsigned char			m_byTeam;
};

struct SN_ChangeTeamOnStart
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 5 };

	int						m_nIndex;
	unsigned char			m_byTeam;
};

struct SN_ChangeTeamOnGame
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 6 };

	typedef					::LogicObject::Player::Character
							CHARACTER;

	int						m_nIndex;
	unsigned char			m_byTeam;
	CHARACTER				m_stCharacter;
};

struct SS_ChangeTeam
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 7 };
};

struct SN_ChangeTeamFail
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 8 };

	struct E_RESULT {		enum ENUM {
		FAIL				= 1,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct CQ_Find
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 9 };

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

		this->SetLength( sizeof( CQ_Find ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	const char *			GetNick() { return m_strNick; }

private:
	char					m_strNick[ TYPE::s_nUserNick ];
};

struct SA_Find
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 10 };

	struct E_RESULT {		enum ENUM {
		FAIL				= 0,

		CHANNEL,
		LOBBY,
		ROOM,
		PLAY,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	unsigned char			m_byChannelIndex;
	short					m_nRoomIndex;

};

struct NN_Whisper
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 11 };

	typedef					::LogicObject::Player::Type
							TYPE;

	void					Set( const char * i_strNick, const char * i_strChat )
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
			m_strBuff[ i ]= i_strChat[ i - m_byNickLength ];
			if( m_strBuff[ i ] == '\0' )
			{
				break;
			}
		}
		m_strBuff[ i ] = '\0';

		this->SetLength( sizeof( NN_Whisper ) - ( TYPE::s_nUserNick + 41 - 1 - i ) );
	}
	const char *			GetNick() { return m_strBuff; }
	const char *			GetChat() { return & m_strBuff[ m_byNickLength ]; }

private:
	unsigned char			m_byNickLength;
	char					m_strBuff[ TYPE::s_nUserNick + 41 ];
};

struct SN_WhisperFail
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 12 };

	typedef					::LogicObject::Player::Type
							TYPE;

	struct E_RESULT {		enum ENUM {
		NOT_EXIST			= 0,
		INVENTORY,
	}; /* ENUM */			}; /* E_RESULT */

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

		this->SetLength( sizeof( SN_WhisperFail ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	unsigned char			m_byResult;
	char					m_strNick[ TYPE::s_nUserNick ];
};

struct CN_Out
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 13 };

	int						m_nIndex;
};

struct SS_Out
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 14 };
};

struct SN_Out
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 15 };

	short					m_nRoomCount;
	short					m_nUserCount;
};

struct CN_Info
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 16 };

	int						m_nIndex;
};

struct SN_Info
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 17 };

	typedef					::LogicObject::Player::Result
							RESULT;
	typedef					::LogicObject::Player::Character
							CHARACTER;

	int						m_nIndex;
	CHARACTER				m_stRed;
	CHARACTER				m_stBlue;
	RESULT					m_stResult;
};

struct SN_Result
:	public					CPacket
{
	enum					{ s_nCode = s_nPlayerCode + 18 };

	typedef					::LogicObject::Player::Result
							RESULT;

	unsigned char			m_byGrade;
	RESULT					m_stResult;
};

} /* Player */				} /* Service */				} /* Packet */