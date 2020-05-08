// ----------------------------------------------------------------------- //
//
// MODULE  : GameClientShell.cpp
//
// PURPOSE : Game Client Shell - Implementation
//
// CREATED : 9/18/97
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "GameClientShell.h"
#include "InterfaceMgr.h"
#include "PlayerMgr.h"
#include "AssertMgr.h"
#include "MsgIds.h"
#include "CommandIds.h"
#include "ClientUtilities.h"
#include "vkdefs.h"
#include "ClientResShared.h"
#include "SoundTypes.h"
#include "Music.h"
#include "VolumeBrushFX.h"
#include "client_physics.h"
#include "WinUtil.h"
#include "WeaponStringDefs.h"
#include "iltmath.h"
#include "iltphysics.h"
#include "VarTrack.h"
#include "GameButes.h"
#include "SystemDependant.h"
#include "SurfaceFunctions.h"
#include "VehicleMgr.h"
#include "BodyFX.h"
#include "PlayerShared.h"
#include "CharacterFX.h"
#include "iltsoundmgr.h"
#include "ClientWeaponBase.h"
#include "ClientWeaponMgr.h"
#include "ClientTrackedNodeMgr.h"
#include "CMoveMgr.h"
#include "PlayerCamera.h"
#include <stdarg.h>
#include <stdio.h>
#include <Shlobj.h>
#include "ClientMultiplayerMgr.h"
#include "ClientButeMgr.h"
#include "MissionMgr.h"
#include "FXButeMgr.h"
#include "ClientFXDB.h"
#include <sys/timeb.h>
#include "PerformanceTest.h"
#include "ScmdConsole.h"
#include "ScmdConsoleDriver_CShell.h"
#include "mmsystem.h"
#include "zmouse.h"
#include "SystemInfo.h"

// ±¤½Ä
#include "WeaponFXTypes.h"
// [05.02.11] : ControlFlags Test
//---------------------------
#include "LTEulerAngles.h"
//---------------------------
#include "../SA/SAHUDMgr.h"
#include "../SA/ScreenMain.h"
#include "../SA/ScreenRoom.h"

#define GAMECLIENTSHELL
#include "..\\sa\\GameHIIME.h"
//---------------------------------------------------------------------------
// GameHi Client
// Added By kwangsik ==>

//------------------------------------------- (B) Novice (minjin) [03/09/2005]
#include "GameText.h"
#include "GameTextFileMngr.h"
#include "GameMessage.h"
#include "FinalGameText.h"
//------------------------------------------- (E) Novice (minjin) [03/09/2005]

#include <time.h>
#define WM_NETWORK WM_USER + 100
#define NETWORK_REFRASH		1
#define NETWORK_PING		2

#ifdef STRICT
	WNDPROC g_pfnMainWndProc = NULL;
#else
	FARPROC	g_pfnMainWndProc = NULL;
#endif

#define MAX_FRAME_DELTA					0.1f

#define WEAPON_MOVE_INC_VALUE_SLOW		0.0025f
#define WEAPON_MOVE_INC_VALUE_FAST		0.005f

#define VK_TOGGLE_GHOST_MODE			VK_F1
#define VK_TOGGLE_SPECTATOR_MODE		VK_F2
#define VK_TOGGLE_SCREENSHOTMODE		VK_F3
	
//[SPIKE]
#define LOGO_LOADING_TX0  "SA_interface\\menu\\Textures\\Common\\InitializeLoading0.dtx"
#define LOGO_LOADING_TX1  "SA_interface\\menu\\Textures\\Common\\InitializeLoading1.dtx"
#define LOGO_LOADING_TX2  "SA_interface\\menu\\Textures\\Common\\InitializeLoading2.dtx"
#define LOGO_LOADING_TX3  "SA_interface\\menu\\Textures\\Common\\InitializeLoading3.dtx"
#define LOGO_LOADING_TX4  "SA_interface\\menu\\Textures\\Common\\InitializeLoading4.dtx"
#define LOGO_LOADING_TX5  "SA_interface\\menu\\Textures\\Common\\InitializeLoading5.dtx"
#define LOGO_LOADING_TX6  "SA_interface\\menu\\Textures\\Common\\InitializeLoading6.dtx"
#define LOGO_LOADING_TX7  "SA_interface\\menu\\Textures\\Common\\InitializeLoading7.dtx"
#define LOGO_LOADING_TX8  "SA_interface\\menu\\Textures\\Common\\InitializeLoading8.dtx"


bool				g_bScreenShotMode   = false;
bool				g_bRunWindowMode	= false;

HWND				g_hMainWnd = NULL;
RECT*				g_prcClip = NULL;

CGameClientShell*   g_pGameClientShell = NULL;

LTVector            g_vWorldWindVel(0.0f, 0.0f, 0.0f);

PhysicsState		g_normalPhysicsState;
PhysicsState		g_waterPhysicsState;

VarTrack			g_varStartLevelScreenFadeTime;
VarTrack			g_varStartLevelScreenFade;
VarTrack			g_vtUseSoundFilters;
VarTrack			g_vtMakeCubicEnvMapSize;
VarTrack			g_vtMakeCubicEnvMapName;
VarTrack			g_vtApplyWorldOffset;

VarTrack			g_vtPTestMinFPS;
VarTrack			g_vtPTestMaxFPS;

LTBOOL				g_bNoneVideoDevice = LTTRUE;

#ifdef _TRON_E3_DEMO
	#define TRON_E3_DEMO_IDLE_TIME		120.0f
	float             s_fDemoTime		= 0.0f;
#endif

extern CCheatMgr*	g_pCheatMgr;
extern LTVector		g_vPlayerCameraOffset;
extern VarTrack		g_vtFOVXNormal;
extern VarTrack		g_vtFOVYNormal;

// Sample rate
extern int g_nSampleRate;

// Speed hack prevention variables
static _timeb g_StartTimeB;
static uint32 g_nStartClientTime = 0;
static uint32 g_nStartTicks = 0;

char*	g_pTempBuffer = LTNULL;

BOOL HookWindow();
void UnhookWindow();

BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg);
LRESULT CALLBACK HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void InitClientShell()
{
	// Hook up the AssertMgr

	CAssertMgr::Enable();

	// Get our ClientDE pointer

    _ASSERT(g_pLTClient);

	// Init our LT subsystems

    g_pModelLT = g_pLTClient->GetModelLT();
    g_pTransLT = g_pLTClient->GetTransformLT();
    g_pPhysicsLT = g_pLTClient->Physics();
	g_pLTBase = static_cast<ILTCSBase*>(g_pLTClient);

	g_pPhysicsLT->SetStairHeight(DEFAULT_STAIRSTEP_HEIGHT);
}

void TermClientShell()
{
	// Delete our client shell

	// Unhook the AssertMgr and let the CRT handle asserts once again

	CAssertMgr::Disable();
}


void FragSelfFn(int argc, char **argv)
{
	SendEmptyServerMsg(MID_FRAG_SELF);
}

void ClientFXFn(int argc, char **argv)
{
	if(argc == 4)
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_CONSOLE_CLIENTFX);
		cMsg.Writeuint8(0); // 0 = No object
		cMsg.WriteString(argv[0]);
		LTVector vPos;
		vPos.x = (float)atof(argv[1]);
		vPos.y = (float)atof(argv[2]);
		vPos.z = (float)atof(argv[3]);
		cMsg.WriteLTVector(vPos);
		g_pLTClient->SendToServer(cMsg.Read(),MESSAGE_GUARANTEED);
	}
	else if(argc == 2)
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_CONSOLE_CLIENTFX);
		cMsg.Writeuint8(1); // 1 = Object
		cMsg.WriteString(argv[0]);
		cMsg.WriteString(argv[1]);
		g_pLTClient->SendToServer(cMsg.Read(),MESSAGE_GUARANTEED);
	}
	else
	{
		g_pLTClient->CPrint("ClientFX <fxname> [<objectname> OR <xPos> <yPos> <zPos>]\n");
	}
}

void StimulusFn(int argc, char **argv)
{
	if( (argc == 1) || (argc == 2) )
	{
		// Message may contain a multiplier.
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_STIMULUS);
		if(argc == 1)
		{
			cMsg.Writeuint8(1);
			cMsg.WriteString(argv[0]);
		}
		else {
			cMsg.Writeuint8(2);
			cMsg.WriteString(argv[0]);
			cMsg.Writefloat(float(atof(argv[1])));	// multiplier.
		}
		g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);		
	}
	else {
		g_pLTClient->CPrint("Stimulus <stimulus type> [multiplier]");
	}
}

void RenderStimulusFn(int argc, char **argv)
{
	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_RENDER_STIMULUS);
	if(argc == 1)
	{
		cMsg.Writeuint8( (uint8)atoi(argv[0]) );
	}
	else {
		cMsg.Writeuint8( 1 );
	}
	g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);		
}

void AddGoalFn(int argc, char **argv)
{
	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_ADD_GOAL);
	if(argc >= 2)
	{
		cMsg.WriteString(argv[0]);
		cMsg.WriteString(argv[1]);
			
		cMsg.Writeuint32(argc - 2);
		int iArg = 2;
		while( iArg < argc )
		{
			cMsg.WriteString(argv[iArg++]);
		}

		g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);		
	}
	else {
		g_pLTClient->CPrint("AddGoal <AI name> <goal> [name=val ...]");
	}
}

void RemoveGoalFn(int argc, char **argv)
{
	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_REMOVE_GOAL);
	if(argc == 2)
	{
		cMsg.WriteString(argv[0]);
		cMsg.WriteString(argv[1]);
			
		g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);		
	}
	else 
	{
		g_pLTClient->CPrint("RemoveGoal <AI name> <goal>");
	}
}

void ReloadWeaponAttributesFn(int argc, char **argv)
{
    g_pWeaponMgr->Reload();
    g_pLTClient->CPrint("Reloaded weapons attributes file...");
}

void ReloadSurfacesAttributesFn(int argc, char **argv)
{
    g_pSurfaceMgr->Reload();
    g_pLTClient->CPrint("Reloaded surface attributes file...");
}

void ReloadFXAttributesFn(int argc, char **argv)
{
    g_pFXButeMgr->Reload();
    g_pLTClient->CPrint("Reloaded fx attributes file...");

	// Make sure we re-load the weapons and surface data, it has probably
	// changed...
	ReloadWeaponAttributesFn(0, 0);
	ReloadSurfacesAttributesFn(0, 0);
}

void ExitLevelFn(int argc, char **argv)
{
	if (g_pCheatMgr)
	{
		CParsedMsg cMsg( 0, LTNULL );
		g_pCheatMgr->Process( CHEAT_EXITLEVEL, cMsg );
	}
}

void TeleportFn(int argc, char **argv)
{
	if (argc < 3)
	{
        g_pLTClient->CPrint("Teleport x y z");
		return;
	}
	
	if (g_pPlayerMgr)
	{
		LTVector vPos;
		vPos.x = (float) atof(argv[0]);
		vPos.y = (float) atof(argv[1]);
		vPos.z = (float) atof(argv[2]);

		//handle the shift from the current world to the source world
		if((uint32)g_vtApplyWorldOffset.GetFloat(1.0f))
		{
			LTVector vOffset;
			g_pLTClient->GetSourceWorldOffset(vOffset);
			vPos -= vOffset;
		}


		g_pPlayerMgr->Teleport(vPos);
	}
}

void ChaseToggleFn(int argc, char **argv)
{
	if (g_pGameClientShell)
	{
		g_pGameClientShell->ToggleDebugCheat(CHEAT_CHASETOGGLE);
	}
}

void CmdFn(int argc, char **argv)
{
	if (argc < 2)
	{
        g_pLTClient->CPrint("Cmd <command>");
		return;
	}

	// Send message to server...
	char buf[256];
	buf[0] = '\0';
	sprintf(buf, "%s", argv[0]);
	for (int i=1; i < argc; i++)
	{
		strcat(buf, " ");
		strcat(buf, "\"");
		strcat(buf, argv[i]);
		strcat(buf, "\"");
	}

    HSTRING hstrCmd = g_pLTClient->CreateString(buf);

	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_CONSOLE_COMMAND);
	cMsg.WriteHString(hstrCmd);
    g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);

    g_pLTClient->FreeString(hstrCmd);
}

void TriggerFn(int argc, char **argv)
{
	if (argc < 2)
	{
        g_pLTClient->CPrint("Trigger <objectname> <message>");
		return;
	}

	// Send message to server...

    HSTRING hstrObjName = g_pLTClient->CreateString(argv[0]);
    HSTRING hstrMsg     = g_pLTClient->CreateString(argv[1]);

	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_CONSOLE_TRIGGER);
    cMsg.WriteHString(hstrObjName);
    cMsg.WriteHString(hstrMsg);
    g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);

    g_pLTClient->FreeString(hstrObjName);
    g_pLTClient->FreeString(hstrMsg);
}

void ListFn(int argc, char **argv)
{
	if (argc < 1 || !argv)
	{
        g_pLTClient->CPrint("List <classtype>");
		return;
	}

	// Send message to server...

	char buf[100];
	sprintf(buf, "List %s", argv[0]);

    HSTRING hstrMsg = g_pLTClient->CreateString(buf);

	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_CONSOLE_TRIGGER);
    cMsg.WriteHString(LTNULL);
    cMsg.WriteHString(hstrMsg);
    g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);

    g_pLTClient->FreeString(hstrMsg);
}

void ExitGame(bool bResponse, uint32 nUserData)
{
	if (bResponse)
	{
        g_pLTClient->Shutdown();
	}
}

void InitSoundFn(int argc, char **argv)
{
	if (g_pGameClientShell)
	{
		g_pGameClientShell->InitSound();
	}
}

