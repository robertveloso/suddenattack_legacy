#pragma once

#include "../IPlayer.h"

namespace GameServer {		namespace Player {
class Memento;
class IState;
} /* Player */				} /* GameServer */

namespace GameServer {		namespace Player {			namespace Result {
class Provider;
} /* Result */				} /* Player */				} /* GamerServer */

namespace GameServer {		namespace Player {

class Provider
:	public					IPlayer
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							Provider();
	virtual					~Provider();

	void					Capture( PEER * i_pPeer );
	void					Release();

	virtual PEER *			GetPeer();

	virtual void			OnOpen();
	virtual void			OnPacket( PACKET * i_pPacket );
	virtual void			OnClose();
	virtual void			OnSignal( CUINT i_uiMessage );

	virtual void			OnResponse( IQUERY * i_pQuery );

	// IPlayer
	virtual IRESULT *		GetResult();

	virtual CDWORD			GetIndex() const;
	virtual MEMENTO *		GetMemento();
//	virtual MEMENTO *		GetMemento() const;

	virtual E_STATE::ENUM	GetState();

	virtual void			LogoutProcess();
	virtual void			RecordResult( E_TEAM::ENUM i_eWinner );
	virtual void			RecordPenalty();
	virtual void			CastResult();

	virtual void			InitPlay();

	virtual void			SetLogined();
	virtual CBOOL			IsLogined();

private:
	void					castResultTotal();

private:
	Memento *				m_pMemento;
	PEER *					m_pPeer;

	Result::Provider *		m_pResult;

	DWORD					m_dwIndex;
	static DWORD			s_dwIndex;
	BOOL					m_bLogined;

};

} /* Player */				} /* GamerServer */