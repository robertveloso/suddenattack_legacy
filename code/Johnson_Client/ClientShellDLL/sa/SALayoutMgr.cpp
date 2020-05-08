// ----------------------------------------------------------------------- //
//
// MODULE  : SALayoutMgr.cpp
//
// PURPOSE : Attribute file manager for interface layout info
//			 SA-specific functionality
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SALayoutMgr.h"

CLayoutMgr* g_pLayoutMgr = LTNULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSALayoutMgr::CSALayoutMgr() : CLayoutMgr()
{
	g_pLayoutMgr = this;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTronLayoutMgr::Init()
//
//	PURPOSE:	Init mgr
//
// ----------------------------------------------------------------------- //

LTBOOL CSALayoutMgr::Init(const char* szAttributeFile)
{
	if (!CLayoutMgr::Init(szAttributeFile))
		return LTFALSE;

	return LTTRUE;
}


