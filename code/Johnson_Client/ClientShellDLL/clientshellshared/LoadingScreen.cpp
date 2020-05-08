// ----------------------------------------------------------------------- //
//
// MODULE  : LoadingScreen.cpp
//
// PURPOSE : Background-thread loading screen encapsulation class
//
// CREATED : 2000
//
// (c) 2000-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "LoadingScreen.h"
#include "InterfaceMgr.h"
#include "GameClientShell.h"
#include "WinUtil.h"
#include "BaseScreen.h"
#include "MissionMgr.h"
#include "MissionButeMgr.h"
#include "FXButeMgr.h"
#include "ClientMultiplayerMgr.h"
#include "ScreenPostload.h"

extern CGameClientShell* g_pGameClientShell;
#define LOADINGBAR_TEXTUREUVOFFSET		0.02929f
#define LOADINGBAR_MAXUV				0.6445f
#define LOADINGBAR_TEXTUREWIDTHOFFSET   15.0f
#define LOADINGBAR_MAXWIDTH				330.0f

uint8 g_nSleepTimer = 100;

CLoadingScreen::CLoadingScreen() :
	m_eCurState(STATE_NONE)
{
	// Create the event handles
	m_hEventEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventThreadRunning = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	InitializeCriticalSection(&m_MissionUpdate);

	m_hThreadHandle = LTNULL;

	m_fSendPing = 0.0f;
	
	m_nCurrLoadingState = -1;
	m_nLastLoadingState = 0;
	m_bStartLoading = LTFALSE;
}

CLoadingScreen::~CLoadingScreen()
{
	// Terminate the object, just in case...
	Term();
	//DeleteObject(m_hEventEnd);
	//DeleteObject(m_hEventThreadRunning);
	CloseHandle(m_hEventEnd);
	CloseHandle(m_hEventThreadRunning);
	DeleteCriticalSection(&m_MissionUpdate);

	if( m_hThreadHandle )
	{
		CloseHandle(m_hThreadHandle);
		m_hThreadHandle = LTNULL;
	}
}