void MusicFn(int argc, char **argv)
{
	if (!g_pGameClientShell->GetMusic()->IsInitialized())
	{
        g_pLTClient->CPrint("Direct Music hasn't been initialized!");
	}

	if (argc < 2)
	{
        g_pLTClient->CPrint("Music <command>");
        g_pLTClient->CPrint("  Commands: (syntax -> Command <Required> [Optional])");
        g_pLTClient->CPrint(" ");
        g_pLTClient->CPrint("    I  <intensity number to set>  [Enact]  - Change Intensity");
        g_pLTClient->CPrint("    PS <segment name> [Enact]              - Play secondary segment");
        g_pLTClient->CPrint("    PM <motif style> <motif name> [Enact]  - Play motif");
        g_pLTClient->CPrint("    V  <volume adjustment in db>           - Change volume");
        g_pLTClient->CPrint("    SS <segment name> [Enact]              - Stop secondary segment");
        g_pLTClient->CPrint("    SM <motif name> [Enact]                - Stop motif");
        g_pLTClient->CPrint("    S  [Enact]                             - Stop music");
        g_pLTClient->CPrint("    P  <intensity> [Enact]                 - Play music");
        g_pLTClient->CPrint(" ");
        g_pLTClient->CPrint("  Enact Change Values:");
        g_pLTClient->CPrint("    Default     - Will use the default value that is defined in the Control File or by DirectMusic");
        g_pLTClient->CPrint("    Immediately - Will happen immediately");
        g_pLTClient->CPrint("    Beat        - Will happen on the next beat");
        g_pLTClient->CPrint("    Measure     - Will happen on the next measure");
        g_pLTClient->CPrint("    Grid        - Will happen on the next grid");
        g_pLTClient->CPrint("    Segment     - Will happen on the next segment transition");

		return;
	}

	// Build the command...

	char buf[512];
	buf[0] = '\0';
	sprintf(buf, "Music");
	for (int i=0; i < argc; i++)
	{
		strcat(buf, " ");
		strcat(buf, argv[i]);
	}

	g_pGameClientShell->GetMusic()->ProcessMusicMessage(buf);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::CGameClientShell()
//
//	PURPOSE:	Initialization
//
// ----------------------------------------------------------------------- //

CGameClientShell::CGameClientShell()
{
	uint32 nTempBufferSize = GetTickCount()%10000;
	g_pTempBuffer = new char[nTempBufferSize];

	m_pMissionMgr = NULL;

	
	g_pGameClientShell = this;

	m_fFrameTime			= 0.0f;

	g_vWorldWindVel.Init();


    m_bMainWindowMinimized  = false;


    m_bGamePaused           = false;

    m_bTweakingWeapon       = false;
    m_bTweakingWeaponMuzzle = false;
	m_bTweakingWeaponBreachOffset = false;
    m_bAdjust1stPersonCamera = false;


    m_bFlashScreen      = false;
	m_fFlashTime		= 0.0f;
	m_fFlashStart		= 0.0f;
	m_fFlashRampUp		= 0.0f;
	m_fFlashRampDown	= 0.0f;
	m_vFlashColor.Init();

    m_bShowPlayerPos        = false;
	m_bShowCamPosRot		= false;

	for (int i=0; i < kMaxDebugStrings; i++)
	{
		m_pLeftDebugString[i] = NULL;
		m_pRightDebugString[i] = NULL;
	}

    m_bAdjustLightScale     = false;
    m_bAdjustLightAdd       = false;
    m_bAdjustFOV            = false;

    m_bFirstUpdate          = false;
    m_bRestoringGame        = false;


	m_eDifficulty			= GD_NORMAL;
	m_eGameType				= eGameTypeTeamDeathmatch;

    m_hBoundingBox          = NULL;

    m_bMainWindowFocus      = false;
	m_bRendererInit			= false;


	m_fNextSoundReverbTime	= 0.0f;
    m_bUseReverb            = false;
	m_fReverbLevel			= 0.0f;

	m_vLastReverbPos.Init();

	m_pClientTrackedNodeMgr = NULL;

	m_pClientMultiplayerMgr = NULL;

	m_bInWorld = false;

	m_bServerPaused = false;
	m_eSwitchingWorldsState = eSwitchingWorldsStateNone;


	m_fInitialServerTime = 0.0f;
	m_fInitialLocalTime = 0.0f;
	m_nSpeedCheatCounter = 0;
 
	//[SPIKE]
	for (i=0; i<STARTLOADING_PARTITIONCOUNT; i++)
	{
		m_hLogoFrame[i] = NULL;
	}

	m_hAgeMark	= LTNULL;

	m_hMutex = LTNULL;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::~CGameClientShell()
//
//	PURPOSE:	Destruction
//
// ----------------------------------------------------------------------- //

CGameClientShell::~CGameClientShell()
{
	for (int i=0; i < kMaxDebugStrings; i++)
	{
		if (m_pLeftDebugString[i])
		{
			g_pFontManager->DestroyPolyString(m_pLeftDebugString[i]);
			m_pLeftDebugString[i] = NULL;
		}

		if (m_pRightDebugString[i])
		{
			g_pFontManager->DestroyPolyString(m_pRightDebugString[i]);
			m_pRightDebugString[i] = NULL;
		}
	}
	
	if (m_hBoundingBox)
	{
        g_pLTClient->RemoveObject(m_hBoundingBox);
	}

	if (g_prcClip)
	{
		debug_delete(g_prcClip);
        g_prcClip = NULL;
	}

	if ( m_pClientTrackedNodeMgr != 0 )
	{
		debug_delete( m_pClientTrackedNodeMgr );
		m_pClientTrackedNodeMgr = 0;
	}

	if( m_pClientMultiplayerMgr )
	{
		debug_delete( m_pClientMultiplayerMgr );
		m_pClientMultiplayerMgr = NULL;
	}

	if( m_hMutex ) CloseHandle( m_hMutex );

	if( g_pTempBuffer )
	{
		delete [] g_pTempBuffer;
		g_pTempBuffer = LTNULL;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::InitSound
//
//	PURPOSE:	Initialize the sounds
//
// ----------------------------------------------------------------------- //

void CGameClientShell::InitSound()
{
	CGameSettings* pSettings = GetInterfaceMgr( )->GetSettings();
	if (!pSettings) 
	{
		g_pLTClient->CPrint( "ERROR 1. CGameClientShell::InitSound()" );
		WriteConsoleFloat( "soundenable", 0.0f );
		WriteConsoleFloat( "musicenable", 0.0f );
		return;
	}

	Sound3DProvider *pSound3DProviderList, *pSound3DProvider;
	InitSoundInfo soundInfo;
    uint32 dwProviderID;
	char sz3dSoundProviderName[_MAX_PATH + 1];

    uint32 dwAdvancedOptions = GetInterfaceMgr( )->GetAdvancedOptions();
	if (!(dwAdvancedOptions & AO_SOUND))
	{
		g_pLTClient->CPrint( "ERROR 2. CGameClientShell::InitSound()" );
		WriteConsoleFloat( "soundenable", 0.0f );
		WriteConsoleFloat( "musicenable", 0.0f );
		return;
	}

    soundInfo.Init();

	// Reload the sounds if there are any...

	soundInfo.m_dwFlags	= INITSOUNDINFOFLAG_RELOADSOUNDS;


	// Get the maximum number of 3d voices to use.
    HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("Max3DVoices");
	if (hVar)
	{
	    soundInfo.m_nNum3DVoices = (uint8)g_pLTClient->GetVarValueFloat(hVar);
	}
	else
	{
		// NOVICE
		soundInfo.m_nNum3DVoices = 32;
	}

	// Get the 3d sound provider id....
	if (!(dwAdvancedOptions & AO_HARDWARESOUND))
	{
		dwProviderID = SOUND3DPROVIDERID_NONE;
	}
	else
	{
		dwProviderID = SOUND3DPROVIDERID_UNKNOWN;
	}

	// if hardware isn't turned off, try and use hardware
	if ( dwProviderID == SOUND3DPROVIDERID_UNKNOWN )
	{
		sz3dSoundProviderName[0] = 0;
        hVar = g_pLTClient->GetConsoleVar("3DSoundProviderName");
		if ( hVar )
		{
            SAFE_STRCPY( sz3dSoundProviderName, g_pLTClient->GetVarValueString( hVar ));
		}
		else
		{
			// try DX hardware
		    SAFE_STRCPY( sz3dSoundProviderName, "DirectSound Hardware" );
		}
	}

	// See if the provider exists....
	if ( dwProviderID != SOUND3DPROVIDERID_NONE )
	{
        ((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->GetSound3DProviderLists( pSound3DProviderList, false, soundInfo.m_nNum3DVoices );
		if ( !pSound3DProviderList )
		{
			g_pLTClient->CPrint( "ERROR 3. CGameClientShell::InitSound()" );
			WriteConsoleFloat( "soundenable", 0.0f );
			WriteConsoleFloat( "musicenable", 0.0f );
			return;
		}

		pSound3DProvider = pSound3DProviderList;
		while ( pSound3DProvider )
		{
			// If the provider is selected by name, then compare the names.
			if (  dwProviderID == SOUND3DPROVIDERID_UNKNOWN )
			{
				if ( strcmp(( const char * )sz3dSoundProviderName, ( const char * )pSound3DProvider->m_szProvider ) == 0 )
					break;
			}
			// Or compare by the id's.
			else if ( pSound3DProvider->m_dwProviderID == dwProviderID )
				break;

			// Not this one, try next one.
			pSound3DProvider = pSound3DProvider->m_pNextProvider;
		}

		// Check if we found one.
		if (pSound3DProvider)
		{
			// Use this provider.
			SAFE_STRCPY( soundInfo.m_sz3DProvider, pSound3DProvider->m_szProvider);
		}
		
        ((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->ReleaseSound3DProviderList(pSound3DProviderList);
	}
	else 
	{
		soundInfo.m_sz3DProvider[0] = 0;
	}

	// Get the maximum number of sw voices to use.
    hVar = g_pLTClient->GetConsoleVar("MaxSWVoices");
	if (hVar)
	{
        soundInfo.m_nNumSWVoices = (uint8)g_pLTClient->GetVarValueFloat(hVar);
	}
	else
	{
		// NOVICE
		soundInfo.m_nNumSWVoices = 128;
	}

	soundInfo.m_nSampleRate		= g_nSampleRate;
	soundInfo.m_nBitsPerSample	= 16;
	soundInfo.m_nVolume			= (unsigned short)pSettings->SoundVolume();

	if ( !pSettings->Sound16Bit( ) )
	{
		soundInfo.m_dwFlags |= INITSOUNDINFOFLAG_CONVERT16TO8;
	}

	soundInfo.m_fDistanceFactor = 1.0f / 64.0f;
	soundInfo.m_fDopplerFactor = 1.0f;

	// Go initialize the sounds...

    m_bUseReverb = false;
    if (((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->InitSound(&soundInfo) == LT_OK)
	{
		if (soundInfo.m_dwResults & INITSOUNDINFORESULTS_REVERB)
		{
            m_bUseReverb = true;
		}

        hVar = g_pLTClient->GetConsoleVar("ReverbLevel");
		if (hVar)
		{
            m_fReverbLevel = g_pLTClient->GetVarValueFloat(hVar);
		}
		else
		{
			m_fReverbLevel = g_pClientButeMgr->GetReverbAttributeFloat(REVERB_BUTE_DEFAULTLEVEL);
		}

		ReverbProperties reverbProperties;
		reverbProperties.m_dwParams = REVERBPARAM_VOLUME;
		reverbProperties.m_fVolume = m_fReverbLevel;
        ((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetReverbProperties(&reverbProperties);
	}


	// [KLS 9/10/02] Since we just reinitialized the sound system we need to make sure
	// any lip-flap sounds that are currently playing are updated since their data
	// may have changed and is accessed directly by the game...

	ResetCharacterFXSoundData();

	m_bSoundInitted = true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::ResetCharacterFXSoundData
//
//	PURPOSE:	Tell all the CharacterFX to update their lip-flap
//				sound data...
//
// ----------------------------------------------------------------------- //

void CGameClientShell::ResetCharacterFXSoundData()
{
	CSpecialFXList* pList = m_sfxMgr.GetFXList(SFX_CHARACTER_ID);
	if (!pList) return;

	int nNumChars = pList->GetSize();

	for (int i=0; i < nNumChars; i++)
	{
		if ((*pList)[i])
		{
			CCharacterFX* pChar = (CCharacterFX*)(*pList)[i];
			pChar->ResetSoundBufferData();
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::CSPrint
//
//	PURPOSE:	Displays a line of text on the client
//
// ----------------------------------------------------------------------- //

void CGameClientShell::CSPrint(char* msg, ...)
{
	// parse the message

	char pMsg[256];
	va_list marker;
	va_start (marker, msg);
	int nSuccess = vsprintf (pMsg, msg, marker);
	va_end (marker);

	if (nSuccess < 0) return;

	// now display the message
	//g_pChatMsgs->AddMessage(pMsg);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnEngineInitialized
//
//	PURPOSE:	Called after engine is fully initialized
//				Handle object initialization here
//
// ----------------------------------------------------------------------- //

uint32 CGameClientShell::OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid)
{
	g_pLTClient->CPrint( "CGameClientShell::OnEngineInitialized" );
#ifdef _FINAL
	m_hMutex = CreateMutex( NULL,TRUE,
							TEXT("???/?$??Rÿh<?_?8?=?i?$ÿh???<") );

	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		g_pLTClient->CPrint( "\t[ERROR] Already runned" );
		g_pLTClient->ShutdownWithMessage("ÀÌ¹Ì °ÔÀÓÀÌ ½ÇÇà Áß ÀÔ´Ï´Ù.\r\n\r\nÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù.");
		return LT_ERROR;		
	}
#endif

	m_bSoundInitted = false;
	
	InitClientShell();

	//CWinUtil::DebugBreak();

	*pAppGuid = GAMEGUID;

	if (!g_hMainWnd)
	{
		HookWindow();
		SetWindowLong(g_hMainWnd,GWL_STYLE,WS_VISIBLE);
		
		float fGammaR = GetConsoleFloat( "gammar", 1.0f );
		float fGammaG = GetConsoleFloat( "gammag", 1.0f );
		float fGammaB = GetConsoleFloat( "gammab", 1.0f );
		this->SetGammaRamp( fGammaR, fGammaG, fGammaB );
	}
	
	//[MURSUM]================================================================
	// ScreenÀ» »ç¿ëÇÏ´Â Network Module °¡ Screenº¸´Ù ¸ÕÀú »ý¼ºµÊ
	// Screen »ý¼º ÀÌÈÄ¿¡ »ý¼ºÇÏµµ·Ï À§Ä¡ º¯°æ
	//========================================================================
	
	// Initialize all the global bute mgrs...

	if (!m_GlobalMgr.Init())
	{
		g_pLTClient->CPrint( "\t[ERROR] m_GlobalMgr.Init()" );
		return LT_ERROR;
	}

	// SPIKE ºñµð¿À ¸Þ¸ð¸® ¼¼ÆÃ  À§Ä¡ º¯°æ  
	//g_pProfileMgr->GetCurrentProfile()->SetVideoMem(m_GlobalMgr.GetVideoMemorySize());  //
	// Initialize global console variables...

	g_varStartLevelScreenFade.Init(g_pLTClient, "ScreenFadeAtLevelStart", NULL, 0.0f);
	g_varStartLevelScreenFadeTime.Init(g_pLTClient, "ScreenFadeInAtLevelStartTime", NULL, 1.5f);
	
	g_vtUseSoundFilters.Init(g_pLTClient, "SoundFilters", NULL, 1.0f);

	g_vtMakeCubicEnvMapSize.Init(g_pLTClient, "MakeCubicEnvMapSize", NULL, 256.0f);
	g_vtMakeCubicEnvMapName.Init(g_pLTClient, "MakeCubicEnvMapName", "CubicEnvMap", 0.0f);
	g_vtApplyWorldOffset.Init(g_pLTClient, "ApplyWorldOffset", NULL, 1.0f);

	g_vtPTestMinFPS.Init(g_pLTClient, "PerformanceMinTestFPS", NULL, 25.0f);
	g_vtPTestMaxFPS.Init(g_pLTClient, "PerformanceMaxTestFPS", NULL, 40.0f);

    HCONSOLEVAR hIsSet = g_pLTClient->GetConsoleVar("UpdateRateInitted");
    if (!hIsSet || g_pLTClient->GetVarValueFloat(hIsSet) != 1.0f)
	{
		// Initialize the update rate.
        g_pLTClient->RunConsoleString("+UpdateRateInitted 1");
        g_pLTClient->RunConsoleString("+UpdateRate 60");
	}

	m_cheatMgr.Init();
	m_LightScaleMgr.Init();

	// would be nice to move these to a centralized registry function

    g_pLTClient->RegisterConsoleProgram("Cmd", CmdFn);
    g_pLTClient->RegisterConsoleProgram("Trigger", TriggerFn);
    g_pLTClient->RegisterConsoleProgram("List", ListFn);
    g_pLTClient->RegisterConsoleProgram("FragSelf", FragSelfFn);
    g_pLTClient->RegisterConsoleProgram("ReloadWeapons", ReloadWeaponAttributesFn);
    g_pLTClient->RegisterConsoleProgram("ReloadSurfaces", ReloadSurfacesAttributesFn);
    g_pLTClient->RegisterConsoleProgram("ReloadFX", ReloadFXAttributesFn);
    g_pLTClient->RegisterConsoleProgram("InitSound", InitSoundFn);
    g_pLTClient->RegisterConsoleProgram("ExitLevel", ExitLevelFn);
    g_pLTClient->RegisterConsoleProgram("Teleport", TeleportFn);
    g_pLTClient->RegisterConsoleProgram("ChaseToggle", ChaseToggleFn);
    g_pLTClient->RegisterConsoleProgram("Music", MusicFn);
    g_pLTClient->RegisterConsoleProgram("Stimulus", StimulusFn);
    g_pLTClient->RegisterConsoleProgram("RenderStimulus", RenderStimulusFn);
    g_pLTClient->RegisterConsoleProgram("AddGoal", AddGoalFn);
    g_pLTClient->RegisterConsoleProgram("RemoveGoal", RemoveGoalFn);
	g_pLTClient->RegisterConsoleProgram("ClientFX", ClientFXFn);
	
	g_pLTClient->SetModelHook((ModelHookFn)DefaultModelHook, this);

	g_bRunWindowMode = GetConsoleInt( "windowed", 0 ) > 0 ? true : false;

//	// Make sure the save directory exists...
//	if (!CWinUtil::DirExist("Save"))
//	{
//		CWinUtil::CreateDir("Save");
//	}

	// Add to NumRuns count...

	HCONSOLEVAR hVar;

	float nNumRuns = 0.0f;
    hVar = g_pLTClient->GetConsoleVar("NumRuns");
	if (hVar)
	{
        nNumRuns = g_pLTClient->GetVarValueFloat(hVar);
	}
	nNumRuns++;

	char strConsole[64];
	sprintf (strConsole, "+NumRuns %f", nNumRuns);
    g_pLTClient->RunConsoleString(strConsole);

    bool bNetworkGameStarted = false;
	
	//setup our render mode to only allow hardware TnL rendering modes at 32 bpp
	int nCurW, nCurH, nCurBPP;
	nCurW = GetPrivateProfileInt( "AutoOptionSetting", "ScreenWidth", USABLE_WIDTH_I, "./Profiles/Player.txt" );
	nCurH = GetPrivateProfileInt( "AutoOptionSetting", "ScreenHeight", USABLE_HEIGHT_I, "./Profiles/Player.txt" );
	nCurBPP = GetPrivateProfileInt( "AutoOptionSetting", "ScreenBitdepth", USABLE_BPP_I, "./Profiles/Player.txt" );
	
	if( nCurW != USABLE_WIDTH_I && nCurH != USABLE_HEIGHT_I ) nCurBPP = USABLE_BPP_I;

	RMode rMode;

	rMode.m_Width		= USABLE_WIDTH_I;	//pMode->m_Width;
	rMode.m_Height		= USABLE_HEIGHT_I;	//pMode->m_Height;
	rMode.m_BitDepth	= nCurBPP;			//pMode->m_BitDepth;
	rMode.m_bHWTnL		= true;
	rMode.m_pNext		= NULL;
	
	sprintf(rMode.m_InternalName, "%s", pMode->m_InternalName);
	sprintf(rMode.m_Description, "%s", pMode->m_Description);

	LTRESULT ltResult;
	// Initialize the renderer
	ltResult = g_pLTClient->SetRenderMode(&rMode);

    if( ltResult != LT_OK )
	{
		rMode.m_Width		= USABLE_WIDTH_I;
		rMode.m_Height		= USABLE_HEIGHT_I;
		rMode.m_BitDepth	= (nCurBPP==USABLE_BPP_I) ? 32 : USABLE_BPP_I;
		rMode.m_bHWTnL		= true;
		rMode.m_pNext		= NULL;

		g_pLTClient->CPrint( "[DX_INIT ERROR] Try to create %d * %d * %d",
							 rMode.m_Width, rMode.m_Height, rMode.m_BitDepth );

		ltResult = g_pLTClient->SetRenderMode(&rMode);
		if(ltResult != LT_OK)
		{
			//they want to continue, so create a software version
			rMode.m_Width		= USABLE_WIDTH_I;
			rMode.m_Height		= USABLE_HEIGHT_I;
			rMode.m_BitDepth	= nCurBPP;
			rMode.m_bHWTnL		= false;
			rMode.m_pNext		= NULL;

			g_pLTClient->CPrint( "[DX_INIT ERROR] Try to create %d * %d * %d without HW TnL",
							 rMode.m_Width, rMode.m_Height, rMode.m_BitDepth );

			ltResult = g_pLTClient->SetRenderMode(&rMode);
			if (ltResult != LT_OK)
			{
				rMode.m_Width		= USABLE_WIDTH_I;
				rMode.m_Height		= USABLE_HEIGHT_I;
				rMode.m_BitDepth	= (nCurBPP==USABLE_BPP_I) ? 32 : USABLE_BPP_I;
				rMode.m_bHWTnL		= false;
				rMode.m_pNext		= NULL;

				g_pLTClient->CPrint( "[DX_INIT ERROR] Try to create %d * %d * %d without HW TnL",
							 rMode.m_Width, rMode.m_Height, rMode.m_BitDepth );
				
				ltResult = g_pLTClient->SetRenderMode(&rMode);
				if (ltResult != LT_OK)
				{
				
					g_pLTClient->CPrint( "[DX_INIT ERROR] Exit Game..." );
					
					GetLocalHWInfo();
					if( g_bNoneVideoDevice ||
						0 >= GetGlobalClientMgr()->GetVideoMemorySize() )
					{
						g_pLTClient->ShutdownWithMessage("ºñµð¿ÀÄ«µå µå¶óÀÌ¹ö°¡ ¼³Ä¡µÇ¾î ÀÖÁö ¾Ê½À´Ï´Ù.\r\n\r\n"
														 "µå¶óÀÌ¹ö ¼³Ä¡ ÈÄ ´Ù½Ã ½ÃµµÇØ ÁÖ½Ã±â ¹Ù¶ø´Ï´Ù.\r\n\r\n"
														 "ÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù.");
					}
					else
					{
						g_pLTClient->ShutdownWithMessage("DirectX ÀåÄ¡¸¦ ÃÊ±âÈ­ ÇÒ ¼ö ¾ø½À´Ï´Ù.\r\n\r\n"
														 "ÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù.");

						if( IDYES == MessageBox(g_hMainWnd,
												"DirectX ÀåÄ¡¸¦ ÃÊ±âÈ­ ÇÒ ¼ö ¾ø½À´Ï´Ù.\r\n\r\n"
												"Á¤È®ÇÑ ¿¡·¯ ºÐ¼®À» À§ÇÏ¿© ¿À·ù·Î±×¸¦ º¸³»½Ã°Ú½À´Ï±î?",
												"SuddenAttack",
												MB_YESNO ) )
						{
							g_pLTClient->SetDumpDir( "DX_Init_Err" );
							g_pLTClient->SendAddedFiles();
						}
					}					
					return LT_ERROR;
				}
			}
			WriteConsoleInt("HWTnLDisabled",1);
		}
	}

	SetFocusGame();

	char	szVersionStr[32] = {0,};
	CWinUtil::WinGetPrivateProfileString( "GAME_VERSION", "CURR", "ERROR", szVersionStr, 32, ".\\gvi.dat" );
	g_pLTClient->CPrint( "[START GAME] ----------------------------------------------------------" );
	g_pLTClient->CPrint( "Game Version\t\t: %s", szVersionStr );
	GetLocalHWInfo();
	g_pLTClient->CPrint( "-----------------------------------------------------------------------" );
	
	LogoInitialize(pMode->m_Width, pMode->m_Height);
	
	//----------------------------------------------------------- Novice (minjin)
	//	GameText, GameTextFileMngr									[03/09/2005]

	NF_Buff nbContextBuf;

	NF_FILE_MNGR()->Create( GAME_MESSAGE_FILE_NAME, FALSE );
	if ( NF_FILE_SIGNATURE_SIZE > NF_FILE_MNGR()->Load( TRUE ) )
	{
		// ÆÄÀÏÀÌ ¸ÖÂÄÇÏÁö ¾Ê´Ù..
		NF_FILE_MNGR()->Destroy();
		g_pLTClient->CPrint( "\t[ERROR] TextMgr Init" );
		return LT_ERROR;
	}
	
	// ÆÄÀÏÀÇ ³»¿ëÀ», GameText ÂÊÀ¸·Î Àü´Þ.
	NF_FILE_MNGR()->GetBuffer( &nbContextBuf );
	NF_GAME_TEXT()->SetContext( (LPVOID)nbContextBuf.pBuf, nbContextBuf.dwBufSize );

	// Final Game Text ¿¡ ¼ÂÆÃÇÏ°í,
	NF_FINAL_GAME_TEXT()->SetHeaderWithParsing(	 NF_GAME_TEXT()->GetContextHeader() );
	NF_FINAL_GAME_TEXT()->SetContextWithParsing( NF_GAME_TEXT()->GetContextHeader(),
												 NF_GAME_TEXT()->GetContext() );

	// ³ª¸ÓÁö´Â ³¯¸°´Ù.
	NF_GAME_TEXT()->Destroy();
	NF_FILE_MNGR()->Destroy();

	//---------------------------------------------------------------------------

	IDirect3DDevice8* pDevice = g_pLTClient->GetD3DDevice();

	//[MURSUM]===================================================================
	// ¸Å´ÏÁ® Å¬·¡½º ÃÊ±âÈ­
	g_pSAD3DTextMgr	= &m_SAD3DTextMgr;

	if( g_pSAD3DTextMgr )
	{
		g_pSAD3DTextMgr->SetDevice( pDevice );
		g_pSAD3DTextMgr->Init();
	}

	if( g_pImeCtrl ) g_pImeCtrl->SetMainHandle( g_hMainWnd );
	//===========================================================================

	m_pLogText	= new CSAD3DText();
	m_pLogText->CreateD3DText( 512, 16, true );
	m_pLogText->SetPosition( 550, USABLE_HEIGHT_I-40 );
	m_pLogText->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pLogText->SetTransparent(true);
	
	m_p18Notice	= new CSAD3DText();
	m_p18Notice->CreateD3DText( 512, 16, false );
	m_p18Notice->SetPosition( 453, USABLE_HEIGHT_I-19 );
	m_p18Notice->SetTextColor( RGB(255,255,255) );
	m_p18Notice->SetTransparent(true);
	m_p18Notice->DrawD3DText( "", 0 );
	
	HCONSOLEVAR hVarAge		= LTNULL;
	hVarAge		= g_pLTClient->GetConsoleVar("UserAge");
	if( hVarAge )
	{
		const char* lpszAge = g_pLTClient->GetVarValueString( hVarAge );
		if( lpszAge )
		{
			if( atoi(lpszAge) < 18 )
			{
				m_hAgeMark	= g_pInterfaceResMgr->GetTexture("sa_interface\\menu\\textures\\common\\limit15.dtx");				
			}
			else
			{
				m_hAgeMark	= g_pInterfaceResMgr->GetTexture("sa_interface\\menu\\textures\\common\\limit18.dtx");
				m_p18Notice->DrawD3DText( NF_GetMsg(SA_TEXT_LIMIT_18), strlen(NF_GetMsg(SA_TEXT_LIMIT_18)) );
			}
		}
	}

	ScreenRender(TEXT(""));

	ScreenRender( "Initializing Client FX..." );
	
	// Init the ClientFX Database
	if(!CClientFXDB::GetSingleton().Init(g_pLTClient))
	{
		g_pLTClient->CPrint( "\t[ERROR] ClientFXDB Init" );
		g_pLTClient->ShutdownWithMessage( "Could not init ClientFXDB!" );
		return LT_ERROR;
	}

	// Init the ClientFX mgr... (this must become before most other classes)
	if( !m_ClientFXMgr.Init( g_pLTClient ) )
	{
		// Make sure ClientFX.fxd is built and in the game dir
		g_pLTClient->CPrint( "\t[ERROR] ClientFXMgr Init" );
		g_pLTClient->ShutdownWithMessage( "Could not init ClientFXMgr!" );
		return LT_ERROR;
	}

	ScreenRender( "Initializing Damage FX..." );
	
	// Init the DamageFX mgr...
	if( !m_DamageFXMgr.Init() )
	{
		g_pLTClient->CPrint( "\t[ERROR] DamageFXMgr Init" );
		g_pLTClient->ShutdownWithMessage( "Could not init DamageFXMgr!" );
		return false;
	}

	ScreenRender( "Initializing Mission manager..." );

	// Mission stuff...
	m_pMissionMgr = debug_new(CMissionMgr);
	if (!m_pMissionMgr || !m_pMissionMgr->Init())
	{
		// Don't call ShutdownWithMessage since MissionMgr will have called
		// that, so calling it here will overwrite the message...
		g_pLTClient->CPrint( "\t[ERROR] MissionMgr Init" );
 		return LT_ERROR;
	}

	ScreenRender( "Initializing Interface manager..." );
	
	// Interface stuff...
	if (!GetInterfaceMgr( ) || !GetInterfaceMgr( )->Init())
	{
		// Don't call ShutdownWithMessage since InterfaceMgr will have called
		// that, so calling it here will overwrite the message...
		g_pLTClient->CPrint( "\t[ERROR] InterfaceMgr Init" );
 		return LT_ERROR;
	}

	ScreenRender( "Initializing Player manager..." );

	// Player stuff...
	if (!GetPlayerMgr() || !GetPlayerMgr()->Init())
	{
		// Don't call ShutdownWithMessage since PlayerMgr will have called
		// that, so calling it here will overwrite the message...
		g_pLTClient->CPrint( "\t[ERROR] PlayerMgr Init" );
		return LT_ERROR;
	}

	//We need to make sure to setup the camera for the FX Mgr
	m_ClientFXMgr.SetCamera(GetPlayerMgr()->GetCamera());

	// Tracked Node stuff...
	if( m_pClientTrackedNodeMgr == 0 )
	{
		m_pClientTrackedNodeMgr = debug_new( CClientTrackedNodeMgr );
		ASSERT( 0 != m_pClientTrackedNodeMgr );
	}

	// Setup the music stuff...(before we setup the interface!)

	//////////////////////////////////////////////////////////////////////////
	// NOVICE : ¾È¾´´Ù. ÃÊ±âÈ­ ÇÏÁö ¾Ê´Â´Ù.			
	/*
	uint32 dwAdvancedOptions = GetInterfaceMgr( )->GetAdvancedOptions();
	
	if (!m_Music.IsInitialized() && (dwAdvancedOptions & AO_MUSIC))
	{
        m_Music.Init(g_pLTClient);
	}																	
	*/
	//////////////////////////////////////////////////////////////////////////


	ScreenRender( "Connecting Server..." );

	g_Network_Init( g_hMainWnd, g_pLTClient );
	//[MURSUM]================================================================
	// ScreenÀ» »ç¿ëÇÏ´Â Network Module °¡ Screenº¸´Ù ¸ÕÀú »ý¼ºµÊ
	// Screen »ý¼º ÀÌÈÄ¿¡ »ý¼ºÇÏµµ·Ï À§Ä¡ º¯°æ
	HCONSOLEVAR	hVarIP		= LTNULL;
	HCONSOLEVAR	hVarPort	= LTNULL;
	HCONSOLEVAR	hService	= LTNULL;	
	
	const char*	lpszNO		= LTNULL;
	const char*	lpszIP		= LTNULL;
	
	T_LOCAL_INFO		tLocalInfo;
	memset( &tLocalInfo, 0, sizeof(T_LOCAL_INFO) );
	
	hVarIP		= g_pLTClient->GetConsoleVar("ServerIP");
	hVarPort	= g_pLTClient->GetConsoleVar("ServerPort");
	hService	= g_pLTClient->GetConsoleVar("Service");
	
	if( !hVarIP )
	{
		g_pLTClient->CPrint( "\t[ERROR] hVarIP" );
		return LT_ERROR;
	}

	lpszIP		= g_pLTClient->GetVarValueString( hVarIP );
	if( lpszIP )
	{
		strcpy( tLocalInfo.szServerIP, lpszIP );
	}
	else
	{
		g_pLTClient->CPrint( "[ERROR] ServerIP == LTNULL  hVarIP( 0x%08x )", hVarIP );
	}

	tLocalInfo.nServerPort	= (WORD)g_pLTClient->GetVarValueFloat( hVarPort );

	if( tLocalInfo.nServerPort <= 0 )
	{
		g_pLTClient->CPrint( "[ERROR] ServerPort == 0  hVarPort( 0x%08x )", hVarPort );
	}

	if( hService )
	{		
		const char* lpszService	= g_pLTClient->GetVarValueString( hService );
		if( lpszService )	//³Ý¸¶ºí
		{
			if( stricmp( lpszService, "cj_internet" ) == 0 )
			{
				tLocalInfo.nProvider	= PROVIDER_CJ;
			}
		}
	}
	else
	{
		HCONSOLEVAR	hVarNO;
		hVarNO		= g_pLTClient->GetConsoleVar("UserNO");			
		lpszNO		= g_pLTClient->GetVarValueString( hVarNO );		
		if( lpszNO ) strcpy( tLocalInfo.szUserNo, lpszNO );
		
		tLocalInfo.nProvider	= PROVIDER_GAMEHI;			
	}	
	//=====================================================================================

	g_Network.SetLocalInfo( &tLocalInfo );

	// Initialize the global physics states...
	g_normalPhysicsState.m_vGravityAccel.Init(0.0f, -1000.0f, 0.0f);
	g_normalPhysicsState.m_fVelocityDampen = 0.5f;
	g_waterPhysicsState.m_vGravityAccel.Init(0.0f, -500.0f, 0.0f);
	g_waterPhysicsState.m_fVelocityDampen = 0.25f;


	ScreenRender( "Initializing SFX manager..." );

	//m_pClientMultiplayerMgr = debug_new( CClientMultiplayerMgr );

	m_pClientMultiplayerMgr = debug_new( CClientMultiplayerMgr );
	
	// Init the special fx mgr...
	if (!m_sfxMgr.Init(g_pLTClient))
	{
		g_pLTClient->CPrint( "\t[ERROR] sfxMgr Init" );
		g_pLTClient->ShutdownWithMessage("Could not initialize SFXMgr!");
		return LT_ERROR;
	}

	// Get the name of the mod we want to play.  If no mod specified then we consider that the 'Retail' mod...
	// Set this when the game initializes and use g_pClientMultiplayerMgr->GetModName() in case the console var changes.

	HCONSOLEVAR	hModVar = g_pLTClient->GetConsoleVar( "Mod" );
	if( hModVar )
	{
        g_pClientMultiplayerMgr->SetModName( g_pLTClient->GetVarValueString( hModVar) );
	}
	else
	{
		g_pClientMultiplayerMgr->SetModName( LoadTempString( IDS_RETAIL ) );
	}	


#ifdef _TO2DEMO
	WriteConsoleInt( "ConsoleEnable", 0 );
#endif // _TO2DEMO

#ifndef _DEBUG
	WriteConsoleInt( "numconsolelines", 0 );
#endif


	// Intialize the scmd handler.
	static ScmdConsoleDriver_CShell scmdConsoleDriver_CShell;
	if( !ScmdConsole::Instance( ).Init( scmdConsoleDriver_CShell ))
	{
		g_pLTClient->CPrint( "\t[ERROR] ScmdConsole Init" );
		return LT_ERROR;
	}

	// ¼º°øÀÌ¸é ÇÁ·ÎÆÄÀÏ¿¡ ¾ÆÀÌµð¸¦ ¾÷µ¥ÀÌÆ® ÇÑ´Ù.
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();
	if (pProfile)
	{
		HCONSOLEVAR hVar;
		if ( hVar = g_pLTClient->GetConsoleVar("UserID") )
		{
			const char* pUserID = g_pLTClient->GetVarValueString(hVar);
			pProfile->m_sPlayerName = std::string(pUserID);
		}
	}

	g_Global_Data.GlobalInit();
	g_pMissionButeMgr->Init( MISSION_DEFAULT_FILE );

#ifdef _STANDALONE
	GetInterfaceMgr( )->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
#else

	PreLoadFXRes();
	
	m_bDrawLogo	= true;

	if( LTFALSE == g_Network_Connect() )
	{
		//"¿¬°á ½ÇÆÐ!" -> ¼­¹ö¿¡ Á¢¼ÓÇÒ ¼ö ¾ø½À´Ï´Ù. Àá½Ã ÈÄ ´Ù½Ã..
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ERROR_ON_CONNECT_SERVER,
										  LTMB_OK, CGameClientShell::OnConnectFail, LTNULL );
	}
#endif

	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnEngineTerm()
//
//	PURPOSE:	Called before the engine terminates itself
//				Handle object destruction here
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnEngineTerm()
{
    UnhookWindow();

	if( m_pLogText )
	{
		delete m_pLogText;
		m_pLogText	= LTNULL;
	}

	if( m_p18Notice )
	{
		delete m_p18Notice;
		m_p18Notice = LTNULL;
	}

	GetInterfaceMgr( )->Term();

	if( m_pMissionMgr )
	{
		debug_delete(m_pMissionMgr);
		m_pMissionMgr = NULL;
	}

	//Make sure that the FX Mgr isn't still holding on to our camera
	m_ClientFXMgr.SetCamera(NULL);

	GetPlayerMgr()->Term();

	m_Music.Term();
	m_LightScaleMgr.Term();

	if( m_pClientMultiplayerMgr )
	{
		debug_delete( m_pClientMultiplayerMgr );
		m_pClientMultiplayerMgr = NULL;
	}

	// Destroy Game Text Instance.. by Novice. 2005. 03. 15.
	NF_FINAL_GAME_TEXT()->Destroy();

	g_pSAD3DTextMgr->Term();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnEvent()
//
//	PURPOSE:	Called for asynchronous errors that cause the server
//				to shut down
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnEvent(uint32 dwEventID, uint32 dwParam)
{
	switch(dwEventID)
	{
		// Client disconnected from server.  dwParam will
		// be a error flag found in de_codes.h.

		case LTEVENT_DISCONNECT :
		{
			//SetWorldNotLoaded();
			//m_bServerPaused = false;
			//m_eSwitchingWorldsState = eSwitchingWorldsStateNone;
			g_pLTClient->CPrint( "[ON EVENT] LTEVENT_DISCONNECT -----------------------------------------" );
		} break;

        case LTEVENT_LOSTFOCUS:
		{
			g_pLTClient->CPrint( "[ON EVENT] LTEVENT_LOSTFOCUS ------------------------------------------" );
		}
		break;

		case LTEVENT_GAINEDFOCUS:
		{
			g_pLTClient->CPrint( "[ON EVENT] LTEVENT_GAINEDFOCUS ---------------------------------------" );
		}
		break;

        case LTEVENT_RENDERTERM:
		{
			m_bMainWindowFocus = FALSE;
			m_bRendererInit = false;

			// Let the ClientFx mgr know the renderer is shutting down
			m_ClientFXMgr.OnRendererShutdown();
			g_pLTClient->CPrint( "[ON EVENT] LTEVENT_RENDERTERM ---------------------------------------" );
		}
		break;

		case LTEVENT_RENDERINIT:
		{
			m_bMainWindowFocus = TRUE;
			m_bRendererInit = true;

			// Clip the cursor if we're NOT in a window...

            HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("Windowed");
			BOOL bClip = TRUE;
			if (hVar)
			{
                float fVal = g_pLTClient->GetVarValueFloat(hVar);
				if (fVal == 1.0f)
				{
					bClip = FALSE;
				}
			}

			if (bClip)
			{
				if (!g_prcClip)
				{
					g_prcClip = debug_new(RECT);
				}

				
				if (!g_hMainWnd)
				{
					HookWindow();
				}

				GetWindowRect(g_hMainWnd, g_prcClip);
				ClipCursor(g_prcClip);
			}

			if (g_pCursorMgr)
				g_pCursorMgr->Init();

			//setup the glow table
			char pszRS[2][_MAX_PATH];
			g_pClientButeMgr->GetDefaultGlowRS(pszRS[0], _MAX_PATH);
			g_pLTClient->SetGlowDefaultRenderStyle(pszRS[0]);

			g_pClientButeMgr->GetNoGlowRS(pszRS[0], _MAX_PATH);
			g_pLTClient->SetNoGlowRenderStyle(pszRS[0]);

			for(uint32 nCurrMap = 0; nCurrMap < g_pClientButeMgr->GetNumGlowMappings(); nCurrMap++)
			{
				g_pClientButeMgr->GetGlowMappingRS(nCurrMap, pszRS[0], _MAX_PATH, pszRS[1], _MAX_PATH);
				g_pLTClient->AddGlowRenderStyleMapping(pszRS[0], pszRS[1]);
			}

			if( g_pPlayerStats && g_pInterfaceMgr && GS_PLAYING == g_pInterfaceMgr->GetGameState() )
			{
				g_pPlayerStats->RestartWpnChooser();
			}
			g_pLTClient->CPrint( "[ON EVENT] LTEVENT_RENDERINIT ---------------------------------------" );
		}
		break;
	}

	if (GetInterfaceMgr( ))
		GetInterfaceMgr( )->OnEvent(dwEventID, dwParam);
	if( g_pClientMultiplayerMgr )
		g_pClientMultiplayerMgr->OnEvent(dwEventID, dwParam);
}


LTRESULT CGameClientShell::OnObjectMove(HOBJECT hObj, bool bTeleport, LTVector *pPos)
{
	return g_pPlayerMgr->GetMoveMgr()->OnObjectMove(hObj, bTeleport, pPos);
}


LTRESULT CGameClientShell::OnObjectRotate(HOBJECT hObj, bool bTeleport, LTRotation *pNewRot)
{
	//[MURSUM] ÇÊ¿ä¾ø´Â ¿¬»ê »èÁ¦!
	//m_sfxMgr.OnObjectRotate( hObj, bTeleport, pNewRot );
	return g_pPlayerMgr->GetMoveMgr()->OnObjectRotate(hObj, bTeleport, pNewRot);
}

LTRESULT CGameClientShell::OnObjectPitch(HOBJECT hObj, float fPitch)
{
	return m_sfxMgr.OnObjectPitch( hObj, fPitch );
}

LTRESULT CGameClientShell::OnObjectAnimSwitch( HLOCALOBJ hObj, uint32 nAnimIndex, bool bLooping )
{
	return LT_OK;
}

LTRESULT CGameClientShell::OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, float forceMag)
{
	m_sfxMgr.OnTouchNotify(hMain, pInfo, forceMag);
	return LT_OK;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnEnterWorld()
//
//	PURPOSE:	Handle entering world
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnEnterWorld()
{
	// Reset our speed hack.
	g_nStartTicks = 0;
	
	// In world now.
	m_bInWorld = true;

	// Make sure we enable all object render groups so we don't have any weird state
	// left over from previous levels
	g_pLTClient->SetAllObjectRenderGroupEnabled();

	((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->ResumeSounds();

	// Set the stair height gotten from the server...
	
	g_pPhysicsLT->SetStairHeight( DEFAULT_STAIRSTEP_HEIGHT );

	m_bFirstUpdate = true;

	m_LightScaleMgr.Init();
	GetInterfaceMgr( )->AddToClearScreenCount();

	if( !g_GameDoc.IsMandateHost() && !g_GameDoc.IsRejoinHost() )
	{
		g_pLTClient->ClearInput();
	}

	GetInterfaceMgr( )->OnEnterWorld( (m_bRestoringGame||g_GameDoc.IsMandateHost()||g_GameDoc.IsRejoinHost()) );
	GetPlayerMgr()->OnEnterWorld();

    m_bRestoringGame = false;

	m_vLastReverbPos.Init();

	//MirrorSConVar( "RespawnWaitTime", "RespawnWaitTime" );
	//MirrorSConVar( "RespawnMultiWaitTime", "RespawnMultiWaitTime" );
	//MirrorSConVar( "RespawnDoomsdayWaitTime", "RespawnDoomsdayWaitTime" );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnExitWorld()
//
//	PURPOSE:	Handle exiting the world
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnExitWorld()
{
    ((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->PauseSounds();

	// Not in world any more.
    m_bInWorld = false;

	m_DamageFXMgr.Clear();					// Remove all the sfx
	m_sfxMgr.RemoveAll();					// Remove all the sfx
	m_ClientFXMgr.ShutdownAllFX();			// Stop all the client FX

	GetInterfaceMgr( )->OnExitWorld();

	if( GetPlayerMgr() )
		GetPlayerMgr()->OnExitWorld();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::PreUpdate()
//
//	PURPOSE:	Handle client pre-updates
//
// ----------------------------------------------------------------------- //

void CGameClientShell::PreUpdate()
{
	GetPlayerMgr()->PreUpdate();
	GetInterfaceMgr( )->PreUpdate();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::Update()
//
//	PURPOSE:	Handle client updates
//
// ----------------------------------------------------------------------- //

void CGameClientShell::Update()
{
	// Allow multiplayermgr to update.
#ifndef _STANDALONE
	g_Network_Update( g_pLTClient->GetTime() );
#endif

	g_pClientMultiplayerMgr->Update( );

	// Set up the time for this frame...
    m_fFrameTime = g_pLTClient->GetFrameTime();
	if (m_fFrameTime > MAX_FRAME_DELTA)
	{
		m_fFrameTime = MAX_FRAME_DELTA;
	}

	// Update tint if applicable (always do this to make sure tinting
	// gets finished)...

	UpdateScreenFlash();
	m_ScreenTintMgr.Update();



	// Update client-side physics structs...

	if (IsServerPaused())
	{
		SetPhysicsStateTimeStep(&g_normalPhysicsState, 0.0f);
		SetPhysicsStateTimeStep(&g_waterPhysicsState, 0.0f);
	}
	else
	{
		SetPhysicsStateTimeStep(&g_normalPhysicsState, m_fFrameTime);
		SetPhysicsStateTimeStep(&g_waterPhysicsState, m_fFrameTime);
	}

#ifdef _TRON_E3_DEMO
	// Check to see if they've left the game idle for too long
	// If so, take them to the demo screen state
	s_fDemoTime += g_pLTClient->GetFrameTime();
	if(s_fDemoTime > TRON_E3_DEMO_IDLE_TIME)
	{
		GetInterfaceMgr()->ShowDemoScreens(LTFALSE);
	}
#endif

	// Update the interface (don't do anything if the interface mgr
	// handles the update...)

	if (GetInterfaceMgr()->Update())
	{
		return;
	}
	else if( m_bDrawLogo )
	{
		ScreenRender( "Connecting to server..." );
		return;
	}

	GetPlayerMgr()->Update();

	// At this point we only want to proceed if the player is in the world...
	if (GetPlayerMgr()->IsPlayerInWorld() && g_pInterfaceMgr->GetGameState() != GS_UNDEFINED)
	{
		UpdatePlaying();
	}
	else if( GS_MANDATEHOST == g_pInterfaceMgr->GetGameState() ||
			 GS_REJOINHOST == g_pInterfaceMgr->GetGameState() )
	{
		UpdatePlaying();
	}
	else
	{
		// we should not be here, since we think we should rendering the world, but we are not
		bool bBlackScreen = true;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateGoreSettings()
//
//	PURPOSE:	Looks at the gore settings and makes sure all objects reflect the setting
//
// ----------------------------------------------------------------------- //
void CGameClientShell::UpdateGoreSettings()
{
	//default to no gore
	bool bGore = false;

	if(g_pProfileMgr && g_pProfileMgr->GetCurrentProfile() && g_pVersionMgr)
	{
 	/*	if( g_GameDoc.IsLowViolence() )
 		{
 			//force the profile to be low violence here (to prevent people from hacking their profile to get around the limits)
 			g_pProfileMgr->GetCurrentProfile()->m_bGore = false;
 		}
 		else*/
 		{
 			bGore = (bool)!!(g_pProfileMgr->GetCurrentProfile()->m_bGore);
 		}
   	}
	//setup the FX managers to handle the effects
	GetClientFXMgr()->SetGoreEnabled(bGore);
	GetInterfaceMgr()->GetInterfaceFXMgr().SetGoreEnabled(bGore);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdatePlaying()
//
//	PURPOSE:	Handle updating playing (normal) game state
//
// ----------------------------------------------------------------------- //

uint32 g_nSamplePeriod = 250;
uint32 g_nTolerance = 400;

void CGameClientShell::UpdatePlaying()
{
	// Handle first update...

	if (m_bFirstUpdate)
	{
		FirstUpdate();
	}

	//[MURSUM]
	//½ºÇÙÀº ¼­¹ö¼­ Ã¼Å©ÇØ¾ßÁö..
	/*
	if(IsMultiplayerGame() && g_pPlayerMgr->IsPlayerInWorld())
	{
		// Only check the speed hack for remote clients.
		bool bIsLocalClient = false;
		g_pLTClient->IsLocalToServer(&bIsLocalClient);
		if( !bIsLocalClient )
		{
			uint32 nEndClientTime = timeGetTime( );
			uint32 nEndTicks = GetTickCount();
			_timeb nEndTimeB;
			_ftime(&nEndTimeB);

			// Get the client time since the last check.  Account for wrap.
			uint32 nDeltaClientTime = ( nEndClientTime > g_nStartClientTime ) ? ( nEndClientTime - g_nStartClientTime ) : 
				( nEndClientTime + ~g_nStartClientTime );

			// Check at a periodic rate.
			if( nDeltaClientTime > g_nSamplePeriod )
			{
				// Make sure we've gone through this at least once to make sure all the counters
				// are properly initialized.
				if( g_nStartTicks > 0 )
				{
					// Get the time between _ftime's.
					uint32 nDeltaTimeB = (nEndTimeB.time - g_StartTimeB.time)*1000 + (nEndTimeB.millitm - g_StartTimeB.millitm);

					// Get the time between GetTickCount's.  Account for wrapping.
					uint32 nDeltaTicks = ( nEndTicks > g_nStartTicks ) ? ( nEndTicks - g_nStartTicks ) : 
						( nEndTicks + ~g_nStartTicks );

					// Make sure all the counters match up.
					if((( uint32 )abs( nDeltaTimeB - nDeltaClientTime ) > g_nTolerance ) || 
						(( uint32 )abs( nDeltaTicks - nDeltaClientTime ) > g_nTolerance ) ||
						(( uint32 )abs( nDeltaTimeB - nDeltaTicks ) > g_nTolerance ))
					{
						g_pLTClient->CPrint( "Speedhack kick" );
						g_pLTClient->CPrint( "nDeltaTimeB %d", nDeltaTimeB );
						g_pLTClient->CPrint( "nDeltaTicks %d", nDeltaTicks );
						g_pLTClient->CPrint( "nDeltaClientTime %d", nDeltaClientTime );
					
						// You hAxOr!

						// Disconnect from the server.
						if(g_pLTClient->IsConnected())
						{
							g_pLTClient->Disconnect();
						}
					}
				}

				// Reset the timers, the initial _ftime call must be before
				// the performance counter call!
				_ftime( &g_StartTimeB );
				g_nStartTicks = GetTickCount();
				g_nStartClientTime = timeGetTime( );
			}
		}
	}
	*/

	g_pPlayerMgr->UpdatePlaying();


	// Update any debugging information...

#ifdef _DEBUG
	UpdateDebugInfo();
#endif

	// Update cheats...(if a cheat is in effect, just return)...
#ifndef _FINAL
	if( UpdateCheats( ))
		return;
#endif

	// Update any overlays...

	GetInterfaceMgr( )->UpdateOverlays();


	// Render the camera...

	RenderCamera();
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::PostUpdate()
//
//	PURPOSE:	Handle post updates - after the scene is rendered
//
// ----------------------------------------------------------------------- //

void CGameClientShell::PostUpdate()
{
	GetPlayerMgr()->PostUpdate();

	// Conditions where we don't want to flip...

	if (GetPlayerMgr()->GetPlayerState() == PS_UNKNOWN && IsWorldLoaded() && GetInterfaceMgr( )->GetGameState() != GS_SCREEN)
	{
		return;
	}


	GetInterfaceMgr( )->PostUpdate();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::GetDynamicSoundFilter()
//
//	PURPOSE:	Get the dynamic sound filter
//
// ----------------------------------------------------------------------- //

SOUNDFILTER* CGameClientShell::GetDynamicSoundFilter()
{
	// See if we have a current container filter override...

	SOUNDFILTER* pFilter = g_pSoundFilterMgr->GetFilter(g_pPlayerMgr->GetSoundFilter());
	if (!pFilter) return NULL;


	if (!g_pSoundFilterMgr->IsDynamic(pFilter))
	{
		// Found it...
		return pFilter;
	}
	else  // Calculate the filter based on the listener...
	{
		// For now just return global default (from WorldProperties)

		return g_pSoundFilterMgr->GetFilter(g_pPlayerMgr->GetGlobalSoundFilter());
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateCheats()
//
//	PURPOSE:	Update cheats...
//
// ----------------------------------------------------------------------- //

bool CGameClientShell::UpdateCheats()
{
	if (m_bAdjustLightScale)
	{
		AdjustLightScale();
	}

	if (m_bAdjustLightAdd)
	{
		AdjustLightAdd();
	}

	if (m_bAdjustFOV)
	{
		AdjustFOV();
	}

	if (m_bAdjust1stPersonCamera)
	{
		Adjust1stPersonCamera();
	}

	// If in spectator mode, just do the camera stuff...

	if (g_pPlayerMgr->IsSpectatorMode() || g_pPlayerMgr->IsPlayerDead())
	{
		g_pPlayerMgr->UpdateCamera();
		RenderCamera();
        return true;
	}

	// Update weapon position if appropriated...

	if (m_bTweakingWeapon)
	{
		UpdateWeaponPosition();
		g_pPlayerMgr->UpdateCamera();
		RenderCamera();
        return true;
	}
	else if (m_bTweakingWeaponMuzzle)
	{
		UpdateWeaponMuzzlePosition();
		g_pPlayerMgr->UpdateCamera();
		RenderCamera();
        return true;
	}
	else if (m_bTweakingWeaponBreachOffset)
	{
		UpdateWeaponBreachOffset();
		g_pPlayerMgr->UpdateCamera();
		RenderCamera();
        return true;
	}

    return false;
}




// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateScreenFlash
//
//	PURPOSE:	Update the screen flash
//
// --------------------------------------------------------------------------- //

void CGameClientShell::UpdateScreenFlash()
{
	// [KLS 2/27/02] - Removed ALL screen flashing...
	return;

	if (!m_bFlashScreen) return;

    LTVector vLightAdd;
	VEC_SET(vLightAdd, 0.0f, 0.0f, 0.0f);

    float fTime  = g_pLTClient->GetTime();
	if ((m_fFlashRampUp > 0.0f) && (fTime < m_fFlashStart + m_fFlashRampUp))
	{
        float fDelta = (fTime - m_fFlashStart);
		vLightAdd.x = fDelta * (m_vFlashColor.x) / m_fFlashRampUp;
		vLightAdd.y = fDelta * (m_vFlashColor.y) / m_fFlashRampUp;
		vLightAdd.z = fDelta * (m_vFlashColor.z) / m_fFlashRampUp;
	}
	else if (fTime < m_fFlashStart + m_fFlashRampUp + m_fFlashTime)
	{
		VEC_COPY(vLightAdd, m_vFlashColor);
	}
	else if ((m_fFlashRampDown > 0.0f) && (fTime < m_fFlashStart + m_fFlashRampUp + m_fFlashTime + m_fFlashRampDown))
	{
        float fDelta = (fTime - (m_fFlashStart + m_fFlashRampUp + m_fFlashTime));

		vLightAdd.x = m_vFlashColor.x - (fDelta * (m_vFlashColor.x) / m_fFlashRampUp);
		vLightAdd.y = m_vFlashColor.y - (fDelta * (m_vFlashColor.y) / m_fFlashRampUp);
		vLightAdd.z = m_vFlashColor.z - (fDelta * (m_vFlashColor.z) / m_fFlashRampUp);
	}
	else
	{
        m_bFlashScreen = false;
	}

	// Make sure values are in range...

	vLightAdd.x = (vLightAdd.x < 0.0f ? 0.0f : (vLightAdd.x > 1.0f ? 1.0f : vLightAdd.x));
	vLightAdd.y = (vLightAdd.y < 0.0f ? 0.0f : (vLightAdd.y > 1.0f ? 1.0f : vLightAdd.y));
	vLightAdd.z = (vLightAdd.z < 0.0f ? 0.0f : (vLightAdd.z > 1.0f ? 1.0f : vLightAdd.z));

	m_ScreenTintMgr.Set(TINT_SCREEN_FLASH,&vLightAdd);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::ClearScreenTint
//
//	PURPOSE:	Clear any screen tinting
//
// --------------------------------------------------------------------------- //

void CGameClientShell::ClearScreenTint()
{
	m_ScreenTintMgr.ClearAll();
	if (m_bFlashScreen)
	{
        m_bFlashScreen = false;
	}

	m_LightScaleMgr.Term();
	m_LightScaleMgr.Init();
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnCommandOn()
//
//	PURPOSE:	Handle client commands
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnCommandOn(int command)
{

#ifdef _TRON_E3_DEMO
	// Reset the demo timer
	s_fDemoTime = 0.0f;
#endif

	// Make sure we're in the world...

	if (!GetPlayerMgr()->IsPlayerInWorld()) return;


	// Let the interface handle the command first...

	if (IsMultiplayerGame() || !GetPlayerMgr()->IsPlayerDead())
	{
		if (GetInterfaceMgr( )->OnCommandOn(command))
		{
			return;
		}
	}


	if (GetPlayerMgr()->OnCommandOn(command))
	{
		return;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnCommandOff()
//
//	PURPOSE:	Handle command off notification
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnCommandOff(int command)
{
#ifdef _TRON_E3_DEMO
	// Reset the demo timer
	s_fDemoTime = 0.0f;
#endif

	// Let the interface handle the command first...
	if (GetInterfaceMgr( )->OnCommandOff(command))
	{
		return;
	}
	if (GetPlayerMgr()->OnCommandOff(command))
	{
		return;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnKeyDown(int key, int rep)
//
//	PURPOSE:	Handle key down notification
//				Try to avoid using OnKeyDown and OnKeyUp as they
//				are not portable functions
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnKeyDown(int key, int rep)
{
	// [RP] - 8/03/02: WinXP likes to add a second OnKeyDown() message with an invalid key of 255
	//		  for certain single key presses.  Just ignore invalid key codes 255 and larger.

	if( key >= 0xFF )
		return;

	// The engine handles the VK_F8 key for screenshots.
	if( key == VK_F8 )
	{
		return;
	}

/**/
#ifndef _FINAL
	if( g_pInterfaceMgr->GetGameState() == GS_PLAYING && key == VK_F4 )
	{
		g_Network.TestMandate();
	}
#endif


	//[MURSUM] TEST
	if( g_pInterfaceMgr->GetGameState() == GS_PLAYING )
	{
#ifndef _FINAL
		if( key == VK_F12 )
		{
			if( g_pHUDGameState->IsVisible() )
			{
				g_pHUDGameState->Show(false);
			}
			else
			{
				g_pHUDGameState->Show(true);
			}
		}
		else if( g_pHUDGameState->IsVisible() && key >= VK_F2 && key <= VK_F12 ) //[yaiin] : VK_F1À» VK_F2ºÎÅÍ·Î
		{
			g_pHUDGameState->Set( key );
			return;
		}

		if( key == VK_F11 )
		{
			g_pLTClient->MakeCubicEnvMap(GetPlayerMgr()->GetCamera(), (uint32)(g_vtMakeCubicEnvMapSize.GetFloat(256.0f) + 0.5f), g_vtMakeCubicEnvMapName.GetStr("CubicEnvMap"));
			return;
		}
#endif
		if( key == VK_F5 )
		{
			if( LTTRUE == g_pNickList->IsVisible() )
			{
				g_pNickList->SetVisible(LTFALSE);
				WriteConsoleInt( "NameRadar", 0 );
			}
			else
			{
				g_pNickList->SetVisible(LTTRUE);
				WriteConsoleInt( "NameRadar", 1 );
			}
		}
	}

	if (key == VK_TOGGLE_SCREENSHOTMODE)
	{
#ifndef _FINAL
		g_bScreenShotMode = !g_bScreenShotMode;

		bool bDrawHUD = !g_bScreenShotMode; // && GetPlayerMgr()->IsSpectatorMode());
		if (bDrawHUD)
		{
			GetInterfaceMgr( )->SetHUDRenderLevel(kHUDRenderFull);
		}
		else
		{
			GetInterfaceMgr( )->SetHUDRenderLevel(kHUDRenderNone);
		}

        g_pLTClient->CPrint("Screen shot mode: %s", g_bScreenShotMode ? "ON" : "OFF");
#endif
		return;
	}

	if (key == VK_TOGGLE_SPECTATOR_MODE)
	{		
#ifndef _FINAL
		char *pCheat = "mpclip";
		g_pCheatMgr->Check(CParsedMsg( 1, &pCheat ));
#endif
		return;
	}

	//[yaiin] : ÁÖ¼®Ã³¸® - µµ¿ò¸»µµ F1ÀÌ±â ¶§¹®
	/*
	if (key == VK_TOGGLE_GHOST_MODE)
	{
#ifndef _FINAL
		char *pCheat = "mppoltergeist";
		g_pCheatMgr->Check(CParsedMsg( 1, &pCheat ));
#endif
	}*/
	///////<-

#ifndef _DEMO
#ifndef _TRON_E3_DEMO
#ifndef _FINAL
	if (key == VK_MULTIPLY)
	{
		static bool g_bCycleOn = false;
		g_bCycleOn = !g_bCycleOn;
		if(g_bCycleOn)
		{
			g_pLTClient->RunConsoleString("cmd msg player StartLightCycle");
		}
		else
		{
			g_pLTClient->RunConsoleString("cmd msg player EndLightCycle");
		}
	}
#endif
#endif
#endif

	// Let the interface mgr have a go at it...
	if (GetInterfaceMgr( )->OnKeyDown(key, rep))
	{
		return;
	}
	if (GetPlayerMgr()->OnKeyDown(key, rep))
	{
		return;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnKeyUp(int key, int rep)
//
//	PURPOSE:	Handle key up notification
//
// ----------------------------------------------------------------------- //
void CGameClientShell::OnKeyUp(int key)
{
	if( key == VK_F8 )
	{
		
		LPITEMIDLIST pidl;
		LPMALLOC pMalloc;
		char szPath[MAX_PATH];
		char szFileName[MAX_PATH];

		SHGetSpecialFolderLocation( g_hMainWnd, CSIDL_DESKTOP, &pidl );
		SHGetPathFromIDList( pidl, szPath );	
		SHGetMalloc( &pMalloc );
		pMalloc->Free( pidl );
		pMalloc->Release();

		strcat( szPath, "\\" );

		for( int i=0 ; i<3000 ; ++i ) 
		{
            sprintf(szFileName, "%s\\ScreenShot_%d.bmp", szPath, i);

			FILE *fp = fopen(szFileName, "rb");
            if( fp ) 
			{
                fclose( fp );
            }
            else 
			{   
				sprintf(szFileName, "ScreenShot_%d.bmp", i);
				strcat( szPath, szFileName );
				g_pLTClient->MakeScreenShot(szPath);

				if( g_pInterfaceMgr )
				{
					sprintf( szPath, "%s°¡ ¹ÙÅÁÈ­¸é¿¡ ÀúÀåµÇ¾ú½À´Ï´Ù.", szFileName );

					if( g_pInterfaceMgr->GetGameState() == GS_PLAYING )
					{
						g_pChatMsgs->AddMessage( szPath, kMsgNotice );
					}
					else if( g_pInterfaceMgr->GetGameState() == GS_SCREEN )
					{
						if( g_pInterfaceMgr->GetCurrentScreen() == SCREEN_ID_MAIN )
						{
							g_pScreenMain->AddChat( szPath, CHAT_TYPE_NOTICE );
						}
						else if( g_pInterfaceMgr->GetCurrentScreen() == SCREEN_ID_SCREEN_ROOM )
						{
							g_pScreenRoom->AddChat( szPath, CHAT_TYPE_NOTICE );
						}
					}
				}
                break;
            }
        }
		return;
	}
	
	if (GetInterfaceMgr( )->OnKeyUp(key))
		return;
	if (GetPlayerMgr()->OnKeyUp(key)) 
		return;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnMessage()
//
//	PURPOSE:	Handle client messages
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnMessage(ILTMessage_Read *pMsg)
{
	// Check inputs.
	if( !pMsg )
		return;

	// Read the message ID
	uint8 messageID = pMsg->Readuint8();

	// Act like the message ID wasn't in the message
	CLTMsgRef_Read cSubMsg(pMsg->SubMsg(pMsg->Tell()));

	// Let interface handle message first...

	if (GetInterfaceMgr( )->OnMessage(messageID, cSubMsg)) return;
	if (GetPlayerMgr()->OnMessage(messageID, cSubMsg)) return;

	if( g_pClientMultiplayerMgr->OnMessage(messageID, cSubMsg ))
		return;

	if( g_pMissionMgr->OnMessage( messageID, *cSubMsg ))
		return;

	// See if the scmd can handle this message.
	if( ScmdConsole::Instance( ).OnMessage( messageID, *cSubMsg ))
		return;

	/***************************************************************************/

	switch(messageID)
	{
		case MID_SFX_MESSAGE:			HandleMsgSFXMessage			(cSubMsg);	break;	
		case MID_WEAPON_FIRE:			HandleMsgWeaponFire			(cSubMsg);	break;	// ±¤½Ä
		case MID_PLAYER_UPDATE :		HandleMsgDeathUpdate		(cSubMsg);	break;	// ±¤½Ä : Á×¾úÀ» ¶§
		case MID_PLAYER_HH_WEAPON:		HandleMsgHHWeapon			(cSubMsg);	break;
		case MID_HIT_SFX:				HandleMsgHitSFX				(cSubMsg);	break;
		case MID_BODY_HIT_SFX:			HandleMsgBodyHitSFX			(cSubMsg);	break;
		case MID_SERVER_ERROR:			HandleMsgServerError		(cSubMsg);	break;
		case MID_PROJECTILE:			HandleMsgProjectile			(messageID, cSubMsg);	break;		
		case MID_SWITCHINGWORLDSSTATE:	HandleMsgSwitchingWorldState(cSubMsg);	break;
		case MID_MULTIPLAYER_OPTIONS:	HandleMsgMultiplayerOptions	(cSubMsg);	break;		
		case STC_BPRINT:				HandleMsgBPrint				(cSubMsg);	break;
		case MID_MUSIC:					HandleMsgMusic				(cSubMsg);	break;
		case MID_PLAYER_LOADCLIENT:		HandleMsgPlayerLoadClient	(cSubMsg);	break;
		case MID_GAME_PAUSE:			HandleMsgPauseGame			(cSubMsg);	break;

		default: 						HandleMsgDefault			(cSubMsg);	break;
	}
}


void CGameClientShell::HandleMsgDeathUpdate(ILTMessage_Read *pMsg)
{
	g_pMoveMgr->SetControlFlags( pMsg->Readuint32() );
	/*
	uint16 tempClientChangeFlags = pMsg->Readuint16();

	if (tempClientChangeFlags & CLIENTUPDATE_ALLOWINPUT)
	{
		uint16 tempFlags = g_pPlayerMgr->GetPlayerInfoChangeFlags();
		tempFlags &= ~CLIENTUPDATE_ALLOWINPUT;
		g_pPlayerMgr->SetPlayerInfoChangeFlags(tempFlags);
		bool tempbAllowInput = (LTBOOL)pMsg->Readuint8();
	}
	else
	{
		g_pPlayerMgr->SetPlayerInfoChangeFlags( tempClientChangeFlags );

		if (tempClientChangeFlags & CLIENTUPDATE_PLAYERROT)
		{
			uint8 byteRotation = pMsg->Readuint8();

			//LTRotation rRot;			
			//UncompressRotationByte(byteRotation, &rRot);
			//g_pLTClient->SetObjectRotation( g_pMoveMgr->GetObject(), &rRot );

			//EulerAngles EA = Eul_FromQuat( rRot, EulOrdYXZr );			
			//g_pPlayerMgr->SetYaw(EA.x);
			//g_pPlayerMgr->SetRoll(EA.z);
		}
		else if ( tempClientChangeFlags & CLIENTUPDATE_ACCURATEPLAYERROT )
		{
			uint16 wRotation = pMsg->Readuint16();

			//LTRotation rRot;
			//UncompressRotationShort( wRotation, &rRot );
			//g_pLTClient->SetObjectRotation( g_pMoveMgr->GetObject(), &rRot );
			//EulerAngles EA = Eul_FromQuat( rRot, EulOrdYXZr );			
			//g_pPlayerMgr->SetYaw(EA.x);
			//g_pPlayerMgr->SetRoll(EA.z);
		}
		else if( tempClientChangeFlags & CLIENTUPDATE_FULLPLAYERROT )
		{
			
			LTRotation temprFullPlayerRot = pMsg->ReadCompLTRotation();

			//g_pLTClient->SetObjectRotation( g_pMoveMgr->GetObject(), &temprFullPlayerRot );
			//EulerAngles EA = Eul_FromQuat( temprFullPlayerRot, EulOrdYXZr );			
			//g_pPlayerMgr->SetYaw(EA.x);
			//g_pPlayerMgr->SetRoll(EA.z);
		}

		if ( tempClientChangeFlags & ( CLIENTUPDATE_PLAYERROT | CLIENTUPDATE_ACCURATEPLAYERROT ) )
		{
			uint8 ucPitch = pMsg->Readuint8();
			
			//float fPitch;
			//UncompressAngleFromByte( ucPitch, &fPitch );			
		}

		if ( tempClientChangeFlags & CLIENTUPDATE_CAMERAOFFSET )
		{
			TVector3< short > vCompressedCameraOffset;

			vCompressedCameraOffset.x = pMsg->Readint16();
			vCompressedCameraOffset.y = pMsg->Readint16();
			vCompressedCameraOffset.z = pMsg->Readint16();
		}
	}

	g_pMoveMgr->SetControlFlags(pMsg->Readuint32() );	
	//uint32 controlFlags = pMsg->Readuint32();

	//--------------------------------------------------------------------------

	//LTVector newPos, curPos, curVel, newVel;
	LTVector newPos, newVel;
    uint8 moveCode;
    LTBOOL bOnGround;

    moveCode = pMsg->Readuint8();

    newPos = pMsg->ReadLTVector();

	newVel = pMsg->ReadLTVector();

    bOnGround = pMsg->Readuint8();
    SurfaceType tempeStandingOnSurface = (SurfaceType) pMsg->Readuint8();

	//g_pLTClient->SetObjectPos( g_pMoveMgr->GetObject(), &newPos );
	*/
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgSFXMessage()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgSFXMessage (ILTMessage_Read *pMsg)
{
	// ±¤½Ä
	//------------------------------------
	/*
	uint8 nFXType   = pMsg->Readuint8();
	
	if( nFXType == SFX_WEAPON_ID )
	{
		LTVector vImpactPos, vImpactNormal, vFirePos;
		SurfaceType eType;
		uint8 nPlayer = pMsg->Readuint8();
		CSpecialFXList* pList = m_sfxMgr.GetFXList(SFX_CHARACTER_ID);
		uint8 nWeaponId = WMGR_INVALID_ID;
		if (!pList) return;
		int nNumChars = pList->GetSize();
		if( nPlayer >= 0 && nPlayer < nNumChars )
		{
			if( (*pList)[nPlayer] )
			{
				CCharacterFX* pChar = (CCharacterFX*)(*pList)[nPlayer];

				HOBJECT hLocalObj = g_pLTClient->GetClientObject();
				if( pChar->GetServerObj() == hLocalObj ) return;
				nWeaponId = pChar->GetCurrentWeaponID();
				g_pPlayerMgr->GetClientWeaponMgr()->GetCurrentClientWeapon()->OtherPeopleFire(nPlayer,
																							  pChar->GetModelPitch(),
																							  nWeaponId,
																							  vFirePos,
																							  vImpactPos,
																							  vImpactNormal,
																							  eType);

				CAutoMessage cMsg;
				cMsg.Writeuint8(SFX_WEAPON_ID);
				cMsg.WriteObject(LTNULL);					// serverobj
				cMsg.WriteObject(LTNULL);					// objecthit
				cMsg.WriteObject(pChar->GetServerObj());	// FireFrom
				cMsg.Writeuint8(nWeaponId);					// WeaponId
				cMsg.Writeuint8(eType);						// SurfaceType
				cMsg.Writeuint16(WFX_FIRESOUND | WFX_ALTFIRESND); // IgnoreFX
				cMsg.Writeuint8(nPlayer);					// ShooterId
				//cMsg.WriteLTVector(vFirePos);				// FirePos
				//cMsg.WriteLTVector(vImpactPos);				// Pos
				cMsg.WriteCompLTVector(vFirePos);
				cMsg.WriteCompLTVector(vImpactPos);
				cMsg.WriteLTVector(vImpactNormal);			// Surface Normal
				
				m_sfxMgr.OnSFXMessage(cMsg.Read());
				
			}
		}
		return;
	}
	pMsg->SeekTo(0);
	*/
	//----------------------------------

	m_sfxMgr.OnSFXMessage(pMsg);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgMusic()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgMusic (ILTMessage_Read *pMsg)
{
	if (m_Music.IsInitialized())
	{
		m_Music.ProcessMusicMessage(pMsg);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgPlayerLoadClient()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgPlayerLoadClient (ILTMessage_Read *pMsg)
{
	g_pVersionMgr->SetCurrentSaveVersion( pMsg->Readuint32( ));
	CLTMsgRef_Read pSaveMessage = pMsg->ReadMessage();
	UnpackClientSaveMsg(pSaveMessage);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgServerError()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgServerError (ILTMessage_Read *pMsg)
{
	if (!pMsg) return;

    uint8 nError = pMsg->Readuint8();
	switch (nError)
	{
		case SERROR_SAVEGAME :
		{
			//DoMessageBox(IDS_SAVEGAMEFAILED, TH_ALIGN_CENTER);
		}
		break;

		case SERROR_LOADGAME :
		{
			//DoMessageBox(IDS_NOLOADLEVEL, TH_ALIGN_CENTER);

			GetInterfaceMgr( )->ChangeState(GS_SCREEN);
		}
		break;

		default : break;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgBPrint()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgBPrint (ILTMessage_Read *pMsg)
{
	char msg[50];
	pMsg->ReadString(msg, sizeof(msg));
	CSPrint(msg);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgProjectile()
//
//	PURPOSE:	Handle messages from the server regarding projectiles
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgProjectile( uint8 messageID, ILTMessage_Read *pMsg )
{
	// pass the message to the client weapon manager
	ASSERT( 0 != g_pPlayerMgr );
	CClientWeaponMgr *pClientWeaponMgr = g_pPlayerMgr->GetClientWeaponMgr();
	pClientWeaponMgr->OnMessage( messageID, pMsg );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgPauseGame()
//
//	PURPOSE:	Handle server telling us the game is paused.
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgPauseGame( ILTMessage_Read* pMsg )
{
	//[MURSUM]=================================================
	//¾Æ¹«Áþµµ ¾ÈÇÔ~!
	//=========================================================
	//m_bServerPaused = pMsg->Readbool( );
}

//[MURSUM]=============================================================
void CGameClientShell::SetPauseGame( bool bPause )
{
	m_bServerPaused	= bPause;
}
//=====================================================================

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgSwitchingWorldState()
//
//	PURPOSE:	Handle server telling us the switching world state.
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgSwitchingWorldState( ILTMessage_Read* pMsg )
{
	m_eSwitchingWorldsState = ( SwitchingWorldsState )pMsg->Readuint8();
}


void CGameClientShell::SetSwitchingWorldState( SwitchingWorldsState eState )
{
	m_eSwitchingWorldsState = eState;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgMultiplayerOptions()
//
//	PURPOSE:	Handle server telling us the game options.
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgMultiplayerOptions( ILTMessage_Read* pMsg )
{
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();
	if( !pProfile )
		return;	

	uint8 nRunSpeed = pMsg->Readuint8( );
	uint8 nScoreLimit = pMsg->Readuint8( );
	uint8 nTimeLimit = pMsg->Readuint8( );
	uint8 nRounds = pMsg->Readuint8();
	bool bFriendlyFire = pMsg->Readbool( );
	uint8 nDifficulty = pMsg->Readuint8( );
	float fPlayerDiffFactor = pMsg->Readfloat( );

	switch (m_eGameType)
	{
	case eGameTypeCooperative:
		pProfile->m_ServerGameOptions.GetCoop().m_bFriendlyFire = bFriendlyFire;
		pProfile->m_ServerGameOptions.GetCoop().m_nDifficulty = nDifficulty;
		break;
	case eGameTypeDeathmatch:
		pProfile->m_ServerGameOptions.GetDeathmatch().m_nScoreLimit = nScoreLimit;
		pProfile->m_ServerGameOptions.GetDeathmatch().m_nTimeLimit = nTimeLimit;
		pProfile->m_ServerGameOptions.GetDeathmatch().m_nRounds = nRounds;
		break;
	case eGameTypeTeamDeathmatch:
		pProfile->m_ServerGameOptions.GetTeamDeathmatch().m_nScoreLimit = nScoreLimit;
		pProfile->m_ServerGameOptions.GetTeamDeathmatch().m_nTimeLimit = nTimeLimit;
		pProfile->m_ServerGameOptions.GetTeamDeathmatch().m_nRounds = nRounds;
		pProfile->m_ServerGameOptions.GetTeamDeathmatch().m_bFriendlyFire = bFriendlyFire;
		break;
	case eGameTypeDoomsDay:
		pProfile->m_ServerGameOptions.GetDoomsday().m_nTimeLimit = nTimeLimit;
		pProfile->m_ServerGameOptions.GetDoomsday().m_nRounds = nRounds;
		pProfile->m_ServerGameOptions.GetDoomsday().m_bFriendlyFire = bFriendlyFire;
		break;

	case eGameTypeSurvival :
		pProfile->m_ServerGameOptions.GetSurvival().m_nScoreLimit = nScoreLimit;
		pProfile->m_ServerGameOptions.GetSurvival().m_nTimeLimit = nTimeLimit;
		pProfile->m_ServerGameOptions.GetSurvival().m_nRounds = nRounds;
		pProfile->m_ServerGameOptions.GetSurvival().m_bFriendlyFire = bFriendlyFire;
		break;
	}

	pProfile->Save( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleMsgDefault()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleMsgDefault (ILTMessage_Read *pMsg)
{

}

void CGameClientShell::HandleMsgWeaponFire(ILTMessage_Read* pMsg)
{
	IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
	if( pClientWeapon )
	{
		LTRotation rRot;
		LTVector vPos(0, 0, 0);
		g_pLTClient->GetObjectPos(g_pPlayerMgr->GetCamera(), &vPos);
		g_pLTClient->GetObjectRotation(g_pPlayerMgr->GetCamera(), &rRot);
		FireType eFireType = FT_NORMAL_FIRE;

		//g_pPlayerMgr->SetControlFlags(BC_CFLG_FIRING);
		// ±¤½Ä
		// [05.02.11] : ControlFlags Test
		/*
		g_pPlayerMgr->GetClientWeaponMgr()->Update( rRot, vPos, true, eFireType );
		g_pPlayerMgr->GetClientWeaponMgr()->Update( rRot, vPos, true, eFireType );
		g_pPlayerMgr->GetClientWeaponMgr()->Update( rRot, vPos, false, eFireType );
		*/
		//g_pPlayerMgr->    StartMuzzleFlash
	}
}
/***************************************************************************/


void CGameClientShell::HandleMsgHHWeapon( ILTMessage_Read *pMsg )
{
	uint8	nPlayerID		= pMsg->Readuint8();
	HOBJECT	hWeaponModel	= pMsg->ReadObject();

	CCharacterFX* pFX = m_sfxMgr.GetCharacterFX( nPlayerID );
	if( pFX )
	{
		pFX->SetCurrentWeapon( hWeaponModel );
	}
}

void CGameClientShell::HandleMsgHitSFX( ILTMessage_Read* pMsg )
{
	m_sfxMgr.CreateHitFX(pMsg);
}

void CGameClientShell::HandleMsgBodyHitSFX( ILTMessage_Read* pMsg )
{
	m_sfxMgr.CreateBodyHitFX(pMsg);
}

void CGameClientShell::HandleWeaponChange( uint16 nPlayerID, uint16 nWeaponID, uint16 nAmmoCount, uint16 nAmmoInClip )
{
	if( LTFALSE == GetPlayerMgr()->IsPlayerInWorld() ||
		GS_PLAYING != g_pInterfaceMgr->GetGameState() ) return;

	WEAPON const* pWeapon	= g_pWeaponMgr->GetWeapon(nWeaponID);
	if( !pWeapon ) return;

	uint8 nAmmoID = pWeapon->nDefaultAmmoId;

	CCharacterFX* pFX = m_sfxMgr.GetCharacterFX((uint8)nPlayerID);
	if( !pFX ) return;

	HOBJECT hWeaponModel = pFX->GetCurrentWeapon();	
	if( hWeaponModel &&
		LTTRUE == g_pLTClient->CheckValidObject( hWeaponModel, OT_MODEL ) )
	{
		ObjectCreateStruct cOCS;
		cOCS.Clear();				
		SAFE_STRCPY( cOCS.m_Filename, pWeapon->szHHModel );
		pWeapon->blrHHSkins.CopyList( 0, cOCS.m_SkinNames[0], MAX_CS_FILENAME_LEN +1 );	
		g_pCommonLT->SetObjectFilenames( hWeaponModel, &cOCS );	
		pFX->OnWeaponChanged( (uint8)nWeaponID, nAmmoID, nAmmoCount, nAmmoInClip );
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::FlashScreen()
//
//	PURPOSE:	Tint screen
//
// ----------------------------------------------------------------------- //

void CGameClientShell::FlashScreen(const LTVector &vFlashColor, const LTVector &vPos, float fFlashRange,
                                  float fTime, float fRampUp, float fRampDown, bool bForce)
{
	// [KLS 2/27/02] - Removed ALL screen flashing...
	return;


	CGameSettings* pSettings = GetInterfaceMgr( )->GetSettings();
	if (!pSettings) return;

	if (!bForce && !pSettings->ScreenFlash()) return;

    LTVector vCamPos;
	g_pLTClient->GetObjectPos(GetPlayerMgr()->GetCamera(), &vCamPos);

	// Determine if we can see this...

    LTVector vDir;
	vDir =  vPos - vCamPos;
    float fDirMag = vDir.Length();
	if (fDirMag > fFlashRange) return;

	// Okay, not adjust the tint based on the camera's angle to the tint pos.

    LTRotation rRot;
	g_pLTClient->GetObjectRotation(GetPlayerMgr()->GetCamera(), &rRot);

	vDir.Normalize();
    float fMul = vDir.Dot(rRot.Forward());
	if (fMul <= 0.0f) return;

	// {MD} See if we can even see this point.
	ClientIntersectQuery iQuery;
	ClientIntersectInfo iInfo;
	iQuery.m_From = vPos;
	iQuery.m_To = vCamPos;
    if(g_pLTClient->IntersectSegment(&iQuery, &iInfo))
	{
		// Something is in the way.
		return;
	}

	// Tint less if the pos was far away from the camera...

    float fVal = 1.0f - (fDirMag/fFlashRange);
	fMul *= (fVal <= 1.0f ? fVal : 1.0f);

    m_bFlashScreen  = true;
    m_fFlashStart   = g_pLTClient->GetTime();
	m_fFlashTime		= fTime;
	m_fFlashRampUp	= fRampUp;
	m_fFlashRampDown	= fRampDown;
	VEC_COPY(m_vFlashColor, vFlashColor);
	VEC_MULSCALAR(m_vFlashColor, m_vFlashColor, fMul);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::PauseGame()
//
//	PURPOSE:	Pauses/Unpauses the server
//
// ----------------------------------------------------------------------- //

void CGameClientShell::PauseGame(bool bPause, bool bPauseSound)
{
	m_bGamePaused = bPause;

	SetInputState(!bPause && GetPlayerMgr()->IsPlayerMovementAllowed());
	GetPlayerMgr()->SetMouseInput(!bPause, LTTRUE);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::SetInputState()
//
//	PURPOSE:	Allows/disallows input
//
// ----------------------------------------------------------------------- //

void CGameClientShell::SetInputState(bool bAllowInput)
{
    g_pLTClient->SetInputState(bAllowInput);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateWeaponMuzzlePosition()
//
//	PURPOSE:	Update the current weapon muzzle pos
//
// ----------------------------------------------------------------------- //

void CGameClientShell::UpdateWeaponMuzzlePosition()
{
    float fIncValue = WEAPON_MOVE_INC_VALUE_SLOW;
    bool bChanged = false;

    LTVector vOffset(0, 0, 0);

	uint32 dwPlayerFlags = GetPlayerMgr()->GetPlayerFlags();

	// Move weapon faster if running...

	if (dwPlayerFlags & BC_CFLG_RUN)
	{
		fIncValue = WEAPON_MOVE_INC_VALUE_FAST;
	}

  	IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
	if ( pClientWeapon )
	{
		vOffset = pClientWeapon->GetMuzzleOffset();
	}


	// Move weapon forward or backwards...

	if ((dwPlayerFlags & BC_CFLG_FORWARD) || (dwPlayerFlags & BC_CFLG_REVERSE))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_FORWARD ? fIncValue : -fIncValue;
		vOffset.z += fIncValue;
        bChanged = true;
	}


	// Move the weapon to the player's right or left...

	if ((dwPlayerFlags & BC_CFLG_STRAFE_RIGHT) ||
		(dwPlayerFlags & BC_CFLG_STRAFE_LEFT))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vOffset.x += fIncValue;
        bChanged = true;
	}


	// Move the weapon up or down relative to the player...

	if ((dwPlayerFlags & BC_CFLG_JUMP) || (dwPlayerFlags & BC_CFLG_DUCK))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_DUCK ? -fIncValue : fIncValue;
		vOffset.y += fIncValue;
        bChanged = true;
	}


	// Okay, set the offset...

	if (bChanged)
	{
		if ( pClientWeapon )
		{
			pClientWeapon->SetMuzzleOffset( vOffset );
		}
	}

	//if (bChanged)
	//{
	//	CSPrint ("Muzzle offset = %f, %f, %f", vOffset.x, vOffset.y, vOffset.z);
	//}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateWeaponPosition()
//
//	PURPOSE:	Update the position of the current weapon
//
// ----------------------------------------------------------------------- //

void CGameClientShell::UpdateWeaponPosition()
{
	float fIncValue = WEAPON_MOVE_INC_VALUE_SLOW;
	bool bChanged = false;

	LTVector vOffset;
	VEC_INIT(vOffset);

	uint32 dwPlayerFlags = GetPlayerMgr()->GetPlayerFlags();

	// Move weapon faster if running...

	if (dwPlayerFlags & BC_CFLG_RUN)
	{
		fIncValue = WEAPON_MOVE_INC_VALUE_FAST;
	}

	IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
	if ( pClientWeapon )
	{
		vOffset = pClientWeapon->GetWeaponOffset();
	}

	// Move weapon forward or backwards...

	if ((dwPlayerFlags & BC_CFLG_FORWARD) || (dwPlayerFlags & BC_CFLG_REVERSE))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_FORWARD ? fIncValue : -fIncValue;
		vOffset.z += fIncValue;
		bChanged = true;
	}


	// Move the weapon to the player's right or left...

	if ((dwPlayerFlags & BC_CFLG_STRAFE_RIGHT) ||
		(dwPlayerFlags & BC_CFLG_STRAFE_LEFT))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vOffset.x += fIncValue;
		bChanged = true;
	}


	// Move the weapon up or down relative to the player...

	if ((dwPlayerFlags & BC_CFLG_JUMP) || (dwPlayerFlags & BC_CFLG_DUCK))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_DUCK ? -fIncValue : fIncValue;
		vOffset.y += fIncValue;
		bChanged = true;
	}


	// Okay, set the offset...

	if (bChanged)
	{
		if ( pClientWeapon )
		{
			pClientWeapon->SetWeaponOffset( vOffset );
		}
	}

	//if (bChanged)
	//{
	//	CSPrint ("Weapon offset = %f, %f, %f", vOffset.x, vOffset.y, vOffset.z);
	//}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateWeaponBreachOffset()
//
//	PURPOSE:	Update the current weapon breach offset
//
// ----------------------------------------------------------------------- //

void CGameClientShell::UpdateWeaponBreachOffset()
{
    float fIncValue = WEAPON_MOVE_INC_VALUE_SLOW;
    bool bChanged = false;

    LTVector vOffset(0, 0, 0);

	uint32 dwPlayerFlags = GetPlayerMgr()->GetPlayerFlags();

	// Move weapon faster if running...

	if (dwPlayerFlags & BC_CFLG_RUN)
	{
		fIncValue = WEAPON_MOVE_INC_VALUE_FAST;
	}

  	IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
	if ( pClientWeapon )
	{
		vOffset = pClientWeapon->GetBreachOffset();
	}


	// Move weapon forward or backwards...

	if ((dwPlayerFlags & BC_CFLG_FORWARD) || (dwPlayerFlags & BC_CFLG_REVERSE))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_FORWARD ? fIncValue : -fIncValue;
		vOffset.z += fIncValue;
        bChanged = true;
	}


	// Move the weapon to the player's right or left...

	if ((dwPlayerFlags & BC_CFLG_STRAFE_RIGHT) ||
		(dwPlayerFlags & BC_CFLG_STRAFE_LEFT))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vOffset.x += fIncValue;
        bChanged = true;
	}


	// Move the weapon up or down relative to the player...

	if ((dwPlayerFlags & BC_CFLG_JUMP) || (dwPlayerFlags & BC_CFLG_DUCK))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_DUCK ? -fIncValue : fIncValue;
		vOffset.y += fIncValue;
        bChanged = true;
	}


	// Okay, set the offset...

	if (bChanged)
	{
		if ( pClientWeapon )
		{
			pClientWeapon->SetBreachOffset( vOffset );
		}
	}

	if (bChanged)
	{
		CSPrint ("Breach offset = %f, %f, %f", vOffset.x, vOffset.y, vOffset.z);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::Adjust1stPersonCamera()
//
//	PURPOSE:	Update the 1st-person camera offset
//
// ----------------------------------------------------------------------- //

void CGameClientShell::Adjust1stPersonCamera()
{
    float fIncValue = 0.1f;
    bool bChanged = false;

	uint32 dwPlayerFlags = GetPlayerMgr()->GetPlayerFlags();

	// Move offset faster if running...

	if (dwPlayerFlags & BC_CFLG_RUN)
	{
		fIncValue = fIncValue * 2.0f;
	}

	// Move 1st person offset.x forward or backwards...

	if ((dwPlayerFlags & BC_CFLG_FORWARD) || (dwPlayerFlags & BC_CFLG_REVERSE))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_FORWARD ? fIncValue : -fIncValue;
		g_vPlayerCameraOffset.x += fIncValue;
        bChanged = true;
	}

	// Move 1st person offset.y up or down...

	if ((dwPlayerFlags & BC_CFLG_JUMP) || (dwPlayerFlags & BC_CFLG_DUCK))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_JUMP ? fIncValue : -fIncValue;
		g_vPlayerCameraOffset.y += fIncValue;
        bChanged = true;
	}


	if (bChanged)
	{
		// Okay, set the offset...

		GetPlayerMgr()->GetPlayerCamera()->SetFirstPersonOffset(g_vPlayerCameraOffset);
        g_pLTClient->CPrint("1st person camera offset: %.2f, %.2f, %.2f", g_vPlayerCameraOffset.x, g_vPlayerCameraOffset.y, g_vPlayerCameraOffset.z);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::AdjustLightScale()
//
//	PURPOSE:	Update the current global light scale
//
// ----------------------------------------------------------------------- //

void CGameClientShell::AdjustLightScale()
{
    float fIncValue = 0.01f;
    bool bChanged = false;

    LTVector vScale;
	VEC_INIT(vScale);

    g_pLTClient->GetGlobalLightScale(&vScale);

	uint32 dwPlayerFlags = GetPlayerMgr()->GetPlayerFlags();

	// Move faster if running...

	if (dwPlayerFlags & BC_CFLG_RUN)
	{
		fIncValue = .5f;
	}


	// Move Red up/down...

	if ((dwPlayerFlags & BC_CFLG_FORWARD) || (dwPlayerFlags & BC_CFLG_REVERSE))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_FORWARD ? fIncValue : -fIncValue;
		vScale.x += fIncValue;
		vScale.x = vScale.x < 0.0f ? 0.0f : (vScale.x > 1.0f ? 1.0f : vScale.x);

        bChanged = true;
	}


	// Move Green up/down...

	if ((dwPlayerFlags & BC_CFLG_STRAFE_RIGHT) ||
		(dwPlayerFlags & BC_CFLG_STRAFE_LEFT))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vScale.y += fIncValue;
		vScale.y = vScale.y < 0.0f ? 0.0f : (vScale.y > 1.0f ? 1.0f : vScale.y);

        bChanged = true;
	}


	// Move Blue up/down...

	if ((dwPlayerFlags & BC_CFLG_JUMP) || (dwPlayerFlags & BC_CFLG_DUCK))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_DUCK ? -fIncValue : fIncValue;
		vScale.z += fIncValue;
		vScale.z = vScale.z < 0.0f ? 0.0f : (vScale.z > 1.0f ? 1.0f : vScale.z);

        bChanged = true;
	}


	// Okay, set the light scale.

    g_pLTClient->SetGlobalLightScale(&vScale);

	if (bChanged)
	{
		CSPrint ("Light Scale = %f, %f, %f", vScale.x, vScale.y, vScale.z);
	}

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::AdjustLightAdd()
//
//	PURPOSE:	Update the current global light add
//
// ----------------------------------------------------------------------- //

void CGameClientShell::AdjustLightAdd()
{
    float fIncValue = 0.01f;
    bool bChanged = false;

    LTVector vScale;
	VEC_INIT(vScale);

    g_pLTClient->GetCameraLightAdd(GetPlayerMgr()->GetCamera(), &vScale);

	uint32 dwPlayerFlags = GetPlayerMgr()->GetPlayerFlags();

	// Move faster if running...

	if (dwPlayerFlags & BC_CFLG_RUN)
	{
		fIncValue = .5f;
	}


	// Move Red up/down...

	if ((dwPlayerFlags & BC_CFLG_FORWARD) || (dwPlayerFlags & BC_CFLG_REVERSE))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_FORWARD ? fIncValue : -fIncValue;
		vScale.x += fIncValue;
		vScale.x = vScale.x < 0.0f ? 0.0f : (vScale.x > 1.0f ? 1.0f : vScale.x);

        bChanged = true;
	}


	// Move Green up/down...

	if ((dwPlayerFlags & BC_CFLG_STRAFE_RIGHT) ||
		(dwPlayerFlags & BC_CFLG_STRAFE_LEFT))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vScale.y += fIncValue;
		vScale.y = vScale.y < 0.0f ? 0.0f : (vScale.y > 1.0f ? 1.0f : vScale.y);

        bChanged = true;
	}


	// Move Blue up/down...

	if ((dwPlayerFlags & BC_CFLG_JUMP) || (dwPlayerFlags & BC_CFLG_DUCK))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_DUCK ? -fIncValue : fIncValue;
		vScale.z += fIncValue;
		vScale.z = vScale.z < 0.0f ? 0.0f : (vScale.z > 1.0f ? 1.0f : vScale.z);

        bChanged = true;
	}


	// Okay, set the light add.

    g_pLTClient->SetCameraLightAdd(GetPlayerMgr()->GetCamera(), &vScale);

	if (bChanged)
	{
		CSPrint ("Light Add = %f, %f, %f", vScale.x, vScale.y, vScale.z);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::AdjustFOV()
//
//	PURPOSE:	Update the current FOV
//
// ----------------------------------------------------------------------- //

void CGameClientShell::AdjustFOV()
{
    float fIncValue = 0.001f;
    bool bChanged = false;

    float fCurFOVx, fCurFOVy;

	// Save the current camera fov...

    g_pLTClient->GetCameraFOV(GetPlayerMgr()->GetCamera(), &fCurFOVx, &fCurFOVy);

	uint32 dwPlayerFlags = GetPlayerMgr()->GetPlayerFlags();

	// Move faster if running...

	if (dwPlayerFlags & BC_CFLG_RUN)
	{
		fIncValue = .01f;
	}


	// Adjust X

	if ((dwPlayerFlags & BC_CFLG_STRAFE_RIGHT) || (dwPlayerFlags & BC_CFLG_STRAFE_LEFT))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		fCurFOVx += fIncValue;

        bChanged = true;
	}


	// Adjust Y

	if ((dwPlayerFlags & BC_CFLG_JUMP) || (dwPlayerFlags & BC_CFLG_DUCK))
	{
		fIncValue = dwPlayerFlags & BC_CFLG_DUCK ? -fIncValue : fIncValue;
		fCurFOVy += fIncValue;

        bChanged = true;
	}


	// Okay, set the FOV..

	// Adjust the fov...

	GetPlayerMgr()->SetCameraFOV(fCurFOVx, fCurFOVy);

	if (bChanged)
	{
		CSPrint ("FOV X = %.2f, FOV Y = %.2f", RAD2DEG(fCurFOVx), RAD2DEG(fCurFOVy));
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::SpecialEffectNotify()
//
//	PURPOSE:	Handle creation of a special fx
//
//				This function is called by the engine directly. 
//				When an effect takes place the server notifies all clients
//				who are able to witness the effect.  NOT ALL EFFECTS will
//				necesarily be recieves through this routine.  If the server
//				needs to gaurantee that all clients get a particular effect
//				it will use the MID_SFX_MESSAGE identifier which will be
//				recieved in the OnMessage routine and the OnSFXMessage 
//				method will be envoked to handle the effect instead. 
//
// ----------------------------------------------------------------------- //

void CGameClientShell::SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMsg)
{
	if (hObj)
	{
        g_pCommonLT->SetObjectFlags(hObj, OFT_Client, CF_NOTIFYREMOVE, CF_NOTIFYREMOVE);
	}
	m_sfxMgr.HandleSFXMsg(hObj, pMsg);

	// Reset the message and send to the other FX mgr
	pMsg->SeekTo(0);
	m_ClientFXMgr.OnSpecialEffectNotify( hObj, pMsg );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnObjectRemove()
//
//	PURPOSE:	Handle removal of a server created object...
//
// ----------------------------------------------------------------------- //

void CGameClientShell::OnObjectRemove(HLOCALOBJ hObj)
{
	if (!hObj) return;

	m_sfxMgr.RemoveSpecialFX(hObj);

	m_ClientFXMgr.OnObjectRemove( hObj );

	GetInterfaceMgr()->OnObjectRemove(hObj);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::PreLoadWorld()
//
//	PURPOSE:	Called before world loads
//
// ----------------------------------------------------------------------- //

void CGameClientShell::PreLoadWorld(const char *pWorldName)
{
	if (IsMainWindowMinimized())
	{
		RestoreMainWindow();
	}

	g_pMissionMgr->PreLoadWorld( pWorldName );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HasJoystick()
//
//	PURPOSE:	Determines whether or not there is a joystick device
//				present
//
// ----------------------------------------------------------------------- //

bool CGameClientShell::HasJoystick()
{
	// Ask the engine if we really have a joystick...
	DeviceObject *pJoysticks = g_pLTClient->GetDeviceObjects(DEVICETYPE_JOYSTICK);    
	if (pJoysticks)
	{
		g_pLTClient->FreeDeviceObjects(pJoysticks);
		return true;
	}

	return false;
	
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::IsJoystickEnabled()
//
//	PURPOSE:	Determines whether or not there is a joystick device
//				enabled
//
// ----------------------------------------------------------------------- //

bool CGameClientShell::IsJoystickEnabled()
{
	// first attempt to find a joystick device

	char strJoystick[128];
	memset (strJoystick, 0, 128);
    LTRESULT result = g_pLTClient->GetDeviceName (DEVICETYPE_JOYSTICK, strJoystick, 127);
    if (result != LT_OK) return false;

	// ok - we found the device and have a name...see if it's enabled

    bool bEnabled = false;
    g_pLTClient->IsDeviceEnabled (strJoystick, &bEnabled);

	return bEnabled;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::EnableJoystick()
//
//	PURPOSE:	Attempts to find and enable a joystick device
//
// ----------------------------------------------------------------------- //

bool CGameClientShell::EnableJoystick()
{
	// first attempt to find a joystick device

	char strJoystick[128];
	memset(strJoystick, 0, 128);
    LTRESULT result = g_pLTClient->GetDeviceName(DEVICETYPE_JOYSTICK, strJoystick, 127);
    if (result != LT_OK) return false;

	// ok, now try to enable the device

	char strConsole[256];
	sprintf(strConsole, "EnableDevice \"%s\"", strJoystick);
    g_pLTClient->RunConsoleString(strConsole);

    bool bEnabled = false;
    g_pLTClient->IsDeviceEnabled(strJoystick, &bEnabled);

	return bEnabled;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HasGamepad()
//
//	PURPOSE:	Determines whether or not there is a Gamepad device
//				present
//
// ----------------------------------------------------------------------- //

bool CGameClientShell::HasGamepad()
{
	// Ask the engine if we really have a Gamepad...
	DeviceObject *pGamepads = g_pLTClient->GetDeviceObjects(DEVICETYPE_GAMEPAD);    
	if (pGamepads)
	{
		g_pLTClient->FreeDeviceObjects(pGamepads);
		return true;
	}

	return false;

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::IsGamepadEnabled()
//
//	PURPOSE:	Determines whether or not there is a Gamepad device
//				enabled
//
// ----------------------------------------------------------------------- //

bool CGameClientShell::IsGamepadEnabled()
{
	// first attempt to find a Gamepad device

	char strGamepad[128];
	memset (strGamepad, 0, 128);
    LTRESULT result = g_pLTClient->GetDeviceName (DEVICETYPE_GAMEPAD, strGamepad, 127);
    if (result != LT_OK) return false;

	// ok - we found the device and have a name...see if it's enabled

    bool bEnabled = false;
    g_pLTClient->IsDeviceEnabled (strGamepad, &bEnabled);

	return bEnabled;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::EnableGamepad()
//
//	PURPOSE:	Attempts to find and enable a Gamepad device
//
// ----------------------------------------------------------------------- //

bool CGameClientShell::EnableGamepad()
{
	// first attempt to find a Gamepad device

	char strGamepad[128];
	memset(strGamepad, 0, 128);
    LTRESULT result = g_pLTClient->GetDeviceName(DEVICETYPE_GAMEPAD, strGamepad, 127);
    if (result != LT_OK) return false;

	// ok, now try to enable the device

	char strConsole[256];
	sprintf(strConsole, "EnableDevice \"%s\"", strGamepad);
    g_pLTClient->RunConsoleString(strConsole);

    bool bEnabled = false;
    g_pLTClient->IsDeviceEnabled(strGamepad, &bEnabled);

	return bEnabled;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::StartPerformanceTest()
//
//	PURPOSE:	Start the perfomance test
//
// ----------------------------------------------------------------------- //

void CGameClientShell::StartPerformanceTest()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::StopPerformanceTest()
//
//	PURPOSE:	Stop the perfomance test
//
// ----------------------------------------------------------------------- //

void CGameClientShell::StopPerformanceTest()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::AbortPerformanceTest()
//
//	PURPOSE:	Cancel out of the perfomance test
//
// ----------------------------------------------------------------------- //

void CGameClientShell::AbortPerformanceTest()
{
	StopPerformanceTest();

	// Make sure we're disconnected from server.
	if(g_pLTClient->IsConnected())
	{
		g_pInterfaceMgr->SetIntentionalDisconnect( true );
		g_pClientMultiplayerMgr->ForceDisconnect();
	}

	//since our history was cleared by loading the level... rebuild it
	g_pInterfaceMgr->GetScreenMgr()->AddScreenToHistory( SCREEN_ID_MAIN );
	g_pInterfaceMgr->GetScreenMgr()->AddScreenToHistory( SCREEN_ID_OPTIONS );

	if ( GetInterfaceMgr( )->GetGameState() == GS_LOADINGLEVEL)
	{
		//GetInterfaceMgr( )->LoadFailed(SCREEN_ID_PERFORMANCE,IDS_PERFORMANCE_TEST_ABORTED);
	}
	else
	{

		MBCreate mb;						// [NOVICE(minjin)] ¹«½¼ ±ÛÀÜÁö ¸ð¸£°Ú´Ù -_-;;
		GetInterfaceMgr( )->ShowMessageBox((int)IDS_PERFORMANCE_TEST_ABORTED, &mb);

		//we aborted performance testing go back to performance screen
		//g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_PERFORMANCE);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateDebugInfo()
//
//	PURPOSE:	Update debugging info.
//
// ----------------------------------------------------------------------- //

void CGameClientShell::UpdateDebugInfo()
{
	char buf[100];

	ClearDebugStrings();

	//determine the offset we should use on the positions
	LTVector vOffset(0, 0, 0);
	const char* pszOffsetDescription = "(Actual)";

	if((uint32)g_vtApplyWorldOffset.GetFloat(1.0f))
	{
		g_pLTClient->GetSourceWorldOffset(vOffset);
		pszOffsetDescription = "(Level)";
	}

	// Check to see if we should show the player position...

    HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
	if (m_bShowPlayerPos && hPlayerObj)
	{
        LTVector vPos;
		g_pLTClient->GetObjectPos(hPlayerObj, &vPos);

		//handle the shift from the current world to the source world
		vPos += vOffset;

		sprintf(buf, "Player Position %s: %6.0f, %6.0f, %6.0f  ", pszOffsetDescription, vPos.x, vPos.y, vPos.z);

		SetDebugString(buf);
	}

	if (m_bShowCamPosRot)
	{
        LTVector vPos;
		g_pLTClient->GetObjectPos(GetPlayerMgr()->GetCamera(), &vPos);

		//handle the shift from the current world to the source world
		vPos += vOffset;

		// Convert pitch and yaw to the same units used by DEdit...

		float fYawDeg = RAD2DEG(GetPlayerMgr()->GetYaw());
		while (fYawDeg < 0.0f)
		{
			fYawDeg += 360.0f;
		}
		while (fYawDeg > 360.0f)
		{
			fYawDeg -= 360.0f;
		}

		float fPitchDeg = RAD2DEG(GetPlayerMgr()->GetPitch());
		while (fPitchDeg < 0.0f)
		{
			fPitchDeg += 360.0f;
		}
		while (fPitchDeg > 360.0f)
		{
			fPitchDeg -= 360.0f;
		}

		sprintf(buf, "Camera Position %s: %6.0f %6.0f %6.0f  ", pszOffsetDescription, vPos.x, vPos.y, vPos.z);
		SetDebugString(buf, eDSBottomRight, 0);

		sprintf(buf, "Camera Yaw: %6.0f  ", fYawDeg);
		SetDebugString(buf, eDSBottomRight, 2);

		sprintf(buf, "Camera Pitch: %6.0f  ", fPitchDeg);
		SetDebugString(buf, eDSBottomRight, 1);
	}



	// See if the FOV has changed...

	if (GetPlayerMgr()->CanChangeFOV())
	{
		float fovX, fovY;
		g_pLTClient->GetCameraFOV(GetPlayerMgr()->GetCamera(), &fovX, &fovY);

		if (fovX != g_vtFOVXNormal.GetFloat() || fovY != g_vtFOVYNormal.GetFloat())
		{
			GetPlayerMgr()->SetCameraFOV(DEG2RAD(g_vtFOVXNormal.GetFloat()), DEG2RAD(g_vtFOVYNormal.GetFloat()));
		}
	}


	// Check to see if we are in spectator or invisible mode...

	LTBOOL bSpectator = GetPlayerMgr()->IsSpectatorMode();
	if ((bSpectator || GetPlayerMgr()->IsInvisibleMode()) && !g_bScreenShotMode)
	{
		SetDebugString(bSpectator ? "SPECTATOR MODE" : "GHOST MODE", eDSBottomLeft);

#ifndef _TO2DEMO
        LTVector vPos;
		g_pLTClient->GetObjectPos(GetPlayerMgr()->GetCamera(), &vPos);

		//handle the shift from the current world to the source world
		vPos += vOffset;

		sprintf(buf, "Camera Position %s: %6.0f %6.0f %6.0f", pszOffsetDescription, vPos.x, vPos.y, vPos.z);
		SetDebugString(buf);
#endif // _TO2DEMO
	}

	HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("PlayerDims");
	if (hVar)
	{
        if (g_pLTClient->GetVarValueFloat(hVar) > 0.0f)
		{
			CreateBoundingBox();
			UpdateBoundingBox();
		}
		else if (m_hBoundingBox)
		{
            g_pLTClient->RemoveObject(m_hBoundingBox);
            m_hBoundingBox = NULL;
		}
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::SetDebugString
//
//	PURPOSE:	Set the debug string (create it if necessary).
//
// --------------------------------------------------------------------------- //

void CGameClientShell::SetDebugString(char* strMessage, DSSL eLoc, uint8 nLine)
{
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::ClearDebugStrings
//
//	PURPOSE:	Clear all the debug strings
//
// --------------------------------------------------------------------------- //

void CGameClientShell::ClearDebugStrings()
{
	for (int i=0; i < kMaxDebugStrings; i++)
	{
		if (m_pLeftDebugString[i])
		{
			m_pLeftDebugString[i]->SetText("");
		}

		if (m_pRightDebugString[i])
		{
			m_pRightDebugString[i]->SetText("");
		}
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::RenderDebugStrings
//
//	PURPOSE:	Render all the debug strings
//
// --------------------------------------------------------------------------- //

void CGameClientShell::RenderDebugStrings()
{
    HSURFACE hScreen = g_pLTClient->GetScreenSurface();
    uint32 nScreenWidth, nScreenHeight;
    g_pLTClient->GetSurfaceDims(hScreen, &nScreenWidth, &nScreenHeight);

	float fx = 0.0f, fy = 0.0f;
	float fYLeftOffset = 0.0f, fYRightOffset = 0.0f;
	float fScreenWidth  = float(nScreenWidth);
	float fScreenHeight = float(nScreenHeight);

	for (int i=0; i < kMaxDebugStrings; i++)
	{
		if (m_pLeftDebugString[i])
		{
			fYLeftOffset += m_pLeftDebugString[i]->GetHeight();

			fy = fScreenHeight - fYLeftOffset;
	
			m_pLeftDebugString[i]->SetPosition(0.0f, fy);
			m_pLeftDebugString[i]->Render();
		}

		if (m_pRightDebugString[i])
		{
			fYRightOffset += m_pRightDebugString[i]->GetHeight();

			fx = fScreenWidth  - m_pRightDebugString[i]->GetWidth();
			fy = fScreenHeight - fYRightOffset;
	
			m_pRightDebugString[i]->SetPosition(fx, fy);
			m_pRightDebugString[i]->Render();
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::ToggleDebugCheat
//
//	PURPOSE:	Handle debug cheat toggles
//
// --------------------------------------------------------------------------- //

void CGameClientShell::ToggleDebugCheat(CheatCode eCheat)
{
	switch (eCheat)
	{
		case CHEAT_POSWEAPON_MUZZLE :
		{
			if (!g_pPlayerMgr->IsSpectatorMode())
			{
				m_bTweakingWeaponMuzzle = !m_bTweakingWeaponMuzzle;

				g_pPlayerMgr->GetMoveMgr()->AllowMovement(!m_bTweakingWeaponMuzzle);

				// Save tweaks...

				if (!m_bTweakingWeaponMuzzle)
				{
                    g_pWeaponMgr->WriteFile();
				}
			}
		}
		break;

		case CHEAT_WEAPON_BREACHOFFSET :
		{
			if (!g_pPlayerMgr->IsSpectatorMode())
			{
				m_bTweakingWeaponBreachOffset = !m_bTweakingWeaponBreachOffset;

				g_pPlayerMgr->GetMoveMgr()->AllowMovement(!m_bTweakingWeaponBreachOffset);

				// Save tweaks...

				if (!m_bTweakingWeaponBreachOffset)
				{
                    g_pWeaponMgr->WriteFile();
				}
			}
		}
		break;

		case CHEAT_POSWEAPON :
		{
			if (!g_pPlayerMgr->IsSpectatorMode())
			{
				m_bTweakingWeapon = !m_bTweakingWeapon;

				g_pPlayerMgr->GetMoveMgr()->AllowMovement(!m_bTweakingWeapon);

				// Save tweaks...

				if (!m_bTweakingWeapon)
				{
                    g_pWeaponMgr->WriteFile();
				}
			}
		}
		break;

		case CHEAT_POS1STCAM :
		{
			if (!g_pPlayerMgr->IsSpectatorMode())
			{
				m_bAdjust1stPersonCamera = !m_bAdjust1stPersonCamera;

				g_pPlayerMgr->GetMoveMgr()->AllowMovement(!m_bAdjust1stPersonCamera);
			}
		}
		break;

		case CHEAT_LIGHTSCALE :
		{
			m_bAdjustLightScale = !m_bAdjustLightScale;

			g_pPlayerMgr->GetMoveMgr()->AllowMovement(!m_bAdjustLightScale);
		}
		break;

		case CHEAT_LIGHTADD :
		{
			m_bAdjustLightAdd = !m_bAdjustLightAdd;

			g_pPlayerMgr->GetMoveMgr()->AllowMovement(!m_bAdjustLightAdd);
		}
		break;

		case CHEAT_FOV :
		{
			m_bAdjustFOV = !m_bAdjustFOV;

			g_pPlayerMgr->GetMoveMgr()->AllowMovement(!m_bAdjustFOV);
		}
		break;

		case CHEAT_CHASETOGGLE :
		{
			if (GetPlayerMgr()->GetPlayerState() == PS_ALIVE && !GetPlayerMgr()->IsZoomed())
			{
				GetPlayerMgr()->SetExternalCamera(GetPlayerMgr()->IsFirstPerson());
			}
		}
		break;


		default : break;
	}
}

void CGameClientShell::DebugWriteCameraPosition()
{
	static char		s_szDebugCamName[64] = "";
	static uint32	s_nDebugCamCount = 0;

	char szTmp[128] = "";
	GetConsoleString("DebugCameraName",szTmp,"CameraPoint");
	if (stricmp(s_szDebugCamName,szTmp) != 0)
	{
		SAFE_STRCPY(s_szDebugCamName,szTmp);
		s_nDebugCamCount = 0;
	}
	char szFileName[64];
	sprintf(szFileName,"Game\\%s%02d.txt", s_szDebugCamName, s_nDebugCamCount);

	FILE* pFile = fopen (szFileName, "wt");
	if (!pFile) return;

    LTVector vPos;
	g_pLTClient->GetObjectPos(GetPlayerMgr()->GetCamera(), &vPos);

	//handle the shift from the current world to the source world
	if((uint32)g_vtApplyWorldOffset.GetFloat(1.0f))
	{
		LTVector vOffset;
		g_pLTClient->GetSourceWorldOffset(vOffset);
		vPos += vOffset;
	}

	// Convert pitch and yaw to user friendly units...

	float fYawDeg = RAD2DEG(GetPlayerMgr()->GetYaw());
	while (fYawDeg < 0.0f)
	{
		fYawDeg += 360.0f;
	}
	while (fYawDeg > 360.0f)
	{
		fYawDeg -= 360.0f;
	}

	float fPitchDeg = RAD2DEG(GetPlayerMgr()->GetPitch());
	while (fPitchDeg < 0.0f)
	{
		fPitchDeg += 360.0f;
	}
	while (fPitchDeg > 360.0f)
	{
		fPitchDeg -= 360.0f;
	}


	sprintf(szTmp,"\n[CameraPoint]\n");
	fwrite (szTmp, strlen(szTmp), 1, pFile);

	s_nDebugCamCount++;
	sprintf(szTmp,"Name     = \"%s%02d\"\n", s_szDebugCamName, s_nDebugCamCount);
	fwrite (szTmp, strlen(szTmp), 1, pFile);

	sprintf(szTmp,"Pos      = <%6.0f,%6.0f,%6.0f>\n", vPos.x, vPos.y, vPos.z);
	fwrite (szTmp, strlen(szTmp), 1, pFile);

	sprintf(szTmp,"Rotation = <%0.5f, %0.5f, 0.00000>\n", GetPlayerMgr()->GetPitch(), GetPlayerMgr()->GetYaw());
	fwrite (szTmp, strlen(szTmp), 1, pFile);


	fwrite ("\n", 1, 1, pFile);

	fclose (pFile);

	g_pLTClient->CPrint("%s%02d:Pos=<%6.0f,%6.0f,%6.0f>;Rot=<%6.0f,%6.0f,     0>", 
						s_szDebugCamName, s_nDebugCamCount, vPos.x, vPos.y, vPos.z, fPitchDeg, fYawDeg);


}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::FirstUpdate
//
//	PURPOSE:	Handle first update (each level)
//
// --------------------------------------------------------------------------- //

void CGameClientShell::FirstUpdate()
{
	if (!m_bFirstUpdate) return; 

#ifdef _TRON_E3_DEMO
	s_fDemoTime = 0.0f;
#endif

	GetPlayerMgr()->FirstUpdate();

    m_bFirstUpdate = false;


	// Set up the level-start screen fade...

	if (g_varStartLevelScreenFade.GetFloat())
	{
		GetInterfaceMgr( )->StartScreenFadeIn(g_varStartLevelScreenFadeTime.GetFloat());
	}


	// Set prediction if we are playing multiplayer...We turn this
	// off for single player because projectiles look MUCH better...

	/* // ÀÌ ºÎºÐ ¿ì¸®°¡ »èÁ¦
	if ( g_pClientMultiplayerMgr->IsConnectedToRemoteServer( ))
	{
	   g_pLTClient->RunConsoleString("Prediction 1");
	}
	else
	{
	   g_pLTClient->RunConsoleString("Prediction 0");
	}
	*/


	// Set misc console vars...

	MirrorSConVar("AllSkyPortals", "AllSkyPortals");


	// Set up the global (per level) wind values...

    g_pLTClient->GetSConValueFloat("WindX", g_vWorldWindVel.x);
    g_pLTClient->GetSConValueFloat("WindY", g_vWorldWindVel.y);
    g_pLTClient->GetSConValueFloat("WindZ", g_vWorldWindVel.z);


	// Set up the global (per level) fog values...

	ResetDynamicWorldProperties();


	// Initialize the music playlists...

	RestoreMusic();


	// Start with a clean slate...

	//g_pLTClient->ClearInput();
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::RestoreMusic
//
//	PURPOSE:	Restore the music state
//
// --------------------------------------------------------------------------- //

void CGameClientShell::RestoreMusic()
{
	if (m_Music.IsInitialized())
	{
		// Initialize music for the current level...

		char aMusicDirectory[256];
		char* pMusicDirectory = NULL;
		if (g_pLTClient->GetSConValueString("MusicDirectory", aMusicDirectory, sizeof(aMusicDirectory)) == LT_OK)
		{
			pMusicDirectory = aMusicDirectory;
		}

		char aMusicControlFile[256];
		char* pMusicControlFile = NULL;
		if (g_pLTClient->GetSConValueString("MusicControlFile", aMusicControlFile, sizeof(aMusicControlFile)) == LT_OK)
		{
			pMusicControlFile = aMusicControlFile;
		}

		if (pMusicDirectory && pMusicControlFile)
		{
			CMusicState* pMS = m_Music.GetMusicState();

			CMusicState musicState;
			musicState.nIntensity = pMS->nIntensity;
			strcpy(musicState.szDirectory, pMusicDirectory);
			strcpy(musicState.szControlFile, pMusicControlFile);

			m_Music.RestoreMusicState(musicState);
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::MirrorSConVar
//
//	PURPOSE:	Takes the value of the server-side variable specified by
//				pSVarName and sets its value into the client-sdie variable
///				specified by pCVarName.
//
// --------------------------------------------------------------------------- //
void CGameClientShell::MirrorSConVar(char *pSVarName, char *pCVarName)
{
	char buf[512];
	float fVal = 0.0f;

    g_pLTClient->GetSConValueFloat(pSVarName, fVal);

	// Special case, make all farz calls go through this function...
	if (stricmp(pCVarName, "FarZ") == 0)
	{
		SetFarZ((int)fVal);
	}
	else
	{
		sprintf(buf, "%s %f", pCVarName, fVal);
		g_pLTClient->RunConsoleString(buf);
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::ResetGlobalFog
//
//	PURPOSE:	Reset the global fog values based on the saved values...
//
// --------------------------------------------------------------------------- //

void CGameClientShell::ResetDynamicWorldProperties(LTBOOL bUseWorldFog)
{
	// Set the FarZ for the level...

	MirrorSConVar("FarZ", "FarZ");
	MirrorSConVar("WorldNorth","WorldNorth");

	//Setup the fog to reflect the server's fog if applicable
	if(bUseWorldFog)
	{
		// See if fog should be disabled
		uint32 dwAdvancedOptions = GetInterfaceMgr( )->GetAdvancedOptions();

		if (!(dwAdvancedOptions & AO_FOG))
		{
			g_pLTClient->RunConsoleString("FogEnable 0");
			return;
		}

		MirrorSConVar("FogEnable", "FogEnable");
		MirrorSConVar("FogNearZ", "FogNearZ");
		MirrorSConVar("FogFarZ", "FogFarZ");
		MirrorSConVar("LMAnimStatic", "LMAnimStatic");
		MirrorSConVar("FogR", "FogR");
		MirrorSConVar("FogG", "FogG");
		MirrorSConVar("FogB", "FogB");

		MirrorSConVar("SkyFogEnable", "SkyFogEnable");
		MirrorSConVar("SkyFogNearZ", "SkyFogNearZ");
		MirrorSConVar("SkyFogFarZ", "SkyFogFarZ");

		// VFog....

		MirrorSConVar("SC_VFog", "VFog");
		MirrorSConVar("SC_VFogMinY", "VFogMinY");
		MirrorSConVar("SC_VFogMaxY", "VFogMaxY");
		MirrorSConVar("SC_VFogDensity", "VFogDensity");
		MirrorSConVar("SC_VFogMax", "VFogMax");
		MirrorSConVar("SC_VFogMaxYVal", "VFogMaxYVal");
		MirrorSConVar("SC_VFogMinYVal", "VFogMinYVal");
	}

	//update the shadow information
	MirrorSConVar("ModelShadow_Proj_Alpha", "ModelShadow_Proj_Alpha");
	MirrorSConVar("ModelShadow_Proj_MinColorComponent", "ModelShadow_Proj_MinColorComponent");
	MirrorSConVar("ModelShadow_Proj_MaxProjDist", "ModelShadow_Proj_MaxProjDist");
	MirrorSConVar("DrawAllModelShadows", "DrawAllModelShadows");
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::RenderCamera()
//
//	PURPOSE:	Sets up the client and renders the camera
//
// ----------------------------------------------------------------------- //

void CGameClientShell::RenderCamera(bool bDrawInterface)
{
	if (!m_bMainWindowFocus)
	{
		return;
	}
	
	if (!GetPlayerMgr()->PreRender())
	{
		return;
	}

	// Important to update this after the weapon model has been updated
	// (some fx depend on the position of the weapon model)...

	m_sfxMgr.UpdateSpecialFX();

	// Update any client-side special effects...

	m_DamageFXMgr.Update();

	//make sure to update the gore settings in case they changed
	UpdateGoreSettings();

	HLOCALOBJ hCamera = GetPlayerMgr()->GetCamera();

	//handle updating the paused status
	m_ClientFXMgr.Pause(IsServerPaused());

	//update all the effects status as well as any that might effect the camera
	m_ClientFXMgr.UpdateAllActiveFX( m_bMainWindowFocus );
	
    g_pLTClient->Start3D();	
	
	float fFrameTime = (IsServerPaused()) ? 0.0f : GetFrameTime();

	g_pLTClient->RenderCamera(hCamera, fFrameTime);

	// Render the the dynamic FX.
	m_sfxMgr.RenderFX(hCamera);

	// Render the effects
	m_ClientFXMgr.RenderAllActiveFX(m_bMainWindowFocus );
	
    g_pLTClient->StartOptimized2D();

	GetInterfaceMgr( )->Draw();

	// Display any necessary debugging info...

	RenderDebugStrings();

    g_pLTClient->EndOptimized2D();
    g_pLTClient->End3D();	
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::BuildClientSaveMsg
//
//	PURPOSE:	Save all the necessary client-side info
//
// --------------------------------------------------------------------------- //

void CGameClientShell::BuildClientSaveMsg(ILTMessage_Write *pMsg, SaveDataState eSaveDataState)
{
	if (!pMsg) return;

	// Save the state of the save data...

	pMsg->Writeuint8( eSaveDataState );

	// Save complex data members...

	GetInterfaceMgr( )->Save(pMsg);
	GetPlayerMgr()->Save(pMsg, eSaveDataState);

	m_Music.Save(pMsg);

	// Save all necessary data members...

	pMsg->WriteLTVector(m_vFlashColor);

	pMsg->Writeuint8(m_eDifficulty);
	pMsg->Writebool(m_bFlashScreen);

	pMsg->Writefloat(m_fFlashTime);
	pMsg->Writefloat(m_fFlashStart);
	pMsg->Writefloat(m_fFlashRampUp);
	pMsg->Writefloat(m_fFlashRampDown);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UnpackClientSaveMsg
//
//	PURPOSE:	Load all the necessary client-side info
//
// --------------------------------------------------------------------------- //

void CGameClientShell::UnpackClientSaveMsg(ILTMessage_Read *pMsg)
{
	if (!pMsg) return;

    m_bRestoringGame = true;

	// Get the state of the data we are about to load...

	SaveDataState eLoadDataState = static_cast<SaveDataState>(pMsg->Readuint8());

	// Load complex data members...

	GetInterfaceMgr( )->Load(pMsg);
	GetPlayerMgr()->Load(pMsg, eLoadDataState);
	
	m_Music.Load(pMsg);

	// Load data members...

    m_vFlashColor				= pMsg->ReadLTVector();

    m_eDifficulty               = (GameDifficulty) pMsg->Readuint8();
    m_bFlashScreen              = pMsg->Readbool();
    m_fFlashTime                = pMsg->Readfloat();
    m_fFlashStart               = pMsg->Readfloat();
    m_fFlashRampUp              = pMsg->Readfloat();
    m_fFlashRampDown            = pMsg->Readfloat();
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnModelKey
//
//	PURPOSE:	Handle weapon model keys
//
// --------------------------------------------------------------------------- //

void CGameClientShell::OnModelKey(HLOCALOBJ hObj, ArgList *pArgs)
{
	bool bResult;

	// give the client weapon mgr first shot at it
	bResult = g_pPlayerMgr->GetClientWeaponMgr()->OnModelKey( hObj, pArgs );

	// if it hasn't been handled...
	if ( !bResult )
	{
		// try the sfx mgr
		m_sfxMgr.OnModelKey(hObj, pArgs);
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::OnPlaySound
//
//	PURPOSE:	Handle a sound being played...
//
// --------------------------------------------------------------------------- //

void CGameClientShell::OnPlaySound(PlaySoundInfo* pPSI)
{
#ifdef USE_DX8_SOFTWARE_FILTERS
	if (!pPSI || !g_vtUseSoundFilters.GetFloat()) return;

	SOUNDFILTER* pFilter = g_pSoundFilterMgr->GetFilter((uint8)pPSI->m_UserData);
	if (!pFilter)
	{
		g_pLTClient->CPrint("ERROR in CGameClientShell::OnPlaySound()!");
		g_pLTClient->CPrint("  couldn't find filter (%d)", pPSI->m_UserData);
		return;
	}

	if (pFilter && pPSI->m_hSound)
	{
		// See if we need to calculate the filter dynamically...

		if (g_pSoundFilterMgr->IsDynamic(pFilter))
		{
			pFilter = GetDynamicSoundFilter();

			if (!pFilter)
			{
				g_pLTClient->CPrint("ERROR in CGameClientShell::OnPlaySound()!");
				g_pLTClient->CPrint("  couldn't find a dynamic filter (%s)", pFilter->szName);
				return;
			}
		}

		// Some sounds are unfiltered...

		if (g_pSoundFilterMgr->IsUnFiltered(pFilter)) return;


		// Set up the filter

		ILTClientSoundMgr *pSoundMgr = (ILTClientSoundMgr *)g_pLTClient->SoundMgr();

		pSoundMgr->SetSoundFilter(pPSI->m_hSound, pFilter->szFilterName);
		for (int i=0; i < pFilter->nNumVars; i++)
		{
			pSoundMgr->SetSoundFilterParam(pPSI->m_hSound, pFilter->szVars[i], pFilter->fValues[i]);
		}

		// TEMP, let us test what filter is being used...
		// g_pLTClient->CPrint("Using Filter: %s", pFilter->szName);
	}
	else
	{
		g_pLTClient->CPrint("ERROR in CGameClientShell::OnPlaySound()!");
		if (pFilter)
		{
			g_pLTClient->CPrint("  Invalid sound associated with Filter: %s", pFilter->szName);
		}
		else
		{
			g_pLTClient->CPrint("  Invalid filter associated with FilterId: %d", pPSI->m_UserData);
		}
	}
#endif
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::CreateBoundingBox
//
//	PURPOSE:	Create a box around the MoveMgr object
//
// --------------------------------------------------------------------------- //

void CGameClientShell::CreateBoundingBox()
{
	if (m_hBoundingBox) return;

	HLOCALOBJ hMoveMgrObj = g_pPlayerMgr->GetMoveMgr()->GetObject();
	if (!hMoveMgrObj) return;

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT(theStruct);

    LTVector vPos;
	g_pLTClient->GetObjectPos(hMoveMgrObj, &vPos);
	theStruct.m_Pos = vPos;

	SAFE_STRCPY(theStruct.m_Filename, "Models\\1x1_square.ltb");
	SAFE_STRCPY(theStruct.m_SkinName, "SpecialFX\\smoke.dtx");
	theStruct.m_ObjectType = OT_MODEL;
	theStruct.m_Flags = FLAG_VISIBLE;

    m_hBoundingBox = g_pLTClient->CreateObject(&theStruct);

	UpdateBoundingBox();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateBoundingBox()
//
//	PURPOSE:	Update the bounding box
//
// ----------------------------------------------------------------------- //

void CGameClientShell::UpdateBoundingBox()
{
	if (!m_hBoundingBox) return;

	HLOCALOBJ hMoveMgrObj = g_pPlayerMgr->GetMoveMgr()->GetObject();
	if (!hMoveMgrObj) return;

    LTVector vPos;
	g_pLTClient->GetObjectPos(hMoveMgrObj, &vPos);
	g_pLTClient->SetObjectPos(m_hBoundingBox, &vPos);

    LTVector vDims;
    g_pPhysicsLT->GetObjectDims(hMoveMgrObj, &vDims);

    LTVector vScale;
	VEC_DIVSCALAR(vScale, vDims, 0.5f);
    g_pLTClient->SetObjectScale(m_hBoundingBox, &vScale);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	DefaultModelHook
//
//	PURPOSE:	Default model hook function
//
// --------------------------------------------------------------------------- //

void DefaultModelHook (ModelHookData *pData, void *pUser)
{

	CGameClientShell* pShell = (CGameClientShell*) pUser;
	if (!pShell) return;

    uint32 nUserFlags = 0;
    g_pCommonLT->GetObjectFlags(pData->m_hObject, OFT_User, nUserFlags);

	// If we're using spy vision, turn off the glow on all models
	// that use the default model hook function...

	if (g_pPlayerMgr->UsingSpyVision())
	{
		pData->m_HookFlags |= MHF_NOGLOW;
	}

	if (nUserFlags & USRFLG_GLOW)
	{
		pData->m_LightAdd = g_pPlayerMgr->GetModelGlow();
		VEC_CLAMP((pData->m_LightAdd), 0.0f, 255.0f);
	}
	else if (nUserFlags & USRFLG_MODELADD)
	{
		// Get the new color out of the upper 3 bytes of the
		// user flags...

        float r = (float)(nUserFlags>>24);
        float g = (float)(nUserFlags>>16);
        float b = (float)(nUserFlags>>8);

		VEC_SET (pData->m_LightAdd, r, g, b);
		VEC_CLAMP((pData->m_LightAdd), 0.0f, 255.0f);
	}
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	HookedWindowProc
//
//	PURPOSE:	Hook it real good
//
// --------------------------------------------------------------------------- //

LRESULT CALLBACK HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//[yaiin] : ÇØ¼® : g_pImeCtrl¿¡¼­ ¿¡µðÆ® ¹Ú½º °ü·Ã Çàµ¿À» ¹Ì¸® ÈÉÃÄ Ã³¸®ÇÔ.
	if( g_pImeCtrl && g_pImeCtrl->WndProc( hWnd, uMsg, wParam, lParam ) ) return 0L;
	
	switch(uMsg)
	{
	case WM_NETWORK :
#ifndef _STANDALONE
		g_Network_HandleMsg( wParam, lParam );
#endif
		break;
		
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, CGameClientShell::OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, CGameClientShell::OnLButtonUp);
		HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK, CGameClientShell::OnLButtonDblClick);
		HANDLE_MSG(hWnd, WM_RBUTTONUP, CGameClientShell::OnRButtonUp);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, CGameClientShell::OnRButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONDBLCLK, CGameClientShell::OnRButtonDblClick);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, CGameClientShell::OnMouseMove);

	case WM_CHAR:
		CGameClientShell::OnChar( hWnd, wParam, lParam );
		break;
		
	case WM_MOUSEWHEEL:
		CGameClientShell::OnMouseWheel(hWnd, (int)LOWORD(lParam), (int)HIWORD(lParam), (int)HIWORD(wParam));
		break;
	}
	_ASSERT(g_pfnMainWndProc);
	return(CallWindowProc(g_pfnMainWndProc,hWnd,uMsg,wParam,lParam));
}

void CGameClientShell::OnChar( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	g_pInterfaceMgr->OnChar((unsigned char)wParam, (uint32)lParam);
}


void CGameClientShell::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	g_pInterfaceMgr->OnLButtonUp(x,y);
}

void CGameClientShell::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	/*if (!g_tmrDblClick.Stopped() &&
		(g_mouseMgr.GetClickPosX() == x) && (g_mouseMgr.GetClickPosY() == y))
	{
		g_tmrDblClick.Stop();
		OnLButtonDblClick(hwnd,fDoubleClick,x,y,keyFlags);
	}
	else
	{
		g_mouseMgr.SetClickPos(x,y);
		g_tmrDblClick.Start(.5);
	}*/

#ifndef _FINAL
	if( g_bRunWindowMode )
	{
		RECT WinRect;
		POINT MousePos;
		GetCursorPos( &MousePos );
		
		GetWindowRect( g_hMainWnd, &WinRect );
		WinRect.bottom	= WinRect.top + 20;
		WinRect.right	= WinRect.left + 200;

		if( PtInRect( &WinRect, MousePos ) )
		{
			PostMessage( g_hMainWnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL );
		}
	}
#endif	//#ifndef _FINAL
	g_pInterfaceMgr->OnLButtonDown(x,y);
}

void CGameClientShell::OnLButtonDblClick(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	g_pInterfaceMgr->OnLButtonDblClick(x,y);
}

void CGameClientShell::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	g_pInterfaceMgr->OnRButtonUp(x,y);
}

void CGameClientShell::OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	g_pInterfaceMgr->OnRButtonDown(x,y);
}

void CGameClientShell::OnRButtonDblClick(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	g_pInterfaceMgr->OnRButtonDblClick(x,y);
}

void CGameClientShell::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
//	g_mouseMgr.SetMousePos(x,y);
//	g_pLTClient->CPrint("X:%d Y:%d", x,y);
	g_pInterfaceMgr->OnMouseMove(x,y);
}

void CGameClientShell::OnMouseWheel(HWND hwnd, int x, int y, UINT Partition)
{
	if(Partition == 120)
		OnWheelUp(hwnd, x, y, 1);
	else
		OnWheelDown(hwnd, x, y, 1);
}
void CGameClientShell::OnWheelUp(HWND hwnd, int x, int y, UINT UpVelue)
{
	g_pInterfaceMgr->OnWheelUp(x,y);
}

void CGameClientShell::OnWheelDown(HWND hwnd, int x, int y, UINT DownVelue)
{
	g_pInterfaceMgr->OnWheelDown(x,y);
}

BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	return TRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	HookWindow
//
//	PURPOSE:	HOOK IT!
//
// --------------------------------------------------------------------------- //

BOOL HookWindow()
{
	// Hook the window
    if(g_pLTClient->GetEngineHook("HWND",(void **)&g_hMainWnd) != LT_OK)
	{
		TRACE("HookWindow - ERROR - could not get the engine window!\n");
		return FALSE;
	}

	// Get the window procedure
#ifdef STRICT
	g_pfnMainWndProc = (WNDPROC)GetWindowLong(g_hMainWnd,GWL_WNDPROC);
#else
	g_pfnMainWndProc = (FARPROC)GetWindowLong(g_hMainWnd,GWL_WNDPROC);
#endif

	if(!g_pfnMainWndProc)
	{
		TRACE("HookWindow - ERROR - could not get the window procedure from the engine window!\n");
		return FALSE;
	}

	// Replace it with ours
	if(!SetWindowLong(g_hMainWnd,GWL_WNDPROC,(LONG)HookedWindowProc))
	{
		TRACE("HookWindow - ERROR - could not set the window procedure!\n");
		return FALSE;
	}

	return TRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	UnhookWindow
//
//	PURPOSE:	Unhook the window
//
// --------------------------------------------------------------------------- //

void UnhookWindow()
{
	if(g_pfnMainWndProc && g_hMainWnd)
	{
		SetWindowLong(g_hMainWnd, GWL_WNDPROC, (LONG)g_pfnMainWndProc);
		g_hMainWnd = 0;
		g_pfnMainWndProc = NULL;
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::SetDifficulty
//
//	PURPOSE:	Dynamically change our difficulty level
//
// --------------------------------------------------------------------------- //

void CGameClientShell::SetDifficulty(GameDifficulty e)
{
	m_eDifficulty = e;
	
	//check to see if we've got a server
	if (g_pLTClient->IsConnected())
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_DIFFICULTY);
		cMsg.Writeuint8(m_eDifficulty);
		g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleWeaponPickup()
//
//	PURPOSE:	Handle picking up weapon
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleWeaponPickup(uint8 nWeaponID, uint32 nAmmoCount, uint8 nAmmoInClip)
{
	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(nWeaponID);
	if( !pWeapon || !pWeapon->aAmmoIds ) return;
	g_pPlayerStats->UpdateAmmo( nWeaponID, pWeapon->aAmmoIds[0], nAmmoCount, LTTRUE );

	//[MURSUM] ¼­¹ö¿¡¼­ ¹ÞÀº AmmoInClip·Î ¼ÂÆÃ
	IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetClientWeaponMgr()->GetWeaponFromID(nWeaponID);
	if( pClientWeapon ) pClientWeapon->SetAmmoInClip( nAmmoInClip );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleGearPickup()
//
//	PURPOSE:	Handle picking up gear
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleGearPickup(uint8 nGearId, bool bSuccess /* = true */)
{
	GEAR const *pGear = g_pWeaponMgr->GetGear(nGearId);
	if (!pGear) return;

	int nNameId = pGear->nNameId;
	if (!nNameId) return;

	char szName[64];
	LoadString(nNameId,szName,sizeof(szName));
	if (!strlen(szName)) return;

	char szMsg[128] = "";
	if (bSuccess)
	{
		FormatString(IDS_GEARPICKUP,szMsg,sizeof(szMsg),szName);
	}
	else if( IsCoopMultiplayerGameType( ))
	{
		FormatString(IDS_GEAR_NOPICKUP,szMsg,sizeof(szMsg),szName);

		// Play the sound bute NoPickupSound
		g_pClientSoundMgr->PlaySoundLocal("NoPickupSound", SOUNDPRIORITY_PLAYER_HIGH);
	}

	if (szMsg[0])
	{
		//g_pPickupMsgs->AddMessage(szMsg,pGear->szIcon);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleModPickup()
//
//	PURPOSE:	Handle picking up mod
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleModPickup(uint8 nModId, bool bSuccess /* = true */)
{
	MOD const *pMod = g_pWeaponMgr->GetMod(nModId);
	if (!pMod) return;

	int nNameId = pMod->nNameId;
	if (!nNameId) return;

	char szName[64];
	LoadString(nNameId,szName,sizeof(szName));
	if (!strlen(szName)) return;
    
	char szMsg[128] = "";
	if (bSuccess)
	{
		FormatString(IDS_MODPICKUP,szMsg,sizeof(szMsg),szName);
	}
	else if( IsCoopMultiplayerGameType( ))
	{
		FormatString(IDS_MOD_NOPICKUP,szMsg,sizeof(szMsg),szName);

		// Play the sound bute NoPickupSound
		g_pClientSoundMgr->PlaySoundLocal("NoPickupSound", SOUNDPRIORITY_PLAYER_HIGH);
	}

	if (szMsg[0])
	{
		//g_pPickupMsgs->AddMessage(szMsg,pMod->szIcon);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::HandleAmmoPickup()
//
//	PURPOSE:	Handle picking up ammo
//
// ----------------------------------------------------------------------- //

void CGameClientShell::HandleAmmoPickup(uint8 nAmmoId, int nAmmoCount, 
										bool bSuccess /* = true */,
										uint8 nWeaponId /* = WMGR_INVALID_ID */)
{
	AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(nAmmoId);
	if (!pAmmo) return;

	if (pAmmo->nSelectionAmount >= 1000)
		return;

	std::string icon = pAmmo->GetNormalIcon();
	if( nWeaponId != WMGR_INVALID_ID )
	{
		g_pWpnChooser->AddWeapon( g_pWeaponMgr->GetWeaponClass(nWeaponId)-1, nWeaponId, LTFALSE, LTTRUE );
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::SetFarZ
//
//	PURPOSE:	Localize setting the far z (cause it can screw things up
//				when set to 0)
//
// --------------------------------------------------------------------------- //

void CGameClientShell::SetFarZ(int nFarZ)
{
	// Don't EVER set the farZ really close!
	if (nFarZ > 50)
	{
		WriteConsoleInt("FarZ", nFarZ);
	}
}


void CGameClientShell::OnLockRenderer()
{
	if (GetInterfaceMgr()->IsLoadScreenVisible())
	{
		GetInterfaceMgr()->PauseLoadScreen();
	}
}

void CGameClientShell::OnUnLockRenderer()
{
	if (GetInterfaceMgr()->IsLoadScreenVisible())
	{
		GetInterfaceMgr()->ResumeLoadScreen();
	}
}

void CGameClientShell::SendPingToGameServer()
{
	g_Network.SendPing();
}

void CGameClientShell::PostLevelLoadFirstUpdate()
{
	// Draw the screen once
	g_pPlayerMgr->UpdateCamera();
	RenderCamera(false);
	
	// Restore the game's music state.  (This can take a while...)
	RestoreMusic();
}

void CGameClientShell::OnScreenShotNotify( const char* lpszFileName )
{
	if( !lpszFileName || !g_pInterfaceMgr ) return;

	char	szNoti[512];
	sprintf( szNoti, "%s°¡ ¹ÙÅÁÈ­¸é¿¡ ÀúÀåµÇ¾ú½À´Ï´Ù.", lpszFileName );

	if( g_pInterfaceMgr->GetGameState() == GS_PLAYING )
	{
		g_pChatMsgs->AddMessage( szNoti, kMsgNotice );
	}
	else if( g_pInterfaceMgr->GetGameState() == GS_SCREEN )
	{
		if( g_pInterfaceMgr->GetCurrentScreen() == SCREEN_ID_MAIN )
		{
			g_pScreenMain->AddChat( szNoti, CHAT_TYPE_NOTICE );
		}
		else if( g_pInterfaceMgr->GetCurrentScreen() == SCREEN_ID_SCREEN_ROOM )
		{
			g_pScreenRoom->AddChat( szNoti, CHAT_TYPE_NOTICE );
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::UpdateSpeedMonitor()
//
//	PURPOSE:	Handle updating the server speed monitor...
//
// ----------------------------------------------------------------------- //

void CGameClientShell::UpdateSpeedMonitor()
{
	LTFLOAT fLocalTime = g_pLTClient->GetTime();
	LTFLOAT fServerTime = g_pLTClient->GetGameTime();
	if(fServerTime - m_fInitialServerTime > 5.0f)
	{
		// time to do our check
		LTFLOAT fServerDelta	= fServerTime - m_fInitialServerTime;
		LTFLOAT fLocalDelta		= fLocalTime - m_fInitialLocalTime;

		if(fServerDelta / fLocalDelta < 0.98)
		{
			// possible cheater, increment cheat counter
			m_nSpeedCheatCounter++;

			if(m_nSpeedCheatCounter > 24)
			{
				// Disconnect from the server.
				if(g_pLTClient->IsConnected())
				{
					g_pLTClient->Disconnect();
				}
			}
		}
		else
		{
			// reset the instance counter
			m_nSpeedCheatCounter = 0;
		}

		// reset the time counters
		m_fInitialServerTime	= fServerTime;
		m_fInitialLocalTime		= fLocalTime;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::SendClientLoadedMessage
//
//	PURPOSE:	Sent when client done with loading and postloading screens.
//
// ----------------------------------------------------------------------- //

void CGameClientShell::SendClientLoadedMessage( )
{
	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_CLIENT_LOADED);
	g_pLTClient->SendToServer(cMsg.Read(),MESSAGE_GUARANTEED);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameClientShell::LauncherServerApp()
//
//	PURPOSE:	Launches the serverapp.
//
// --------------------------------------------------------------------------- //

bool CGameClientShell::LauncherServerApp( char const* pszProfileFile )
{
	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;
	RMode rMode;

	// Save the current render mode.  We'll need to restore it if the serverapp
	// launching fails.
	g_pLTClient->GetRenderMode( &rMode );

	// Shutdown the renderer, minimize it, and hide it...
	g_pLTClient->ShutdownRender( 0 ); // RSHUTDOWN_MINIMIZEWINDOW | RSHUTDOWN_HIDEWINDOW );

	// Initialize the startup info.
	memset( &startInfo, 0, sizeof( STARTUPINFO ));
	startInfo.cb = sizeof( STARTUPINFO );

	// Setup the command line.
	std::string sCmdLine = "NOLF2Srv.exe -profile ";
	
	// Enclose the profile name in quotes since we allow spaces in the name...

	sCmdLine += "\"";
	sCmdLine += pszProfileFile;
	sCmdLine += "\"";

	// Start the server app.
	if( !CreateProcess( "NOLF2Srv.exe", ( char * )sCmdLine.c_str( ), NULL, NULL, FALSE, 0, NULL, NULL, 
		&startInfo, &procInfo ))
	{
		// Serverapp failed.  Restore the render mode.
		g_pLTClient->SetRenderMode( &rMode );
		return false;
	}

	// We're done with this process.
	g_pLTClient->Shutdown();

	return true;
}

void CGameClientShell::SetGameType(GameType eGameType)	
{
	if (m_eGameType == eGameType) return;

	switch( eGameType )
	{
		// Single player and coop use the same weapon files...

		case eGameTypeSingle:
		case eGameTypeCooperative:
		{
			g_pWeaponMgr->LoadOverrideButes( WEAPON_DEFAULT_FILE );
		}
		break;

		case eGameTypeDeathmatch:
		case eGameTypeTeamDeathmatch:
		case eGameTypeDoomsDay:
		case eGameTypeSurvival :
		{
			//g_pWeaponMgr->LoadOverrideButes( WEAPON_DEFAULT_MULTI_FILE );
			g_pWeaponMgr->LoadOverrideButes( WEAPON_DEFAULT_FILE );
		}
		break;
		
	}
	m_eGameType = eGameType;
}

//[MURSUM]====================================================
//°¨¸¶¸¦ º¯°æÇÏ´Â ÇÔ¼ö
void CGameClientShell::SetGammaRamp( float fGamma )
{
	SetGammaRamp( fGamma, fGamma, fGamma );
}

void CGameClientShell::SetGammaRamp( float fGammaR, float fGammaG, float fGammaB )
{
	uint16	nRamp[768]; //3*256
	float	fInvGammaR	= 1.0f/fGammaR;
	float	fInvGammaG	= 1.0f/fGammaG;
	float	fInvGammaB	= 1.0f/fGammaB;
    	
	for( uint32 nCurEntry=0 ; nCurEntry<256 ; ++nCurEntry )
	{
		nRamp[nCurEntry] = (uint16)(65535.0f * powf((float)nCurEntry / 255.0f, fInvGammaR));		//* fContrast;
		nRamp[nCurEntry+256] = (uint16)(65535.0f * powf((float)nCurEntry / 255.0f, fInvGammaG));	//* fContrast;
		nRamp[nCurEntry+512] = (uint16)(65535.0f * powf((float)nCurEntry / 255.0f, fInvGammaB));	//* fContrast;
    }	
	
    SetDeviceGammaRamp( GetDC(LTNULL), nRamp );
}

void CGameClientShell::DrawLogText( const char* lpszText )
{
	if( !m_pLogText ) return;

	m_pLogText->DrawD3DText( lpszText, strlen(lpszText), DT_LEFT, D3D_TEXT_OUTLINE, false );

	m_pLogText->Render();
}
//·Î±×Âï´Â ÇÔ¼ö

#ifndef _FINAL
//=================================================================
void CGameClientShell::CreateSphere( LTVector vPos, float fR, float fG, float fB )
{
	ObjectCreateStruct ocs;
	LTVector vScale;

	vScale.x = 10.0f;
	vScale.y = 10.0f;
	vScale.z = 10.0f;
	ocs.m_ObjectType = OT_MODEL;
	ocs.m_Flags	= FLAG_VISIBLE;
	ocs.m_Pos = vPos;
	ocs.m_Scale = vScale;
	strcpy(ocs.m_Filename, "GLOBAL\\MODELS\\sphere.ltb");

	g_pLTClient->SetObjectColor( g_pLTClient->CreateObject(&ocs), fR, fG, fB, 1.0f );	
}
#endif

// ±¤½Ä
void CGameClientShell::LogoInitialize(uint32 nWidth, uint32 nHeight)
{
	RECT screenRect;
	GetWindowRect(GetDesktopWindow(), &screenRect);

	m_rLogoRect[0].Init(  0,  0,512,512);
	m_rLogoRect[1].Init(512,  0,256,512);
	m_rLogoRect[2].Init(768,  0, 32,512);
	m_rLogoRect[3].Init(  0,512,512, 64);
	m_rLogoRect[4].Init(512,512,256, 64);
	m_rLogoRect[5].Init(768,512, 32, 64);
	m_rLogoRect[6].Init(  0,576,512, 32);
	m_rLogoRect[7].Init(512,576,256, 32);
	m_rLogoRect[8].Init(768,576, 32, 32);
	
	m_hLogoFrame[0] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX0);
	m_hLogoFrame[1] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX1);
	m_hLogoFrame[2] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX2);
	m_hLogoFrame[3] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX3);
	m_hLogoFrame[4] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX4);
	m_hLogoFrame[5] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX5);
	m_hLogoFrame[6] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX6);
	m_hLogoFrame[7] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX7);
	m_hLogoFrame[8] = g_pInterfaceResMgr->GetTexture(LOGO_LOADING_TX8);
	
	//[SPIKE] ¼öÁ¤ 
	for(int i=0; i<STARTLOADING_PARTITIONCOUNT; i++)
	{	
		if( m_hLogoFrame[i] )
		{
			SetupQuadUVs(m_pLogoPoly[i], m_hLogoFrame[i], 0.0f, 0.0f, 1.0f, 1.0f);
			g_pDrawPrim->SetRGBA(&m_pLogoPoly[i], argbWhite);
	
			g_pDrawPrim->SetXYWH(&m_pLogoPoly[i], (float)m_rLogoRect[i].left, (float)m_rLogoRect[i].top, (float)m_rLogoRect[i].right, (float)m_rLogoRect[i].bottom);
		}
	}

	g_pDrawPrim->SetRGBA( &m_PolyAge, 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_PolyAge, USABLE_WIDTH_F-37.0f, 5.0f, 32.0f, 32.0f );
	g_pDrawPrim->SetUVWH( &m_PolyAge, 0.0f, 0.0f, 1.0f, 1.0f );
}

void CGameClientShell::LogoRender()
{
	//g_pDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	//g_pDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
	//g_pDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
	//g_pDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	//g_pDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);

	for(int i=0; i<STARTLOADING_PARTITIONCOUNT; i++)
	{
		if(m_hLogoFrame[i])
		{
			g_pDrawPrim->SetTexture(m_hLogoFrame[i]);
			g_pDrawPrim->DrawPrim(&m_pLogoPoly[i]);
		}
	}

	if( m_hAgeMark )
	{
		g_pDrawPrim->SetTexture( m_hAgeMark );
		g_pDrawPrim->DrawPrim( &m_PolyAge );
	}

	/**/
	if( m_p18Notice )
	{
		m_p18Notice->Render();
	}		
}

void CGameClientShell::ScreenRender(const char* lpszText)
{
	g_pLTClient->ClearScreen(LTNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);	
	g_pLTClient->Start3D();	
	g_pLTClient->StartOptimized2D();

	LogoRender();

	DrawLogText(lpszText);

	g_pLTClient->EndOptimized2D();
	g_pLTClient->End3D();
    g_pLTClient->FlipScreen(0);
}

void CGameClientShell::HandleConeType( uint32 nConeType )
{
#ifdef __VIRTUAL_IP__
	if( nConeType & UTC_FULL_CONE )
	{
		g_Network.CS_HostInfo( UTC_FULL_CONE, g_pLTClient->GetHostPort() );
	}
	else if( nConeType & UTC_RESTRICTED_CONE )
	{
		g_Network.CS_HostInfo( UTC_RESTRICTED_CONE, g_pLTClient->GetHostPort() );
	}
	else if( nConeType & UTC_PORT_RESTRICTED_CONE )
	{
		g_Network.CS_HostInfo( UTC_PORT_RESTRICTED_CONE, g_pLTClient->GetHostPort() );
	}
#endif
}

void CGameClientShell::OnConnectFail( LTBOOL bReturn, void *pData )
{
	if( g_pLTClient ) g_pLTClient->Shutdown();
}

void CGameClientShell::CloneCharacter()
{
#ifdef _DEBUG
	if( g_pMoveMgr )
	{
		CCharacterFX* pTempChar = g_pMoveMgr->GetCharacterFX();
		if( pTempChar )	pTempChar->CreateTestModelFX();
	}
#endif
}

void CGameClientShell::SetFocusGame()
{
	if( g_hMainWnd )
	{
		::ShowWindow( g_hMainWnd, SW_SHOWNORMAL );
		::SetFocus( g_hMainWnd );
		::SetForegroundWindow( g_hMainWnd );
	}	
}

void CGameClientShell::OnHackDetected()
{
	if( g_pInterfaceMgr->GetGameState() == GS_PLAYING )
	{
		g_Network.CS_HackDetected();
	}

	g_Network.Close();
}

void CGameClientShell::GetLocalHWInfo() 
{
	char szInfo[1024];
	
	g_GameDoc.BeginClientHWInfo();

	CSystemInfo si;
	memset( szInfo, 0, 1024 );
	si.GetCPUDesc( szInfo );	
	g_pLTClient->CPrint( "CPU\t\t\t: %s", szInfo );

	g_GameDoc.AddClientHWInfo( "cpu=" );
	g_GameDoc.AddClientHWInfo( szInfo );
	g_GameDoc.AddClientHWInfo( "|" );

	memset( szInfo, 0, 1024 );
	si.GetOS( szInfo );
	g_pLTClient->CPrint( "Operating System\t: %s", szInfo );

	g_GameDoc.AddClientHWInfo( "os=" );
	g_GameDoc.AddClientHWInfo( szInfo );
	g_GameDoc.AddClientHWInfo( "|" );

	MEMORYSTATUS memory;
	memory.dwLength = sizeof(memory); 
	GlobalMemoryStatus(&memory);
	g_pLTClient->CPrint( "SYSTEM MEMORY\t\t: %dMB / %dMB",
						 memory.dwAvailPhys/1048576,
						 memory.dwTotalPhys/1048576 );

	sprintf( szInfo, "%d", memory.dwTotalPhys/1048576 );
	g_GameDoc.AddClientHWInfo( "ram=" );
	g_GameDoc.AddClientHWInfo( szInfo );
	g_GameDoc.AddClientHWInfo( "|" );

	memset( szInfo, 0, 1024 );
	si.GetVgaFullInfo( szInfo, "DISPLAY" );
	g_pLTClient->CPrint( "Graphic Device\t\t: %s", szInfo );

	if( stricmp( szInfo, "none" ) == 0 )
	{
		g_bNoneVideoDevice = LTTRUE;
	}
	else
	{
		g_bNoneVideoDevice = LTFALSE;
	}

	g_GameDoc.AddClientHWInfo( "graphic=" );
	g_GameDoc.AddClientHWInfo( szInfo );
	

	uint32 iVideoMem = GetGlobalClientMgr()->GetVideoMemorySize();
	g_pLTClient->CPrint( "VIDEO MEMORY\t\t: %dMB", iVideoMem/1048576 );
	sprintf( szInfo, "%d", iVideoMem/1048576 );
	g_GameDoc.AddClientHWInfo( " VidMem: " );
	g_GameDoc.AddClientHWInfo( szInfo );
	g_GameDoc.AddClientHWInfo( "|" );

	memset( szInfo, 0, 1024 );
	si.GetVgaFullInfo( szInfo, "MEDIA" );
	g_pLTClient->CPrint( "Sound Device\t\t: %s", szInfo );
	g_GameDoc.AddClientHWInfo( "sound=" );
	g_GameDoc.AddClientHWInfo( szInfo );
	g_GameDoc.AddClientHWInfo( "|" );

	char szVersion[32];
	char szDXVer[32];
	HKEY  key;
	DWORD dwDisp;
	unsigned long	nSize = 32;
	if( RegCreateKeyEx( HKEY_LOCAL_MACHINE, "Software\\Microsoft\\DirectX\\", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL,
		&key, &dwDisp )	== ERROR_SUCCESS )
	{
		if( RegQueryValueEx( key, "Version", 0, NULL, (LPBYTE)szVersion, &nSize ) == ERROR_SUCCESS )
		{
			szDXVer[0] = szVersion[3];
			szDXVer[1] = szVersion[4];
			szDXVer[2] = szVersion[5];
			szDXVer[3] = szVersion[6];
			szDXVer[4] = NULL;	
			g_pLTClient->CPrint( "DirectX Version\t\t: %s", szDXVer );	
		}			
	}
	RegCloseKey( key );

	g_GameDoc.AddClientHWInfo( "directx=" );
	g_GameDoc.AddClientHWInfo( szDXVer );
	g_GameDoc.AddClientHWInfo( "|" );

	g_GameDoc.EndClientHWInfo();

	const char* pp = g_GameDoc.GetClientHWInfo();
}

void CGameClientShell::PreLoadFXRes()
{
	ObjectCreateStruct ocs;
	ocs.m_ObjectType		= OT_SPRITE;

// Surface
	// Metal 
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal00.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal01.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal02.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal10.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal11.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal12.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal20.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal21.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Surface_Metal22.spr" );
	g_pLTClient->CreateObject( &ocs );

	// ETC (Wood, Glass, Asphalt...)
	strcpy( ocs.m_Filename, "SA_SPRITES\\smoke1.spr" );
	g_pLTClient->CreateObject( &ocs );


// Blood
	strcpy( ocs.m_Filename, "FX\\Test\\Blood\\Spr\\BloodL1.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Test\\Blood\\Spr\\BloodL2.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Test\\Blood\\Spr\\BloodL3.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Test\\Blood\\Spr\\BloodL4.spr" );
	g_pLTClient->CreateObject( &ocs );

// 1,3ÀÎÄª Muzzle
	// °øÅë 
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz_Center.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz_Side2.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz_Side3.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz_Side5.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz_Side6.spr" );
	g_pLTClient->CreateObject( &ocs );
	// ±ÇÃÑ 
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Pistol_HH_Muzz0.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Pistol_HH_Muzz1.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Pistol_HH_Muzz2.spr" );
	g_pLTClient->CreateObject( &ocs );
	// ¶óÀÌÇÃ 
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz1.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_Rifle_HH_Muzz2.spr" );
	g_pLTClient->CreateObject( &ocs );
	// ¼¦°Ç 
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_ShotGun_HH_Muzz0.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_ShotGun_HH_Muzz1.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\SA_ShotGun_HH_Muzz2.spr" );
	g_pLTClient->CreateObject( &ocs );
	// 1ÀÎÄª 
	strcpy( ocs.m_Filename, "SA_SPRITES\\WEAPON\\PV\\SA_PV_MFX_P90.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "SA_SPRITES\\WEAPON\\PV\\SA_PV_MFX_PYTHON.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "SA_SPRITES\\WEAPON\\PV\\SA_PV_MFX_SPAS.spr" );
	g_pLTClient->CreateObject( &ocs );

// Explode °ü·Ã 
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\TimeBomb_Impact.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\Exp_Basic.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\LEN00.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\LEN01.spr" );
	g_pLTClient->CreateObject( &ocs );
	strcpy( ocs.m_Filename, "FX\\Effect_Sprite\\bombmark001.spr" );
	g_pLTClient->CreateObject( &ocs );
}