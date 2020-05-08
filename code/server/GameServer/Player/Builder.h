#pragma once

#include "./Provider.h"
#include "./IState.h"
#include "../../System/Collections/List.h"
#include "../../System/Memory/Segment.h"

#include "./Result/Grade.h"

namespace GameServer {		namespace Player {

class Builder
:	public					IPlayer::IBuilder
{
public:
	typedef					::Dispatcher::IClient
							ICLIENT;

public:
							Builder();
	virtual					~Builder();

	virtual ICLIENT *		Build( PACKET * i_pPacket, PEER * i_pPeer );
	virtual void			Destroy( IPlayer * i_pPlayer );

	virtual void			SetLock( CBOOL i_bLock, CBYTE i_byFinMin, CBYTE i_byType );
	virtual CBOOL			IsLock();
	virtual CBYTE			GetFinishType();
	virtual CBYTE			GetFinishMin();

private:
	typedef					Memory::Segment<IPlayer::s_nMaxCount, Provider>
							POOL;
	typedef					Memory::Segment<10>
							NODE_POOL;
	typedef					Collections::List<IState, NODE_POOL>
							LIST;

private:
	POOL *					m_pPool;	//所有Player
	LIST *					m_pList;	//逻辑组合

	BOOL					m_bLock;
	BYTE					m_byFinishMin;
	BYTE					m_byFinishType;


	Result::Grade *			m_pGrade;

};

} /* Player */				} /* GamerServer */