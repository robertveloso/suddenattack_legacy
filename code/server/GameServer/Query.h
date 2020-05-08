#pragma once

#include "../System.h"

namespace GameServer {		namespace Query {

struct Session
{
	struct E_CODE {			enum ENUM {
		REQ_LOGIN			= 100,
		REQ_LOGIN_NETMARBLE,
		REQ_NICK,
		REQ_CHARACTER,
		NOT_LOGOUT,

	}; /* ENUM */			}; /* E_CODE */
};

struct Result
{
	struct E_CODE {			enum ENUM {
		REQ_READ			= 200,
		REQ_RECORD,
		REQ_GAME,

	}; /* ENUM */			}; /* E_CODE */
};

struct Server
{
	struct E_CODE {			enum ENUM {
		REQ_START			= 300,
		REQ_LEVEL,
		NOT_CONNECTIONS,

		REQ_RANK,
	}; /* ENUM */			}; /* E_CODE */

	static CULONG			s_nLevelLength
							= 2000;
};

} /* Query */				} /* GameServer */