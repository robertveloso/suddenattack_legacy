// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerMgr.h
//
// PURPOSE : Definition of class to handle managment of player and camera.
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SA_PLAYER_MGR_H__
#define __SA_PLAYER_MGR_H__

#include "PlayerMgr.h"



class CSAPlayerMgr : public CPlayerMgr
{
public:
	CSAPlayerMgr();
	virtual ~CSAPlayerMgr();

	virtual LTBOOL	Init();

	virtual LTBOOL	OnCommandOn(int command);

	virtual void	OnEnterWorld();

protected:

	int		m_nFlashlightID;
	

};

#endif
