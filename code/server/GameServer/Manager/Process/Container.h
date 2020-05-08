#pragma once

#include "../IProcess.h"

#include "../../../System/Collections/Tree.h"
#include "../../../System/Memory/Segment.h"

namespace GameServer {		namespace Manager {			namespace Process {

class Container		//IProcessÈÝÆ÷
:	public					IProcess::IContainer
{
public:
							Container();
	virtual					~Container();

	virtual IProcess *		Search( CINT i_nCode );

private:
	void					regist( IProcess * i_pProcess );

private:
	typedef					Collections::Coupler< INT, IProcess * >
							COUPLER;
	typedef					Memory::Segment< 16, COUPLER >
							POOL;
	typedef					Collections::Tree< COUPLER, Less< COUPLER >, POOL >
							TREE;

private:
	POOL *					m_pPool;
	TREE *					m_pTree;
};

} /* Process */				} /* Manager */				} /* GamerServer */