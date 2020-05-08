// ----------------------------------------------------------------------- //
//
// MODULE  : SALayoutMgr.h
//
// PURPOSE : Attribute file manager for interface layout info
//			 SA-specific items
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#if !defined(_SALAYOUTMGR_H_)
#define _SALAYOUTMGR_H_

#include "LayoutMgr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSALayoutMgr : public CLayoutMgr
{
public:
	CSALayoutMgr();
    LTBOOL			Init(const char* szAttributeFile=LO_DEFAULT_FILE);
};

#endif // _SALAYOUTMGR_H_