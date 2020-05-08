#pragma once

#ifdef _SERVER

	#include "../../../Jolyo/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Result.h"
	#include "../../LogicObject/Player/Stock.h"

#elif _TESTCLIENT

	#include "../../../../SAClient/Network/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Result.h"
	#include "../../LogicObject/Player/Stock.h"

#else

	#include "../../Lib_Network/NJean/Application/Packet.h"
	#include "../../Lib_LogicObject/Player/Type.h"
	#include "../../Lib_LogicObject/Player/Result.h"
	#include "../../Lib_LogicObject/Player/Stock.h"

#endif

namespace Packet {			namespace Service {			namespace Game {

enum						{ s_nGameCode = 300 };

struct SS_Play
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 0 };
};

struct SN_Play
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 1 };

	typedef					::LogicObject::Player::Type
							TYPE;

	void					SetIP( const char * i_strIP )
	{
		for( int i = 0; i < TYPE::s_nIP; i++ )
		{
			m_strIP[ i ]	= i_strIP[ i ];
			if( m_strIP[ i ] == '\0' )
			{
				break;
			}
		}
		m_strIP[ TYPE::s_nIP - 1 ] = '\0';
	}

	int						m_nHostIndex;
    bool					m_bSpectator;
	unsigned short			m_nPort;

	char					m_strIP[ TYPE::s_nIP ];
};

struct NN_Kill
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 2 };

	bool					m_bHeadShot;
	int						m_nKiller;
	int						m_nVictim;

	unsigned short			m_nModelID;
};

struct NN_Suicide
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 3 };

	int						m_nVictim;

	unsigned short			m_nModelID;
};

struct CN_Timeout
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 4 };
};

struct SN_ResultTotal
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 5 };

	unsigned char			m_byWinnerTeam;
	unsigned char			m_byResultCount;
};

struct SN_ResultPlayer
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 6 };

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

		this->SetLength( sizeof( SN_ResultPlayer ) - ( TYPE::s_nUserNick - 1 - i ) );
	}

	unsigned char			m_byTeam;
	unsigned short			m_nKill;
	unsigned short			m_nDead;
	short					m_nExp;
	short					m_nMoney;
	unsigned char			m_byGrade;
	unsigned char			m_byMission;

	char					m_strNick[ TYPE::s_nUserNick ];
};

struct SS_End
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 7 };
};

struct CN_Exit
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 8 };
};

struct SN_ExitFail
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 9 };
};

struct CN_LoadComplate
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 10 };
};

struct NN_Chat
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 11 };

	struct E_TYPE {			enum ENUM {
		ALL					= 0,
		TEAM,
		GHOST_ALL,
		GHOST_TEAM,
	}; /* ENUM */			}; /* E_TYPE */

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

	unsigned char			m_byType;

private:
	char					strChatMessage[ 81 ];
};

struct SN_Pause
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 12 };
};

struct SS_ChangeHost
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 13 };
};

struct CN_Bomb
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 14 };

	struct E_TYPE {			enum ENUM {
		DEFUSED				= 0,
		DETONATED,
	}; /* ENUM */			}; /* E_TYPE */

	unsigned char			m_byType;
	int						m_nPlayer;
};

struct SN_RoundResult
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 15 };

	struct E_TYPE {			enum ENUM {
		TIMEOUT					= 0,
		ALL_KILL,
		DEFUSED,
		DETONATED,
		CANCEL,				// Host 선정 실패로 판이 비긴 경우
	}; /* ENUM */			}; /* E_TYPE */

	unsigned char			m_byType;
	unsigned char			m_byWinnerTeam;

	unsigned char			m_byRestRound;
	unsigned char			m_byRedWin;
	unsigned char			m_byBlueWin;
};

struct CN_NextRound
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 16 };
};

struct SS_StartRound
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 17 };
};

struct SS_EndRound
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 18 };
};

struct CN_BombPlanted
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 22 };

	int						m_nPlanterIndex;

	unsigned int			m_nPlayerID;
	float					m_fDetonateTimer;
	float					m_fTimeBombPosX;
	float					m_fTimeBombPosY;
	float					m_fTimeBombPosZ;
};

struct SN_BombPlanted
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 23 };

	int						m_nPlanterIndex;

	unsigned int			m_nPlayerID;
	float					m_fDetonateTimer;
	float					m_fTimeBombPosX;
	float					m_fTimeBombPosY;
	float					m_fTimeBombPosZ;
};

struct SN_Bomb
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 24 };

	struct E_TYPE {			enum ENUM {
		DEFUSED				= 0,
		DETONATED,
	}; /* ENUM */			}; /* E_TYPE */

	unsigned char			m_byType;
};

struct NN_Radio
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 25 };

	unsigned short			m_nPlayerIndex;
	unsigned short			m_nRadioID;
	unsigned short			m_nTeamID;
};

struct NN_WeaponChange
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 26 };

	unsigned short			m_nPlayerIndex;
	unsigned int			m_nWeaponModel;
	unsigned short			m_nWeaponID;
	unsigned short			m_nAmmoCount;
	unsigned short			m_nAmmoInClip;
};

struct CN_NetLoss
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 27 };

	unsigned char			m_byNetLoss;
};

struct SN_NetLossPlayer
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 28 };

	int						m_nPlayerIndex;
	unsigned char			m_byNetLoss;
};

struct SN_NetLossRoom
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 29 };

	int						m_nRoomIndex;
	unsigned char			m_byNetLoss;
};

struct SN_Exit
	:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 30 };

	int						m_nPlayerIndex;
};

struct CN_Speed
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 31 };
};

struct SN_Speed
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 31 };

	int						m_nPlayerIndex;
};

struct SN_ServerTick
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 32 };

	unsigned int			m_dwTick;
};

struct CN_ServerTick
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 33 };
};

struct SS_Speed
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 34 };
};

struct SN_PlayerInfo
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 40 };

	enum					{ s_nNull = 255 };

	int						m_nIndex;
	unsigned short			m_nKill;
	unsigned short			m_nDead;

	unsigned char			m_byCount;
	unsigned char			m_byItemCode[ 12 ];

	void					Optimize()
	{
		char n;
		for( n = 11; n >= 0; n -- )
		{
			if( m_byItemCode[ n ] < s_nNull )
			{
				break;
			}
		}

		m_byCount			= n + 1;

		int nCodeSize		= sizeof( unsigned char ) * m_byCount;
		int nMaxSize		= sizeof( unsigned char ) * 12;
		this->SetLength( sizeof( SN_PlayerInfo ) - ( nMaxSize - nCodeSize ) );
	}

};

struct SN_RespawnInfo
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 41 };

	unsigned short			m_nRedKill;
	unsigned short			m_nRedDead;

	unsigned short			m_nBlueKill;
	unsigned short			m_nBlueDead;
};

struct SN_BombInfo
:	public					CPacket
{
	enum					{ s_nCode = s_nGameCode + 42 };

	unsigned char			m_byRound;

	unsigned char			m_byRedWin;
	unsigned char			m_byBlueWin;
};

} /* Channel */				} /* Service */				} /* Packet */