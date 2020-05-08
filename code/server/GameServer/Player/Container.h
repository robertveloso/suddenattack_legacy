#pragma once

#include "../IPlayer.h"
#include "../../System/Collections/Tree.h"
#include "../../System/Memory/Segment.h"

namespace GameServer {		namespace Player {

class Container
:	public					IPlayer::IContainer
{
public:
							Container();
	virtual					~Container();

	virtual CBOOL			Insert( IPlayer * i_pPlayer );
	virtual void			Remove( IPlayer * i_pPlayer );

	virtual IPlayer *		Find( CNICK & i_strNick );
	virtual IPlayer *		FindUserNo( CUSERNO & i_struserNo );

	virtual IPlayer *		First();
	virtual IPlayer *		Next();

	virtual CINT			GetCount();

private:
	typedef					Collections::Coupler<NICK, IPlayer *>
							COUPLER;
	typedef					Memory::Segment<IPlayer::s_nMaxCount, COUPLER>
							POOL;
	typedef					Collections::Tree<COUPLER, Less<COUPLER>, POOL>
							TREE;

	typedef					Collections::Coupler<USERNO, IPlayer *>
							COUPLER_N;
	typedef					Memory::Segment<IPlayer::s_nMaxCount, COUPLER_N>
							POOL_N;
	typedef					Collections::Tree<COUPLER_N, Less<COUPLER_N>, POOL_N>
							TREE_N;

private:
	POOL *					m_pPool;
	TREE *					m_pTree;
	TREE::Iterator			m_stIterator;

	POOL_N *				m_pPoolUserNo;
	TREE_N *				m_pTreeUserNo;

};

} /* Player */				} /* GamerServer */