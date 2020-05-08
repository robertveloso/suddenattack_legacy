// ----------------------------------------------------------------------- //
//
// MODULE  : InterfaceMgr.cpp
//
// PURPOSE : Manage all interface related functionality
//
// CREATED : 4/6/99
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "InterfaceMgr.h"
#include "GameClientShell.h"
#include "MsgIDs.h"
#include "WeaponStringDefs.h"
#include "VKDefs.h"
#include "SoundMgr.h"
#include "InterfaceResMgr.h"
#include "VarTrack.h"
#include "ClientButeMgr.h"
#include "CharacterFX.h"
#include "GameButes.h"
#include "UberAssert.h"
#include "ClientWeaponBase.h"
#include "ClientWeaponMgr.h"
#include "ClientMultiplayerMgr.h"
#include "MissionMgr.h"
#include "ScreenPreload.h"
#include "ScreenPostload.h"
#include "timer.h"
#include "WaveFn.h"
#include "../SA/SAHUDMgr.h"
#include "WinUtil.h"
#include "PlayerCamera.h"
#include <mmsystem.h>

//------------------------------------------- (B) Novice (minjin) [03/12/2005]
#include "GameText.h"
#include "GameMessage.h"
//------------------------------------------- (E) Novice (minjin) [03/12/2005]

CInterfaceMgr*  g_pInterfaceMgr = LTNULL;

#define IM_SPLASH_SOUND		"Interface\\Menu\\Snd\\theme_mp3.wav"

#ifdef _TO2DEMO
#define IM_SPLASH_SCREEN	"Interface\\demosplash.pcx"
#else
#define IM_SPLASH_SCREEN	"Interface\\Menu\\Art\\splash.pcx"
#endif

#define MAX_INTERFACE_SFX		100
#define MAX_INTERFACE_LIGHTS	5
#define INVALID_ANI			((HMODELANIM)-1)
#define KILLNAMESPACE "%s                     %s"
#define HEADSHOT_KILLNAMESPACE "%s                     %s [ HeadShot! ]"

#define FRAME_INTERVAL		0.013f

static enum MovieOrderEnum 
{
	eSierra,
	eFox,
	eMonolith,
	eLithTech
};

//hack to track whether we are trying to host/join a LAN only or an internet game
bool g_bLAN = false;
//bool g_bLAN = true;

static MovieOrderEnum gMovieOrder[] =
{
	eSierra, eFox, eMonolith, eLithTech
};

LTFLOAT      g_fFovXTan = 0.0f;
LTFLOAT      g_fFovYTan = 0.0f;

LTVector     g_vOverlaySpriteScale(0.02f, 0.02f, 1.0f);
LTVector     g_vOverlayModelScale(1.0f, 1.0f, 1.0f);
LTFLOAT      g_fOverlaySpriteDist = 1.0f;
LTFLOAT      g_fOverlayModelDist = 0.25f;

LTVector     g_vBaseBackScale(0.8f, 0.6f, 1.0f);
LTFLOAT      g_fBackDist = 200.0f;

HSURFACE    g_hDemo = LTNULL;
int			g_nDemo = 0;

#define NUM_DEMO_SCREENS 3
char  g_szDemoScreens[NUM_DEMO_SCREENS][64] =
{
	"Interface\\DemoEndSplash1.pcx",
	"Interface\\DemoEndSplash2.pcx",
	"Interface\\DemoEndSplash3.pcx",
};
uint8 g_nNumDemoScreens = sizeof(g_szDemoScreens)/sizeof(char*);


VarTrack	g_vtDrawInterface;
VarTrack	g_vtLetterBox;
VarTrack	g_vtLetterBoxFadeInTime;
VarTrack	g_vtLetterBoxFadeOutTime;
VarTrack	g_vtLetterBoxDisabled;
VarTrack	g_vtDisableMovies;
VarTrack	g_vtInterfaceFOVX;
VarTrack	g_vtInterfaceFOVY;
VarTrack	g_vtPauseTintAlpha;
VarTrack	g_vtMainScreenFadeIn;
VarTrack	g_vtExitLevelScreenFadeTime;

#ifndef _FINAL
VarTrack	g_vtPVModelModifyMode;
#endif

extern VarTrack g_vtFOVXNormal;
extern VarTrack g_vtFOVYNormal;

extern VarTrack g_vtScreenFadeInTime;

const char* c_GameStateNames[] =
{
	"GS_UNDEFINED",
	"GS_PLAYING",
	"GS_EXITINGLEVEL",
	"GS_LOADINGLEVEL",
	"GS_SPLASHSCREEN",
	"GS_POPUP",
	"GS_SCREEN",
	"GS_PAUSED",
	"GS_DEMOSCREEN",
	"GS_MOVIE",
	"GS_MANDATEHOST",
	"GS_REJOINHOST",
};


static LTMatrix GetCameraTransform(HOBJECT hCamera)
{
    LTVector vPos, vRight, vUp, vForward;
    LTRotation rRot;

	g_pLTClient->GetObjectPos(hCamera, &vPos);
	g_pLTClient->GetObjectRotation(hCamera, &rRot);

	vPos.x = -vPos.x;
	vPos.y = -vPos.y;
	vPos.z = -vPos.z;

    LTMatrix mTran, mRot, mFull;

	mRot.SetBasisVectors((LTVector*)&rRot.Right(), (LTVector*)&rRot.Up(), (LTVector*)&rRot.Forward());
	MatTranspose3x3(&mRot);

	Mat_Identity(&mTran);
	mTran.m[0][3] = vPos.x;
	mTran.m[1][3] = vPos.y;
	mTran.m[2][3] = vPos.z;

	MatMul(&mFull, &mRot, &mTran);

	return mFull;
}


namespace
{
	LTBOOL g_bInGameFogEnabled = LTTRUE;

	HLTCOLOR hBackColor;

	//**********************************************************
	//*** definitions needed for rendering FX in the interface
	//**********************************************************
	typedef struct FXRenderInfo_t
	{
		HOBJECT hObj;
		uint8	nLayer;
	} FXRenderInfo;

	const int kMaxFX = MAX_INTERFACE_SFX + MAX_INTERFACE_LIGHTS + 2;
	static FXRenderInfo sRenderInfo[kMaxFX];
	typedef std::list<FXRenderInfo *> FXRenderList;
	FXRenderList renderList;


	typedef std::vector<int> KeystrokeList;
	KeystrokeList g_keys;

	bool g_bLockPopup = true;

	CTimer g_SplashScreenTimer;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::CInterfaceMgr
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CInterfaceMgr::CInterfaceMgr()
{
	g_pInterfaceMgr	= this;

	m_eGameState			= GS_UNDEFINED;
	m_eLastGameState		= GS_UNDEFINED;

	m_dwAdvancedOptions		= AO_DEFAULT_ENABLED;
	m_dwOrignallyEnabled	= 0;

	m_nClearScreenCount		= 0;
    m_bClearScreenAlways    = LTFALSE;

	m_fMenuSaveFOVx			= 0.0f;
	m_fMenuSaveFOVy			= 0.0f;

    m_bSwitchingModes       = LTFALSE;

    m_nFailStringId         = LTNULL;

	m_bUseInterfaceCamera	= LTTRUE;
	m_hInterfaceCamera		= LTNULL;

	for (int i =0; i < NUM_OVERLAY_MASKS; i++)
	{
        m_hOverlays[i] = LTNULL;
		m_fOverlayScaleMult[i] = 1.0f;
	}
	m_nOverlayCount = 0;

    m_hScubaSound		= LTNULL;
	m_hSound			= LTNULL;

    m_hFadeSurface      = LTNULL;
    m_hLetterBoxSurface = LTNULL;
/*
	m_nBorderSize		= 0;
*/
	m_fLetterBoxAlpha	= 0.0f;
	m_bLetterBox		= LTFALSE;
	m_bWasLetterBox		= LTFALSE;
	m_fLetterBoxFadeEndTime = 0.0f;

    m_bScreenFade       = LTFALSE;
    m_bFadeInitialized  = LTFALSE;
	m_fTotalFadeTime	= 0.0f;
	m_fCurFadeTime		= 0.0f;
    m_bFadeIn           = LTTRUE;

	m_bSavedGameMusic   = LTFALSE;

	m_eNextSound		= IS_NONE;

	m_bQuitAfterDemoScreens = LTFALSE;
	m_bSeenDemoScreens = LTFALSE;

	m_hMovie			= LTNULL;
	m_nCurMovie			= gMovieOrder[0];

	m_bLoadFailed		= LTFALSE;
	m_bCommandLineJoin = false;
		
	m_hGamePausedSurface = LTNULL;

	m_fLastUpdateRequestTime = 0.0f;

	m_SelectFX.ClearLink();

	m_bInitialized = false;
	m_bHideHUDInPopup = false;
	m_ePrePopupHUDRenderLevel = kHUDRenderFull;

	m_bSuppressNextFlip = false;

	m_bEnteredScreenState = false;
	m_bIntentionalDisconnect = false;

	//[MURSUM]====================================
	/*
	m_InterfaceTimer.SetTeamId( INVALID_TEAM );
	m_BlueInterfaceTimer.SetTeamId( 0 );
	m_RedInterfaceTimer.SetTeamId( 1 );
	*/
	m_hBGM = LTNULL;
	//============================================

	m_bSkipPreLoadScreen = false;
	// 광식
	//ZeroMemory(m_szKillText, 256);

	//[MURSUM]====================================
	m_bExitMessage			= false;
	m_bChangeModeReserved	= LTFALSE;
	m_nBackupSoundVar		= 1;
	//============================================
	
	//[SPIKE]
	m_bScopeOn		= LTFALSE;

	//[NOVICE]------------------------------------
	m_bServerPatch	= LTFALSE;
	//
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::~CInterfaceMgr
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CInterfaceMgr::~CInterfaceMgr()
{
	if (m_hScubaSound)
	{
        g_pLTClient->SoundMgr()->KillSound(m_hScubaSound);
	}

	if (m_hSound)
	{
        g_pLTClient->SoundMgr()->KillSound(m_hSound);
	}

	KillBGM();

	if (m_hFadeSurface)
	{
        g_pLTClient->DeleteSurface(m_hFadeSurface);
	}

	if (m_hLetterBoxSurface)
	{
        g_pLTClient->DeleteSurface(m_hLetterBoxSurface);
	}

	g_pInterfaceMgr = LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::Init
//
//	PURPOSE:	Init the mgr
//
// ----------------------------------------------------------------------- //
LTBOOL CInterfaceMgr::Init()
{
	if (!m_ScreenSpriteMgr.Init())
	{
        g_pLTClient->CPrint("ERROR in CInterfaceMgr::Init():  Could not initialize ScreenSpriteMgr!");
		return LTFALSE;
	}

    if (!m_CursorMgr.Init())
	{
        g_pLTClient->CPrint("ERROR in CInterfaceMgr::Init():  Could not initialize Cursor!");
        return LTFALSE;
	}

    g_vtDrawInterface.Init(g_pLTClient, "DrawInterface", NULL, 1.0f);

	g_vtLetterBox.Init(g_pLTClient, "LetterBox", NULL, 0.0f);
	g_vtLetterBoxDisabled.Init(g_pLTClient, "LetterBoxDisabled", NULL, 0.0f);
    g_vtLetterBoxFadeInTime.Init(g_pLTClient, "LetterBoxFadeInTime", NULL, 0.5f);
    g_vtLetterBoxFadeOutTime.Init(g_pLTClient, "LetterBoxFadeOutTime", NULL, 1.0f);
    g_vtDisableMovies.Init(g_pLTClient, "NoMovies", NULL, 0.0f);

    g_vtInterfaceFOVX.Init(g_pLTClient, "FovXInterface", NULL, 90.0f);
    g_vtInterfaceFOVY.Init(g_pLTClient, "FovYInterface", NULL, 75.0f);
	// 광식 : UI 상에 모델이 뚱뚱해 보여서..
	//g_vtInterfaceFOVX.Init(g_pLTClient, "FovXInterface", NULL, 80.0f);
    //g_vtInterfaceFOVY.Init(g_pLTClient, "FovYInterface", NULL, 60.0f);

	g_vtPauseTintAlpha.Init(g_pLTClient, "PauseTintAlpha", NULL, 0.65f);
	g_vtMainScreenFadeIn.Init(g_pLTClient, "MainScreenFadeInTime", NULL, 3.0f);
	
	g_vtExitLevelScreenFadeTime.Init( g_pLTClient, "ExitLevelScreenFadeTime", LTNULL, 1.0f );

#ifndef _FINAL
	g_vtPVModelModifyMode.Init( g_pLTClient, "PVModelModify", LTNULL, 0.0f );
#endif

	ProcessAdvancedOptions();

	// Create the Interface camera...
    uint32 dwWidth = 640;
    uint32 dwHeight = 480;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &dwWidth, &dwHeight);

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT(theStruct);

	theStruct.m_ObjectType = OT_CAMERA;

    m_hInterfaceCamera = g_pLTClient->CreateObject(&theStruct);
	_ASSERT(m_hInterfaceCamera);

    g_pLTClient->SetCameraRect(m_hInterfaceCamera, LTFALSE, 0, 0, dwWidth, dwHeight);
    g_pLTClient->SetCameraFOV(m_hInterfaceCamera, DEG2RAD(g_vtInterfaceFOVX.GetFloat()), DEG2RAD(g_vtInterfaceFOVY.GetFloat()));

	// read in the settings
    m_Settings.Init (g_pLTClient, g_pGameClientShell);

    if (!g_pLayoutMgr || !g_pLayoutMgr->Init())
	{
        g_pLTClient->CPrint("ERROR in CInterfaceMgr::Init():  Could not initialize LayoutMgr!");
        return LTFALSE;
	}
	hBackColor =  g_pLayoutMgr->GetBackColor();

    if (!m_InterfaceResMgr.Init())
	{
		// If we couldn't init, something critical must have happened (like no render dlls)

        g_pLTClient->CPrint("ERROR in CInterfaceMgr::Init():  Could not initialize InterfaceResMgr!");
        return LTFALSE;
	}

    if (!GetScreenMgr( )->Init())
	{
		// If we couldn't init, something critical must have happened
        g_pLTClient->CPrint("ERROR in CInterfaceMgr::Init():  Could not initialize ScreenMgr!");
        return LTFALSE;
	}

	m_PlayerInfoMgr.Init();

	if (!GetPlayerStats( )->Init())
	{
        g_pLTClient->CPrint("ERROR in CInterfaceMgr::Init():  Could not initialize Player Stats!");
        return LTFALSE;
	}
	if (!GetHUDMgr()->Init())
	{
        g_pLTClient->CPrint("ERROR in CInterfaceMgr::Init():  Could not initialize HUDMgr!");
        return LTFALSE;
	}

	m_WeaponChooser.Init();
	m_AmmoChooser.Init();

	m_CursorPos.x = 0;
	m_CursorPos.y = 0;

	g_fFovXTan = (float)tan( DEG2RAD( 85.0f * 0.5f ) );
	g_fFovYTan = (float)tan( DEG2RAD( 80.0f * 0.5f ) );

	m_MessageBox.Init();

	//[MURSUM]=============================
	/*
	if (!m_PerformanceMgr.Init()) 
	{
		g_pLTClient->ShutdownWithMessage("ERROR in CPerformanceMgr::Init():  Could not initialize PerformanceMgr!");
		m_PerformanceMgr.Term();
		return LTFALSE;
	}
	*/
	//=====================================


	if (!m_ProfileMgr.Init()) 
	{
		g_pLTClient->CPrint("ERROR in CProfileMgr::Init():  Could not initialize ProfileMgr!");
		m_ProfileMgr.Term();
		return LTFALSE;
	}

	// Create the surface used for making letterboxed cinematics...
    LTRect rcSrc;
	rcSrc.Init(0, 0, 2, 2);
    HLTCOLOR hTransColor = g_pLTClient->SetupColor1(1.0f, 1.0f, 1.0f, LTTRUE);

	m_hLetterBoxSurface = g_pLTClient->CreateSurface(2, 2);

	g_pLTClient->SetSurfaceAlpha(m_hLetterBoxSurface, 0.0f);
    g_pLTClient->FillRect(m_hLetterBoxSurface, &rcSrc, kBlack);
    g_pLTClient->OptimizeSurface(m_hLetterBoxSurface, hTransColor);

	if (!m_InterfaceFXMgr.Init(g_pLTClient,LTFALSE)) 
	{
		g_pLTClient->CPrint("ERROR in CClientFXMgr::Init():  Could not initialize interface FX mgr!");
		m_InterfaceFXMgr.Term();
		return LTFALSE;
	}
	m_InterfaceFXMgr.SetCamera(m_hInterfaceCamera);

	m_szMouseFXName[0] = 0;
	m_szSelectFXName[0] = 0;

	// Consider ourselves initialized.
	m_bInitialized = true;

    return LTTRUE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::Term()
//
//	PURPOSE:	Term the mgr
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::Term()
{
	// Turn off the loading screen if it's currently up
	if (m_LoadingScreen.IsActive())
		m_LoadingScreen.Hide();

	if (m_hGamePausedSurface)
	{
		g_pInterfaceResMgr->FreeSharedSurface(m_hGamePausedSurface);
		m_hGamePausedSurface = LTNULL;
	}

	if (m_hInterfaceCamera)
	{
        g_pLTClient->RemoveObject(m_hInterfaceCamera);
        m_hInterfaceCamera = LTNULL;
	}

	//make sure that the interface manager isn't still holding onto a reference
	m_InterfaceFXMgr.SetCamera(NULL);

	// ABM TODO term the subroutinemgr and ratingsmgr

	if( GetCurrentScreen() != SCREEN_ID_NONE )
	{
		GetScreenMgr()->GetCurrentScreen()->OnFocus(LTFALSE);
	}

	GetScreenMgr( )->Term();
	if (g_pLayoutMgr)
		g_pLayoutMgr->Term();
	GetPlayerStats( )->Term();
	GetHUDMgr()->Term();
	m_WeaponChooser.Term();
	m_AmmoChooser.Term();
	m_InterfaceResMgr.Term();

	m_MessageBox.Term();
	
	m_CursorMgr.Term();

	m_ScreenSpriteMgr.Term();

	if ((m_dwOrignallyEnabled & AO_SOUND) && !(m_dwAdvancedOptions & AO_SOUND))
	{
        g_pLTClient->RunConsoleString("SoundEnable 1");
	}

	if ((m_dwOrignallyEnabled & AO_MUSIC) && !(m_dwAdvancedOptions & AO_MUSIC))
	{
        g_pLTClient->RunConsoleString("MusicEnable 1");
	}

	if (m_hScubaSound)
	{
        g_pLTClient->SoundMgr()->KillSound(m_hScubaSound);
		m_hScubaSound = LTNULL;
	}

	if (m_hSound)
	{
        g_pLTClient->SoundMgr()->KillSound(m_hSound);
		m_hSound = LTNULL;
	}

	// Unititialized.
	m_bInitialized = false;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnEnterWorld()
//
//	PURPOSE:	Handle entering new world
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::OnEnterWorld(LTBOOL bRestoringGame)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	GetPlayerStats( )->OnEnterWorld(bRestoringGame);

	// Update every HUD element so they display accurate info
	GetHUDMgr()->QueueUpdate( kHUDAll );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnExitWorld()
//
//	PURPOSE:	Handle exiting a world
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::OnExitWorld()
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	GetPlayerStats( )->OnExitWorld();

	//[MURSUM]=================================
	/*
	m_InterfaceTimer.SetTime(0.0f,false);
	m_BlueInterfaceTimer.SetTime(0.0f,false);
	m_RedInterfaceTimer.SetTime(0.0f,false);
	*/
	g_pHUDTimer->SetTime( 0.0f, false );
	//==========================================

	if (m_AmmoChooser.IsOpen())
	{
		m_AmmoChooser.Close();
	}
	if (m_WeaponChooser.IsOpen())
	{
		m_WeaponChooser.Close();
	}


	for (int i =0; i < NUM_OVERLAY_MASKS; i++)
	{
		if (m_hOverlays[i])
            g_pLTClient->RemoveObject(m_hOverlays[i]);

        m_hOverlays[i] = LTNULL;
	}

    m_bFadeInitialized = LTFALSE;


}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreUpdate()
//
//	PURPOSE:	Handle pre-updates
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreUpdate()
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	// Don't clear the screen if the loading screen's up
	if (m_LoadingScreen.IsVisible())
		return LTTRUE;

	if (m_bClearScreenAlways)
	{
        g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);
	}
	else if (m_nClearScreenCount)
	{
        g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);
		m_nClearScreenCount--;
	}
	else
	{
        g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_RENDER);
	}

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostUpdate()
//
//  PURPOSE:    Handle post-updates (return LTTRUE to FLIP
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostUpdate()
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	if (m_eGameState != GS_LOADINGLEVEL)
	{
		if (!m_bSuppressNextFlip)
			g_pLTClient->FlipScreen(0);
		else
			m_bSuppressNextFlip = false;
	}

	//m_CursorMgr.CheckForReinit();

    return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::Update()