LTBOOL CLoadingScreen::Init()
{
	if (m_eCurState != STATE_NONE)
		return LTFALSE;

	MISSION* pMission = g_pMissionButeMgr->GetMission( g_GameDoc.GetCurrentMission() );
	char	szTemp[MAX_CS_FILENAME_LEN];

	if( pMission )
	{
		sprintf( szTemp, "%s_1.dtx", pMission->szLayout );
	}
	else
	{
		szTemp[0] = LTNULL;
	}

	// 배경프레임 생성
	float	fX, fY, fW, fH;
	m_hFrame[0] = g_pInterfaceResMgr->GetTexture(szTemp);
	SetupQuadUVs( m_PolyBack[0], m_hFrame[0], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[0], argbWhite );
	fX	= 0.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 0.0f*g_pInterfaceResMgr->GetXRatio();
	fW	= 512.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 512.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[0], fX, fY, fW, fH );

	if( pMission )
	{
		sprintf( szTemp, "%s_2.dtx", pMission->szLayout );
	}
	else
	{
		szTemp[0] = LTNULL;
	}
	m_hFrame[1] = g_pInterfaceResMgr->GetTexture(szTemp);
	SetupQuadUVs( m_PolyBack[1], m_hFrame[1], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[1], argbWhite );
	fX	= 512.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 0.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 256.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 512.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[1], fX, fY, fW, fH );

	m_hFrame[2] = g_pInterfaceResMgr->GetTexture("SA_Interface\\Menu\\Loading\\base_3.dtx");
	SetupQuadUVs( m_PolyBack[2], m_hFrame[2], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[2], argbWhite );
	fX	= 768.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 0.0f*g_pInterfaceResMgr->GetXRatio();
	fW	= 32.0f*g_pInterfaceResMgr->GetYRatio();
	fH	= 512.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[2], fX, fY, fW, fH );

	if( pMission )
	{
		sprintf( szTemp, "%s_4.dtx", pMission->szLayout );
	}
	else
	{
		szTemp[0] = LTNULL;
	}
	m_hFrame[3] = g_pInterfaceResMgr->GetTexture(szTemp);
	SetupQuadUVs( m_PolyBack[3], m_hFrame[3], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[3], argbWhite );
	fX	= 0.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 512.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 512.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 64.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[3], fX, fY, fW, fH );

	if( pMission )
	{
		sprintf( szTemp, "%s_5.dtx", pMission->szLayout );
	}
	else
	{
		szTemp[0] = LTNULL;
	}
	m_hFrame[4] = g_pInterfaceResMgr->GetTexture(szTemp);
	SetupQuadUVs( m_PolyBack[4], m_hFrame[4], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[4], argbWhite );
	fX	= 512.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 512.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 256.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 64.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[4], fX, fY, fW, fH );

	m_hFrame[5] = g_pInterfaceResMgr->GetTexture("SA_Interface\\Menu\\Loading\\base_6.dtx");
	SetupQuadUVs( m_PolyBack[5], m_hFrame[5], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[5], argbWhite );
	fX	= 768.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 512.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 32.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 64.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[5], fX, fY, fW, fH );

	m_hFrame[6] = g_pInterfaceResMgr->GetTexture("SA_Interface\\Menu\\Loading\\base_7.dtx");
	SetupQuadUVs( m_PolyBack[6], m_hFrame[6], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[6], argbWhite );
	fX	= 0.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 576.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 512.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 32.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[6], fX, fY, fW, fH );

	m_hFrame[7] = g_pInterfaceResMgr->GetTexture("SA_Interface\\Menu\\Loading\\base_8.dtx");
	SetupQuadUVs( m_PolyBack[7], m_hFrame[7], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[7], argbWhite );
	fX	= 512.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 576.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 256.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 32.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[7], fX, fY, fW, fH );

	m_hFrame[8] = g_pInterfaceResMgr->GetTexture("SA_Interface\\Menu\\Loading\\base_9.dtx");
	SetupQuadUVs( m_PolyBack[8], m_hFrame[8], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[8], argbWhite );
	fX	= 768.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 576.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 32.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 32.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[8], fX, fY, fW, fH );

	m_hFrame[9] = g_pInterfaceResMgr->GetTexture("SA_Interface\\Menu\\Loading\\FmLoading.dtx");
	SetupQuadUVs( m_PolyBack[9], m_hFrame[9], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBack[9], argbWhite );
	fX	= 38.0f*g_pInterfaceResMgr->GetXRatio();
	fY	= 543.0f*g_pInterfaceResMgr->GetYRatio();
	fW	= 128.0f*g_pInterfaceResMgr->GetXRatio();
	fH	= 16.0f*g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH( &m_PolyBack[9], fX, fY, fW, fH );

	//[SPIKE] Loading Bar ===========================================
	m_hLoadingBar = g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Loading\\FmLoadBar.dtx");
	m_hCompleteText = g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Loading\\FmCompleteText.dtx");
	g_pDrawPrim->SetRGBA( &m_LoadingPolyBar, argbWhite );
	g_pDrawPrim->SetUVWH( &m_LoadingPolyBar, 0.0f, 0.0f, 1.0f, 1.0f );	
	//[SPIKE] Loading Bar ===========================================

	m_fIncrease	= -1.0f;
	m_fAlpha	= 255.0f;
	
	// Reset the frame counter
	m_nFrameCounter = 0;
	m_fLastFrameTime = CWinUtil::GetTime();
	m_fCurFrameDelta = 0.0f;
	
	m_eCurState = STATE_INIT;

	return LTTRUE;
}

LTBOOL CLoadingScreen::Term()
{
	if (m_eCurState != STATE_INIT)
		return LTFALSE;

	m_eCurState = STATE_NONE;
	return LTTRUE;
}

unsigned long WINAPI CLoadingScreen::ThreadBootstrap(void *pData)
{
	return ((CLoadingScreen*)pData)->RunThread();
}

int CLoadingScreen::RunThread()
{
	// Change state
	m_eCurState = STATE_ACTIVE;
	
	// Tell the main thread we're now in our main loop
	SetEvent(m_hEventThreadRunning);

	// The main rendering loop...  (i.e. keep drawing until someone tells us to stop)
	while (WaitForSingleObject(m_hEventEnd, 0) == WAIT_TIMEOUT)
	{
		// Draw the frame..
		Update();
		
		// Make sure we're not running faster than 10fps so stuff can still happen in the background
		Sleep(g_nSleepTimer);
	}
	return 0;
}

