// ----------------------------------------------------------------------- //
//
// MODULE  : SAGameServerShell.cpp
//
// PURPOSE : Game's Server Shell - Definition
//
// CREATED : 9/18/97
//
// ----------------------------------------------------------------------- //

#ifndef __SA_GAME_SERVER_SHELL_H__
#define __SA_GAME_SERVER_SHELL_H__

#include "GameServerShell.h"
#include "SAVersionMgr.h"
#include "SAMissionButeMgr.h"

class CSAGameServerShell : public CGameServerShell
{
	public :

		declare_interface(CSAGameServerShell);

        CSAGameServerShell();
		virtual ~CSAGameServerShell();

		// Not used in SA
		virtual bool DropInventoryObject(Body* pBody) { return false; }
	
		// Cap the number of bodies in a radius.
		virtual bool	IsCapNumberOfBodies( );
		
		// Are we able to use the radar functionality
		virtual bool	ShouldUseRadar( ) { return true; }

	protected:
	    virtual LTRESULT OnServerInitialized();
		virtual void OnServerTerm();
        virtual void Update(LTFLOAT timeElapsed);

	private:

		virtual CPlayerObj* CreatePlayer(HCLIENT hClient, ModelId ePlayerModelId );
		virtual CPlayerObj* CreatePlayerExt(HCLIENT hClient, ModelId ePlayerModelId, ModelLoadType eModelType);
		CSAVersionMgr	m_VersionMgr; // Same as g_pVersionMgr
		CSAMissionButeMgr	m_MissionButeMgr;	// Same as g_pMissionButeMgr

		uint32	m_nLastPublishTime;
};


#endif  // __SA_GAME_SERVER_SHELL_H__