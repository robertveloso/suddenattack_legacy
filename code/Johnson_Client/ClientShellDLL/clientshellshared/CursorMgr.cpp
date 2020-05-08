// ----------------------------------------------------------------------- //
//
// MODULE  : CursorMgr.cpp
//
// PURPOSE : Manage all mouse cursor related functionality
//
// CREATED : 12/3/01
//
// (c) 2001-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"
#include "InterfaceMgr.h"
#include "CursorMgr.h"

#define CURSOR_W	32.0f
#define CURSOR_H	32.0f

CCursorMgr * g_pCursorMgr = LTNULL;


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCursorMgr constructor and destructor
//
//	PURPOSE:	Set initial values on ctor, force a Term() on dtor
//
// ----------------------------------------------------------------------- //

CCursorMgr::CCursorMgr()
{
	g_pCursorMgr = this;
}

CCursorMgr::~CCursorMgr()
{
	Term();

	g_pCursorMgr = LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCursorMgr::Init
//
//	PURPOSE:	Init the cursor
//
// ----------------------------------------------------------------------- //

LTBOOL CCursorMgr::Init()
{
	if (m_bInitialized)
		return LTTRUE;

	m_hTexCursor = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/CURSOR/cursor.dtx" );
	g_pDrawPrim->SetRGBA( &m_PolyCursor, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyCursor, 0.0f, 0.0f, 1.0f, 1.0f );
	
	m_bInitialized = LTTRUE;

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCursorMgr::Term
//
//	PURPOSE:	Free cursor resources
//
// ----------------------------------------------------------------------- //

void CCursorMgr::Term()
{
	if (!m_bInitialized)
		return;

	m_bInitialized = LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCursorMgr::UseCursor
//
//	PURPOSE:	Handle activation and deactivation of visible cursor
//
// ----------------------------------------------------------------------- //

void CCursorMgr::UseCursor(LTBOOL bUseCursor, LTBOOL bLockCursorToCenter)
{
	m_bUseCursor = bUseCursor;
	
	if(bLockCursorToCenter)
	{
		g_pLTClient->RunConsoleString("CursorCenter 1");
	}
	else
	{
		g_pLTClient->RunConsoleString("CursorCenter 0");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCursorMgr::Update
//
//	PURPOSE:	Display a cursor bitmap, if required, or update the sprite coords
//
// ----------------------------------------------------------------------- //

void CCursorMgr::Update()
{
	if( !m_bUseCursor ) return;

	LTIntPt CursorPos = g_pInterfaceMgr->GetCursorPos();

	g_pLTClient->Start3D();
	g_pLTClient->StartOptimized2D();

	g_pDrawPrim->SetTexture( m_hTexCursor );
	g_pDrawPrim->SetXYWH( &m_PolyCursor, (float)CursorPos.x, (float)CursorPos.y, CURSOR_W, CURSOR_H );
	g_pDrawPrim->DrawPrim( &m_PolyCursor );

	g_pLTClient->EndOptimized2D();
	g_pLTClient->End3D();
}

