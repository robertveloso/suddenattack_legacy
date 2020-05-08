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

namespace Packet {			namespace Service {			namespace Channel {

enum						{ s_nChannelCode = 200 };

struct SN_Info
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 0 };

	unsigned char			m_byChannelCount;
};

struct SN_Enter
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 1 };

	unsigned char			m_byIndex;
	unsigned short			m_nMaxUser;
	unsigned short			m_nCurrentUser;
};

struct CQ_Join
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 2 };

	unsigned char			m_byChannelIndex;
};

struct SA_Join
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 3 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		WRONG_INDEX,
		FULL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	short					m_nRoomCount;
	short					m_nUserCount;
};

struct CN_Away
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 4 };
};

struct CN_Refresh
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 5 };
};

struct CQ_QuickJoin
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 6 };

	unsigned char			m_byChannelIndex;

};

struct SA_QuickJoin
:	public					CPacket
{
	enum					{ s_nCode = s_nChannelCode + 7 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_WRONG_CHANNEL,
		FAIL_CHANNEL_FULL,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;

	unsigned char			m_byChannelIndex;
	short					m_nRoomCount;
	short					m_nUserCount;
};

} /* Channel */				} /* Service */				} /* Packet */