LTBOOL CLoadingScreen::Update()
{
	// Make sure we're in a valid state...
	if ((m_eCurState != STATE_ACTIVE) && (m_eCurState != STATE_SHOW))
		return LTFALSE;

	EnterCriticalSection(&m_MissionUpdate);

	if( g_pGameClientShell->IsMainWindowFocused() )
	{
		g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);	
		g_pLTClient->Start3D();
		g_pLTClient->StartOptimized2D();
		
		g_pDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
		g_pDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
		g_pDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
		g_pDrawPrim->SetFillMode(DRAWPRIM_FILL);
		g_pDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
		g_pDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
		g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);

		
		for( int i=0 ; i<9 ; ++i )
		{
			if( m_hFrame[i] )
			{
				g_pDrawPrim->SetTexture( m_hFrame[i] );
				g_pDrawPrim->DrawPrim( &m_PolyBack[i] );
			}
		}
		
		//[SPIKE] LoadingBar ====================================================
		if( m_nCurrLoadingState != g_pLTClient->GetLoadingState() )
		{
			m_nCurrLoadingState = g_pLTClient->GetLoadingState();
			m_bStartLoading = LTTRUE;
		}
		
		if(m_bStartLoading && m_nLastLoadingState < m_nCurrLoadingState)
		{
			m_nLastLoadingState++;
			if(m_nLastLoadingState >= 16) g_nSleepTimer = 30;
			if(m_nLastLoadingState >= 19) g_nSleepTimer = 10;
			if(m_nLastLoadingState >= 22)
			{
				g_nSleepTimer = 0;
				m_nLastLoadingState = 22;
			}
		}
		LTFLOAT	fLoadingBarUVOffset		= LOADINGBAR_TEXTUREUVOFFSET * m_nLastLoadingState;
		LTFLOAT	fLoadingBarWidthOffset	= LOADINGBAR_TEXTUREWIDTHOFFSET * m_nLastLoadingState;
	
		if(fLoadingBarUVOffset > LOADINGBAR_MAXUV) fLoadingBarUVOffset = LOADINGBAR_MAXUV;
		if(fLoadingBarWidthOffset > LOADINGBAR_MAXWIDTH) fLoadingBarWidthOffset = LOADINGBAR_MAXWIDTH;
		
		//Bar
		g_pDrawPrim->SetUVWH( &m_LoadingPolyBar, 0.0f, 0.0f, fLoadingBarUVOffset, 1.0f );	
		g_pDrawPrim->SetXYWH( &m_LoadingPolyBar, 41.0f * g_pInterfaceResMgr->GetXRatio(), 558.0f * g_pInterfaceResMgr->GetYRatio(), 
								fLoadingBarWidthOffset * g_pInterfaceResMgr->GetXRatio(), 16.0f * g_pInterfaceResMgr->GetYRatio() );
		g_pDrawPrim->SetTexture( m_hLoadingBar );
		
		g_pDrawPrim->DrawPrim(&m_LoadingPolyBar);

		
		//Text
		if(m_nLastLoadingState < 22)
		{
			g_pDrawPrim->SetTexture( m_hFrame[9] );
		}
		else
		{
			m_fAlpha = 200;
			g_pDrawPrim->SetTexture( m_hCompleteText );
		}
		g_pDrawPrim->SetALPHA( &m_PolyBack[9], (uint8)m_fAlpha );
		g_pDrawPrim->DrawPrim( &m_PolyBack[9] );
		//[SPIKE] LoadingBar ====================================================

		g_pLTClient->EndOptimized2D();
		g_pLTClient->End3D();	
		g_pLTClient->FlipScreen(0);
	}

	LeaveCriticalSection(&m_MissionUpdate);

	// Count it..
	++m_nFrameCounter;

	LTFLOAT fCurTime = CWinUtil::GetTime();
	m_fCurFrameDelta = fCurTime - m_fLastFrameTime;
	m_fLastFrameTime = fCurTime;

	m_fSendPing -= m_fCurFrameDelta;
	if( m_fSendPing <= 0.0f )
	{
		m_fSendPing = 10.0f;
		g_Network.SendPing();
	}
	
	m_fAlpha += m_fIncrease * m_fCurFrameDelta * 255.0f;

	if( m_fAlpha < 50.0f )
	{
		m_fAlpha = 70.0f;
		m_fIncrease	= 2.0f;
	}
	else if( m_fAlpha > 255.0f )
	{
		m_fAlpha = 255.0f;
		m_fIncrease = -2.0f;
	}
	
	return LTTRUE;
}

