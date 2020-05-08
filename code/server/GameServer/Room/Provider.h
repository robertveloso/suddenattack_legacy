#pragma once

#include "../IRoom.h"
#include "./Result/Provider.h"

#include "../../System/Memory/Segment.h"
#include "../../System/Collections/Tree.h"

#include "../../Share/LogicObject/Room/Option.h"

namespace GameServer {		namespace Room {

class Provider
:	public					IRoom
{
public:
							Provider();
	virtual					~Provider();

	void					Capture( IChannel * i_pChannel, IPlayer * i_pPlayer, CNAME & i_strName, CPASS & i_strPass, COPTION & i_stOption );
	void					Capture( IChannel * i_pChannel );
	void					Release();

	virtual CINT			GetIndex();
	virtual CINT			GetPlayerCount();
	
	virtual IChannel *		GetChannel();

	virtual void			AddPlayer( IPlayer * i_pPlayer );
	virtual void			RemovePlayer( IPlayer * i_pPlayer );
	
	virtual IPlayer *		GetFirstPlayer();
	virtual IPlayer *		GetNextPlayer();

	virtual IPlayer *		FindPlayer( CINT i_nIndex );

	virtual CBOOL			IsPass();
	virtual CPASS &			GetPass();
	virtual void			SetPass( CPASS & i_strPass );
	virtual void			ReleasePass();

	virtual CNAME &			GetName();
	virtual void			SetName( CNAME & i_strName );

	virtual IPlayer *		GetCaptain();

	virtual IPlayer *		GetSuperPeer();
	virtual void			SetSuperPeer( IPlayer * i_pSuperPeer );

	virtual IPlayer *		GetTempSuperPeer();
	virtual void			SetTempSuperPeer( IPlayer * i_pSuperPeer );

	virtual E_JOIN::ENUM	IsJoinable( CPASS & i_strPass );

	virtual COPTION &		GetOption() const;
	virtual OPTION &		GetOption();
	virtual void			SetOption( COPTION & i_stOption );

	virtual IRESULT *		GetResult();

	virtual void			SetPause( CBOOL i_bPause );
	virtual CBOOL			IsPause();

	virtual void			SetBombReady( CBOOL i_bReady );
	virtual CBOOL			IsBombReady();

	virtual void			CheckWinner();

	virtual void			ResetStartTick();
	virtual CBOOL			IsSpectatorTick();

	virtual void			SetNat( CBOOL i_bNat );
	virtual CBOOL			IsNat();

	virtual void			SetCaptain( IPlayer * i_pPlayer );

private:
	void					checkRespawnWinner();
	void					checkBombWinner();

private:
	static INT				s_nIndex;

private:
	typedef					::LogicObject::Room::Type
							TYPE;
	typedef					String<TYPE::s_nName>
							NAME;
	typedef					String<TYPE::s_nPass>
							PASS;
	
	typedef					Collections::Coupler<INT, IPlayer *>
							COUPLER;
	typedef					Memory::Segment<128, COUPLER>
							POOL;
	typedef					Collections::Tree<COUPLER, Less<COUPLER>, POOL>
							TREE;
	typedef					TREE::Iterator
							ITERATOR;

private:
	INT						m_nIndex;
	IChannel *				m_pChannel;
	NAME					m_strName;
	PASS					m_strPass;
	OPTION					m_stOption;
	POOL *					m_pPool;
	TREE *					m_pPlayerTree;
	ITERATOR				m_stItr;

	IPlayer *				m_pCaptain;
	IPlayer *				m_pSuperPeer;		//host
	IPlayer *				m_pTempSuperPeer;
	Result::Provider *		m_pResult;

	BOOL					m_bPause;
	BOOL					m_bLobby;
	BOOL					m_bBombReady;
	BOOL					m_bNat;

	ULONG					m_ulStartTick;

};

} /* Room */				} /* GameServer */