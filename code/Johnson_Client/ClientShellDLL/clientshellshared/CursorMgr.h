// ----------------------------------------------------------------------- //
//
// MODULE  : CursorMgr.h
//
// PURPOSE : Manage all mouse cursor related functionality
//
// CREATED : 12/3/01
//
// (c) 2001-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CURSOR_MGR_H__
#define __CURSOR_MGR_H__

#include "iltcursor.h"
#include "ScreenSpriteMgr.h"


class CCursorMgr;
extern CCursorMgr* g_pCursorMgr;

class CCursorMgr
{
public:
	CCursorMgr();
	~CCursorMgr();

	LTBOOL		Init();
	void		Term();

	void		UseCursor(LTBOOL bUseCursor, LTBOOL bLockCursorToCenter = LTFALSE);
	void		Update();

private:

	LTBOOL		m_bInitialized;
    LTBOOL      m_bUseCursor;    
	HTEXTURE	m_hTexCursor;
	LTPoly_GT4	m_PolyCursor;
};

#endif // __CURSOR_MGR_H__