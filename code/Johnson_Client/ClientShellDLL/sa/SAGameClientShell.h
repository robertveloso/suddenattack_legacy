// ----------------------------------------------------------------------- //
//
// MODULE  : SAGameClientShell.h
//
// PURPOSE : Game Client Shell - Definition
//
// CREATED : 11/5/01
//
// (c) 1997-2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SAGAME_CLIENT_SHELL_H__
#define __SAGAME_CLIENT_SHELL_H__

#include "GameClientShell.h"
#include "SAInterfaceMgr.h"
#include "SAPlayerMgr.h"
#include "SAVersionMgr.h"
#include "SAClientWeaponAllocator.h"
#include "SAMissionButeMgr.h"

class CSAGameClientShell : public CGameClientShell
{
	public:
		declare_interface(CSAGameClientShell);

		CSAGameClientShell();
		~CSAGameClientShell();

		virtual		CInterfaceMgr	*		GetInterfaceMgr() { return &m_InterfaceMgr;}
		virtual		CPlayerMgr *			GetPlayerMgr() { return &m_PlayerMgr;}
		virtual		CClientWeaponAllocator const *GetClientWeaponAllocator() const
				{ return &m_ClientWeaponAllocator; }

		virtual uint32 OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid);
	    virtual LTRESULT ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4], uint16 senderPort);
	    virtual void OnMessage(ILTMessage_Read *pMsg);
		
		// Are we able to use the radar functionality
		virtual bool	ShouldUseRadar( ) { return true; }

		virtual void OnEnterWorld();


	private:
		CSAInterfaceMgr			m_InterfaceMgr;		// Interface manager
		CSAVersionMgr				m_VersionMgr;		// Same as g_pVersionMgr
		CSAPlayerMgr				m_PlayerMgr;		// Interface manager
		CSAMissionButeMgr			m_MissionButeMgr;	// Same as g_pMissionButeMgr

		// allocator for client weapons
		CSAClientWeaponAllocator	m_ClientWeaponAllocator;
};

#endif  // __TRON_GAME_CLIENT_SHELL_H__
