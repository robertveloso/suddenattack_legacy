#pragma once

#include "../IServer.h"

#include "../../System/Memory/Segment.h"
#include "../../System/Collections/List.h"

namespace GameServer {		namespace Server {

class Provider
:	public					IServer
{
public:
							Provider();
	virtual					~Provider();

private:
	virtual void			Open( CSTRING & i_strIP, CINT i_nPort );
	
	virtual CSTRING &		GetID();
	virtual void			SetID( CSTRING & i_strID );

	virtual CSTRING &		GetName();
	virtual void			SetName( CSTRING & i_strName );

	virtual CSTRING &		GetIP();
	virtual CINT			GetPort();

	virtual CINT			GetChannelCount();
	virtual IChannel *		Search( CBYTE i_byIndex );

	virtual void			AddChannel( IChannel * i_pChannel );
	virtual void			RemoveChannel( IChannel * i_pChannel );
	
	virtual IChannel *		GetFirstChannel();
	virtual IChannel *		GetNextChannel();

	virtual void			SetLoginGamehi( CBOOL i_bCan );
	virtual void			SetLoginNetmarble( CBOOL i_bCan );

	virtual CBOOL			CanLoginGamehi();
	virtual CBOOL			CanLoginNetmarble();

	virtual IRANK *			GetRank();

private:
	typedef					Memory::Segment< 16 >
							POOL;
	typedef					Collections::List<IChannel, POOL>
							LIST;
	typedef					LIST::Iterator
							ITERATOR;

private:
	//配置文件中读取,初试化时赋值
	BOOL					m_bOpen;
	STRING					m_strIP;
	INT						m_nPort;
	//
	STRING					m_strID;
	STRING					m_strName;

	LIST *					m_pList;
	ITERATOR				m_stItr;

	BOOL					m_bLoginGamehi;
	BOOL					m_bLoginNetmarble;

	IRANK *					m_pRank;

};

} /* Server */				} /* GameServer */