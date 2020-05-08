#pragma once

#include "../../System.h"

namespace GameServer {
class IRoom;
class IPlayer;
} /* GameServer */

namespace GameServer {		namespace Room {

class IProcess	//IRoom操作结果的处理：发送网络结果、log文件记录
:	public					Singleton<IProcess>
{
public:
	virtual					~IProcess() {}

	virtual void			CastCreate( IRoom * i_pRoom ) = 0;
	virtual void			CastDestroy( IRoom * i_pRoom ) = 0;
	virtual void			ChangeCaptain( IRoom * i_pRoom ) = 0;
	virtual void			CastEnter( IPlayer * i_pPlayer, IRoom * i_pRoom ) = 0;
	virtual void			CastLeave( IPlayer * i_pPlayer, IRoom * i_pRoom ) = 0;
	virtual void			CastStateForReturn( IPlayer * i_pPlayer, IRoom * i_pRoom ) = 0;
	virtual void			CastStateForPlay( IPlayer * i_pPlayer, IRoom * i_pRoom ) = 0;
	virtual CBOOL			InspactFinish( IRoom * i_pRoom ) = 0;
	virtual void			FinishGame( IRoom * i_pRoom ) = 0;
	virtual void			FinishRound( IRoom * i_pRoom ) = 0;
	virtual void			CancelRound( IRoom * i_pRoom ) = 0;

};
static inline				IProcess *
IPROCESS()					{ return IProcess::GetInstance(); }

} /* Room */				} /* GameServer */