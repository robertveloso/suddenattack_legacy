#pragma once

#include "../IProcess.h"
#include "../../../Share/LogicObject/Player/Type.h"

namespace GameServer {		namespace Room {			namespace Process {

class Provider
:	public					IProcess
{
public:
							Provider();
	virtual					~Provider();

	virtual void			CastCreate( IRoom * i_pRoom );
	virtual void			CastDestroy( IRoom * i_pRoom );
	virtual void			ChangeCaptain( IRoom * i_pRoom );
	virtual void			CastEnter( IPlayer * i_pPlayer, IRoom * i_pRoom );
	virtual void			CastLeave( IPlayer * i_pPlayer, IRoom * i_pRoom );
	virtual void			CastStateForReturn( IPlayer * i_pPlayer, IRoom * i_pRoom );
	virtual void			CastStateForPlay( IPlayer * i_pPlayer, IRoom * i_pRoom );
	virtual CBOOL			InspactFinish( IRoom * i_pRoom );
	virtual void			FinishGame( IRoom * i_pRoom );
	virtual void			FinishRound( IRoom * i_pRoom );
	virtual void			CancelRound( IRoom * i_pRoom );

private:
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	void					calculate( IRoom * i_pRoom );
	CBYTE					getGamePlayerCount( IRoom * i_pRoom );
	void					castFinishGame( IRoom * i_pRoom );
	void					castFinishRound( IRoom * i_pRoom );
};

} /* Process */				} /* Room */				} /* GameServer */