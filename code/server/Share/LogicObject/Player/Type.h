#pragma once

namespace LogicObject {		namespace Player {

struct Type
{
	struct E_TEAM {			enum ENUM {
		RED					= 0,
		BLUE,
		__MAX__,

		DRAW				= __MAX__,
		CANCEL
	}; /* ENUM */			}; /* E_TEAM */

	struct E_STATE {		enum ENUM {
		LOGIN				= 0,
		LOBBY,
		BUFF,
		RETURN_BUFF,
		READY,
		INVENTORY,
		PLAY_READY,
		PLAY,
	}; /* ENUM */			}; /* E_STATE */

	enum
	{
		s_nUserNo			= 12 + 1,
		s_nUserNick			= 40 + 1,
		s_nIP				= 16 + 1,
	};

};


} /* Player */				} /* LogicObject */