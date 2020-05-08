// ----------------------------------------------------------------------- //
//
// MODULE  : SAVersionMgr.h
//
// PURPOSE : Definition of versioning manager
//
// CREATED : 11/16/2000
//
// (c) 2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef _SAVERSIONMGR_H_
#define _SAVERSIONMGR_H_

#include "VersionMgr.h"

class CSAVersionMgr : public CVersionMgr
{
	public :

		CSAVersionMgr();
		virtual ~CSAVersionMgr() {}

		virtual void Update();

		virtual const char* GetVersion();
		virtual const char* GetBuild();
		virtual const uint32 GetSaveVersion();
		virtual const char* GetNetGameName();
		virtual const char* GetNetVersion();
		virtual LTGUID const* GetBuildGuid();
};

#endif _SAVERSIONMGR_H_