LTBOOL CLoadingScreen::Show(LTBOOL bRun)
{
	if (bRun && !GetConsoleInt("DynamicLoadScreen",1))
		bRun = LTFALSE;
	// Make sure we're in the correct state
	if (m_eCurState == STATE_NONE)
	{
		if (!Init())
			return LTFALSE;
	}

	if (m_eCurState != STATE_INIT)
		return LTFALSE;

	
	// Turn off the cursor
	g_pCursorMgr->UseCursor(LTFALSE);
	
	// Set up the FarZ & turn off fog (farz of 0 is bogus)

	m_nOldFarZ = GetConsoleInt("FarZ", 10000);
	m_nOldFarZ = m_nOldFarZ == 0 ? 10000 : m_nOldFarZ;

	m_bOldFogEnable = (LTBOOL) GetConsoleInt("FogEnable", 0);

	g_pGameClientShell->SetFarZ(10000);
	WriteConsoleInt("FogEnable", 0);

	// Make sure we're not in optimized 2D mode (happens sometimes...)
	g_pLTClient->EndOptimized2D();

	
	// Go into the right state..
	m_eCurState = STATE_SHOW;
	
	//[SPIKE] Loading Bar ===========================================
	m_nLastLoadingState	= 0;
	m_nCurrLoadingState = 25; // 항시 마지막 인덱스로 세팅!!!
	m_bStartLoading = LTFALSE;
	g_nSleepTimer			= 100;
	//[SPIKE] Loading Bar ===========================================
	
	m_fSendPing = 0.0f;

	// Update once so the screen's showing
	Update();

	// Start updating if they wanted it to..
	if (bRun)
		return Resume();

	// Ok, it's visible or active
	return LTTRUE;
}

LTBOOL CLoadingScreen::Pause()
{
	// Make sure we're in the right state
	if (m_eCurState != STATE_ACTIVE)
		return LTFALSE;

	// Shut down the loading screen thread
	SetEvent(m_hEventEnd);
	WaitForSingleObject(m_hThreadHandle, INFINITE);

	// Ok, it's just visible now..
	m_eCurState = STATE_SHOW;

	return LTTRUE;
}

LTBOOL CLoadingScreen::Resume()
{
	// Ensure our state
	if (m_eCurState != STATE_SHOW)
		return LTFALSE;

	// Reset the events
	ResetEvent(m_hEventEnd);
	ResetEvent(m_hEventThreadRunning);

	if( m_hThreadHandle )
	{
		CloseHandle(m_hThreadHandle);
		m_hThreadHandle = LTNULL;
	}

	g_Network.SendPing();
	// Start up the loading screen thread
	uint32 uThreadID;
	m_hThreadHandle = CreateThread(NULL, 0, ThreadBootstrap, (void *)this, 0, (unsigned long *)&uThreadID);

	// Handle what shouldn't be possible..
	if (!m_hThreadHandle)
		return LTFALSE;

	// Wait for the loading thread to stop touching stuff..
	WaitForSingleObject(m_hEventThreadRunning, INFINITE);

	// Now we're actually active.  (Thank you Mr. Thread..)
	return LTTRUE;
}

LTBOOL CLoadingScreen::Hide()
{
	// Ensure our state
	if (m_eCurState == STATE_ACTIVE)
	{
		// Stop!!!
		if (!Pause())
			return LTFALSE;
	}

	if (m_eCurState != STATE_SHOW)
		return LTFALSE;

	// Change state
	m_eCurState = STATE_INIT;

	// Clean up
	Term();

	// Re-set the console...
	g_pGameClientShell->SetFarZ(m_nOldFarZ);
	WriteConsoleInt("FogEnable", (int)m_bOldFogEnable);

	return LTTRUE;
}

bool CLoadingScreen::NeedsPostLoadScreen() const
{
	return false;
}





