//
//	PURPOSE:	Handle updating the interface
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::Update()
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	// Update based on the game state...
    LTBOOL bHandled = LTFALSE;
	switch (m_eGameState)
	{
		case GS_PLAYING:
		{
			UpdatePlayingState();
            bHandled = LTFALSE;  // Allow further processing
		}
		break;

		case GS_POPUP:
		{
			UpdatePopupState();
            bHandled = LTFALSE;  // Allow further processing
		}
		break;

		case GS_EXITINGLEVEL:
		{
			UpdateExitingLevelState();
            bHandled = LTFALSE;
		}
		break;

		case GS_LOADINGLEVEL:
		{
			UpdateLoadingLevelState();
            bHandled = LTTRUE;
		}
		break;

		case GS_SCREEN :
		{
			UpdateScreenState();
            bHandled = LTTRUE;
		}
		break;

		case GS_PAUSED:
		{
			UpdatePausedState();
            bHandled = LTTRUE;
		}
		break;

		case GS_SPLASHSCREEN:
		{
			UpdateSplashScreenState();
            bHandled = LTTRUE;
		}
		break;

		case GS_MOVIE:
		{
			UpdateMovieState();
            bHandled = LTTRUE;
		}
		break;

		case GS_DEMOSCREEN:
		{
			UpdateDemoScreenState();
            bHandled = LTTRUE;
		}
		break;

		case GS_MANDATEHOST:
		{
			UpdateMandateHostState();
            bHandled = LTFALSE;
		}
		break;

		case GS_REJOINHOST:
		{
			UpdateRejoinHostState();
			bHandled = LTFALSE;
		}
		break;
	}

	//in playing state, message box is drawn in InterfaceMgr::Draw(),
	// otherwise draw it here
	if (bHandled)
	{
		if (m_MessageBox.IsVisible())
		{
			g_pLTClient->Start3D();
			g_pLTClient->StartOptimized2D();
			m_MessageBox.Draw();
			g_pLTClient->EndOptimized2D();
			g_pLTClient->End3D();
		}

		m_CursorMgr.Update();
	}

	m_ScreenSpriteMgr.Update();

	return bHandled;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::DrawSFX()
