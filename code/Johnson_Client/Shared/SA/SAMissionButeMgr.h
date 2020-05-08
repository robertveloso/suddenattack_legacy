/****************************************************************************
;
;	MODULE:			SAMissionButeMgr.h
;
;	PURPOSE:		SA-specific Mission Bute Manager
;
;	HISTORY:		2/26/2002 [kml] This file was created
;
;	COMMENT:		Copyright (c) 2002, Monolith Productions, Inc.
;
****************************************************************************/

#ifndef _SA_MISSION_BUTE_MGR_H_
#define _SA_MISSION_BUTE_MGR_H_

#include "MissionButeMgr.h"

class CSAMissionButeMgr : public CMissionButeMgr
{
	public :

		CSAMissionButeMgr();
		LTBOOL		Init(const char* szAttributeFile=MISSION_DEFAULT_FILE);
};

#endif // _SA_MISSION_BUTE_MGR_H_