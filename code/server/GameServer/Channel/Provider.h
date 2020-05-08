#pragma once

#include "../IChannel.h"
#include "../../System/Memory/Segment.h"
#include "../../System/Collections/Tree.h"

namespace GameServer {		namespace Channel {

class Provider
:	public					IChannel
{
public:
							Provider();
	virtual					~Provider();

	void					Capture( CBYTE i_byIndex, CINT i_nMaxUser );
	void					Release();

	virtual CBYTE			GetIndex();
	virtual IRoom *			GetLobby();

	virtual CINT			GetMaxUser();
	virtual CINT			GetCurrentUser();

	virtual CINT			GetRoomCount();
	virtual IRoom *			Search( CINT i_nRoomIndex );

	virtual void			AddRoom( IRoom * i_pRoom );
	virtual void			RemoveRoom( IRoom * i_pRoom );
	
	virtual IRoom *			GetFirstRoom();
	virtual IRoom *			GetNextRoom();

private:
	typedef					Collections::Coupler<INT, IRoom *>
							COUPLER;
	typedef					Memory::Segment<128, COUPLER>
							POOL;
	typedef					Collections::Tree<COUPLER, Less<COUPLER>, POOL>
							TREE;
	typedef					TREE::Iterator
							ITERATOR;

private:
	IRoom *					m_pLobby;	//大厅，在capture时创建，其实也是一个room，管理所有在大厅中的角色
	INT						m_nMaxUser;	//控制最大人数
	BYTE					m_byIndex;	
	POOL *					m_pPool;	//room 集合，128个
	TREE *					m_pTree;	//room 集合形成的二叉树
	ITERATOR				m_stItr;

};

} /* Channel */				} /* GameServer */