//
//	PURPOSE:	Renders the currently active special effects
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::DrawSFX()
{
	UpdateInterfaceSFX();
	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateScreenState()
//
//	PURPOSE:	Update screen state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdateScreenState()
{
	if (GetScreenMgr( )->GetCurrentScreenID() == SCREEN_ID_NONE)
	{
		SwitchToScreen(SCREEN_ID_MAIN);
	}
	
	// [KLS 9/2/02] FogEnabled is cleared in PreScreenState, however it is possible for
	// it to get set to 1 while in the screen state.  This will ensure that we never
	// render the screen state with fog enabled.  PostScreenState will return FogEnabled
	// to whatever it was before the screen state was entered.
	WriteConsoleInt("FogEnable", 0);

	/**/
	if( LTTRUE == m_bChangeModeReserved )
	{
		if( g_pLTClient->IsConnected() ) return;
		
		//[MURSUM]====================================================
		RMode rSaveMode;
		rSaveMode.m_Width		= g_pProfileMgr->GetCurrentProfile()->m_nScreenWidth;
		rSaveMode.m_Height		= g_pProfileMgr->GetCurrentProfile()->m_nScreenHeight;
		rSaveMode.m_BitDepth	= g_pProfileMgr->GetCurrentProfile()->m_nScreenDepth;
		
		if( rSaveMode.m_Width != USABLE_WIDTH_I || rSaveMode.m_Height != USABLE_HEIGHT_I )
		{
			g_pProfileMgr->GetCurrentProfile()->m_nScreenWidth  = USABLE_WIDTH_I;
			g_pProfileMgr->GetCurrentProfile()->m_nScreenHeight = USABLE_HEIGHT_I;
			g_pProfileMgr->GetCurrentProfile()->m_nScreenDepth  = USABLE_BPP_I;
			g_pProfileMgr->GetCurrentProfile()->ApplyResolution();
			// Save Resolution
			g_pProfileMgr->GetCurrentProfile()->m_nScreenWidth  = rSaveMode.m_Width;
			g_pProfileMgr->GetCurrentProfile()->m_nScreenHeight = rSaveMode.m_Height;
			g_pProfileMgr->GetCurrentProfile()->m_nScreenDepth  = rSaveMode.m_BitDepth;
			
			g_pProfileMgr->GetCurrentProfile()->SetChangeOptionSetting();  // Profile에 저장   		
		}
		//================================================================
		m_bChangeModeReserved = LTFALSE;		
	}
	


	if (!GetScreenMgr()->UpdateInterfaceSFX())
		m_bSuppressNextFlip = true;
	else
	{
		// 광식 [05.03.18] : Model을 구분하기. UI 쪽으로 Character 가져오기...
		//-------------------------------------------------------------------------

		//[MURSUM]
		// -> ScreenMgr::Render() 안으로 이동

		g_pLTClient->Start3D();
		
		//g_pLTClient->StartOptimized2D();

		m_InterfaceResMgr.DrawScreen();
		UpdateScreenFade();

		//g_pLTClient->EndOptimized2D();		
		//DrawSFX();

		g_pLTClient->End3D();
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdatePlayingState()
//
//	PURPOSE:	Update playing state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdatePlayingState()
{
	GetPlayerStats()->Update();

	// Update auto chooser switching...
	m_WeaponChooser.Update();
	m_AmmoChooser.Update();

	// Update the player stats...
	GetHUDMgr()->Update();

	if( g_pLTClient->GetTime() - m_fLastNetStateTime > 5.0f )
	{
		int nCurrNetState = g_pLTClient->GetNetState();
		//if( nCurrNetState != m_nLastNetState )
		{
			m_nLastNetState = nCurrNetState;
			g_Network.CS_NetLoss( m_nLastNetState );

			m_PlayerInfoMgr.SetPlayerPing( g_GameDoc.GetUniqueIndex(), m_nLastNetState );
		}

		m_fLastNetStateTime	= g_pLTClient->GetTime();
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdatePopupState()
//
//	PURPOSE:	Update dialogue state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdatePopupState()
{
	ChangeState(m_eLastGameState);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateExitingLevelState()
//
//	PURPOSE:	Update exiting level state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdateExitingLevelState()
{
	// Update the screen fade.
// Edited By IN'T ==>
// Time : 2004-05-04 오후 7:55:59
//	UpdateScreenFade();
// Edited By IN'T <==

	// Check if screen is done fading out or is fading in at all.
// Edited By IN'T ==>
// Time : 2004-05-04 오후 7:56:09
//	if( ScreenFadedOut( ) || FadingScreenIn( ))
// Edited By IN'T <==
	{
		if (g_pMissionMgr->IsExitingMission())
		{
			switch( g_pGameClientShell->GetGameType( ))
			{
				case eGameTypeTeamDeathmatch:
					//SwitchToScreen(SCREEN_ID_END_DM_MISSION);
					if( !g_pMissionMgr->IsExitingMission() && g_pMissionMgr->IsExitingLevel() )
					{
						//g_pInterfaceMgr->SetLoadingRenderScreen( this );

						g_pInterfaceMgr->SkipPreLoad( true );
	
						// Handle restarting the round...

						g_pMissionMgr->FinishExitLevel( );
						g_pMissionMgr->FinishStartGame( );

					}
					break;
				default:
					MessageBox( NULL, "GameType에러_1  CInterfaceMgr::UpdateExitingLevelState()", "에러", MB_OK|MB_ICONERROR );
					break;
			}
		}
		else
		{
			switch( g_pGameClientShell->GetGameType( ))
			{
				case eGameTypeTeamDeathmatch:
					//SwitchToScreen(SCREEN_ID_END_DM_MISSION);
					{
						//CAutoMessage cMsg;
						//cMsg.Writeuint8( MID_START_GAME );
						//g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
					}
					break;				
				default:					
				{
					/*
					CScreenPreload *pPreload = (CScreenPreload *) (GetScreenMgr( )->GetScreenFromID(SCREEN_ID_PRELOAD));
					if (pPreload)
					{
						pPreload->SetWaitingToExit(true);
						//SwitchToScreen(SCREEN_ID_PRELOAD);

					}
					else
					{
						g_pMissionMgr->FinishExitLevel();
					}
					*/
					g_pLTClient->CPrint( "ERROR ExitLevel %s %d", __FILE__, __LINE__ );
				}
				break;
			}			
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateLoadingLevelState()
//
//	PURPOSE:	Update loading level state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdateLoadingLevelState()
{
	if (g_pPlayerMgr->IsPlayerInWorld())
	{
		// Make sure the loading screen isn't busy
		m_LoadingScreen.Pause();
		// Let the game client shell do stuff that's time consuming so we're not stuck on a black screen
		g_pGameClientShell->PostLevelLoadFirstUpdate();
		// Turn off the loading screen
		m_LoadingScreen.Hide();

		g_pGameClientShell->SendClientLoadedMessage();
		g_pInterfaceMgr->ChangeState(GS_PLAYING);
	}
	else if ((m_bLoadFailed) || (g_pLTClient->IsConnected() && IsKeyDown(VK_ESCAPE)))
	{
		m_bLoadFailed = LTFALSE;
		m_LoadingScreen.Hide();

		this->SetExitMessage( NF_GetMsg(SA_TEXT_GAME_LOAD_FAILED) );

		g_Network.CS_ExitGameRequest();	
		g_Network.EnableNetwork(LTTRUE);
	}
	else if (!m_LoadingScreen.IsActive() && m_LoadingScreen.IsVisible())
	{
		// Update the loading screen if it's not active, but it is visible
		// so we can see the console on failure conditions.
		m_LoadingScreen.Update();
	}
}

void CInterfaceMgr::UpdateMandateHostState()
{
	UpdatePlayingState();

	if (g_pPlayerMgr->IsPlayerInWorld())
	{
		g_pGameClientShell->PostLevelLoadFirstUpdate();		
		g_pGameClientShell->SendClientLoadedMessage();
		g_pInterfaceMgr->ChangeState(GS_PLAYING);
	}
}

void CInterfaceMgr::UpdateRejoinHostState()
{
	UpdatePlayingState();

	if (g_pPlayerMgr->IsPlayerInWorld())
	{
		g_pGameClientShell->PostLevelLoadFirstUpdate();	
		g_pGameClientShell->SendClientLoadedMessage();
		g_pInterfaceMgr->ChangeState(GS_PLAYING);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::HideLoadScreen()
//
//	PURPOSE:	Called externally to hide the loading screen
//
// ----------------------------------------------------------------------- //
void CInterfaceMgr::HideLoadScreen()
{
	m_LoadingScreen.Hide();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdatePausedState()
//
//	PURPOSE:	Update paused state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdatePausedState()
{
    g_pLTClient->Start3D();

	UpdateInterfaceSFX();

    g_pLTClient->StartOptimized2D();

	LTRect rcFull(0,0,g_pInterfaceResMgr->GetScreenWidth()-1,g_pInterfaceResMgr->GetScreenHeight()-1);
	g_pLTClient->FillRect(g_pLTClient->GetScreenSurface(),&rcFull,hBackColor);


	if (m_hGamePausedSurface)
	{
        g_pLTClient->ScaleSurfaceToSurface(g_pLTClient->GetScreenSurface(), m_hGamePausedSurface, &rcFull, LTNULL);
	}

    g_pLTClient->EndOptimized2D();
    g_pLTClient->End3D();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateSplashScreenState()
//
//	PURPOSE:	Update splash screen state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdateSplashScreenState()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::EndSplashScreen
//
//	PURPOSE:	Ends the splash screen.
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::EndSplashScreen()
{
	//[MURSUM]

	/*
	// If the user didn't start a join from the command line, proceed
	// like normal to the movies.
	if( !GetCommandLineJoin( ))
	{
		ChangeState(GS_MOVIE);
		return;
	}

	// If user is joining a mp game from the command line,
	// start the connection here.
	char szIP[256] = "";
	LTStrCpy( szIP, GetConsoleTempString( "join", "" ), ARRAY_LEN( szIP ));

	char szPassword[256];
	LTStrCpy( szPassword, GetConsoleTempString( "password", "" ), ARRAY_LEN( szPassword ));

	char szGameType[256] = "";
	LTStrCpy( szGameType, GetConsoleTempString( "gametype", "" ), ARRAY_LEN( szGameType ));

	if( szGameType[0] == 0 )
	{
		LTStrCpy( szGameType, GameTypeToString( eGameTypeDeathmatch ), ARRAY_LEN( szGameType ));
	}
	
	GameType eGameType = GameStringTypeToGameType( szGameType );
	g_pGameClientShell->SetGameType( eGameType );

	if( !szIP[0] || !g_pClientMultiplayerMgr->SetupClient( szIP, NULL, szPassword ) || !g_pMissionMgr->StartGameAsClient( ))
	{
		// drop them into the join menu
		LoadFailed( SCREEN_ID_MAIN );

		MBCreate mb;
		ShowMessageBox(IDS_CANT_CONNECT_TO_SERVER, &mb);
	}
	*/
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateMovieState()
//
//	PURPOSE:	Update movie state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdateMovieState()
{
	ILTVideoMgr* pVideoMgr = g_pLTClient->VideoMgr();
	if (!pVideoMgr || !m_hMovie)
	{
		SwitchToScreen(SCREEN_ID_MAIN);
		return;
	}

	// Update and check if we're finished...

	if (pVideoMgr->UpdateVideo(m_hMovie) != LT_OK ||
		pVideoMgr->GetVideoStatus(m_hMovie) == LT_FINISHED)
	{
		NextMovie();
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateDemoScreenState()
//
//	PURPOSE:	Update demo screen state
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::UpdateDemoScreenState()
{
#ifdef _TRON_E3_DEMO
	s_fDemoTime = 0.0f;
#endif

	if (m_bSeenDemoScreens)
	{
		if (m_bQuitAfterDemoScreens)
		{
			g_pLTClient->Shutdown();
		}
		else
		{
			SwitchToScreen(SCREEN_ID_MAIN);
		}
		return;
	}

    HSURFACE hScreen = g_pLTClient->GetScreenSurface();
    uint32 nWidth = 0;
    uint32 nHeight = 0;

    g_pLTClient->GetSurfaceDims(hScreen, &nWidth, &nHeight);

    LTRect rcDst;
	rcDst.Init(0, 0, nWidth, nHeight);

    g_pLTClient->GetSurfaceDims(g_hDemo, &nWidth, &nHeight);

    LTRect rcSrc;
	rcSrc.Init(0, 0, nWidth, nHeight);

    g_pLTClient->Start3D();
    g_pLTClient->StartOptimized2D();

	if (rcSrc.right > rcDst.right)
	{
	    g_pLTClient->ScaleSurfaceToSurface(hScreen, g_hDemo, &rcDst, &rcSrc);
	}
	else
	{
		int xDest = (rcDst.right - (int)nWidth) / 2;
		int yDest = (rcDst.bottom - (int)nHeight) / 2;
		g_pLTClient->DrawSurfaceToSurface(hScreen, g_hDemo, &rcSrc, xDest, yDest);
	}

    g_pLTClient->EndOptimized2D();
    g_pLTClient->End3D();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::SetupMusic()
//
//	PURPOSE:	Handles changes to the state of the menu / level music
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::SetupMusic()
{
	CMusic* pMusic = g_pGameClientShell->GetMusic();
    if (!pMusic) return LTFALSE;

	// Stop the music if it isn't enabled...

	if (!m_Settings.MusicEnabled() || !GetConsoleInt("MusicActive",0))
	{
		pMusic->Stop();
		return LTTRUE;
	}



	// Use the current level's music if we have a level loaded...

	if ( g_pLTClient->IsConnected() )
	{
		// Re-initialize music if necessary...

		if (!pMusic->IsInitialized())
		{
			if (!pMusic->Init(g_pLTClient))
			{
				return LTFALSE;
			}
		}
 
		g_pGameClientShell->RestoreMusic();

		pMusic->Play();
		
		return LTTRUE;
	}


	// Handle using the menu music if necessary...this should only occur if we
	// aren't in a level...

	CMusicState* pMS = pMusic->GetMusicState();

	int nIntensity = g_pLayoutMgr->GetScreenMusicIntensity(GetScreenMgr( )->GetCurrentScreenID());

	if (pMusic->IsPlaying())
	{
		if (pMS->nIntensity != nIntensity)
		{
			pMusic->ChangeIntensity(nIntensity);
		}
		return LTTRUE;
	}

	// Setup the menu music...

	char szFile[128] = "";
	g_pClientButeMgr->GetInterfaceAttributeString("MenuMusicCtrlFile",szFile,sizeof(szFile));
    if (!strlen(szFile)) return LTFALSE;

	if (!pMusic->IsInitialized())
	{
        if (!pMusic->Init(g_pLTClient))
		{
            return LTFALSE;
		}
	}

	if (!pMusic->IsLevelInitialized())
	{
		char szDir[128] = "";
		g_pClientButeMgr->GetInterfaceAttributeString("MenuMusicDir",szDir,sizeof(szDir));

		if (!strlen(szDir))
		{
            return LTFALSE;
		}

		CMusicState MusicState;
		MusicState.nIntensity = nIntensity;
		strcpy(MusicState.szDirectory, szDir);
		strcpy(MusicState.szControlFile, szFile);

		if (!pMusic->RestoreMusicState(MusicState))
		{
			return LTFALSE;
		}
	}

	pMusic->Play();

	if (pMS->nIntensity != nIntensity)
	{
		pMusic->ChangeIntensity(nIntensity);
	}

	m_ProfileMgr.ImplementMusicVolume();

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnMessage()
//
//	PURPOSE:	Handle interface messages
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::OnMessage(uint8 messageID, ILTMessage_Read *pMsg)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
	{
		return LTFALSE;
	}

	switch(messageID)
	{
		case MID_PLAYER_INFOCHANGE :
		{
            uint8 nThing    = pMsg->Readuint8();
            uint8 nType1    = pMsg->Readuint8();
            uint8 nType2    = pMsg->Readuint8();
            LTFLOAT fAmount  = pMsg->Readfloat();

			UpdatePlayerStats(nThing, nType1, nType2, fAmount);

            return LTTRUE;
		}
		break;

		case MID_PLAYER_HEALTH:
		{
			uint8 nHealth = pMsg->Readuint8();
			GetPlayerStats( )->UpdateHealth(nHealth);
		}
		break;

		case MID_PLAYER_ARMOR:
		{
			uint8 nArmor = pMsg->Readuint8();
			GetPlayerStats( )->UpdateArmor(nArmor);
		}
		break;

		case MID_PICKUP_WEAPON:
		{
			uint8 nWeaponID		= pMsg->Readuint8();
            uint8 nAmmoInClip	= pMsg->Readuint8();
            LTFLOAT fAmount		= pMsg->Readfloat();

			g_pGameClientShell->HandleWeaponPickup( nWeaponID,(uint32)fAmount, nAmmoInClip );
		}
		break;			

		case MID_GEAR_PICKEDUP :
		{
            uint8 nGearId = pMsg->Readuint8();
			bool bPickedUp = pMsg->Readbool();

			g_pGameClientShell->HandleGearPickup(nGearId, bPickedUp);

			if (bPickedUp)
			{
				GetPlayerStats( )->UpdateGear(nGearId);
			}
            return LTTRUE;
		}
		break;

		case MID_INTEL_PICKEDUP :
		{
			uint32 nTextId = pMsg->Readuint32();
			uint8 nPopupId = pMsg->Readuint8();
			LTBOOL bIsIntel = (LTBOOL)pMsg->Readuint8();
			LTBOOL bShow = (LTBOOL)pMsg->Readuint8();
			LTBOOL bAdd = (LTBOOL)pMsg->Readuint8();

			if (bShow)
			{
				ShowPopup(nTextId, nPopupId, true);
			}

			if (bAdd)
			{
				GetPlayerStats( )->UpdateIntel(nTextId,nPopupId,bIsIntel);
			}
		}
		break;

		case MID_PLAYER_TEAM :
		{			
			return LTTRUE;
		}
		break;

		case MID_PLAYER_INFO:
		{
			// Only do something if we're in multiplayer...

			if (!IsMultiplayerGame()) break;

			uint32	nID				= pMsg->Readuint8();
			HOBJECT	hObj			= pMsg->ReadObject();
			HOBJECT	hWeaponModel	= pMsg->ReadObject();
			int32	nServerIndex	= pMsg->Readint32();
			uint8	nState			= pMsg->Readuint8();
			CCharacterFX* pFX;

			m_PlayerInfoMgr.SetPlayerInfoFromP2P( nServerIndex, nID, hObj, (PlayerState)nState );
			pFX = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(hObj);
			if( pFX )
			{
				pFX->SetCurrentWeapon(hWeaponModel);
			}
			         
			return LTTRUE;
		}
		break;

		case MID_TEAM_INFO:
		{
			CTeamMgr::Instance().UpdateTeam(pMsg);
		}
		break;

		case MID_PLAYER_SCORED:
		{
			//Only do something if we're in multiplayer...

			if ( GetGameState() == GS_LOADINGLEVEL ) break;

            uint32 nLocalID = 0;
            g_pLTClient->GetLocalClientID(&nLocalID);

            uint8 nVictim		= pMsg->Readuint8();
            uint8 nScorer		= pMsg->Readuint8();
			uint8 nWeaponID		= pMsg->Readuint8();
			uint8 nHitNode		= pMsg->Readuint8();

			T_PLAYER_INFOptr	pVictim	= m_PlayerInfoMgr.GetPlayerByID(nVictim);
			T_PLAYER_INFOptr	pScorer	= m_PlayerInfoMgr.GetPlayerByID(nScorer);
			if( !pScorer || !pVictim ) return LTFALSE;

#ifndef _FINAL
			if( GetConsoleInt( "CamTest", 0 ) > 0 )
			{
				return LTTRUE;
			}
#endif
			g_Network.CS_Kill( pScorer->nServerIndex, pVictim->nServerIndex, !!(NODE_HEAD == nHitNode), nWeaponID );
						
			return LTTRUE;
		}
		break;

		case MID_PLAYER_OVERLAY :
		{
			// retrieve the string from the message, play the chat sound, and display the message

            uint8 nId = pMsg->Readuint8();
            bool bOn = !!pMsg->Readuint8();

			if (nId < NUM_OVERLAY_MASKS)
			{
				if (bOn)
					CreateOverlay((eOverlayMask)nId);
				else
					RemoveOverlay((eOverlayMask)nId);
				return LTTRUE;
			}
			return LTFALSE;
		}
		break;

		case MID_LOAD_FAILED:
		{
			LoadFailed();
			return LTFALSE; 
		}
		break;

		case MID_SERVER_TIME:
		{
			float fTimeLeft	= pMsg->Readfloat();
			g_pHUDTimer->SetTime( fTimeLeft, false );
			return LTTRUE;
		}
		break;

		default : break;
		
		
	}   
	return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnEvent()
//
//	PURPOSE:	Called for asynchronous errors that cause the server
//				to shut down
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::OnEvent(uint32 dwEventID, uint32 dwParam)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	switch (dwEventID)
	{
		// Called when the renderer has switched into
		// the new mode but before it reloads all the textures
		// so you can display a loading screen.

		case LTEVENT_RENDERALMOSTINITTED :
		{
			if (m_bSwitchingModes)
			{
				ClearAllScreenBuffers();

				// because of driver bugs, we need to wait a frame after reinitializing the renderer and
				// reinitialize the cursor
				//m_CursorMgr.ScheduleReinit(2.0f);
			}
		}
		break;

		// Client disconnected from server.  dwParam will
		// be a error flag found in de_codes.h.

		case LTEVENT_DISCONNECT :
		{
			if( g_pGameClientShell->ShouldUseRadar() )
			{
				g_pRadar->Reset();
			}

			// 광식
			//------------------------------------------
			if (m_LoadingScreen.IsActive())
				m_LoadingScreen.Hide();
			Disconnected(dwParam);
			//------------------------------------------
			
			m_bIntentionalDisconnect = false;
		}
		break;

		// Engine shutting down.  dwParam will be a error
		// flag found in de_codes.h.

		case LTEVENT_SHUTDOWN :
		break;

		// The renderer was initialized.  This is called if
		// you call SetRenderMode or if focus was lost and regained.

		case LTEVENT_RENDERINIT :
		{
			g_pInterfaceResMgr->OnResetDevice();

			if (m_LoadingScreen.IsVisible())
			{
				// Hide it to get rid of any resources that may have been around
				m_LoadingScreen.Hide();
				// And then start it up again
				m_LoadingScreen.Show();
			}

		}
		break;

		// The renderer is being shutdown.  This happens when
		// ShutdownRender is called or if the app loses focus.

		case  LTEVENT_RENDERTERM :
		{
			// Stop drawing the loading screen
			if (m_LoadingScreen.IsVisible())
				m_LoadingScreen.Pause();

			g_pInterfaceResMgr->OnLostDevice();
		}
		break;

        case LTEVENT_LOSTFOCUS:
		case LTEVENT_GAINEDFOCUS:
		break;

		default :
		{
            uint32 nStringID = IDS_UNSPECIFIEDERROR;
			SwitchToScreen(SCREEN_ID_MAIN);
			//DoMessageBox(nStringID, TH_ALIGN_CENTER);
		}
		break;
	}

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdatePlayerStats()
//
//	PURPOSE:	Update the player's stats
//
// ----------------------------------------------------------------------- //

// ABM TODO move this function out of base class into TronInterfaceMgr.
void CInterfaceMgr::UpdatePlayerStats(uint8 nThing, uint8 nType1,
                                      uint8 nType2, LTFLOAT fAmount)
{
	switch (nThing)
	{
		case IC_OBJECTIVE_ID :
		case IC_OPTION_ID :
		case IC_PARAMETER_ID :
		{
            GetPlayerStats( )->UpdateObjectives(nThing, nType1, (uint32)fAmount);
		}
		break;
		
		default : break;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnCommandOn()
//
//	PURPOSE:	Handle command on
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::OnCommandOn(int command)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	if (g_pChatInput->IsVisible()) return LTTRUE;
     
	switch (command)
	{
		// 광식
		//---------------------------------------------
		case COMMAND_ID_ACTIVATE :
		{
			if( !g_pPlayerMgr->IsPlayerDead() )
			{
				g_pPlayerMgr->WeaponThrow();
			}
		}break;
		//---------------------------------------------

		case COMMAND_ID_LASTWEAPON :
		{
			//[SPIKE]=============================================================
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;

			CClientWeaponMgr const *pClientWeaponMgr = g_pPlayerMgr->GetClientWeaponMgr();
			IClientWeaponBase const *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
			if (pClientWeapon && !pClientWeaponMgr->WeaponsEnabled()) return LTTRUE;

			if (m_AmmoChooser.IsOpen())
			{
				m_AmmoChooser.Close();
			}
			if (m_WeaponChooser.IsOpen())
			{
				m_WeaponChooser.Close();
			}

			return LTTRUE;

		}
		break;

		case COMMAND_ID_PREV_WEAPON :
		{
			//[SPIKE]=============================================================
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() && g_pPlayerMgr->IsDisabling() )
				return LTTRUE;

			if (m_AmmoChooser.IsOpen())
			{
				m_AmmoChooser.Close();
			}
			if (m_WeaponChooser.Open(0))
			{
				m_WeaponChooser.PrevWeapon();
			}
			if (IsChoosingWeapon())
			{
				uint8 nCurrWeapon = m_WeaponChooser.GetCurrentSelection();
				m_WeaponChooser.Close();
				g_pPlayerMgr->ChangeWeapon( nCurrWeapon );
			}
			//====================================================================
            return LTTRUE;
		}
		break;

		case COMMAND_ID_NEXT_WEAPON	:
		{
			//[SPIKE]=============================================================
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;
			
			if (m_AmmoChooser.IsOpen())
			{
				m_AmmoChooser.Close();
			}
			if (m_WeaponChooser.Open(m_WeaponChooser.GetCurrentClass()))
			{
				m_WeaponChooser.NextWeapon(m_WeaponChooser.GetCurrentClass());
			}
			if (IsChoosingWeapon())
			{
				uint8 nCurrWeapon = m_WeaponChooser.GetCurrentSelection();
				m_WeaponChooser.Close();
				g_pPlayerMgr->ChangeWeapon( nCurrWeapon );
			}
			//====================================================================
            return LTTRUE;
		} break;

		case COMMAND_ID_STRAFE :
		case COMMAND_ID_NEXT_AMMO :
		case COMMAND_ID_HOLSTER :
		case COMMAND_ID_MOVE_BODY :
		case COMMAND_ID_DUCKLOCK :
		case COMMAND_ID_COMPASS :
		{
			//[SPIKE]=============================================================
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;

			g_pPlayerMgr->SetWeaponChanging(LTTRUE);
			//=================================================

			NextWeapon(command);
			//----------------------------------------------------------------------------------
			// [ bestline ] 2004.05.07
			// 선택없이 바로 되도록
			if (IsChoosingWeapon())
			{
				uint8 nCurrWeapon = m_WeaponChooser.GetCurrentSelection();
				m_WeaponChooser.Close();
				g_pPlayerMgr->ChangeWeapon( nCurrWeapon );

				return LTTRUE;

			}
			else if (IsChoosingAmmo())
			{
				uint8 nCurrAmmo = m_AmmoChooser.GetCurrentSelection();
				m_AmmoChooser.Close();

				IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
				if ( pClientWeapon )
				{
					pClientWeapon->ChangeAmmoWithReload( nCurrAmmo );
				}

				return LTTRUE;
			}
			//----------------------------------------------------------------------------------

            return LTTRUE;
		}
		break;

		case COMMAND_ID_FIRING :
		{

			if (IsChoosingWeapon() || IsChoosingAmmo())
			{
                return LTTRUE;
			}
		}
		break;

		case COMMAND_ID_NEXT_LAYOUT :
		{
			/*
			if (m_eGameState == GS_PLAYING)
			{
				GetHUDMgr()->NextLayout();
	            g_pClientSoundMgr->PlayInterfaceSound("Interface\\Snd\\HudLayoutToggle.wav");
			}
			*/
            return LTTRUE;

		}
		break;
		case COMMAND_ID_PREV_LAYOUT :
		{
			/*
			if (m_eGameState == GS_PLAYING)
			{
				GetHUDMgr()->PrevLayout();
	            g_pClientSoundMgr->PlayInterfaceSound("Interface\\Snd\\HudLayoutToggle.wav");
			}
			*/
            return LTTRUE;

		}
		break;

		case COMMAND_ID_MISSION:
		{
			if (m_eGameState == GS_PLAYING )
			{
				g_pScores->Show(true);
				return LTTRUE;
			}
		} break;

		default :
		break;
	}

    return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnCommandOff()
//
//	PURPOSE:	Handle command off
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::OnCommandOff(int command)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	// Only process if not editing a message...

    if(g_pChatInput->IsVisible()) return LTTRUE;

	switch (command)
	{
		case COMMAND_ID_ACTIVATE :
		{
            return LTFALSE;
		}
		break;

		case COMMAND_ID_FIRING :
		{
		//--------------------------------------------------------------------
		// [ bestline ] 2004.05.07
		// 선택메뉴 구조 변경 작업 , 키로 바로 선택되도록 합니다.
		/*	if (IsChoosingWeapon())
			{
                uint8 nCurrWeapon = m_WeaponChooser.GetCurrentSelection();
				m_WeaponChooser.Close();
				g_pPlayerMgr->ChangeWeapon( nCurrWeapon );

                return LTTRUE;

			}
			else if (IsChoosingAmmo())
			{
                uint8 nCurrAmmo = m_AmmoChooser.GetCurrentSelection();
				m_AmmoChooser.Close();

				IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
				if ( pClientWeapon )
				{
					pClientWeapon->ChangeAmmoWithReload( nCurrAmmo );
				}

                return LTTRUE;
			}*/
		//--------------------------------------------------------------------
			return LTTRUE;
		}
		break;

		case COMMAND_ID_LASTWEAPON :
		{
			// 광식
			//-------------------------------------------------------------
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;
			//-------------------------------------------------------------

			ASSERT( 0 != g_pPlayerMgr );
			g_pPlayerMgr->LastWeapon( );			
			g_pWpnChooser->LastWeapon();
			return LTTRUE;
		}
		break;

		case COMMAND_ID_STRAFE :
		case COMMAND_ID_NEXT_AMMO :
		case COMMAND_ID_HOLSTER :
		case COMMAND_ID_MOVE_BODY :
		case COMMAND_ID_DUCKLOCK :
		case COMMAND_ID_COMPASS :
		{
			// 광식
			//-------------------------------------------------------------
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;
			//-------------------------------------------------------------

			//[SPIKE] 
			g_pPlayerMgr->SetWeaponChanging(LTFALSE);
			//=================================================

			// End the timer for quick weapon changing...
			m_WeaponChooser.EndAutoSwitch();			
		}
		break;

		case COMMAND_ID_PREV_WEAPON :
		{
			
			// 광식
			//-------------------------------------------------------------
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;
			//-------------------------------------------------------------

			// End the timer for quick weapon changing...
			m_WeaponChooser.EndAutoSwitch();	
			
		}
		break;
		
		case COMMAND_ID_NEXT_WEAPON :
		{	
			// 광식
			//-------------------------------------------------------------
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;
			//-------------------------------------------------------------

			// End the timer for quick weapon changing...
			m_WeaponChooser.EndAutoSwitch();	
			
		}
		break;
		
		case COMMAND_ID_MISSION:
		{
			g_pScores->Show(false);
		} break;

		default : break;
	}

    return LTFALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnKeyDown()
//
//	PURPOSE:	Handle OnKeyDown messages
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::OnKeyDown(int key, int rep)
{
	// Make sure we're initialized.
	if ( !IsInitialized( ) )
	{
		return LTFALSE;
	}

	if (m_MessageBox.IsVisible())
	{
		return m_MessageBox.HandleKeyDown(key,rep);
	}

	switch (m_eGameState)
	{
		case GS_SCREEN :
		{
			GetScreenMgr( )->HandleKeyDown(key,rep);
            return LTTRUE;
		}
		break;

		case GS_PAUSED :
		{
			// They pressed a key - unpause the game

			ChangeState(GS_PLAYING);
            return LTTRUE;
		}
		break;

		case GS_SPLASHSCREEN :
		{
			// They pressed a key - end splash screen...
 			EndSplashScreen( );
			return LTTRUE;
		}
		break;

		case GS_MOVIE :
		{
			// They pressed a key - end splash screen...

			NextMovie();
            return LTTRUE;
		}
		break;

		case GS_DEMOSCREEN :
		{
			// They pressed a key - go to next screen...
			NextDemoScreen();
            return LTTRUE;
		}
		break;


		case GS_POPUP :
		break;

		// ABM TODO write handlers for the subroutine and ratings states

		case GS_PLAYING :
		{
			// Are We Broadcasting a Message
			if (g_pChatInput->IsVisible())
			{
			//	g_pChatInput->HandleKeyDown(key, rep);
				return LTTRUE;
			}

			switch (key)
			{
				case VK_PAUSE:
				{
					if (IsMultiplayerGame()) return LTFALSE;

					if (!g_pGameClientShell->IsGamePaused())
					{
						ChangeState(GS_PAUSED);
					}

					g_pGameClientShell->PauseGame(!g_pGameClientShell->IsGamePaused(), LTTRUE);
					return LTTRUE;
				}
				break;

				// Escape Key Handling
				case VK_ESCAPE:
				{
					//[yaiin]
					//도움말 HUD는 다른 어떤 HUD보다 상위다.
					//하지만 VK_ESCAPE에는 비활성화 함.
					g_pHelpBoard->Show(LTFALSE);

					bool bHandled = false;
					if (!g_pPlayerMgr->IsPlayerDead() && g_pPlayerMgr->IsPlayerInWorld())
					{
						if (IsChoosingWeapon())
						{
							m_WeaponChooser.Close();
							bHandled = true;
						}
						if (IsChoosingAmmo())
						{
							m_AmmoChooser.Close();
							bHandled = true;
						}
					}

					if (bHandled)
					{
						return LTTRUE;
					}
					else
					{
						if( PS_GHOST == g_pPlayerMgr->GetPlayerState() )
						{
							ShowMessageBox( SA_TEXT_ASK_SURE_EXIT, LTMB_YESNO, ExitGameCallBack, LTNULL );
						}
						else
						{
							//"게임을 종료하시겠습니까?\r\n\r\n게임 중 퇴장하시면 경험치 -1000\r\n\r\n게임머니 -500의 패널티가 부여됩니다."
							ShowMessageBox( SA_TEXT_HELP_EXIT_GAME_PENALTY, LTMB_YESNO, ExitGameCallBack, LTNULL );
						}
					}
				}
				break;

				//[yaiin] : 누른 상태에서 도움말 HUD 켜기
				//CInterfaceMgr에 직접 넣은 이유는 HUDHelp.h 참조
				case VK_F1:
				{
					g_pHelpBoard->Show(LTTRUE);
				}
				break;

				default : break;
			}
		}
		break;

		default : break;
	}


	return LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnKeyUp(int key)
//
//	PURPOSE:	Handle key up notification
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::OnKeyUp(int key)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	// if it's the tilde (~) key then the console has been turned off
	// (this is the only event that causes this key to ever get processed)
	// so clear the back buffer to get rid of any part of the console still showing
	if (key == VK_TILDE)
	{
		AddToClearScreenCount();
        return LTTRUE;
	}

	if(g_pChatInput->IsVisible()) return LTTRUE;

	switch (m_eGameState)
	{
	case GS_SCREEN:
		{
			GetScreenMgr( )->HandleKeyUp(key);
			return LTTRUE;
		}
		break;

	case GS_LOADINGLEVEL :
		{
            return LTTRUE;
		}
		break;
	case GS_POPUP:
		{
			KeystrokeList::iterator iter = g_keys.begin();
			while (iter != g_keys.end() && (*iter) != key)
				iter++;
			bool bKeepLocked = false;
			if (iter != g_keys.end() && (*iter) == key)
			{
				g_keys.erase(iter);
				bKeepLocked = !g_keys.empty();
			}
			g_bLockPopup &= bKeepLocked;
		}
		break;
	case GS_PLAYING: //[yaiin] : 게임 중 도움말 HUD 끄기
		{
			if(key == VK_F1)
			{
				g_pHelpBoard->Show(LTFALSE);
			}
		}
		break;
		
	}

    return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnChar()
//
//	PURPOSE:	Handle OnChar messages
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::OnChar(unsigned char c, uint32 lParam)
{
	if (c < ' ' && lParam>=0x00100000)
	{
		if (m_eGameState == GS_SCREEN)
		{
			GetScreenMgr( )->HandleHotKey(c);
		}
		return;
	}


	if (m_MessageBox.IsVisible())
	{
		m_MessageBox.HandleChar(c);
		return;
	}

	if (g_pChatInput && g_pChatInput->IsVisible())
	{
		return;
	}

	if (m_eGameState == GS_SCREEN)
	{
		GetScreenMgr( )->HandleChar(c);
	}

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::Draw()
//
//	PURPOSE:	Draws any interface stuff that may need to be drawn
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::Draw()
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	// Tint the screen if necessary...

	// Check for interface drawing override...

	if (g_vtDrawInterface.GetFloat())
	{
		// Update letter box...

		UpdateLetterBox();

		// Find out if we're in multiplayer...

		PlayerState ePlayerState = g_pPlayerMgr->GetPlayerState();

		if (GetGameState() == GS_PLAYING ||
			GetGameState() == GS_POPUP ||
			GetGameState() == GS_MANDATEHOST ||
			GetGameState() == GS_REJOINHOST )
		{
			// Draw the player stats (health,armor,ammo) if appropriate...
			GetHUDMgr()->Render();

			//[MURSUM]========================================
			/*
			if (m_InterfaceTimer.GetTime() > 0.0f)
			{
				m_InterfaceTimer.Draw();
			}

			if( IsTeamGameType( ))
			{
				if (m_BlueInterfaceTimer.GetTime() > 0.0f)
				{
					m_BlueInterfaceTimer.Draw();
				}

				if (m_RedInterfaceTimer.GetTime() > 0.0f)
				{
					m_RedInterfaceTimer.Draw();
				}
			}
			*/
			//==================================================
		}

		if( g_pDamageFXMgr->IsDamageActive( DamageTypeToFlag(DT_SLEEPING) ))
		{
			if (IsChoosingWeapon())
			{
				m_WeaponChooser.Close();
			}

			if (IsChoosingAmmo())
			{
				m_AmmoChooser.Close();
			}
		}

		// Update the screen fade alpha only if we are in the playing state and not paused...
 		bool bUpdateAlpha = (GetGameState() == GS_PLAYING ? !g_pGameClientShell->IsGamePaused() : false);

		UpdateScreenFade(bUpdateAlpha);
	}

	//this should be last so it is always on top.
	if (m_MessageBox.IsVisible())
	{
		g_pLTClient->Start3D();
        g_pLTClient->StartOptimized2D();
        m_MessageBox.Draw();
        g_pLTClient->EndOptimized2D();
        g_pLTClient->End3D();
	}

	m_CursorMgr.Update();

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreScreenState()
//
//	PURPOSE:	Initialize the Screen state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreScreenState(GameState eCurState)
{
    if (eCurState == GS_SCREEN) return LTFALSE;

	m_InterfaceResMgr.Setup();

	// Pause the game...
    g_pGameClientShell->PauseGame(LTTRUE, LTTRUE);

	// Disable light scaling when not in the playing state...
	g_pGameClientShell->GetLightScaleMgr()->Disable();
	g_pGameClientShell->GetScreenTintMgr()->ClearAll();


	//CreateInterfaceBackground();

    SetHUDRenderLevel(kHUDRenderNone);
	ClearScreenAlways();

	// No fog in the menus...
	g_bInGameFogEnabled = (LTBOOL) GetConsoleInt("FogEnable", 1);
	WriteConsoleInt("FogEnable", 0);

	// Make sure menus and screens are full screen...
    memset(&m_rcMenuRestoreCamera, 0, sizeof (LTRect));

    uint32 nWidth, nHeight;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);

	m_bUseInterfaceCamera = LTTRUE;

	if (eCurState == GS_LOADINGLEVEL) 
	{
		AbortScreenFade();
	}
	else
	{
		ForceScreenFadeIn(g_vtScreenFadeInTime.GetFloat());
	}

	m_bEnteredScreenState = true;
	
	// Initialize the music...

	SetupMusic();

	
	//[MURSUM]========================================
	GetScreenMgr()->SetPreviousScreenID( SCREEN_ID_NONE );
	//================================================


	
	if(g_pLTClient->IsConnected())
	{
		g_pInterfaceMgr->SetIntentionalDisconnect( true );
		g_pClientMultiplayerMgr->ForceDisconnect();	
		//g_pLTClient->Disconnect();
		m_bChangeModeReserved = LTTRUE;
	}

	WriteConsoleInt( "Anisotropic", 0 );
	WriteConsoleInt( "Bilinear", 0 );
	IDirect3DDevice8* pd3dDevice = g_pLTClient->GetD3DDevice();
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );	

	/*
	//[MURSUM]====================================================
	RMode rSaveMode;
	rSaveMode.m_Width		= g_pProfileMgr->GetCurrentProfile()->m_nScreenWidth;
	rSaveMode.m_Height		= g_pProfileMgr->GetCurrentProfile()->m_nScreenHeight;
	rSaveMode.m_BitDepth	= g_pProfileMgr->GetCurrentProfile()->m_nScreenDepth;
	
	if( rSaveMode.m_Width != USABLE_WIDTH_I || rSaveMode.m_Height != USABLE_HEIGHT_I )
	{
		g_pProfileMgr->GetCurrentProfile()->m_nScreenWidth  = USABLE_WIDTH_I;
		g_pProfileMgr->GetCurrentProfile()->m_nScreenHeight = USABLE_HEIGHT_I;
		g_pProfileMgr->GetCurrentProfile()->m_nScreenDepth  = USABLE_BPP_I;
		g_pProfileMgr->GetCurrentProfile()->ApplyResolution();
		// Save Resolution
		g_pProfileMgr->GetCurrentProfile()->m_nScreenWidth  = rSaveMode.m_Width;
		g_pProfileMgr->GetCurrentProfile()->m_nScreenHeight = rSaveMode.m_Height;
		g_pProfileMgr->GetCurrentProfile()->m_nScreenDepth  = rSaveMode.m_BitDepth;
		
		g_pProfileMgr->GetCurrentProfile()->SetChangeOptionSetting();  // Profile에 저장   		
	}
	//================================================================
	*/

    return LTTRUE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostScreenState()
//
//	PURPOSE:	Handle leaving the Screen state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostScreenState(GameState eNewState)
{
    if (eNewState == GS_SCREEN) return LTFALSE;

	GetScreenMgr( )->ExitScreens();

	if (eNewState != GS_LOADINGLEVEL && eNewState != GS_DEMOSCREEN)
	{
		int nGameMode = GAMEMODE_NONE;
        g_pLTClient->GetGameMode(&nGameMode);
        if (nGameMode == GAMEMODE_NONE) return LTFALSE;
	}

    //ClearScreenAlways(LTFALSE);
	ClearScreenAlways();

	AddToClearScreenCount();


	RemoveAllInterfaceSFX();

	//see what state we need to restore the HUD into (in case we go into the menus
	//while we are dead)
	if(g_pPlayerMgr->IsPlayerDead())
	{
		SetHUDRenderLevel(kHUDRenderDead);
	}
	else
	{
		SetHUDRenderLevel(kHUDRenderFull);
	}

	// m_InterfaceResMgr.Clean();
	// Reset fog value...

	WriteConsoleInt("FogEnable", (int) g_bInGameFogEnabled);

	m_bSuppressNextFlip = true;

	// Match the pause from PreScreeState...

    g_pGameClientShell->PauseGame( LTFALSE );

    return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PrePauseState()
//
//	PURPOSE:	Initialize the Pause state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PrePauseState(GameState eCurState)
{
    if (eCurState == GS_PAUSED) return LTFALSE;

	CreateInterfaceBackground();

	// Create the "paused" surface...
	m_hGamePausedSurface = g_pInterfaceResMgr->GetSharedSurface("interface\\pause.pcx");

	m_bUseInterfaceCamera = LTTRUE;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostPauseState()
//
//	PURPOSE:	Handle leaving the Pause state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostPauseState(GameState eNewState)
{
    if (eNewState == GS_PAUSED) return LTFALSE;

	RemoveAllInterfaceSFX();

	// Remove the "paused" surface...
	if (m_hGamePausedSurface)
	{
		g_pInterfaceResMgr->FreeSharedSurface(m_hGamePausedSurface);
		m_hGamePausedSurface = LTNULL;
	}



    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PrePlayingState()
//
//	PURPOSE:	Initialize the Playing state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PrePlayingState(GameState eCurState)
{
    if (eCurState == GS_PLAYING) return LTFALSE;

	//서버로 로딩완료 메시지 보냄
	g_Network.CS_LoadComplete();

	//////////////////////////////////////////////////////////////////////////
	// NOVICE : 로딩완료되고 나서 사운드 플레이 함
//	WriteConsoleInt( "SoundEnable", 1 );
//	g_pLTClient->CPrint( "@@ Sound Enable Setting to TRUE" );
	//////////////////////////////////////////////////////////////////////////
	// 광식 : Mini Map
	//-------------------------------------------------------------------
	MISSION* pMission = g_pMissionButeMgr->GetMission(g_pMissionMgr->GetCurrentMission());
	if( pMission )
	{
		LTVector vMin, vMax, vSize;
		LTVector vOffset;
		g_pHUDMiniMap->SetMapTexture(pMission->hMiniMap);
		g_pLTClient->GetMapExtentsMinMax(vMin, vMax);
		g_pLTClient->GetSourceWorldOffset(vOffset);
		vSize = vMax - vMin;
		g_pHUDMiniMap->SetMapSize((int)vSize.x, (int)vSize.z);
		g_pHUDMiniMap->SetMapSourceOffset(vOffset);
	}
	//-------------------------------------------------------------------
	
	g_pGameClientShell->PauseGame(LTFALSE);

	// Clear whatever input got us here...
	if( !g_GameDoc.IsMandateHost() && !g_GameDoc.IsRejoinHost() )
	{
		g_pLTClient->ClearInput();
	}

	// Eanble light scaling...

	g_pGameClientShell->GetLightScaleMgr()->Enable();
	g_pPlayerMgr->RestorePlayerModes();

    m_bUseInterfaceCamera = LTFALSE;

	WriteConsoleFloat( "LockFPS", FRAME_INTERVAL );

	g_GameDoc.SetMandateHost(LTFALSE);
	g_GameDoc.SetRejoinHost(LTFALSE);

	m_nLastNetState = 0;
	m_fLastNetStateTime	= g_pLTClient->GetTime();
				
#ifndef _STANDALONE
	//[MURSUM]===================================================
	g_Network.EnableNetwork(LTTRUE);

	EnableInput( LTTRUE );
	//===========================================================
	return LTTRUE;
#else
	return LTTRUE;
#endif
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostPlayingState()
//
//	PURPOSE:	Handle leaving the Playing state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostPlayingState(GameState eNewState)
{
    if (eNewState == GS_PLAYING) return LTFALSE;

	g_pGameClientShell->GetScreenTintMgr()->ClearAll();
	g_pGameClientShell->GetLightScaleMgr()->ClearAllLightScales();

    g_pLTClient->SetModelHook((ModelHookFn)DefaultModelHook, this);
	
	int nTemp = GetConsoleInt( "NameRadar", 1 );
	CWinUtil::WinWritePrivateProfileString( "AutoOptionSetting", 
											"NameRadar",
											nTemp>0 ? "1" : "0",
											"Profiles\\Player.txt" );

	nTemp = (int)GetConsoleFloat("MouseSensitivity",15.0f);
	char	szTemp[32];
	itoa( nTemp, szTemp, 10 );
	CWinUtil::WinWritePrivateProfileString( "Controls", 
											"Sensitivity",
											szTemp,
											"Profiles\\Player.txt" );
	
	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PrePopupState()
//
//	PURPOSE:	Initialize the Popup state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PrePopupState(GameState eCurState)
{
    if (eCurState == GS_POPUP) return LTFALSE;

	// Pause the game...
	if ( !IsMultiplayerGame( ) )
	{
		g_pGameClientShell->PauseGame(LTTRUE, LTTRUE);
	}

	g_pPlayerMgr->AllowPlayerMovement(LTFALSE);
	g_pPlayerMgr->ClearPlayerModes(LTTRUE);

	m_bUseInterfaceCamera = LTFALSE;

	m_ePrePopupHUDRenderLevel = GetHUDMgr()->GetRenderLevel();

	if (m_bHideHUDInPopup)
	{	
		SetHUDRenderLevel(kHUDRenderNone);
	}

	g_keys.clear();
	for (int k = 0; k < 256; k++)
	{
		if (IsKeyDown(k))
			g_keys.push_back(k);
	}

	g_bLockPopup = !g_keys.empty();

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostPopupState()
//
//	PURPOSE:	Handle leaving the Popup state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostPopupState(GameState eNewState)
{
    if (eNewState == GS_POPUP) return LTFALSE;

	g_pPlayerMgr->AllowPlayerMovement(LTTRUE);
	g_pLTClient->ClearInput();

	SetHUDRenderLevel(m_ePrePopupHUDRenderLevel);
	m_bHideHUDInPopup = false;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreLoadingLevelState()
//
//	PURPOSE:	Initialize the LoadingLevel state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreLoadingLevelState(GameState eCurState)
{
    if (eCurState == GS_LOADINGLEVEL) return LTFALSE;

	// Disable light scaling when not in the playing state...
	g_pGameClientShell->GetLightScaleMgr()->ClearAllLightScales();
	
	WriteConsoleInt( "Anisotropic", 0 );
	WriteConsoleInt( "Bilinear", 1 );

	RemoveInterfaceFX();
	m_InterfaceFXMgr.UseSystemTime(true);

	m_bUseInterfaceCamera = LTTRUE;

	/**/
	//ApplyDisplay...
	m_bChangeModeReserved = LTFALSE;
	g_pProfileMgr->GetCurrentProfile()->ApplyResolution();

	// Added By Bestline ==>
	// Time : 2004-07-01 오후 3:45:41
//	CScreenPreload *pPreload = (CScreenPreload *) (g_pInterfaceMgr->GetScreenMgr( )->GetScreenFromID(SCREEN_ID_PRELOAD));
//	SetLoadingRenderScreen(pPreload);
	// Added By Bestline <==

	m_LoadingScreen.Show();
		
	// Turn off the music (this will be turned on when we start the
	// next level...
	//CMusic* pMusic = g_pGameClientShell->GetMusic();
    //if (pMusic)
	//{
	//	pMusic->Stop();
	//}

	// Turn off sound

	//[MURSUM] 로딩중 사운드 플레이 안함
	//((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetVolume(0);
	m_nBackupSoundVar = GetConsoleInt( "SoundEnable", 1 );
	WriteConsoleInt( "SoundEnable", 0 );
	
	WriteConsoleFloat( "LockFPS", 0.0f );	//FRAME_INTERVAL

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostLoadingLevelState()
//
//	PURPOSE:	Handle leaving the LoadingLevel state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostLoadingLevelState(GameState eNewState)
{
    if (eNewState == GS_LOADINGLEVEL) return LTFALSE;

	// Don't allow the loading state to go away until the loading screen has been hidden
	if (m_LoadingScreen.IsVisible())
		return LTFALSE;

	RemoveAllInterfaceSFX();
	m_InterfaceFXMgr.UseSystemTime(false);

	// Turn back on the sound
	m_ProfileMgr.ImplementSoundVolume();

	WriteConsoleInt( "SoundEnable", m_nBackupSoundVar );
	if( m_nBackupSoundVar > 0 ) g_pLTClient->PlayAmbientSound();


//	ClearAllScreenBuffers();
	m_bLoadFailed = LTFALSE;

	if( eNewState == GS_PLAYING )
	{
		MISSION * pMission = g_pMissionButeMgr->GetMission( g_pMissionMgr->GetCurrentMission() );
		if( !pMission || strlen(pMission->szObjectiveSndFileName)<=0 ) return LTTRUE;
		char	szSound[MAX_CS_FILENAME_LEN];
#ifdef BIG_FAT_MODE_TEST
		if( LTFALSE == g_GameDoc.IsBigHeadMode() )
		{
			strcpy( szSound, pMission->szObjectiveSndFileName );
		}
		else
		{
			if( g_pGameClientShell->GetGameType() == eGameTypeTeamDeathmatch )
			{
				sprintf(szSound, "SA_INTERFACE\\SND\\Fatmessage\\mo_deathmatch");
			}
			else
			{
				sprintf(szSound, "SA_INTERFACE\\SND\\Fatmessage\\mo_bomb");
			}			
		}
#else
		strcpy( szSound, pMission->szObjectiveSndFileName );
#endif

		switch( g_pGameClientShell->GetGameType() )
		{
		case eGameTypeTeamDeathmatch:
			strcat( szSound, ".wav" );
			break;
		case eGameTypeSurvival:
			if( g_GameDoc.GetTeam() == TEAM_A )
			{
				strcat( szSound, "_A.wav" );
			}
			else
			{
				strcat( szSound, "_B.wav" );
			}
			break;
		}
		g_pClientSoundMgr->PlaySoundLocal( szSound, SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		
		int nObjID;
		if( g_GameDoc.GetTeam() == TEAM_A )
		{
			nObjID = pMission->nObjectiveID_A;
		}
		else
		{
			nObjID = pMission->nObjectiveID_B;
		}
		
		g_pHelpMsgs->AddMessage( NF_GetMsg(nObjID), 16, kMsgMission, 3.0f );
	}

	return LTTRUE;
}


LTBOOL CInterfaceMgr::PreMandateHostState(GameState eCurState)
{
    if (eCurState == GS_MANDATEHOST) return LTFALSE;

	WriteConsoleFloat( "LockFPS", 0.0f );	//FRAME_INTERVAL
	ClearScreenAlways(LTFALSE);
		
    return LTTRUE;
}

LTBOOL CInterfaceMgr::PostMandateHostState(GameState eNewState)
{
    if (eNewState == GS_MANDATEHOST) return LTFALSE;
	m_bLoadFailed = LTFALSE;
	ClearScreenAlways();
	
	if( eNewState == GS_PLAYING )
	{
		g_Network.CS_StartHostReply( LTTRUE );
	}

	return LTTRUE;
}

LTBOOL CInterfaceMgr::PreRejoinHostState(GameState eCurState)
{
    if (eCurState == GS_REJOINHOST) return LTFALSE;

	WriteConsoleFloat( "LockFPS", 0.0f );	//FRAME_INTERVAL
	ClearScreenAlways(LTFALSE);
    return LTTRUE;
}

LTBOOL CInterfaceMgr::PostRejoinHostState(GameState eNewState)
{
    if (eNewState == GS_REJOINHOST) return LTFALSE;
	m_bLoadFailed = LTFALSE;
	ClearScreenAlways();
			
	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreExitingLevelState()
//
//	PURPOSE:	Initialize the ExitingLevel state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreExitingLevelState(GameState eCurState)
{
    if (eCurState == GS_EXITINGLEVEL) return LTFALSE;

	if (eCurState == GS_SCREEN && GetScreenMgr( )->GetCurrentScreenID() == SCREEN_ID_MAIN )
	{
		return LTFALSE;
	}

// Edited By IN'T ==>
// Time : 2004-05-04 오후 8:02:12
// 	StartScreenFadeOut( g_vtExitLevelScreenFadeTime.GetFloat() );
// Edited By IN'T <==

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostExitingLevelState()
//
//	PURPOSE:	Handle leaving the ExitingLevel state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostExitingLevelState(GameState eNewState)
{
    if (eNewState == GS_EXITINGLEVEL) return LTFALSE;

	// Tell the missionmgr we're leaving exit level.
	g_pMissionMgr->FinishExitLevel( );

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreSplashScreenState()
//
//	PURPOSE:	Initialize the SplashScreen state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreSplashScreenState(GameState eCurState)
{
	if (eCurState == GS_SPLASHSCREEN) return LTFALSE;
	

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostSplashScreenState()
//
//	PURPOSE:	Handle leaving the SplashScreen state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostSplashScreenState(GameState eNewState)
{
    if (eNewState == GS_SPLASHSCREEN) return LTFALSE;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreMovieState()
//
//	PURPOSE:	Initialize the movie state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreMovieState(GameState eCurState)
{
    if (eCurState == GS_MOVIE) return LTFALSE;

	g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN);
	m_bUseInterfaceCamera = LTTRUE;

	m_nCurMovie = gMovieOrder[0];
	NextMovie();

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostMovieState()
//
//	PURPOSE:	Handle leaving the movie state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostMovieState(GameState eNewState)
{
    if (eNewState == GS_MOVIE) return LTFALSE;

    return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreDemoScreenState()
//
//	PURPOSE:	Initialize the DemoScreen state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreDemoScreenState(GameState eCurState)
{
    if (eCurState == GS_DEMOSCREEN) return LTFALSE;

	g_nDemo = -1;
	NextDemoScreen();

	m_bUseInterfaceCamera = LTTRUE;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PostDemoScreenState()
//
//	PURPOSE:	Handle leaving the DemoScreen state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PostDemoScreenState(GameState eNewState)
{
    if (eNewState == GS_DEMOSCREEN) return LTFALSE;

	if (g_hDemo)
	{
        g_pLTClient->DeleteSurface(g_hDemo);
        g_hDemo = LTNULL;
	}

    return LTTRUE;
}







// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::ProcessAdvancedOptions
//
//	PURPOSE:	Process the advanced options
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::ProcessAdvancedOptions()
{
	// Check advanced options...

    HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("DisableMusic");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_MUSIC;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableSound");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_SOUND;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableMovies");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_MOVIES;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableFog");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_FOG;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableLines");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_LINES;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableJoystick");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_JOYSTICK;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableTripBuf");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_TRIPLEBUFFER;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableTJuncs");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_TJUNCTIONS;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableSoundFilters");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_SOUNDFILTERS;
	}

    hVar = g_pLTClient->GetConsoleVar("DisableHardwareSound");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwAdvancedOptions &= ~AO_HARDWARESOUND;
	}

	// Record the original state of sound and music

    hVar = g_pLTClient->GetConsoleVar("SoundEnable");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwOrignallyEnabled |= AO_SOUND;
	}

    hVar = g_pLTClient->GetConsoleVar("MusicEnable");
    if (hVar && g_pLTClient->GetVarValueFloat(hVar))
	{
		m_dwOrignallyEnabled |= AO_MUSIC;
	}

	// Implement any advanced options here (before renderer is started)
	if ( !(m_dwAdvancedOptions & AO_SOUNDFILTERS) )
	{
        g_pLTClient->RunConsoleString("SoundFilters 0");
	}
	else
	{
		// only set this on, if there isn't a current setting
		hVar = g_pLTClient->GetConsoleVar("SoundFilters");
		if ( !hVar )
	        g_pLTClient->RunConsoleString("SoundFilters 1");
	}

    hVar = g_pLTClient->GetConsoleVar("SoundEnable");
	if (!hVar && (m_dwAdvancedOptions & AO_SOUND))
	{
        g_pLTClient->RunConsoleString("SoundEnable 1");
	}

    hVar = g_pLTClient->GetConsoleVar("MusicEnable");
	if (!hVar && (m_dwAdvancedOptions & AO_MUSIC))
	{
        g_pLTClient->RunConsoleString("MusicEnable 1");
	}


	if (!(m_dwAdvancedOptions & AO_TRIPLEBUFFER))
	{
        g_pLTClient->RunConsoleString("BackBufferCount 1");
	}

	if (!(m_dwAdvancedOptions & AO_FOG))
	{
        g_pLTClient->RunConsoleString("FogEnable 0");
	}

	if (!(m_dwAdvancedOptions & AO_JOYSTICK))
	{
        g_pLTClient->RunConsoleString("JoystickDisable 1");
	}

	if (!(m_dwAdvancedOptions & AO_CURSOR))
	{
        g_pLTClient->RunConsoleString("HardwareCursor 0");
	}

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::ChangeState
//
//	PURPOSE:	Change the game state.  This allows us to do pre/post state
//				change handling
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::ChangeState(GameState eNewState)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return LTFALSE;

	DebugChangeState(eNewState);

	GameState eCurState = m_eGameState;

	// First make sure we change change to the new state from the the
	// state we are currently in...

	if (PreChangeState(eCurState, eNewState))
	{
		m_eGameState = eNewState;

		// Since the state changed, update the cursor
		UpdateCursorState();

//	    g_pLTClient->ClearInput();
		m_eLastGameState = eCurState;

        return LTTRUE;

	}

    return LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::DebugChangeState
//
//	PURPOSE:	Print out debugging info about changing states
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::DebugChangeState(GameState eNewState)
{
#ifdef _DEBUG
//#define _DEBUG_INTERFACE_MGR
#ifdef _DEBUG_INTERFACE_MGR
    g_pLTClient->CPrint("CInterfaceMgr::ChangeState() :");
    g_pLTClient->CPrint("  Old State: %s", c_GameStateNames[m_eGameState]);
    g_pLTClient->CPrint("  New State: %s", c_GameStateNames[eNewState]);
#endif
#endif
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::PreChangeState
//
//	PURPOSE:	Handle pre setting of game state
//
// ----------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::PreChangeState(GameState eCurState, GameState eNewState)
{
	// Do any clean up of the old (current) state...

	switch (eCurState)
	{
		case GS_PLAYING :
		{
            if (!PostPlayingState(eNewState)) return LTFALSE;
		}
		break;

		case GS_POPUP :
		{
            if (!PostPopupState(eNewState)) return LTFALSE;
		}
		break;

		case GS_EXITINGLEVEL:
		{
            if (!PostExitingLevelState(eNewState)) return LTFALSE;
		}
		break;

		case GS_LOADINGLEVEL:
		{
            if (!PostLoadingLevelState(eNewState)) return LTFALSE;
		}
		break;

		case GS_SCREEN :
		{
            if (!PostScreenState(eNewState)) return LTFALSE;
		}
		break;

		case GS_PAUSED :
		{
            if (!PostPauseState(eNewState)) return LTFALSE;
		}
		break;

		case GS_SPLASHSCREEN :
		{
            if (!PostSplashScreenState(eNewState)) return LTFALSE;
		}
		break;

		case GS_MOVIE :
		{
            if (!PostMovieState(eNewState)) return LTFALSE;
		}
		break;

		case GS_DEMOSCREEN :
		{
            if (!PostDemoScreenState(eNewState)) return LTFALSE;
		}
		break;

		case GS_MANDATEHOST :
		{
            if (!PostMandateHostState(eNewState)) return LTFALSE;
		}
		break;

		case GS_REJOINHOST:
		{
			if (!PostRejoinHostState(eNewState)) return LTFALSE;
		}
		break;

		default : break;
	}

	// Do any initial setup for the new state...

	switch (eNewState)
	{
		case GS_PLAYING :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_PLAYING ---------------------------------------------" );
			return PrePlayingState(eCurState);
		}
		break;

		case GS_POPUP :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_POPUP -----------------------------------------------" );
			return PrePopupState(eCurState);
		}
		break;

		case GS_EXITINGLEVEL:
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_EXITINGLEVEL ----------------------------------------" );
			return PreExitingLevelState(eCurState);
		}
		break;

		case GS_LOADINGLEVEL:
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_LOADINGLEVEL ----------------------------------------" );
			return PreLoadingLevelState(eCurState);
		}
		break;

		case GS_SCREEN :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_SCREEN ----------------------------------------------" );
			return PreScreenState(eCurState);
		}
		break;

		case GS_PAUSED :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_PAUSED ----------------------------------------------" );
			return PrePauseState(eCurState);
		}
		break;

		case GS_SPLASHSCREEN :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_SPLASHSCREEN ---------------------------------------------" );
			return PreSplashScreenState(eCurState);
		}
		break;

		case GS_MOVIE :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_MOVIE -----------------------------------------------" );
			return PreMovieState(eCurState);
		}
		break;

		case GS_DEMOSCREEN :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_DEMOSCREEN ------------------------------------------" );
			return PreDemoScreenState(eCurState);
		}
		break;

		case GS_MANDATEHOST :
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_MANDATEHOST -----------------------------------------" );
			return PreMandateHostState(eCurState);
		}
		break;

		case GS_REJOINHOST:
		{
			g_pLTClient->CPrint( "[CHANGE STATE] GS_REJOINHOST ------------------------------------------" );
			return PreRejoinHostState(eCurState);
		}
		break;

		default : break;
	}

    return LTTRUE;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::Save
//
//	PURPOSE:	Save the interface info
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::Save(ILTMessage_Write *pMsg)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	GetPlayerStats( )->Save(pMsg);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::Load
//
//	PURPOSE:	Load the interface info
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::Load(ILTMessage_Read *pMsg)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	GetPlayerStats( )->Load(pMsg);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::GetCurrentScreen()
//
//	PURPOSE:	Finds out what the current screen is
//				- returns SCREEN_ID_NONE if not in a screen state
//
// --------------------------------------------------------------------------- //

eScreenID CInterfaceMgr::GetCurrentScreen()
{
	if (m_eGameState != GS_SCREEN)
	{
		return SCREEN_ID_NONE;
	}
	return GetScreenMgr( )->GetCurrentScreenID();
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::SwitchToScreen
//
//	PURPOSE:	Go to the specified screen
//
// --------------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::SwitchToScreen(eScreenID screenID)
{
	g_Network.SendPing();

	if (m_eGameState != GS_SCREEN)
	{
		if (((m_eGameState == GS_SPLASHSCREEN) || (m_eGameState == GS_DEMOSCREEN)) && (screenID == SCREEN_ID_MAIN))
		{
			StartScreenFadeIn(g_vtMainScreenFadeIn.GetFloat());
		}

        if (!ChangeState(GS_SCREEN)) return LTFALSE;
	}

	GetScreenMgr( )->SetCurrentScreen(screenID);

	g_pLTClient->CPrint( "[SWITCH TO SCREEN] %s", GetScreenMgr()->GetScreenName(screenID) );

    return LTTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::ForceScreenUpdate
//
//	PURPOSE:	Force the current screen to update
//
// --------------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::ForceScreenUpdate(eScreenID screenID)
{
    if (m_eGameState != GS_SCREEN) return LTFALSE;

	return GetScreenMgr( )->ForceScreenUpdate(screenID);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::MissionFailed
//
//	PURPOSE:	Go to the mission failure state
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::MissionFailed(int nFailStringId)
{
	/*
	// The player is no longer in the world...
	g_pGameClientShell->SetWorldNotLoaded();

	m_nFailStringId = nFailStringId;
	SwitchToScreen(SCREEN_ID_FAILURE);
	*/
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::ScreenDimsChanged
//
//	PURPOSE:	Handle the screen dims changing
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::ScreenDimsChanged()
{
	m_InterfaceResMgr.ScreenDimsChanged();
	//[MURSUM] Screen은 800 * 600 으로 고정
	//GetScreenMgr( )->ScreenDimsChanged();
	GetHUDMgr()->ScreenDimsChanged();
	m_MessageBox.ScreenDimsChanged();
	

	// Update the camera rect...
    uint32 dwWidth = 640, dwHeight = 480;

    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &dwWidth, &dwHeight);

	// This may need to be changed to support in-game cinematics...

	ResetMenuRestoreCamera(0, 0, dwWidth, dwHeight);
    g_pLTClient->SetCameraRect (m_hInterfaceCamera, LTTRUE, 0, 0, dwWidth, dwHeight);

	UpdateInterfaceBackground();

}



//mouse handling
void CInterfaceMgr::OnLButtonUp(int x, int y)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	if (m_MessageBox.IsVisible())
	{
		m_MessageBox.OnLButtonUp(x,y);
		return;
	}

	switch (m_eGameState)
	{
	case GS_LOADINGLEVEL:
		{
		} break;
	case GS_SCREEN:
		{
			GetScreenMgr( )->OnLButtonUp(x,y);
		} break;	
	}
}

void CInterfaceMgr::OnLButtonDown(int x, int y)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	if (m_MessageBox.IsVisible())
	{
		m_MessageBox.OnLButtonDown(x,y);
		return;
	}

	switch (m_eGameState)
	{
		case GS_SCREEN:
		{
			GetScreenMgr( )->OnLButtonDown(x,y);
		}	
		break;

		case GS_SPLASHSCREEN:
		{
			// They pressed a mouse button - end splash screen...
			EndSplashScreen( );
		}	
		break;

		case GS_MOVIE:
		{
			// They pressed a mouse button - next movie
			NextMovie();
		}	
		break;

		case GS_DEMOSCREEN:
		{
			// They pressed a mouse button - next demo screen
			NextDemoScreen();
		}	
		break;

		// 광식
		case GS_PLAYING :
		{
			g_pPlayerMgr->OnLButtonDown(x, y);
		}
		break;
	}
}


void CInterfaceMgr::OnLButtonDblClick(int x, int y)
{
	// Make sure we're initialized.
	if( !IsInitialized( ) )
		return;

	if (m_MessageBox.IsVisible())
	{
		return;
	}

	switch (m_eGameState)
	{
		case GS_SCREEN:
		{
			GetScreenMgr( )->OnLButtonDblClick(x,y);
		} 
		break;
	}

}

void CInterfaceMgr::OnRButtonUp(int x, int y)
{
	if (m_MessageBox.IsVisible())
	{
		return;
	}

	switch (m_eGameState)
	{
		case GS_SCREEN:
		{
			GetScreenMgr( )->OnRButtonUp(x,y);
		} 
		break;		
	}


}

void CInterfaceMgr::OnRButtonDown(int x, int y)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	if (m_MessageBox.IsVisible())
	{
		return;
	}

	switch (m_eGameState)
	{
		case GS_SCREEN:
		{
			GetScreenMgr( )->OnRButtonDown(x,y);
		}	
		break;
		
		case GS_SPLASHSCREEN:
		{
			// They pressed a button - end splash screen...
			EndSplashScreen( );
		}	
		break;
	
		case GS_MOVIE:
		{
			// They pressed a button - next movie...
			NextMovie();
		}	
		break;
		
		case GS_DEMOSCREEN:
		{
			// They pressed a button - next demo screen...
			NextDemoScreen();
		}	
		break;

		// 광식
		case GS_PLAYING :
		{
			g_pPlayerMgr->OnRButtonDown(x, y);
		}
		break;
	
	}
}

void CInterfaceMgr::OnRButtonDblClick(int x, int y)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	if (m_MessageBox.IsVisible())
	{
		return;
	}

	switch (m_eGameState)
	{
		case GS_SCREEN:
		{
			GetScreenMgr( )->OnRButtonDblClick(x,y);
		} 
		break;	
	}

}

void CInterfaceMgr::OnMouseMove(int x, int y)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	m_CursorPos.x = x;
	m_CursorPos.y = y;

	if (m_MessageBox.IsVisible())
	{
		m_MessageBox.OnMouseMove(x,y);
		return;
	}

	switch (m_eGameState)
	{
		case GS_SCREEN:
		{
			GetScreenMgr( )->OnMouseMove(x,y);
		}
		break;
	}
}

// SPIKE
void 
CInterfaceMgr::OnWheelUp(int x, int y)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))return;
	if (m_MessageBox.IsVisible())return;
	
	GetScreenMgr( )->OnWheelUp(x,y);

}
void 
CInterfaceMgr::OnWheelDown(int x, int y)
{
	if( !IsInitialized( ))return;
	if (m_MessageBox.IsVisible())return;

	GetScreenMgr( )->OnWheelDown(x,y);

}
//===========================================


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateOverlays()
//
//	PURPOSE:	Update the overlay used as a scope crosshair
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::UpdateOverlays()
{
	if (!m_nOverlayCount) return;


    LTVector vPos(0, 0, 0), vU, vR, vF, vTemp;
    LTRotation rRot;

	HOBJECT hCamera = g_pPlayerMgr->GetCamera();
	if (!hCamera) return;

    LTBOOL bDrawnOne = LTFALSE;
	for (int i = 0; i < NUM_OVERLAY_MASKS; i++)
	{
		if (m_hOverlays[i])
		{
			if ((!bDrawnOne || i >= OVM_NON_EXCLUSIVE) && GS_PLAYING == GetGameState())
			{
				if (i < OVM_NON_EXCLUSIVE)
				{
                    bDrawnOne = LTTRUE;
				}

				if (g_pLayoutMgr->IsMaskSprite((eOverlayMask)i))
				{
					VEC_COPY(vTemp,g_vOverlaySpriteScale);
				}
				else
				{
					VEC_COPY(vTemp,g_vOverlayModelScale);
				}
				VEC_MULSCALAR(vTemp, vTemp, m_fOverlayScaleMult[i]);

				g_pLTClient->SetObjectScale(m_hOverlays[i], &vTemp);

				g_pCommonLT->SetObjectFlags(m_hOverlays[i], OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);

			}
			else
			{
				g_pCommonLT->SetObjectFlags(m_hOverlays[i], OFT_Flags, 0, FLAG_VISIBLE);
			}
		}
	}
}

void CInterfaceMgr::CreateOverlay(eOverlayMask eMask)
{
	if(!g_pPlayerMgr) return;

	HOBJECT hCamera = g_pPlayerMgr->GetCamera();
	if (!hCamera) return;

	// Already created this mask
	if (m_hOverlays[eMask]) return;


	m_fOverlayScaleMult[eMask] = g_pLayoutMgr->GetMaskScale(eMask);

	ObjectCreateStruct createStruct;
	INIT_OBJECTCREATESTRUCT(createStruct);

	createStruct.m_ObjectType = OT_SPRITE;

    LTVector vPos(0,0,0), vU, vR, vF, vTemp;
    LTRotation rRot;

//    g_pLTClient->GetObjectPos(hCamera, &vPos);
//    g_pLTClient->GetObjectRotation(hCamera, &rRot);
	vU = rRot.Up();
	vR = rRot.Right();
	vF = rRot.Forward();

	if (g_pLayoutMgr->IsMaskSprite(eMask))
	{

		VEC_MULSCALAR(vTemp, vF, g_fOverlaySpriteDist);
		VEC_ADD(vPos, vPos, vTemp);
		createStruct.m_Flags = FLAG_VISIBLE | FLAG_SPRITE_NOZ | FLAG_FOGDISABLE | FLAG_NOLIGHT | FLAG_REALLYCLOSE;
		VEC_COPY(createStruct.m_Pos, vPos);
		createStruct.m_Rotation = rRot;


		char sprName[128] = "";
		g_pLayoutMgr->GetMaskSprite(eMask,sprName,sizeof(sprName));

		if (sprName[0])
		{
			SAFE_STRCPY(createStruct.m_Filename, sprName);
			m_hOverlays[eMask] = g_pLTClient->CreateObject(&createStruct);
			m_nOverlayCount++;

			// [KLS 5/15/02] Change the alpha value of the sprite...
			// [JRG 9/1/02] Added alpha values to layout butes
			LTFLOAT r, g, b, a;
			g_pLTClient->GetObjectColor(m_hOverlays[eMask], &r, &g, &b, &a);

			a = g_pLayoutMgr->GetMaskAlpha(eMask);
			
			g_pLTClient->SetObjectColor(m_hOverlays[eMask], r, g, b, a);
		}
	}
	else
	{
		VEC_MULSCALAR(vTemp, vF, g_fOverlayModelDist);
		VEC_ADD(vPos, vPos, vTemp);
		createStruct.m_Flags = FLAG_VISIBLE | FLAG_NOLIGHT;
		VEC_COPY(createStruct.m_Pos, vPos);
		createStruct.m_Rotation = rRot;

		char modName[128] = "";
		char skinName[128] = "";
		g_pLayoutMgr->GetMaskModel(eMask,modName,sizeof(modName));
		g_pLayoutMgr->GetMaskSkin(eMask,skinName,sizeof(skinName));

		createStruct.m_ObjectType = OT_MODEL;

		if (modName[0] && skinName[0])
		{
			SAFE_STRCPY(createStruct.m_Filename, modName);
			SAFE_STRCPY(createStruct.m_SkinName, skinName);
			m_hOverlays[eMask] = g_pLTClient->CreateObject(&createStruct);
/*
		    LTVector vColor;
			LTFLOAT a;
			g_pLTClient->GetObjectColor(m_hOverlays[eMask], &(vColor.x), &(vColor.y), &(vColor.z), &a);
			g_pLTClient->SetObjectColor(m_hOverlays[eMask], vColor.x, vColor.y, vColor.z, 0.999f);
*/
			m_nOverlayCount++;
		}
	}


	// Update to make sure everything is scaled correctly before the next
	// frame is rendered...

	UpdateOverlays();
}

void CInterfaceMgr::RemoveOverlay(eOverlayMask eMask)
{
	if (m_hOverlays[eMask])
	{
		m_nOverlayCount--;
        g_pLTClient->RemoveObject(m_hOverlays[eMask]);
        m_hOverlays[eMask] = LTNULL;
	}
}



void CInterfaceMgr::BeginScope(LTBOOL bVisionMode, LTBOOL bCamera)
{
	if (bVisionMode)
	{
		CreateOverlay(OVM_STATIC);
	}
//[SPIKE]
//	CreateOverlay(OVM_SCOPE);
	SetScopeEnable(LTTRUE);

	if (bCamera)
	{
		CreateOverlay(OVM_CAMERA);
	}
}

void CInterfaceMgr::EndScope()
{
	RemoveOverlay(OVM_SCOPE);
	RemoveOverlay(OVM_STATIC);
	RemoveOverlay(OVM_CAMERA);
	RemoveOverlay(OVM_CAMERA_TARGET);

	SetScopeEnable(LTFALSE);

}

void CInterfaceMgr::BeginZoom(LTBOOL bIn)
{
	CreateOverlay(bIn ? OVM_ZOOM_IN : OVM_ZOOM_OUT);
}

void CInterfaceMgr::EndZoom()
{
	RemoveOverlay(OVM_ZOOM_IN);
	RemoveOverlay(OVM_ZOOM_OUT);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateScreenFade()
//
//	PURPOSE:	Update the screen fade
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::UpdateScreenFade(bool bUpdateAlpha /*=true*/)
{
	if (!m_bScreenFade || m_fTotalFadeTime < 0.1f) return;

    LTRect rcSrc;
	rcSrc.Init(0, 0, 2, 2);

    HSURFACE hScreen = g_pLTClient->GetScreenSurface();
    uint32 dwWidth = 640, dwHeight = 480;
    g_pLTClient->GetSurfaceDims(hScreen, &dwWidth, &dwHeight);

    HLTCOLOR hTransColor = g_pLTClient->SetupColor1(1.0f, 1.0f, 1.0f, LTTRUE);
	
	// Initialize the surface if necessary...

	if (!m_bFadeInitialized)
	{
		if (m_hFadeSurface)
		{
            g_pLTClient->DeleteSurface(m_hFadeSurface);
		}

        m_hFadeSurface = g_pLTClient->CreateSurface(2, 2);
		if (!m_hFadeSurface) return;

        g_pLTClient->SetSurfaceAlpha(m_hFadeSurface, 1.0f);
        g_pLTClient->FillRect(m_hFadeSurface, &rcSrc, kBlack);
        g_pLTClient->OptimizeSurface(m_hFadeSurface, hTransColor);

		m_fCurFadeTime = m_fTotalFadeTime;
        m_bFadeInitialized = LTTRUE;
	}


	// Fade the screen if the surface is around...

	if (m_hFadeSurface)
	{
		LTFLOAT fTimeDelta = m_bFadeIn ? m_fCurFadeTime : (m_fTotalFadeTime - m_fCurFadeTime);

		LTFLOAT fLinearAlpha = (fTimeDelta / m_fTotalFadeTime);
		fLinearAlpha = fLinearAlpha < 0.0f ? 0.0f : (fLinearAlpha > 1.0f ? 1.0f : fLinearAlpha);

		LTFLOAT fAlpha = 1.0f - WaveFn_SlowOn(1.0f - fLinearAlpha);
		fAlpha = fAlpha < 0.0f ? 0.0f : (fAlpha > 1.0f ? 1.0f : fAlpha);

		g_pLTClient->SetSurfaceAlpha(m_hFadeSurface, fAlpha);

		LTRect rcDest;
		rcDest.Init(0, 0, dwWidth, dwHeight);

		g_pLTClient->ScaleSurfaceToSurfaceTransparent(hScreen, m_hFadeSurface, &rcDest, &rcSrc, hTransColor);

		// The alpha value is based on the fade time, only update it if
		// we want the alpha value to change...

		if (bUpdateAlpha)
		{
			m_fCurFadeTime -= g_pGameClientShell->GetFrameTime();
		}
	}

	// See if we're done...

	if (ScreenFadeDone())
	{
		if (m_hFadeSurface)
		{
            g_pLTClient->DeleteSurface(m_hFadeSurface);
            m_hFadeSurface = LTNULL;
		}

		// If we faded in we're done...

		if (m_bFadeIn)
		{
            m_bFadeInitialized  = LTFALSE;
            m_bScreenFade       = LTFALSE;
		}
		else
		{
			// We need to keep the screen black until it is faded
			// back in, so we'll just make sure the screen is clear
			// (and not set m_bScreenFade so we'll be called to
			// clear the screen every frame until we fade back in)...

	        g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN);
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::AbortScreenFade()
//
//	PURPOSE:	Cancel the screen fade
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::AbortScreenFade()
{
	if (m_hFadeSurface)
	{
        g_pLTClient->DeleteSurface(m_hFadeSurface);
        m_hFadeSurface = LTNULL;
	}

	m_bFadeInitialized  = LTFALSE;
	m_bScreenFade       = LTFALSE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateLetterBox()
//
//	PURPOSE:	Update the letterbox
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::UpdateLetterBox()
{
	LTBOOL bOn = (g_vtLetterBoxDisabled.GetFloat() ? LTFALSE : (m_bLetterBox || g_vtLetterBox.GetFloat()));

	LTFLOAT fTime = g_pLTClient->GetTime();

	if (bOn && !m_bWasLetterBox)
	{
		// Handle being turned on...

		m_fLetterBoxAlpha = 0.0f;
		m_fLetterBoxFadeEndTime = fTime + g_vtLetterBoxFadeInTime.GetFloat();
	}
	else if (!bOn && m_bWasLetterBox)
	{
		// Handle being turned off...

		m_fLetterBoxAlpha = 1.0f;
		m_fLetterBoxFadeEndTime = fTime + g_vtLetterBoxFadeOutTime.GetFloat();
	}

	// Store the current state of being on or off...

	m_bWasLetterBox = bOn;


	LTFLOAT fTimeLeft = m_fLetterBoxFadeEndTime - fTime;
	if (fTimeLeft < 0.0f) fTimeLeft = 0.0f;

	// Calculate the current alpha...

	if (bOn)
	{
		// Fading in...

		LTFLOAT fFadeTime = g_vtLetterBoxFadeInTime.GetFloat();

		if (fFadeTime <= 0.0f || fTimeLeft <= 0.0f)
		{
			m_fLetterBoxAlpha = 1.0f;
		}
		else
		{
			m_fLetterBoxAlpha = 1.0f - (fTimeLeft / fFadeTime);
		}
	}
	else
	{
		// Fading out...

		LTFLOAT fFadeTime = g_vtLetterBoxFadeOutTime.GetFloat();

		if (fFadeTime <= 0.0f || fTimeLeft <= 0.0f)
		{
			m_fLetterBoxAlpha = 0.0f;
		}
		else
		{
			m_fLetterBoxAlpha = fTimeLeft / fFadeTime;
		}
	}


	// If not using letter box, and we faded it out, we're done...

	if (!bOn && m_fLetterBoxAlpha <= 0.0f) return;


	// Determine the border size...

	float fBorderPercent = 0.0f;
	float fLetterBoxPercent = g_pClientButeMgr->GetCameraAttributeFloat(CAMERA_BUTE_LETTERBOXPERCENT);

	if (fLetterBoxPercent > 0.01f && fLetterBoxPercent <= 1.0f)
	{
		fBorderPercent = (1.0f - fLetterBoxPercent) / 2.0f;
	}
    HSURFACE hScreen = g_pLTClient->GetScreenSurface();
    uint32 dwWidth = 640, dwHeight = 480;
    g_pLTClient->GetSurfaceDims(hScreen, &dwWidth, &dwHeight);

	g_pLTClient->SetSurfaceAlpha(m_hLetterBoxSurface, m_fLetterBoxAlpha);

	int nBorderSize = (int) (float(dwHeight) * fBorderPercent);

 	LTRect rcDest;
	rcDest.Init(0, 0, dwWidth, nBorderSize);

	LTRect rcSrc;
	rcSrc.Init(0, 0, 2, 2);

	HLTCOLOR hTransColor = g_pLTClient->SetupColor1(1.0f, 1.0f, 1.0f, LTTRUE);

	// Cover top of screen...

	g_pLTClient->ScaleSurfaceToSurfaceTransparent(hScreen, m_hLetterBoxSurface,
	   &rcDest, &rcSrc, hTransColor);

	// Cover bottom of screen...

	rcDest.Init(0, dwHeight - nBorderSize, dwWidth, dwHeight);
 	g_pLTClient->ScaleSurfaceToSurfaceTransparent(hScreen, m_hLetterBoxSurface,
	   &rcDest, &rcSrc, hTransColor);
/*
	if (m_nBorderSize == 0)
	{
	    uint32 dwHeight = 448;
		m_nBorderSize = (int) (float(dwHeight) * fBorderPercent);
		m_LetterBox[0].verts[2].y = m_LetterBox[0].verts[3].y = m_nBorderSize;
		m_LetterBox[1].verts[0].y = m_LetterBox[1].verts[1].y = dwHeight - m_nBorderSize;
	}
	// Set the alpha
	m_LetterBox[0].rgba.a = m_LetterBox[1].rgba.a = 255.0 * m_fLetterBoxAlpha;

	g_pLTDrawprim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	g_pLTDrawprim->SetColorOp(DRAWPRIM_MODULATE);
	g_pLTDrawprim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
	g_pLTDrawprim->SetZBufferMode(DRAWPRIM_NOZ); 
	g_pLTDrawprim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pLTDrawprim->SetClipMode(DRAWPRIM_FASTCLIP);
	g_pLTDrawprim->SetFillMode(DRAWPRIM_FILL);
		
	// draw the two polys
	g_pLTDrawprim->DrawPrim(m_LetterBox, 2);

*/
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::AllowCameraMovement
//
//	PURPOSE:	Can the camera move (while in this state)
//
// --------------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::AllowCameraMovement()
{
#ifndef _FINAL
	if( g_vtPVModelModifyMode.GetFloat(0.0f)>0.0f ) return LTFALSE;
#endif
	switch (m_eGameState)
	{
		case GS_POPUP :
            return LTFALSE;
		break;

		default :
		break;
	}

    return LTTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::AllowCameraRotation
//
//	PURPOSE:	Can the camera rotate (while in this state)
//
// --------------------------------------------------------------------------- //

LTBOOL CInterfaceMgr::AllowCameraRotation()
{
#ifndef _FINAL
	if( g_vtPVModelModifyMode.GetFloat(0.0f)>0.0f ) return LTFALSE;
#endif
	switch( m_eGameState )
	{
		case GS_POPUP:
			return LTFALSE;
		break;
		
		default:
		break;
	}

	return LTTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::CreateInterfaceBackground
//
//	PURPOSE:	Create the sprite used as a background for the menu
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::CreateInterfaceBackground()
{
	char sprName[128] = "";

	BSCREATESTRUCT bcs;

    LTVector vTemp, vScale;
	LTVector vPos, vU, vR, vF;
    LTRotation rRot;

	g_pLTClient->GetObjectPos(m_hInterfaceCamera, &vPos);
	g_pLTClient->GetObjectRotation(m_hInterfaceCamera, &rRot);
	vU = rRot.Up();
	vR = rRot.Right();
	vF = rRot.Forward();

	if( !g_pLayoutMgr )
	{
		UBER_ASSERT( 0, "CInterfaceMgr::CreateInterfaceBackground: No LayoutMgr" );
		return;
	}

	vScale = g_vBaseBackScale * g_pLayoutMgr->GetBackSpriteScale();


	VEC_MULSCALAR(vTemp, vF, g_fBackDist);
//	VEC_MULSCALAR(vTemp, vTemp, g_pInterfaceResMgr->GetXRatio());
	VEC_ADD(vPos, vPos, vTemp);

	VEC_COPY(bcs.vPos, vPos);
    bcs.rRot = rRot;
	VEC_COPY(bcs.vInitialScale, vScale);
	VEC_COPY(bcs.vFinalScale, vScale);

	VEC_SET(bcs.vInitialColor, 1.0f, 1.0f, 1.0f);
	VEC_SET(bcs.vFinalColor, 1.0f, 1.0f, 1.0f);
	bcs.bUseUserColors = LTTRUE;


	g_pLayoutMgr->GetBackSprite(sprName,sizeof(sprName));
	if ( sprName[0] )
	{
		bcs.pFilename = sprName;
		bcs.dwFlags = FLAG_VISIBLE | FLAG_FOGDISABLE | FLAG_NOLIGHT;
		bcs.nType = OT_SPRITE;
		bcs.fInitialAlpha = 1.0f;
		bcs.fFinalAlpha = 1.0f;
		bcs.fLifeTime = 1000000.0f;

		if (m_BackSprite.Init(&bcs))
		{
			m_BackSprite.CreateObject(g_pLTClient);
		}
	}

//TEST: Mouse Light
/*
	if (!g_hLight)
	{
		ObjectCreateStruct createStruct;
		INIT_OBJECTCREATESTRUCT(createStruct);

		createStruct.m_ObjectType	= OT_LIGHT;
		createStruct.m_Flags		= FLAG_VISIBLE | FLAG_ONLYLIGHTOBJECTS;

        g_pLTClient->GetObjectPos(m_hInterfaceCamera, &createStruct.m_Pos);
		//vPos -= (vF * (g_fBackDist / 2.0f));
		//createStruct.m_Pos = vPos;

		createStruct.m_Pos.x += 10.0f;
		createStruct.m_Pos.y += 25.0f;
		createStruct.m_Pos.z -= 300.0f;

        g_hLight = g_pLTClient->CreateObject(&createStruct);

        g_pLTClient->SetLightColor(g_hLight, 1.0f, 1.0f, 1.0f);
        g_pLTClient->SetLightRadius(g_hLight, 1500.0f);
	}
*/
// END TEST!!!

}

void CInterfaceMgr::UpdateInterfaceBackground()
{

/*
	if (GetConsoleInt("MouseLight",0))
	{
		LTVector vPos;
		LTRotation rRot;

		g_pLTClient->GetObjectPos(m_hInterfaceCamera, &vPos);
		g_pLTClient->GetObjectRotation(m_hInterfaceCamera, &rRot);

		vPos.z -= 300.0f;

		float x = ((float)m_CursorPos.x - ((float)g_pInterfaceResMgr->GetScreenWidth() / 2.0f));
		float y = (((float)g_pInterfaceResMgr->GetScreenHeight() / 2.0f) - (float)m_CursorPos.y);

		g_pLTClient->CPrint("light pos: (%0.2f,%0.2f)",x,y);
		vPos.x+=x;
		vPos.y+=y;

		g_pLTClient->SetObjectPos(g_hLight, &vPos);
	//	g_pLTClient->SetObjectPos(m_BackSprite.GetObject(), &vPos);
	}
*/
}

void CInterfaceMgr::RemoveInterfaceBackground()
{
	m_BackSprite.Term();
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::AddInterfaceSFX
//
//	PURPOSE:	Add a SFX object to the interface
//
// --------------------------------------------------------------------------- //
void CInterfaceMgr::AddInterfaceSFX(CSpecialFX* pSFX, ISFXType eType)
{
	if (!pSFX) return;

	SFXArray::iterator iter = m_InterfaceSFX.begin();
	uint32 index = 0;
	while (iter != m_InterfaceSFX.end() && (*iter) != pSFX)
	{
		iter++;
		index++;
	}
	if (index >= m_InterfaceSFX.size())
	{
		if (m_InterfaceSFX.size() < MAX_INTERFACE_SFX)
		{
			m_InterfaceSFX.push_back(pSFX);
		}
	}

	HOBJECT hObj = pSFX->GetObject();
	if (!hObj)
		return;

	if (GetObjectType(hObj)== OT_MODEL)
	{
		char* pAniName = LTNULL;
		switch (eType)
		{
			case IFX_NORMAL :
				// 광식 :
				pAniName = "IF_SELECT";
				//pAniName = "Interface";
			break;

			case IFX_ATTACH :
				pAniName = "Hand";
			break;

			case IFX_WORLD :
				pAniName = "World";
			break;

			case IFX_MENU_ATTACH:	// abm 3/7/02 new fx type for when an ani is prespecified
				pAniName = LTNULL;
			break;

			default :
				pAniName = "World";
			break;


		}

		if (pAniName)
		{
			uint32 dwAni = g_pLTClient->GetAnimIndex(hObj, pAniName);
			if (dwAni != INVALID_ANI)
			{
				g_pLTClient->SetModelAnimation(hObj, dwAni);
			}
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::RemoveInterfaceSFX
//
//	PURPOSE:	Remove a SFX object from the interface
//
// --------------------------------------------------------------------------- //
void CInterfaceMgr::RemoveInterfaceSFX(CSpecialFX* pSFX)
{
	SFXArray::iterator iter = m_InterfaceSFX.begin();
	uint32 index = 0;
	while (iter != m_InterfaceSFX.end() && (*iter) != pSFX)
	{
		iter++;
	}
	if (iter != m_InterfaceSFX.end())
	{
		m_InterfaceSFX.erase(iter);	
	}
}

void CInterfaceMgr::AddInterfaceFX(CLIENTFX_LINK* pLink, char *pFXName, LTVector vPos, LTBOOL bLoop)
{
	uint32 dwFlags = 0;
	if (bLoop)
		dwFlags |= FXFLAG_LOOP;

	CLIENTFX_CREATESTRUCT	fxInit(pFXName , dwFlags, vPos ); 
	CLIENTFX_LINK fxLink;
	if (!pLink)
		pLink = &fxLink;

	m_InterfaceFXMgr.CreateClientFX(pLink, fxInit, LTTRUE );

	if (pLink->IsValid())
	{
		pLink->GetInstance()->SetPos(vPos,vPos);
		pLink->GetInstance()->Show();
	}
	else
	{
		g_pLTClient->CPrint("ERROR: Failed to create interface FX %s",pFXName);
	}

}
void CInterfaceMgr::RemoveInterfaceFX()
{
	m_InterfaceFXMgr.ShutdownAllFX();
	m_SelectFX.ClearLink();
}

void CInterfaceMgr::RemoveInterfaceFX(CLIENTFX_LINK	*pLink)
{
	m_InterfaceFXMgr.ShutdownClientFX(pLink);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	RemoveAllInterfaceSFX
//
//	PURPOSE:	Remove the 3D objects used as a background for the menu
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::RemoveAllInterfaceSFX()
{
	m_InterfaceSFX.clear();
	RemoveInterfaceBackground();
	RemoveInterfaceLights();

	m_InterfaceFXMgr.ShutdownAllFX();
	m_SelectFX.ClearLink();
}

void CInterfaceMgr::AddInterfaceLight(HOBJECT hLight)
{
	if (!hLight) return;

	ObjectArray::iterator iter = m_InterfaceLights.begin();
	while (iter != m_InterfaceLights.end() && (*iter) != hLight)
	{
		iter++;
	}
	if (iter == m_InterfaceLights.end())
	{
		if (m_InterfaceLights.size() < MAX_INTERFACE_LIGHTS)
		{
			m_InterfaceLights.push_back(hLight);
		}
	}

}

void CInterfaceMgr::RemoveInterfaceLights()
{
	ObjectArray::iterator iter = m_InterfaceLights.begin();
	while (iter != m_InterfaceLights.end())
	{
		if( *iter )
		{
		g_pLTClient->RemoveObject((*iter));
		}

		iter++;
	}
	m_InterfaceLights.clear();
}

void CInterfaceMgr::UpdateModelAnimations(LTFLOAT fCurFrameDelta)
{
    for (uint32 i = 0; i < m_InterfaceSFX.size(); i++)
	{
		uint32 nObjType;
		g_pCommonLT->GetObjectType(m_InterfaceSFX[i]->GetObject(), &nObjType);
		if (nObjType == OT_MODEL)
		{
			g_pModelLT->UpdateMainTracker(m_InterfaceSFX[i]->GetObject(), fCurFrameDelta);
		}
	}

	CLinkList<CLIENTFX_INSTANCE *>* pInstList = m_InterfaceFXMgr.GetActiveFXList();
	CLIENTFX_INSTANCE	*pInst;

	CLinkListNode<CLIENTFX_INSTANCE *> *pInstNode = pInstList->GetHead();
	while( pInstNode )
	{
		pInst	= pInstNode->m_Data;

		CLinkListNode<FX_LINK>				*pActiveNode;
		CBaseFX								*pFX;

		pActiveNode = pInst->m_collActiveFX.GetHead();
		while( pActiveNode )
		{
			pFX = pActiveNode->m_Data.m_pFX;
			if( pFX) 
			{
				uint32 nObjType;
				g_pCommonLT->GetObjectType(pFX->GetFXObject(), &nObjType);
				if (nObjType == OT_MODEL)
				{
					g_pModelLT->UpdateMainTracker(pFX->GetFXObject(), fCurFrameDelta);
				}
			}
			pActiveNode = pActiveNode->m_pNext;
		}
		pInstNode = pInstNode->m_pNext;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::UpdateInterfaceSFX
//
//	PURPOSE:	Update the 3D Objects used as a background for the menu
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::UpdateInterfaceSFX()
{

//TEST: Mouse Light
//	UpdateInterfaceBackground();

	uint32 numSfx = m_InterfaceSFX.size();
	HLOCALOBJ objs[kMaxFX];

	int next = 0;
	int infoIndex = 0;
    for (uint32 i = 0; i < numSfx; i++)
	{
		if (m_InterfaceSFX[i]->Update())
		{
			FXRenderList::iterator iter = renderList.begin();

			while (iter != renderList.end() && (*iter)->nLayer < m_InterfaceSFX[i]->GetMenuLayer())
			{
				iter++;
			}

			if(infoIndex < kMaxFX)
			{
				sRenderInfo[infoIndex].hObj = m_InterfaceSFX[i]->GetObject();
				sRenderInfo[infoIndex].nLayer = m_InterfaceSFX[i]->GetMenuLayer();
				renderList.insert(iter,&sRenderInfo[infoIndex]);
				infoIndex++;
			}
		}
	}
	CLinkList<CLIENTFX_INSTANCE *>* pInstList = m_InterfaceFXMgr.GetActiveFXList();
	CLIENTFX_INSTANCE	*pInst;

	CLinkListNode<CLIENTFX_INSTANCE *> *pInstNode = pInstList->GetHead();

	while( pInstNode )
	{
		pInst	= pInstNode->m_Data;

		CLinkListNode<FX_LINK>				*pActiveNode;
		CBaseFX								*pFX;

		pActiveNode = pInst->m_collActiveFX.GetHead();
		while( pActiveNode )
		{
			pFX = pActiveNode->m_Data.m_pFX;
			if( pFX) 
			{
				FXRenderList::iterator iter = renderList.begin();

				while (iter != renderList.end() && (*iter)->nLayer < pFX->GetMenuLayer())
				{
					iter++;
				}

				if(infoIndex < kMaxFX)
				{
					sRenderInfo[infoIndex].hObj = pFX->GetFXObject();
					sRenderInfo[infoIndex].nLayer = pFX->GetMenuLayer();
					renderList.insert(iter,&sRenderInfo[infoIndex]);
					infoIndex++;
				}
			}
			pActiveNode = pActiveNode->m_pNext;
		}
		pInstNode = pInstNode->m_pNext;
	}


	// Render the effects
	m_InterfaceFXMgr.RenderAllActiveFX( true );

	//step through the render list
	FXRenderList::iterator iter = renderList.begin();
	static char szTmp[256];
	static char szNum[16];
	while (iter != renderList.end())
	{
		next = 0;

		//get the menu layer of the first item
		uint8 layer = (*iter)->nLayer;

		//add each item of that same layer to the list of objects to be rendered during this pass
		while (iter != renderList.end() && next < kMaxFX && (*iter)->nLayer == layer)
		{
			objs[next] = (*iter)->hObj;
			next++;
			iter++;
		}

		//add the lights
		for ( ObjectArray::iterator iter = m_InterfaceLights.begin( ); iter !=  m_InterfaceLights.end() && 
			next < kMaxFX; iter++ )
		{
			HOBJECT hObj = *iter;
			if ( hObj )
			{
				objs[next] = hObj;
				next++;
			}
		}


		g_pLTClient->RenderObjects(m_hInterfaceCamera, objs, next,g_pLTClient->GetFrameTime());
	
	}


	//the updating of the active effects comes at the end of the update so that way chained
	//effects will be able to setup prior to the next rendering and prevent popping issues
	m_InterfaceFXMgr.UpdateAllActiveFX( true );

	renderList.clear();	
}


/******************************************************************/
void CInterfaceMgr::RequestInterfaceSound(InterfaceSound eIS)
{
	if (m_eNextSound <= eIS)
	{
//		ClearInterfaceSound();

		m_eNextSound = eIS;

//		m_hSound = UpdateInterfaceSound();
		UpdateInterfaceSound();
	}
}

void CInterfaceMgr::ClearInterfaceSound()
{
	m_eNextSound = IS_NONE;

	if (m_hSound)
	{
        g_pLTClient->SoundMgr()->KillSound(m_hSound);
		m_hSound = LTNULL;
	}
}

HLTSOUND CInterfaceMgr::UpdateInterfaceSound()
{
	HLTSOUND hSnd = LTNULL;

	switch (m_eNextSound)
	{
		case IS_OVER:
			hSnd = g_pClientSoundMgr->PlaySoundLocal("SA_Interface\\Snd\\Button\\Over.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		break;
		case IS_CLICK_NORMAL:
			hSnd = g_pClientSoundMgr->PlaySoundLocal("SA_Interface\\Snd\\Button\\ClickNormal.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		break;
		case IS_CLICK_FLAT:
			hSnd = g_pClientSoundMgr->PlaySoundLocal("SA_Interface\\Snd\\Button\\ClickFlat.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		break;
		case IS_CLICK_RADIO:
			hSnd = g_pClientSoundMgr->PlaySoundLocal("SA_Interface\\Snd\\Button\\ClickRadio.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		break;
		case IS_CLICK_CANCEL:
			hSnd = g_pClientSoundMgr->PlaySoundLocal("SA_Interface\\Snd\\Button\\ClickCancel.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		break;
		default :
		break;
	}
	m_eNextSound = IS_NONE;

	return hSnd;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::NextMovie
//
//	PURPOSE:	Go to the next movie, if there is one
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::NextMovie(bool bEndMovies /*=false*/)
{
	ILTVideoMgr* pVideoMgr = g_pLTClient->VideoMgr();
	if (!pVideoMgr) return;

	if (!(GetAdvancedOptions() & AO_MOVIES) || g_vtDisableMovies.GetFloat())
	{
		SwitchToScreen(SCREEN_ID_MAIN);
		return;
	}

	if (m_hMovie)
	{
		pVideoMgr->StopVideo(m_hMovie);
		m_hMovie = LTNULL;
	}

	char* pMovie = (bEndMovies ? LTNULL : GetCurrentMovie());

	if (!pMovie || pVideoMgr->StartOnScreenVideo(pMovie, PLAYBACK_FULLSCREEN, m_hMovie) != LT_OK)
	{
		m_nCurMovie = 0;
		m_hMovie = LTNULL;
		SwitchToScreen(SCREEN_ID_MAIN);
		return;
	}

 	m_nCurMovie++;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::GetCurrentMovie
//
//	PURPOSE:	Get the name of the current movie to play...
//
// --------------------------------------------------------------------------- //

char* CInterfaceMgr::GetCurrentMovie()
{
	char* pMovie = LTNULL;

	switch (m_nCurMovie)
	{
		case eSierra :
			pMovie = "Movies\\sierralogo.bik";
		break;

		case eFox :
			pMovie = "Movies\\foxlogo.bik";
		break;

		case eMonolith :
			pMovie = "Movies\\lithLogo.bik";
		break;

		case eLithTech :
			pMovie = "Movies\\ltLogo.bik";
		break;

		default : break;
	}

	return pMovie;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::NextDemoScreen
//
//	PURPOSE:	Go to the next marketing screen, if there is one
//
// --------------------------------------------------------------------------- //

void CInterfaceMgr::NextDemoScreen()
{
    g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);

	if (g_hDemo)
	{
		g_pLTClient->DeleteSurface(g_hDemo);
		g_hDemo = LTNULL;
	}
	g_nDemo++;
	if (g_nDemo < g_nNumDemoScreens)
	{
		g_hDemo = g_pLTClient->CreateSurfaceFromBitmap(g_szDemoScreens[g_nDemo]);
	}
	if (!g_hDemo)
	{
		m_bSeenDemoScreens = LTTRUE;
	}
}


void CInterfaceMgr::ShowDemoScreens(LTBOOL bQuit)
{
	m_bQuitAfterDemoScreens = bQuit;
	m_bSeenDemoScreens = LTFALSE;
	ChangeState(GS_DEMOSCREEN);
}



// Find the filename in the missing filename result..
const char *FindFilename(const char *pFileName)
{
	// Jump to the end..
	const char *pResult = &pFileName[strlen(pFileName)];
	// Look for a backslash
	while ((pResult != pFileName) && (pResult[-1] != '\\'))
		--pResult;
	// Return that..
	return pResult;
}

//[MURSUM]=========================================================
void CInterfaceMgr::Disconnected(uint32 nDisconnectFlag)
{
	eScreenID destination = SCREEN_ID_SCREEN_ROOM; 
	
	uint32 nDisconnectCode = g_pClientMultiplayerMgr->GetDisconnectCode();
	const char *pDisconnectMsg = g_pClientMultiplayerMgr->GetDisconnectMsg();
	uint32 nMsgId = 0;
	LTBOOL	bReturnBufferRoom = LTTRUE;

	switch (nDisconnectCode)
	{
		case eDisconnect_NotSameGUID:		//GUID가 다름
			g_pLTClient->CPrint( "CInterfaceMgr::Disconnected - eDisconnect_NotSameGUID" );
			SetExitMessage( "P2P 호스트와 버전이 맞지 않습니다.\r\n\r\nERROR CODE : -1" );			
			break;
		case eDisconnect_WrongPassword:		//PASSWORD가 다름 ( 여기로는 절대 안들어 와야함 )
			g_pLTClient->CPrint( "CInterfaceMgr::Disconnected - eDisconnect_WrongPassword" );
			SetExitMessage( "게임 입장에 필요한 비밀번호가 맞지 않습니다." );
			break;

		case 0 : // Avoid the warning..
		default :
		{
			if( nDisconnectFlag == LT_REJECTED )	//강퇴 or Server FULL
			{
				g_pLTClient->CPrint( "CInterfaceMgr::Disconnected - KickBan" );
				SetExitMessage( "P2P 호스트의 허용 가능한 인원을 초과 하였습니다.\r\n\r\nERROR CODE : -2" );
			}
			else if ( nDisconnectFlag == LT_MISSINGWORLDFILE )	//서버와 다른 월드 파일
			{
				g_pLTClient->CPrint( "CInterfaceMgr::Disconnected - Missing World" );
				SetExitMessage( "P2P 호스트와 버전이 맞지 않습니다.\r\n\r\nERROR CODE : -3" );
			}
			else if( nDisconnectFlag == LT_INVALIDSERVERPACKET )
			{
				//방장 위임때 가끔 나오는 에러
				g_pLTClient->CPrint( "ERROR Disconnected" );
				SetExitMessage( "P2P 호스트와 연결이 끊어졌습니다.\r\n\r\nERROR CODE : -4" );
			}
			else if( nDisconnectFlag == LT_SILENT_REJECTED ) //정상적으로 연결이 끊어진 경우 ( 게임종료 )
			{
				g_pLTClient->CPrint( "CInterfaceMgr::Disconnected - SilentKickBan" );
				SetExitMessage( LTNULL );
			}
			else
			{
				g_pLTClient->CPrint( "CInterfaceMgr::Disconnected - DisconnectedFromServer" );
				bReturnBufferRoom = LTFALSE;
				if( GS_PLAYING == GetGameState() )
				{
					if( g_pPlayerMgr ) g_pPlayerMgr->DisconnectFromServer();
					if( g_pMissionMgr ) g_pMissionMgr->DisconnectFromServer();
				}
			}			
		}
		break;
	}
	
	g_pClientMultiplayerMgr->ClearDisconnectCode();

	if (GetGameState() == GS_LOADINGLEVEL || 
		GetGameState() == GS_MANDATEHOST ||
		GetGameState() == GS_REJOINHOST ||
		bReturnBufferRoom )
	{
		g_Network.EnableNetwork(LTTRUE);
		g_Network.CS_ExitGameRequest();
	}
}

void CInterfaceMgr::ConnectionFailed(uint32 nConnectionError)
{
	if (GetGameState() == GS_LOADINGLEVEL ||
		GetGameState() == GS_MANDATEHOST ||
		GetGameState() == GS_REJOINHOST )
	{
		g_pLTClient->CPrint( "ERROR ConnectionFailed() LOADINGLEVEL nConnectionError(%d)",
							  nConnectionError );

		switch( nConnectionError )
		{
		case LT_TIMEOUT:
			//"P2P 서버로 부터 응답이 없습니다.\r\n\r\nERROR : LT_TIMEOUT"
			SetExitMessage( NF_GetMsg( SA_TEXT_HELP_CONNECTION_FAILED_TIMEOUT ) );
			break;
		case LT_REJECTED:
			//"접속 가능한 인원을 초과 하였습니다.\r\n\r\nERROR : LT_REJECTED"
			SetExitMessage( NF_GetMsg( SA_TEXT_HELP_CONNECTION_FAILED_REJECTED ) );
			break;
		case LT_CANTBINDTOPORT:
			//"네트웍 연결에 문제가 발생하였습니다.\r\n\r\nERROR : LT_CANTBINDTOPORT"
			SetExitMessage( NF_GetMsg( SA_TEXT_HELP_CONNECTION_FAILED_CANTBINDTOPORT ) );
			break;
		case LT_NOTSAMEGUID:
			//"서버와 버전이 맞지 않습니다.\r\n\r\nERROR : LT_NOTSAMEGUID"
			SetExitMessage( NF_GetMsg( SA_TEXT_HELP_CONNECTION_FAILED_NOTSAMEGUID ) );
			break;
		default:
			//"P2P 서버에 접속하던 중 오류가 발생하였습니다.\r\n\r\nERROR : LT_UNKNOWN"
			SetExitMessage( NF_GetMsg( SA_TEXT_HELP_CONNECTION_FAILED_UNKNOWN ) );
			break;
		}
		
		g_Network.EnableNetwork(LTTRUE);
		g_Network.CS_ExitGameRequest();
	}
	else
	{
		g_pLTClient->CPrint( "ERROR ConnectionFailed() nConnectionError(%d)",
							  nConnectionError );
		ClearAllScreenBuffers();	
	}
}
//==============================================================

// hides or shows cursor based on current game state
void CInterfaceMgr::UpdateCursorState()
{
	switch (m_eGameState)
	{
		case GS_EXITINGLEVEL:
		case GS_PLAYING:
		{	
			// 2nd param specifiies whether or not to lock the cursor to the
			// center of the screen
			m_CursorMgr.UseCursor(m_MessageBox.IsVisible(),!m_MessageBox.IsVisible());
		}
		break;

		case GS_LOADINGLEVEL:
		case GS_POPUP:
		case GS_PAUSED:
		case GS_DEMOSCREEN:
		case GS_MOVIE:
		case GS_UNDEFINED:
		case GS_MANDATEHOST:
		case GS_REJOINHOST:
		{
			m_CursorMgr.UseCursor(FALSE);
		}
		break;
	
		case GS_SPLASHSCREEN:
		case GS_SCREEN:
		default:
		{
			m_CursorMgr.UseCursor(TRUE);
		}
	}

	ShowCursor(LTFALSE);
}



void CInterfaceMgr::UpdateClientList()
{
	//[MURSUM]
	//사용안함. 써버로 부터 받는다
	/*
	if (IsMultiplayerGame())
	{
		// Don't send update requests more than once per second
		if ((g_pLTClient->GetTime() - m_fLastUpdateRequestTime) > 1.0f)
		{
			g_pLTClient->CPrint("Asking for clientlist update");
			SendEmptyServerMsg(MID_MULTIPLAYER_UPDATE, MESSAGE_GUARANTEED);
			m_fLastUpdateRequestTime = g_pLTClient->GetTime();
		}
	}
	*/
}



LTVector CInterfaceMgr::GetWorldFromScreenPos(LTIntPt pos, LTFLOAT fDepth)
{
	LTVector vPos;

	LTFLOAT fx = (2.0f * ((LTFLOAT)pos.x / (LTFLOAT)g_pInterfaceResMgr->GetScreenWidth() )) - 1.0f;
	LTFLOAT fy = 1.0f - (2.0f * ((LTFLOAT)pos.y / (LTFLOAT)g_pInterfaceResMgr->GetScreenHeight()) );

	vPos.x =  fx * g_fFovXTan * fDepth;
	vPos.y =  fy * g_fFovYTan * fDepth;
	vPos.z =  fDepth;

	return vPos;
}

//transforms world space to camera space to screen coordinates
//return vector's x and y are screen coordinates
//return vector's z is distance from the camera

LTVector CInterfaceMgr::GetScreenFromWorldPos(LTVector vPos, HOBJECT hCamera)
{
	LTVector tmp,pos;
	LTMatrix mCam;
	mCam = GetCameraTransform(hCamera);
	mCam.Normalize();

	MatVMul((LTVector*)&tmp, &mCam, (LTVector*)&vPos);

	LTFLOAT fx = tmp.x / (g_fFovXTan * tmp.z);
	LTFLOAT fy = tmp.y / (g_fFovYTan * tmp.z);

	pos.x =  ((LTFLOAT)g_pInterfaceResMgr->GetScreenWidth() * (1.0f + fx) * 0.5f );
	pos.y =	 ((LTFLOAT)g_pInterfaceResMgr->GetScreenHeight() * (1.0f - fy) * 0.5f);
	pos.z =  tmp.z;

	return pos;
}

void CInterfaceMgr::SetSelectFX(char* pFXName)
{
	SAFE_STRCPY(m_szSelectFXName,pFXName);
}

void CInterfaceMgr::ShowSelectFX(const LTIntPt &pos)
{
	if (!strlen(m_szSelectFXName)) return;
	if (m_SelectFX.IsValid() && GetConsoleInt("ShutdownSelect",1))
	{
		m_InterfaceFXMgr.ShutdownClientFX(&m_SelectFX);
	}
	LTVector vPos = GetWorldFromScreenPos(pos,25.0f);
	CLIENTFX_CREATESTRUCT	fxInit(m_szSelectFXName , LTNULL, vPos ); 

	m_InterfaceFXMgr.CreateClientFX(&m_SelectFX, fxInit, LTTRUE );
	if( !m_SelectFX.IsValid() )
	{
		char szError[256];
		LTSNPrintF( szError, sizeof(szError), "CInterfaceMgr::ShowSelectFX:  Could not create client fx %s", m_szSelectFXName );
		UBER_ASSERT( 0, szError );
		return;
	}

	if(m_SelectFX.IsValid())
		m_SelectFX.GetInstance()->Show();

}

void CInterfaceMgr::NextWeapon(int nCommand)
{
	CClientWeaponMgr const *pClientWeaponMgr = g_pPlayerMgr->GetClientWeaponMgr();
	IClientWeaponBase const *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
	if (pClientWeapon && !pClientWeaponMgr->WeaponsEnabled()) return;

	//can't bring up chooser while disabling a gadget target
	if (g_pPlayerMgr->IsDisabling()) return;

	uint8 nClass = 0;
	switch (nCommand)
	{
	case COMMAND_ID_STRAFE :
		nClass = 1;
		break;
	case COMMAND_ID_NEXT_AMMO :
		nClass = 2;
		break;
	case COMMAND_ID_HOLSTER :
		nClass = 3;
		break;
	case COMMAND_ID_MOVE_BODY :
		nClass = 4;
		break;
	case COMMAND_ID_DUCKLOCK :
		nClass = 5;
		break;
	case COMMAND_ID_COMPASS :
		nClass = 6;
		break;
	default:
		return;		
	}

	if (m_AmmoChooser.IsOpen())
	{
		m_AmmoChooser.Close();
	}
	if (m_WeaponChooser.Open(nClass))
	{
		m_WeaponChooser.NextWeapon(nClass);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnObjectRemove()
//
//	PURPOSE:	Handle removal of a server created object...
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::OnObjectRemove(HLOCALOBJ hObj)
{
	// Make sure we're initialized.
	if( !IsInitialized( ))
		return;

	if (!hObj) return;

	if( g_pGameClientShell->ShouldUseRadar())
		g_pRadar->RemoveObject( hObj );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::IsInGame()
//
//	PURPOSE:	Check if we're in one of the "in game" states.
//
// ----------------------------------------------------------------------- //

bool CInterfaceMgr::IsInGame( )
{
	switch( GetGameState( ))
	{
		case GS_PLAYING:
		case GS_EXITINGLEVEL:
			return true;
		default:
			return false;
	}

	return false;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::HandleDisplayTimerMsg
//
//	PURPOSE:	Handle update to display timer.
//
// ----------------------------------------------------------------------- //

bool CInterfaceMgr::HandleDisplayTimerMsg( ILTMessage_Read& msg )
{
    float fGameTimeEnd = msg.Readfloat();
	bool bPaused = !!msg.Readuint8();
	uint8 nTeamId = msg.Readuint8( );

	// Calculate time to the end.
	switch( nTeamId )
	{
		case 0:
			//m_BlueInterfaceTimer.SetTime( fGameTimeEnd, bPaused );
			ASSERT( !"CInterfaceMgr::HandleDisplayTimerMsg" );
			break;
		case 1:
			//m_RedInterfaceTimer.SetTime( fGameTimeEnd, bPaused );
			ASSERT( !"CInterfaceMgr::HandleDisplayTimerMsg" );
			break;
		default:
			//m_InterfaceTimer.SetTime( fGameTimeEnd, bPaused );
			//g_pHUDTimer->SetTime( fGameTimeEnd, bPaused );
			break;
	}
    return LTTRUE;
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::LoadFailed
//
//	PURPOSE:	Handle failed load attempt, specify what screen to go to
//
// ----------------------------------------------------------------------- //

void CInterfaceMgr::LoadFailed(eScreenID eScreenToShow /*= SCREEN_ID_NONE*/, uint32 nLoadFailedMsgId /* = -1 */ )  
{	
	m_bLoadFailed = LTTRUE;
}

void CInterfaceMgr::HandlePlayerTeamChange( int32 nServerIndex, uint8 nTeamID )
{
	T_PLAYER_INFOptr pPlayerInfo = m_PlayerInfoMgr.GetPlayerByServerIndex(nServerIndex);
	if( pPlayerInfo )
	{
		pPlayerInfo->nTeamID	= nTeamID;

		g_pRadar->RemoveObject( pPlayerInfo->hObject );		
		g_pRadar->AddPlayer( pPlayerInfo->hObject, pPlayerInfo->nPlayerID );

		CCharacterFX* pFX = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(pPlayerInfo->hObject);
		if( pFX )
		{
			pFX->ChangeModel( (ModelId)pPlayerInfo->nModelID[nTeamID], 0, 0 );
		}
	}

	if( nServerIndex == g_GameDoc.GetUniqueIndex() )
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_CHANGE_TEAM);
		cMsg.Writeuint8(nTeamID);
		cMsg.Writeuint8(pPlayerInfo->nModelID[nTeamID]);
		g_pLTClient->SendToServer(cMsg.Read(),MESSAGE_GUARANTEED);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::StartingNewGame
//
//	PURPOSE:	Called when client is starting its connection to the server.
//
// ----------------------------------------------------------------------- //

bool CInterfaceMgr::StartingNewGame( )
{
	SkipPreLoad( false );

	return true;
}

bool CInterfaceMgr::ExitGame()
{
	g_pHUDFlashBangFX->InitVar();

	switch( GetGameState() )
	{
	case GS_LOADINGLEVEL:
		m_bLoadFailed = LTFALSE;
		m_LoadingScreen.Hide();
		break;
	case GS_PLAYING:
	default:
		if( false == this->IsInGame() ) return false;
		break;
	}
	
	return true;
}

void CInterfaceMgr::ExitGameCallBack( LTBOOL bReturn, void* pData )
{
	if( bReturn )
	{
#ifdef _STANDALONE
		if( g_pInterfaceMgr ) g_pInterfaceMgr->SwitchToScreen( SCREEN_ID_SCREEN_ROOM );
#else
		g_Network.CS_ExitGameRequest();
#endif
	}
}

void CInterfaceMgr::SetExitMessage( char* lpszMsg )
{
	if( LTNULL == lpszMsg )
	{
		m_bExitMessage = false;
		}
	else
	{
		m_bExitMessage	= true;
		strcpy( m_szExitMessage, lpszMsg );		
	}
}

//[NOVICE]
void CInterfaceMgr::SetServerPatchNotice( char* szNotice )
{
	m_bServerPatch = ( LTNULL != szNotice );
	lstrcpy( m_szServerPatchNotice, ( LTNULL != szNotice ) ? szNotice : "" ); 
}

// Inline..
/*void CInterfaceMgr::ShowMessageBox(int nStringID, MBCreate* pCreate, uint8 nFontSize, LTBOOL bDefaultReturn)
{
	if( m_MessageBox.IsVisible() )
		m_MessageBox.Close(LTTRUE);
	
	if( GS_UNDEFINED == GetGameState() )
	{
		ChangeState( GS_SPLASHSCREEN );
	}
	else if( GS_SCREEN == GetGameState() )
	{
		CBaseScreen* pScreen = GetScreenMgr()->GetCurrentScreen();
		if( pScreen )
		{
			pScreen->PrePopupState();
		}
	}
	
	m_MessageBox.Show(nStringID,pCreate,nFontSize,bDefaultReturn);
}*/

void CInterfaceMgr::ShowMessageBox(DWORD dwIdx, MBCreate* pCreate, uint8 nFontSize, LTBOOL bDefaultReturn, LTBOOL bIgnoreAfter )
{
	ShowMessageBox( (char*)NF_GetMsg(dwIdx), pCreate, nFontSize, bDefaultReturn, bIgnoreAfter );
}

void CInterfaceMgr::ShowMessageBox(const char *pString, MBCreate* pCreate, uint8 nFontSize, LTBOOL bDefaultReturn, LTBOOL bIgnoreAfter )
{
	//////////////////////////////////////////////////////////////////////////
	// NOVICE - 기존 메시지 박스가, 다음 메시지 박스를 무시하는 녀석이라면..
	if ( m_MessageBox.IsIgnoreAfter() )
		return;

	m_MessageBox.SetIgnoreAfter( bIgnoreAfter );
	//////////////////////////////////////////////////////////////////////////
	

	if( m_MessageBox.IsVisible() )
		m_MessageBox.Close(LTTRUE);
	
	if( GS_UNDEFINED == GetGameState() )
	{
		ChangeState( GS_SPLASHSCREEN );
	}
	else if( GS_SCREEN == GetGameState() )
	{
		CBaseScreen* pScreen = GetScreenMgr()->GetCurrentScreen();
		if( pScreen )
		{
			pScreen->PrePopupState();
		}
	}
	
	m_MessageBox.Show(pString,pCreate,nFontSize,bDefaultReturn);
}

void CInterfaceMgr::ShowMessageBox( const char* pString, eMBType eType, MBCallBackFn pfnCallBack, void* pData, LTBOOL bIgnoreAfter )
{
	//////////////////////////////////////////////////////////////////////////
	// NOVICE - 기존 메시지 박스가, 다음 메시지 박스를 무시하는 녀석이라면..
	if ( m_MessageBox.IsIgnoreAfter() )
		return;
	
	m_MessageBox.SetIgnoreAfter( bIgnoreAfter );
	//////////////////////////////////////////////////////////////////////////

	if( m_MessageBox.IsVisible() ) m_MessageBox.Close(LTTRUE);

	if( GS_UNDEFINED == GetGameState() )
	{
		ChangeState( GS_SPLASHSCREEN );
	}
	else if( GS_SCREEN == GetGameState() )
	{
		CBaseScreen* pScreen = GetScreenMgr()->GetCurrentScreen();
		if( pScreen )
		{
			pScreen->PrePopupState();
		}
	}

	MBCreate mb;
	mb.eType	= eType;
	mb.pFn		= pfnCallBack;
	mb.pData	= pData;
	
	m_MessageBox.Show( pString, &mb, 0, LTTRUE );
}

void CInterfaceMgr::ShowMessageBox( DWORD dwIdx, eMBType eType, MBCallBackFn pfnCallBack, void* pData, LTBOOL bIgnoreAfter )
{
	ShowMessageBox( (char*)NF_GetMsg(dwIdx), eType, pfnCallBack, pData, bIgnoreAfter );
}

void CInterfaceMgr::CloseMessageBox(LTBOOL bReturn)
{
	m_MessageBox.Close(bReturn);
}

int CInterfaceMgr::ChangeSensitive( int nDelta )
{
	int iCurrentSensi = g_pProfileMgr->GetCurrentProfile()->m_nSensitivity;
	
	iCurrentSensi += nDelta;

	if(iCurrentSensi<0) iCurrentSensi = 0;
	if(iCurrentSensi>100) iCurrentSensi = 100;

	char szTempText[128];
	sprintf(szTempText, "Current MouseSensitivity: %d", iCurrentSensi);
	g_pHelpMsgs->AddMessage( szTempText, 12, kMsgNotice, 1.0f );
		
	g_pProfileMgr->GetCurrentProfile()->m_nSensitivity = iCurrentSensi;
	g_pProfileMgr->GetCurrentProfile()->ApplyMouse();
		
	sprintf(szTempText, "%d", iCurrentSensi);
	::WritePrivateProfileString( "Controls", "Sensitivity", szTempText, ".\\Profiles\\Player.txt" );
	g_pPopupOption->m_pEBSensi->SetText(szTempText);
	g_pPopupOption->m_pScrollSensi->SetPosXRatio(((float)g_pPopupOption->m_pEBSensi->GetInt())/100);
	g_pPopupOption->m_pFmGauge_Sensi->SetSize( ((g_pPopupOption->m_pScrollSensi->GetPos().x+8)-
												(g_pPopupOption->m_pFmGauge_Sensi->GetBasePos().x+2)), 15);

	return iCurrentSensi;
}

void CInterfaceMgr::PlayBGM( const char* lpszSndName, LTBOOL bForcePlay )
{
	if( lpszSndName )
	{
		KillBGM();
		m_hBGM = g_pClientSoundMgr->PlayBGMSound( lpszSndName, bForcePlay,
												  SOUNDPRIORITY_MISC_HIGH,
												  PLAYSOUND_LOOP,
												  90,
												  1.0f );
	}
}

void CInterfaceMgr::KillBGM()
{
	if( m_hBGM )
	{
		g_pLTClient->SoundMgr()->KillSound(m_hBGM,true);
		m_hBGM = LTNULL;
	}	
}

LTBOOL CInterfaceMgr::IsPlayBGM()
{
	return (m_hBGM==LTNULL) ? LTFALSE : LTTRUE;
}

void CInterfaceMgr::HandlePlayerScored( int32 nScorer, int32 nVictim, bool bHeadshot, uint8 nWeaponID )
{
	T_PLAYER_INFOptr	pVictim	= m_PlayerInfoMgr.GetPlayerByServerIndex(nVictim);
	T_PLAYER_INFOptr	pScorer	= m_PlayerInfoMgr.GetPlayerByServerIndex(nScorer);

	if( !pVictim || !pScorer ) return;

#ifdef GAME_SHOW
	if( pVictim->bCaptain && pScorer->bCaptain ) return;
#endif
	
	//점수계산
	if( pScorer->nTeamID != pVictim->nTeamID && pScorer != pVictim )
	{
		++pScorer->nKill;
		g_pScores->AddKillCount( pScorer->nTeamID );
	}

	++pVictim->nDeath;

	if( GS_PLAYING == GetGameState() )
	{
		g_pScores->SetUpdate(true);
		
		const WEAPON* pWeapon = g_pWeaponMgr->GetWeapon(nWeaponID);

		if( pWeapon != LTNULL )
		{
			std::string icon = pWeapon->GetDisabledIcon();

			g_pKillMsgs->AddMessage( pScorer->szNickName,
									 pScorer->nTeamID,
									 pVictim->szNickName,
									 pVictim->nTeamID,
									 icon.c_str(), bHeadshot ? NODE_HEAD : 0 );
			
			if( bHeadshot &&	//대가리
				nVictim != nScorer &&		//자살이 아니고
				nScorer == g_GameDoc.GetUniqueIndex() )		//죽인놈이 나면
			{
#ifdef BIG_FAT_MODE_TEST
				if( LTFALSE == g_GameDoc.IsBigHeadMode() )
				{
					g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\Message\\HeadShot.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
				}
				else
				{
					g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\FatMessage\\HeadShot.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
				}
#else
				g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\Message\\HeadShot.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
#endif
			}
		}
		else
		{
			char	szDeathType[64]={0,};
			switch( nWeaponID )
			{
			case WMGR_INVALID_ID:
				strcpy( szDeathType, "낙사" );
				break;
			case WMGR_INVALID_ID-1:
				strcpy( szDeathType, "익사" );
				break;
			}
			g_pKillMsgs->AddMessage( pScorer->szNickName,
									 pScorer->nTeamID,
									 pVictim->szNickName,
									 pVictim->nTeamID,
									 LTNULL, bHeadshot ? NODE_HEAD : 0,
									 (strlen(szDeathType)>0)?szDeathType : LTNULL );
			
			if( bHeadshot &&	//대가리
				nVictim != nScorer &&		//자살이 아니고
				nScorer == g_GameDoc.GetUniqueIndex() )		//죽인놈이 나면
			{
#ifdef BIG_FAT_MODE_TEST
				if( LTFALSE == g_GameDoc.IsBigHeadMode() )
				{
					g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\Message\\HeadShot.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
				}
				else
				{
					g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\FatMessage\\HeadShot.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
				}
#else
				g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\Message\\HeadShot.wav", SOUNDPRIORITY_MISC_HIGH, 0, 90 );
#endif
			}
		}
	}	
}

void CInterfaceMgr::DoPlayerRadio( uint16 nPlayerID, uint16 nTauntID )
{
	if (GetConsoleInt("IgnoreTaunts",0) > 0) return;

	T_PLAYER_INFOptr pPlayerInfo = g_pInterfaceMgr->GetPlayerInfoMgr()->GetLocalPlayer();

	CSFXMgr* pSFXMgr = g_pGameClientShell->GetSFXMgr();
	if( pSFXMgr && pPlayerInfo )
	{
		CCharacterFX* pChar = pSFXMgr->GetCharacterFX(pPlayerInfo->hObject);
		if( pChar && !pChar->IsPlayingTaunt() )
		{
			g_Network.CS_Radio( nPlayerID, nTauntID, g_GameDoc.GetTeam() );
		}
	}
	
	HandlePlayerRadio( nPlayerID, nTauntID, g_GameDoc.GetTeam() );
}

void CInterfaceMgr::HandlePlayerRadio( uint16 nPlayerID, uint16 nTauntID, uint16 nTeamID )
{
	T_PLAYER_INFOptr pPlayerInfo;
#ifdef GAME_SHOW
	if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
	{
		//HOBJECT hObj = g_pPlayerMgr->GetPlayerCamera()->GetAttachedObject();
		//pPlayerInfo = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByHandle(hObj);
		//if( LTNULL == pPlayerInfo || pPlayerInfo->nTeamID != nTeamID ) return;
	}
	else
	{
		if( g_GameDoc.GetTeam() != nTeamID || GetConsoleInt("IgnoreTaunts",0) ) return;
	}
#else
	if( g_GameDoc.GetTeam() != nTeamID || GetConsoleInt("IgnoreTaunts",0) ) return;
#endif

	pPlayerInfo = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByID((uint8)nPlayerID);
	if( !pPlayerInfo  ) return;

	char szTemp[256];
	char szAttr[64];
	uint32  nLocalID;
	g_pLTClient->GetLocalClientID(&nLocalID);

	//Add Msg
	
	if( GetConsoleInt( "NameRadar", 1 ) > 0 )
	{
		g_pNickList->SetRadioMsg( pPlayerInfo->hObject, g_pRadio->GetRadioString((uint8)nTauntID) );
		if( nPlayerID == nLocalID )
		{
			sprintf( szTemp,"[%s] %s",
					 pPlayerInfo->szNickName,
					 g_pRadio->GetRadioString((uint8)nTauntID) );
			g_pChatMsgs->AddMessage(szTemp,kMsgRadio);
		}
	}
	else
	{
		sprintf( szTemp,"[%s] %s",
				 pPlayerInfo->szNickName,
				 g_pRadio->GetRadioString((uint8)nTauntID) );
		g_pChatMsgs->AddMessage(szTemp,kMsgRadio);
		g_pRadar->SetPlayerTalk(nPlayerID);
	}

	//Play Snd	
#ifdef BIG_FAT_MODE_TEST
	if( LTFALSE == g_GameDoc.IsBigHeadMode() )
	{
		sprintf( szAttr, "Radio_%d", nTauntID );
	}
	else
	{
		sprintf( szAttr, "FatRadio_%d", nTauntID );
	}
#else		
	sprintf( szAttr, "Radio_%d", nTauntID );	
#endif

	g_pClientSoundMgr->GetSoundFilename( "Paths", szAttr, szTemp, 256 );
	if (!szTemp[0]) return;
	g_pClientSoundMgr->PlaySoundLocal( szTemp, SOUNDPRIORITY_MISC_HIGH, 0, 100 );
}

//=====================================================================================================









