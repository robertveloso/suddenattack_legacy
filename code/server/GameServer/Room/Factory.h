#pragma once

#include "./Provider.h"
#include "./Process/Provider.h"

#include "../../System/Memory/Segment.h"

namespace GameServer {		namespace Room {

class Factory
:	public					IRoom::IFactory
{
public:
							Factory();
	virtual					~Factory();

	virtual IRoom *			Create( IChannel * i_pChannel, IPlayer * i_pPlayer, CNAME & i_strName, CPASS & i_strPass, COPTION & i_stOption );
	virtual IRoom *			Create( IChannel * i_pChannel );
	virtual void			Destroy( IRoom * i_pRoom );

	virtual void			SetSpectatorTime( OPTION::E_MODE::ENUM i_eMode, CINT i_nSec );
	virtual CINT			GetSpectatorTime( OPTION::E_MODE::ENUM i_eMode );

private:
	typedef					Memory::Segment<IRoom::s_nMaxCount, Provider>
							POOL;
	typedef					Array< OPTION::E_MODE::__MAX__, INT >
							SPECTATORS;

private:
	POOL *					m_pPool;

	Process::Provider *		m_pProcess;
	SPECTATORS				m_stSpectators;	//RESPAWN和BOMB的时间设置

};

} /* Room */				} /* GameServer */