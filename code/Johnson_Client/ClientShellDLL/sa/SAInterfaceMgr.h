// ----------------------------------------------------------------------- //
//
// MODULE  : SAInterfaceMgr.h
//
// PURPOSE : Manage all interface related functionality
//
// CREATED : 11/5/01
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SAINTERFACE_MGR_H__
#define __SAINTERFACE_MGR_H__

#include "InterfaceMgr.h"
#include "SAHUDMgr.h"
#include "SAScreenMgr.h"
#include "SAPlayerStats.h"
#include "SALayoutMgr.h"

class CSAInterfaceMgr : public CInterfaceMgr
{
public:
	CSAInterfaceMgr();
	~CSAInterfaceMgr();

	// Accessors
	CHUDMgr*		GetHUDMgr()			{ return &m_HUD;}
	CScreenMgr*		GetScreenMgr()		{ return &m_ScreenMgr;}
	CPlayerStats*	GetPlayerStats()	{ return &m_stats; }

    LTBOOL			OnCommandOn(int command);
	LTBOOL			OnKeyDown(int key, int rep);

	void			OnExitWorld();
	
private:
	CSAHUDMgr			m_HUD;					// Heads-Up Display
	CSAScreenMgr		m_ScreenMgr;
	CSAPlayerStats		m_stats;				// Player statistics (health, ammo, armor, etc.)
	CSALayoutMgr		m_SALayoutMgr;			// same as g_pLayoutMgr
};


#endif // __SAINTERFACE_MGR_H__