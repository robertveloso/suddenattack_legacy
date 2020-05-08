//------------------------------------------------------------------
//
//	FILE	  : ClientMgr.cpp
//
//	PURPOSE	  : This is the Mgr for the client side DirectEngine
//              libraries.
//
//	CREATED	  : November 7 1996
//
//	COPYRIGHT : LithTech Inc., 1996-2000
//
//------------------------------------------------------------------

// Includes....
#ifdef LITHTECH_ESD
#include "ltrealaudio_impl.h"
#include "ltrealconsole_impl.h"
#include "ltrealvideo_impl.h"
#endif // LITHTECH_ESD

#include "bdefs.h"

    #include "build_options.h"

    #include "renderstruct.h"
    #include "render.h"
    #include "musicdriver.h"
    #include "ltdirectmusic_impl.h"
    #include "shellbind.h"
#endif

#include "soundmgr.h"
#include "clientmgr.h"
#include "consolecommands.h"
#include "console.h"
#include "memorywatch.h"
#include "clientde_impl.h"
#include "sysinput.h"
#include "errorlog.h"
#include "iclientshell.h"
#include "sysvideo.h"
#include "mpm.h"
#include "bindmgr.h"
#include "clientshell.h"
#include "servermgr.h"
#include "systimer.h"
#include "sysperfcounter.h"
#include "iltclient.h"
#include "ltbenchmark_impl.h"
#include "sprite.h"
#include "cmoveabstract.h"
#include "clmessagehelper.h"
#include "engine_surfaceeffects.h"
#include "stringmgr.h"
#include "debuggeometry.h"
#include "setupobject.h"
#include "systimer.h"


//------------------------------------------------------------------
//------------------------------------------------------------------
// Holders and their headers.
//------------------------------------------------------------------
//------------------------------------------------------------------

//pointer to the client loader thread module
#include "client_loaderthread.h"
#include "cloaderthread.h"
static IClientLoaderThread *loader_thread;
define_holder(IClientLoaderThread, loader_thread);

//pointer to the client debug graph mgr module.
#include "client_graphmgr.h"
#include "debuggraphmgr.h"
static IClientDebugGraphMgr *graph_mgr;
define_holder(IClientDebugGraphMgr, graph_mgr);

//IWorldClientBSP holder
#include "world_client_bsp.h"
#include "de_mainworld.h"
static IWorldClientBSP *world_client;
define_holder(IWorldClientBSP, world_client);

//ICommandLineArgs holder
#include "icommandlineargs.h"
static ICommandLineArgs *command_line_args;
define_holder(ICommandLineArgs, command_line_args);

//------------------------------------------------------------------
//------------------------------------------------------------------
// Console stuff
//------------------------------------------------------------------
//------------------------------------------------------------------

extern int32	g_CV_BitDepth;
extern int32	g_ClientSleepMS;
extern int32	g_bShowMemStats;
extern LTBOOL		g_bInputOn;
extern LTBOOL		g_bSoundOn;
extern int32	g_nConsoleLines;
extern int32 g_CV_ShowFrameRate;
extern int32 g_ScreenWidth, g_ScreenHeight;
extern int32 g_CV_FullLightScale;
extern LTVector	g_ConsoleModelAdd, g_ConsoleModelDirAdd;
extern float g_FarZ;
extern float g_CV_MaxFPS;
extern int32 g_CV_ForceConsole;
extern int32 g_CV_RenderEnable;
extern int32 g_CV_ForceSoundDisable;
extern int32 g_CV_DrawDebugGeometry;
extern int32 g_CV_PerfCounterDisplay;

#ifdef LITHTECH_ESD
ILTRealAudioPlayer* g_pRealAudioPlayer;
ILTRealVideoPlayer* g_pRealVideoPlayer;
#endif // LITHTECH_ESD

// PlayDemo profile info.
uint32 g_PD_Music=0;
uint32 g_PD_Sound=0;
uint32 g_PD_Input=0;
uint32 g_PD_ClientShell=0;
uint32 g_PD_Render=0;

ObjectBank<LTLink> g_DLinkBank(64, 1024);

CClientMgr *g_pClientMgr = LTNULL;

CSoundMgr *GetSoundMgr( ) { return g_pClientMgr->GetSoundMgr( ); }
SMusicMgr *GetMusicMgr( ) { return g_pClientMgr->GetMusicMgr( ); }

extern LTBOOL g_bMusicEnable;
extern LTBOOL g_bSoundEnable;
extern LTBOOL g_bDebugSound, g_bDebugNumSounds;
extern LTBOOL g_bNullRender;

extern ILTCursor *CreateCursorInterface();

// ----------------------------------------------------------------------- //
// CClientMgr functions.
// ----------------------------------------------------------------------- //

void _GetRModeFromConsoleVariables(RMode *pMode)
{
	LTCommandVar *pVar;

	// Init the renderer.
	memset(pMode, 0, sizeof(*pMode));
	pMode->m_Width = g_ScreenWidth;
	pMode->m_Height = g_ScreenHeight;
	pMode->m_BitDepth = g_CV_BitDepth;

	// Use a specific card description if they want.
	pVar = cc_FindConsoleVar(&g_ClientConsoleState, "carddesc");
	if(pVar && pVar->pStringVal)
	{
		strncpy(pMode->m_InternalName, pVar->pStringVal, sizeof(pMode->m_InternalName)-1);
	}
}


void MaybeDrawConsole()
{
	RenderStruct *pStruct;

	if(g_CV_ForceConsole)
	{
		pStruct = r_GetRenderStruct();
		if(pStruct->m_bInitted && g_CV_ForceConsole)
		{
			con_Draw();
			pStruct->SwapBuffers();
		}
	}
}


//
// CClientMgr
//

// multi-protocol manager interface
CLTMultiProtocolMgrClient * CClientMgr::m_pMPM;

CClientMgr::CClientMgr()
{
	// force CLTMultiProtocolMgrClient interface instantiation at runtime
	// note: this will instantiate the MPM interface for CClientMgr _and_ CServerMgr
	CLTMultiProtocolMgrClient::InitMultiProtocolSubsystem(); // ensures proper linkage

	// multi-protocol manager interface
	define_holder(CLTMultiProtocolMgrClient, m_pMPM);
}

//CLTMultiProtocolMgrClient x;

LTRESULT CClientMgr::Init(const char *resTrees[MAX_RESTREES], uint32 nResTrees, const char *pConfigFileName)
{
	const char *pDLLPath;
	char cmd[64];
	TreeType treeTypes[MAX_RESTREES];
	int nTreesLoaded;
	LTRESULT dResult;
	RMode defaultMode;
	char versionStr[32];
	LTRect rGraphMgr;

	r_InitRenderStruct(LTTRUE);

	mw_ResetWatches( );


	m_LastUpdateRate = DEFAULT_CLIENT_UPDATE_RATE;
	m_bInputState = LTTRUE;
	m_bTrackingInputDevices = LTFALSE;
	m_CurTextureFrameCode = 0; // (It gets incremented before it's returned from IncCurObjectFrameCode).


	world_client->ClientTree()->Init(&m_ObjectMgr);

	// Init our ClientDE interface..
	m_pClientDE = CreateLTClient();


	// Setup some default data.
	m_AxisOffsets[0] = m_AxisOffsets[1] = m_AxisOffsets[2] = 0.0f;
	memset(m_Commands, 0, sizeof(m_Commands));
	m_iCurInputSlot = 0;


	// Add files to the file manager.
	if(nResTrees > MAX_RESTREES)
		nResTrees = MAX_RESTREES;

	if(nResTrees == 0)
	{
		SetupError(LT_NOGAMERESOURCES);
		ProcessError(LT_NOGAMERESOURCES | ERROR_SHUTDOWN);
		RETURN_ERROR(1, CClientMgr::Init, LT_NOGAMERESOURCES);
	}

	memcpy(m_ResTrees, resTrees, sizeof(m_ResTrees));
	m_nResTrees = nResTrees;

	cf_AddResourceTrees(m_hFileMgr, resTrees, nResTrees, treeTypes, &nTreesLoaded);
	if(nTreesLoaded == 0)
	{
		SetupError(LT_CANTLOADGAMERESOURCES, resTrees[0]);
		ProcessError(LT_CANTLOADGAMERESOURCES);
		RETURN_ERROR(1, CClientMgr::Init, LT_CANTLOADGAMERESOURCES);
	}

	// The path to the DLLs is either the dos tree or LTNULL, in which case
	// it'll check the registry.
	pDLLPath = LTNULL;
	if(treeTypes[0] == DosTree)
		pDLLPath = resTrees[0];


	// Setup the ClientShellDE.
	dResult = dsi_InitClientShellDE();
	if(dResult != LT_OK)
	{
		ProcessError(dResult | ERROR_SHUTDOWN);
		return dResult;
	}

	c_InitConsoleCommands();
	con_InitBare();

	// Add the version console variable..
	dsi_GetVersionInfo(m_VersionInfo);
	m_VersionInfo.GetString(versionStr, sizeof(versionStr));
	sprintf(cmd, "version %s", versionStr);
	cc_HandleCommand(&g_ClientConsoleState, cmd);


	// Initialize input.
	if(dsi_IsInputEnabled())
	{
		if( !m_InputMgr->Init(m_InputMgr, &g_ClientConsoleState) )
		{
			ProcessError(LT_CANTINITIALIZEINPUT | ERROR_SHUTDOWN);
			RETURN_ERROR(1, CClientMgr::Init, LT_CANTINITIALIZEINPUT);
		}
	}


	// Run the config stuff with vars only.
	RunAutoConfig(pConfigFileName, CC_NOCOMMANDS);
	RunCommandLine(CC_NOCOMMANDS);


	// Open up the error log.
	InitErrorLog();

	// Init some music stuff..
	m_MusicMgr.m_hWnd = dsi_GetMainWindow();
	m_MusicMgr.m_hInstance = dsi_GetInstanceHandle( );

	if(g_bNullRender)
	{
		dsi_ConsolePrint("Warning: NullRender is ON.");
	}

	// Rerun the config stuff with commands only.
	RunAutoConfig(pConfigFileName, CC_NOVARS);
	RunCommandLine(CC_NOVARS);
	m_bCanSaveConfigFile = LTTRUE;

	m_pCollisionInfo = LTNULL;

	// Initialize the listener info.
//	m_bListenerInClient = LTTRUE;
//	VEC_INIT( m_vListenerPos );
//	ROT_INIT( m_rListenerRot );
//	VEC_INIT( m_vLastListenerPos );

	m_pCursorMgr = CreateCursorInterface();

	// Tell the client shell that we are ready to rock!
	_GetRModeFromConsoleVariables(&defaultMode);
	dResult = GetClientShell()->OnEngineInitialized(&defaultMode, &m_NetMgr.m_guidApp);

	// VideoMgr MUST be initialized AFTER the ClientShell, so that the SoundMgr is
	// properly initialized.

	// no videomgr on PS2
	m_pVideoMgr = CreateVideoMgr("BINK");

	// Tell the video stuff.
	if(m_pVideoMgr)
	{
		m_pVideoMgr->OnRenderInit();
	}

	// Initialize the debug graph manager
	rGraphMgr.left = 0;
	rGraphMgr.top = 0;
	rGraphMgr.right = defaultMode.m_Width;
	rGraphMgr.bottom = defaultMode.m_Height;
    graph_mgr->Mgr()->Init(m_pClientDE, &rGraphMgr);

	// Initialize our multi-protocol manager
	if (m_pMPM && !m_pMPM->Init())
		RETURN_ERROR(1, CClientMgr::Init, LT_ERRORINITTINGMPM);

	return dResult;
}


void CClientMgr::TermClientShellDE()
{
	CreateShellFn createFn;
	DeleteShellFn deleteFn;
	LTExtraCommandStruct *pCommand;
	LTLink *pCur, *pNext;


	// Clear rendering hooks.
	m_ModelHookFn = LTNULL;


	// Remove their console programs.
	pCur = g_ClientConsoleState.m_ExtraCommands.m_pNext;
	while(pCur && (pCur != &g_ClientConsoleState.m_ExtraCommands))
	{
		pNext = pCur->m_pNext;

		pCommand = (LTExtraCommandStruct*)pCur->m_pData;
		if(pCommand->flags & CMD_USERCOMMAND)
		{
			cc_RemoveCommand(&g_ClientConsoleState, pCommand);
		}

		pCur = pNext;
	}

	if(m_pClientShell)
	{
		sb_GetShellFunctions(m_hShellModule, &createFn, &deleteFn);
		deleteFn(m_pClientShell);
		m_pClientShell = LTNULL;
	}

	if(m_hShellModule)
	{
		sb_UnloadModule(m_hShellModule);
		m_hShellModule = LTNULL;
	}

	if(m_hClientResourceModule)
	{
		bm_UnbindModule(m_hClientResourceModule);
		m_hClientResourceModule = LTNULL;
	}

	if(m_hLocalizedClientResourceModule)
	{
		bm_UnbindModule(m_hLocalizedClientResourceModule);
		m_hLocalizedClientResourceModule = LTNULL;
	}
}


void CClientMgr::OnEnterWorld(CClientShell *pShell)
{
	if(m_pClientShell)
	{
		m_pClientShell->OnEnterWorld();
	}
}


void CClientMgr::OnExitWorld(CClientShell *pShell)
{
	if(m_pClientShell)
	{
		m_pClientShell->OnExitWorld();
	}
}


LTRESULT CClientMgr::StartShell(StartGameRequest *pRequest)
{
	CClientShell *pShell;
	LTRESULT dResult;
	char errorString[256];
	CBaseDriver *pDriver;
	char *pWorldName, playbackWorldName[256];
	SPPACKET pPacket;


	// Kill the old server connection if there is one.
	EndShell();

	pShell = new CClientShell;

	//	Start a shell.
	pShell->m_ShellMode = pRequest->m_Type;
	pShell->Init();

	dsi_SetConsoleUp(LTFALSE);

	switch(pRequest->m_Type)
	{
		case STARTGAME_CLIENT:
		{
			// Use the first net driver..
			pDriver = m_NetMgr.GetMainDriver();
			if (!pDriver)
			{
				RETURN_ERROR(1, StartGame, LT_NOTINITIALIZED);
			}

			// Try to actually connect...

			if (pRequest->m_flags & SG_LOBBY)
			{
				if (!pDriver->JoinLobbyLaunchSession())
				{
					RETURN_ERROR(1, StartGame, LT_ERROR);
				}
			}
			else
			{
				if (!pRequest->m_pNetSession)
				{
					RETURN_ERROR_PARAM(1, StartGame, LT_INVALIDPARAMS, "Missing m_pNetSession");
				}

				dResult = pDriver->JoinSession(pRequest->m_pNetSession);
				if(dResult != LT_OK)
				{
					return dResult;
				}
			}

			// Setup the shell.
			dResult = pShell->StartupClient(pDriver);
			if(dResult != LT_OK)
				return dResult;
		}
		break;

		case STARTGAME_CLIENTTCP:
		{
			// Use the first net driver..
			pDriver = m_NetMgr.GetDriver("internet");
			if (!pDriver)
			{
				RETURN_ERROR(1, StartGame, LT_NOTINITIALIZED);
			}

			// Connect to the given address.
			dResult = pDriver->ConnectTCP(pRequest->m_TCPAddress);
			if(dResult != LT_OK)
				return dResult;

			// Setup the shell.
			dResult = pShell->StartupClient(pDriver);
			if(dResult != LT_OK)
				return dResult;
		}
		break;

		default:
		{
			pDriver = LTNULL;
			if(pRequest->m_Type == STARTGAME_HOST)
			{
				pDriver = m_NetMgr.GetMainDriver();
				if (!pDriver)
				{
					RETURN_ERROR(1, StartGame, LT_NOTINITIALIZED);
				}

				if (pRequest->m_flags & SG_LOBBY)
				{
					if(pDriver->HostLobbyLaunchSession(&pRequest->m_HostInfo) != LT_OK)
					{
						RETURN_ERROR(1, StartGame, LT_ERROR);
					}
				}
				else
				{
					dResult = pDriver->HostSession(&pRequest->m_HostInfo);
					if(dResult != LT_OK)
					{
						return dResult;
					}
				}
			}

			// Setup the shell.
			dResult = pShell->StartupLocal(pRequest, pRequest->m_Type==STARTGAME_HOST, pDriver);
			if(dResult != LT_OK)
			{
				delete pShell;
				return dResult;
			}

			pWorldName = pRequest->m_WorldName;
			dResult = LT_OK;
			if(pWorldName[0] != 0)
			{
				dResult = pShell->m_pServerMgr->DoStartWorld(pWorldName, LOADWORLD_LOADWORLDOBJECTS|LOADWORLD_RUNWORLD, m_CurTime);
			}

			if(dResult != LT_OK)
			{
				pShell->m_pServerMgr->GetErrorString(errorString, 256);
				SetupError(LT_SERVERERROR, errorString);
				delete pShell;
				RETURN_ERROR_PARAM(1, CClientMgr::StartShell, LT_SERVERERROR, "error loading world");
			}

			// If they asked for a playdemo, fill in the world name.
			if(pRequest->m_PlaybackFilename[0] != 0)
			{
				SAFE_STRCPY(pRequest->m_WorldName, playbackWorldName);
			}
		}
		break;
	}

	// 'Enter' the server.
	m_pCurShell = pShell;
	OnEnterServer();

	// Send the hello message.
	if(m_pCurShell)
	{
		pPacket = packet_Get();
		pPacket->WriteWord((uint16)pRequest->m_ClientDataLen);
		pPacket->WriteRaw(pRequest->m_pClientData, (uint16)pRequest->m_ClientDataLen);
		pPacket->SetPacketID(CMSG_HELLO);
		m_NetMgr.SendPacket(pPacket, m_pCurShell->m_HostID);
	}

	return LT_OK;
}


void CClientMgr::EndShell()
{
	if(m_pCurShell)
	{
		delete m_pCurShell;
		m_pCurShell = LTNULL;
	}
}


LTRESULT CClientMgr::AppInitMusic( char *pszMusicDLL )
{

	if( !g_bMusicEnable || g_CV_ForceSoundDisable )
		RETURN_ERROR(1, CClientMgr::AppInitMusic, LT_ERROR);

	if( pszMusicDLL )
		strncpy( m_MusicDLLName, pszMusicDLL, MAX_DLLNAME_LEN );
	else
		strncpy( m_MusicDLLName, "cdaudio.dll", MAX_DLLNAME_LEN );
	m_MusicDLLName[MAX_DLLNAME_LEN] = '\0';

	// Initialize music...
	switch( music_InitDriver( m_MusicDLLName, &m_MusicMgr ))
	{
		case MUSICDRIVER_CANTLOADLIBRARY:
			SetupError(LT_MISSINGMUSICDLL, m_MusicDLLName);
			RETURN_ERROR(1, CClientMgr::AppInitMusic, LT_MISSINGMUSICDLL);
			break;
		case MUSICDRIVER_INVALIDDLL:
			SetupError(LT_INVALIDMUSICDLL, m_MusicDLLName);
			RETURN_ERROR(1, CClientMgr::AppInitMusic, LT_INVALIDMUSICDLL);
			break;
		case MUSICDRIVER_INVALIDOPTIONS:
			SetupError(LT_UNABLETOINITMUSICDLL, m_MusicDLLName);
			RETURN_ERROR(1, CClientMgr::AppInitMusic, LT_UNABLETOINITMUSICDLL);
			break;
	}

	return LT_OK;
}

void CClientMgr::AppTermMusic( )
{
	music_TermDriver( );
}

LTBOOL CClientMgr::BindClientShellWorlds()
{
	if(m_pCurShell) {
		m_pCurShell->BindWorlds();
	}

	return LTTRUE;
}


void CClientMgr::UnbindClientShellWorlds()
{
	if(m_pCurShell)
		m_pCurShell->UnbindWorlds();
}


void CClientMgr::BindSharedTextures()
{
	LTLink *pCur, *pListHead;

	// Read in the latest console variables..
	if(g_Render.m_bInitted && g_Render.ReadConsoleVariables)
		g_Render.ReadConsoleVariables();

	pListHead = &m_SharedTextures.m_Head;
	for(pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
	{
		r_BindTexture((SharedTexture*)pCur->m_pData, LTFALSE);
		Ping();
	}
}

void CClientMgr::UnbindSharedTextures(bool bUnLoad_EngineData)
{
	LTLink *pCur, *pListHead;

	pListHead = &m_SharedTextures.m_Head;
	for (pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext) {
		r_UnbindTexture((SharedTexture*)pCur->m_pData, bUnLoad_EngineData); }
}

void CClientMgr::InitConsole()
{
	RenderStruct *pRender = r_GetRenderStruct();
	ASSERT(pRender);
	LTRect rect ( 0, 0, pRender->m_Width, pRender->m_Height/2 );

	// Init the console.
	con_Term(LTFALSE);
	con_Init(&rect, c_CommandHandler, pRender);
}


void CClientMgr::TermConsole()
{
	con_Term(LTFALSE);
}


void cm_HandleMaxFPS(uint32 frameTicks)
{
	float frameSeconds, wantedSeconds, startTime, curTime;
	uint32 i;

	if(g_CV_MaxFPS == 0.0f)
		return;

	g_CV_MaxFPS = LTCLAMP(g_CV_MaxFPS, 10.0f, 200.0f);

	wantedSeconds = 1.0f / g_CV_MaxFPS;
	frameSeconds = (float)frameTicks / cnt_NumTicksPerSecond();
	if(frameSeconds < wantedSeconds)
	{
		startTime = time_GetTime();
		for(i=0; i < 500; i++)
		{
			curTime = time_GetTime();
			if((curTime - startTime) > (wantedSeconds - frameSeconds))
				break;

			dsi_ClientSleep(1); // Sleep for 1 millisecond.
		}
	}
}


void CClientMgr::ProcessThreadMessage(LThreadMessage &msg)
{
	FileIdentifier *pIdent;
	Model *pModel;

	if(msg.m_ID == CLT_LOADEDFILE)
	{
		pIdent = (FileIdentifier*)msg.m_Data[1].m_pData;

		if(msg.m_Data[0].m_dwData == FT_MODEL)
		{
			// Update the placeholder models.
			pModel = (Model*)msg.m_Data[2].m_pData;

			InstallModel(pModel, pIdent);
		}
	}
}


void CClientMgr::CheckThreads() {
    LThreadMessage msg;

    // Check the status of the loader thread.
    while (loader_thread->LoaderThread()->m_Outgoing.GetMessage(msg) == LT_OK) {
        ProcessThreadMessage(msg);
    }
}


LTRESULT CClientMgr::Update()
{
	uint16 i;
	Counter totalCounter; //, soundTicks;
	LTRESULT dResult;
	uint32 frameTicks;
	static LTRect rFPS (480,10,630,80);

	uint32 PerfCnter = AddandStartProfileCounter('BASC',"Update");
    

//	ctik_ResetCounters();
	cnt_StartCounter(&totalCounter);

	// Update stuff from threads.
	CheckThreads();

    uint32 videoMgrPerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"Video");
	// Update videos.
	if (m_pVideoMgr) {
		m_pVideoMgr->UpdateVideos(); }
    StopProfileCounter(videoMgrPerfCnt);
    
	// Hack while there's a bug in Shogo..
	if (g_CV_FullLightScale) {
		VEC_SET(m_GlobalLightScale, 1, 1, 1); }

	// Update framerate (don't want to update it if the client's not active).

    uint32 frameRatePerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"Video");
	if(dsi_IsClientActive()) {
		m_CurTime = time_GetTime();
	    m_FrameTime = m_CurTime - m_LastTime;
	    m_LastTime = m_CurTime;
	    UpdateFrameRate();
	}
    StopProfileCounter(frameRatePerfCnt);

	// Sleep in between frames.. helpful for debugging so it doesn't hog
	// all the processor time.
	dsi_ClientSleep(g_ClientSleepMS);

	// Puase music if disabled...
	if( GetMusicMgr( )->m_bValid )
	{
		CountAdder cntAdd(&g_PD_Music);

		if( g_bMusicEnable != GetMusicMgr( )->m_bEnabled )
		{
			GetMusicMgr( )->m_bEnabled = ( LTBOOL )g_bMusicEnable;
			if( !g_bMusicEnable )
				GetMusicMgr( )->Pause( MUSIC_IMMEDIATE );
			else
				GetMusicMgr( )->Resume( );
		}
	}

	// Pause sound if disabled...
	if( GetSoundMgr( )->IsValid( ))
	{
		CountAdder cntAdd(&g_PD_Sound);

		if( g_bSoundEnable != GetSoundMgr( )->IsEnabled( ))
		{
			GetSoundMgr( )->SetEnable( g_bSoundEnable );
			if( !g_bSoundEnable )
				GetSoundMgr( )->PauseSounds( );
			else
				GetSoundMgr( )->ResumeSounds( );
		}
	}

	// Forward windows messages to the scripts.
    uint32 consolePerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"Console");    
	if(!dsi_IsConsoleUp())
		ForwardMessagesToScript();
    StopProfileCounter(consolePerfCnt);

	// Gather and send current input.
    uint32 inputPerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"Input");      
	if(dsi_IsClientActive())
	{
		CountAdder cntAdd(&g_PD_Input);
		ProcessAllInput(LTFALSE);
	}
    StopProfileCounter(inputPerfCnt);
      
	// Update client shells.
	if(m_pCurShell)
	{
        uint32 clientShellUpdatePerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"ClientUpdate");          
		   CountAdder cntAdd(&g_PD_ClientShell);
		   dResult = m_pCurShell->Update();
        StopProfileCounter(clientShellUpdatePerfCnt);
		if(dResult != LT_OK)
			return ProcessError(dResult);
	}
	else if(m_pClientShell)
	{
		// If we don't have any client shells right now, do a 'fake' update.
        uint32 clientShellPreUpdatePerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"ClientPreUpdate");          
		    m_pClientShell->PreUpdate();
        StopProfileCounter(clientShellPreUpdatePerfCnt);
        uint32 clientShellUpdatePerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"ClientUpdate");          
		   m_pClientShell->Update();
        StopProfileCounter(clientShellUpdatePerfCnt);
        uint32 clientShellUpdateObjPerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"ClientUpdateObj");          
		   UpdateObjects();
        StopProfileCounter(clientShellUpdateObjPerfCnt);
        uint32 clientShellPostUpdatePerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"ClientPostUpdate");
           m_pClientShell->PostUpdate();
        StopProfileCounter(clientShellPostUpdatePerfCnt);
	}


    uint32 soundPerfCnt = AddandStartProfileCounter(MAKEFOURCC_PC('M','A','I','N'),"Sound");  
	// Update all sounds.
	{
		CountAdder cntAdd(&g_PD_Sound);

//		cnt_StartCounter(&soundTicks);
			UpdateAllSounds( m_FrameTime );
//		g_SoundTicks += cnt_EndCounter(&soundTicks);
	}
    StopProfileCounter(soundPerfCnt);

	// Update the console if it's up.
	if(dsi_IsConsoleUp())
	{
		for(i=0; i < dsi_NumKeyDowns(); i++)
		{
			con_OnKeyPress(dsi_GetKeyDown(i));
		}

		dsi_ClearKeyDowns();
		dsi_ClearKeyUps();
	}

	if(g_CV_ShowFrameRate)
	{
		con_Printf(CONRGB(255,100,100), 0, "FPS: %.2f", m_FramerateTracker.GetRate());
	}

    if(graph_mgr->Mgr()->CheckGraph(&g_CV_ShowFrameRate, g_CV_ShowFrameRate >= 2))
    {
        DGParams foo ("Framerate (0 - 60)", 0.0f, 60.0f, GetDGColorsRYG());
        graph_mgr->Mgr()->UpdateGraph(&g_CV_ShowFrameRate,
            m_FramerateTracker.GetRate(), foo);
    }

	// Memory stats.
	if(g_bShowMemStats)
	{
		con_WhitePrintf("Memory allocated: %dk", dm_GetBytesAllocated() / 1000);
		con_WhitePrintf("Number of allocations: %d", dm_GetNumAllocations());
	}

    if(graph_mgr->Mgr()->CheckGraph(&g_bShowMemStats, g_bShowMemStats >= 2))
    {
        DGParams foo ("Memory", 0.0f,
            (float)dm_GetBytesAllocated() / 500.0f, GetDGColorsKB());
        graph_mgr->Mgr()->UpdateGraph(&g_bShowMemStats,
            (float)dm_GetBytesAllocated() / 1000.0f, foo);
    }

	// Tick counts.
	frameTicks = cnt_EndCounter(&totalCounter);
//	g_TotalTickCount += frameTicks;

//	ctik_ShowTickStatus();
	mw_ShowMemoryWatches();

	// For debugging...
	MaybeDrawConsole();

	cm_HandleMaxFPS(frameTicks);					// Lock framerate if they want.

	m_pBenchmarkMgr->GraphicsBenchmarkingUpdate();	// Update the benchmark mgr

	#ifdef LITHTECH_ESD
	m_pRealAudioMgr->Update();						// Update the Real video manager
	#endif

	if (m_pMPM)
		m_pMPM->CyclePostUpdate(m_CurTime);		// give some more time to the multi-protocol mangler

	StopProfileCounter(PerfCnter);

	return LT_OK;									// All done
}



void CClientMgr::ProcessAllInput(LTBOOL bForceClear)
{

	int32 changes[MAX_CLIENT_COMMANDS], nChanges = 0;
	int32 commandsOn[MAX_CLIENT_COMMANDS], nCommandsOn = 0;


	uint8 *pPrevSlot, *pCurSlot;
	pPrevSlot = m_Commands[m_iCurInputSlot];
	pCurSlot = m_Commands[!m_iCurInputSlot];

	memset(pCurSlot, 0, MAX_CLIENT_COMMANDS);
	if(!m_bTrackingInputDevices)
	{
		m_InputMgr->ReadInput(m_InputMgr, pCurSlot, m_AxisOffsets);
	}

	if(!m_bInputState || (bForceClear || dsi_IsConsoleUp()))
	{
		memset(pCurSlot, 0, MAX_CLIENT_COMMANDS);
		memset(m_AxisOffsets, 0, sizeof(m_AxisOffsets));
	}

	// Find out how many have changed.
    int32 i;
	for(i=0; i < MAX_CLIENT_COMMANDS; i++)
	{
		if(pCurSlot[i])
		{
			commandsOn[nCommandsOn] = i;
			nCommandsOn++;
		}

		if(pCurSlot[i] != pPrevSlot[i])
		{
			changes[nChanges] = i;
			nChanges++;
		}
	}

	m_iCurInputSlot = !m_iCurInputSlot;

	ForwardCommandChanges(&changes[0], nChanges);

	if(m_pCurShell && m_pCurShell->m_bWorldOpened)
	{
		SendUpdateUnGuaranteed(&m_NetMgr, m_pCurShell->m_HostID, commandsOn, nCommandsOn);
	}
}


LTRESULT CClientMgr::ClearInput()
{
	ProcessAllInput(LTTRUE);
	dsi_ClearKeyDowns();
	dsi_ClearKeyUps();
	dsi_ClearKeyMessages();
	m_InputMgr->ClearInput();

    return LT_OK;
}


int CClientMgr::GetErrorCode()
{
	return m_LastErrorCode;
}


void CClientMgr::ShowDrawSurface(uint flags)
{
//	Counter cnt;
#ifdef LITHTECH_ESD
	m_pRealVideoMgr->Update2D();	// [mds] FORCE IT!!!
#endif // LITHTECH_ESD

    graph_mgr->Mgr()->Draw();

	if (r_GetRenderStruct()->m_bInitted) {
		r_GetRenderStruct()->SwapBuffers(flags); }
}


LTRESULT CClientMgr::PlaySound(PlaySoundInfo *pPlaySoundInfo, FileRef *pFile, float fOffsetTime )
{
	FileIdentifier *pIdent;
	const char *pFileName;

	if( !pPlaySoundInfo )
		return LT_ERROR;

	if( !GetSoundMgr( )->IsValid( ) || !GetSoundMgr( )->IsEnabled( ))
	{
		return LT_ERROR;
	}

	pIdent = cf_GetFileIdentifier( m_hFileMgr, pFile, TYPECODE_SOUND );
	if(!pIdent)
	{
		pFileName = cf_GetFilename(m_hFileMgr, pFile);

		if(g_DebugLevel >= 2)
		{
			dsi_PrintToConsole("Missing sound file %s", pFileName );
		}

		return LT_ERROR;
	}

	LTRESULT result = m_SoundMgr.PlaySound( *pPlaySoundInfo, *pIdent, ( uint32 )( fOffsetTime * 1000.0f + 0.5f ));

	if (result != LT_OK)
		return result;

	// Tell the client shell we're playing a sound
	GetClientShell()->OnPlaySound(pPlaySoundInfo);

	return result;
}


void CClientMgr::UpdateAllSounds( float fFrameTime )
{
	if( !GetSoundMgr( )->IsValid( ) || !GetSoundMgr( )->IsEnabled( ))
		return;

	m_SoundMgr.Update( );

	if( g_bDebugNumSounds )
	{
		con_Printf(CONRGB(255,0,0), 0, "Sounds: playing:%d, heard:%d", GetSoundMgr( )->GetNumSoundsPlaying( ),
			GetSoundMgr( )->GetNumSoundsHeard( ));
	}
}


// ------------------------------------------------------------------ //
// Internal helpers.
// ------------------------------------------------------------------ //

static void ClientStringWhine(char *pString, void *pUser)
{
	con_WhitePrintf("Unfreed string: %s", pString);
}


static void FreeModelList(LTLink *pListHead)
{
	LTLink *pCur, *pNext;

	// Ensure the delete order..
	pCur = pListHead->m_pNext;
	while(pCur != pListHead)
	{
		pNext = pCur->m_pNext;
		((Model*)pCur->m_pData)->AddRef();
		pCur = pNext;
	}

	// Actually delete them.
	pCur = pListHead->m_pNext;
	while(pCur != pListHead)
	{
		pNext = pCur->m_pNext;
		Model *pModel = (Model*)pCur->m_pData;
		pModel->DecRef();
		if (pModel->IsFreeable())
			delete pModel;
		pCur = pNext;
	}

	dl_TieOff(pListHead);
}


static void FreeSpriteList(LTList *pList)
{
	LTLink *pCur, *pNext, *pListHead;

	pListHead = &pList->m_Head;
	pCur = pListHead->m_pNext;
	while(pCur != pListHead)
	{
		pNext = pCur->m_pNext;
		spr_Destroy((Sprite*)pCur->m_pData);
		pCur = pNext;
	}

	dl_InitList(pList);
}


// ------------------------------------------------------------------ //
// C routines.
// ------------------------------------------------------------------ //

static void cm_MemoryCleanup(void *pUser) {

}


void cm_Init() {
	g_pClientMgr = new CClientMgr;

	// fail if we can't allocate the client mgr
	if (g_pClientMgr == LTNULL) return;

	g_pClientMgr->m_ObjectMap = LTNULL;
	g_pClientMgr->m_ObjectMapSize = 0;

	// set up the directmusicmgr
	g_pClientMgr->m_pDirectMusicMgr = new CLTDirectMusicMgr;

	#ifdef LITHTECH_ESD
	// set up the real audio manager
	g_pClientMgr->m_pRealAudioMgr = new CLTRealAudioMgr;
	g_pClientMgr->m_pRealAudioMgr->Init();

	// set up the real audio player
	g_pRealAudioPlayer = g_pClientMgr->m_pRealAudioMgr->CreatePlayer();

	// set up the real video manager
	g_pClientMgr->m_pRealVideoMgr = new CLTRealVideoMgr;
	g_pClientMgr->m_pRealVideoMgr->Init();

	// set up the real video player
	g_pRealVideoPlayer = g_pClientMgr->m_pRealVideoMgr->CreatePlayer();

	// set up the real console manager
	g_pClientMgr->m_pRealConsoleMgr = new CLTRealConsoleMgr;
	#endif

	// set up the benchmark manager
	g_pClientMgr->m_pBenchmarkMgr = new CLTBenchmarkMgr;
	g_pClientMgr->m_pBenchmarkMgr->Init();

	// No errors so far...
	g_pClientMgr->m_ErrorString[0] = '\0';

	// Setup the memory cleanup handler.
	dm_AddCleanupHandler(cm_MemoryCleanup, NULL);

	g_pClientMgr->m_bNotifyRemoves = LTTRUE;
	g_pClientMgr->m_pCurShell = LTNULL;
	g_pClientMgr->m_pClientShell = LTNULL;
	g_pClientMgr->m_hShellModule = LTNULL;
	g_pClientMgr->m_hClientResourceModule = LTNULL;
	g_pClientMgr->m_hLocalizedClientResourceModule = LTNULL;
	strcpy(g_pClientMgr->m_MusicDLLName, "cdaudio.dll");
	g_pClientMgr->m_MusicMgr.m_bValid = 0;
	g_pClientMgr->m_InputSendCounter = 0.0f;

	g_pClientMgr->m_bCanSaveConfigFile = LTFALSE;

	dl_InitList(&g_pClientMgr->m_Sprites);
	dl_TieOff(&g_pClientMgr->m_Models);

	g_pClientMgr->m_SpriteSurfaces = LTNULL;

	VEC_SET(g_pClientMgr->m_GlobalLightScale, 1.0f, 1.0f, 1.0f);
	VEC_SET(g_pClientMgr->m_GlobalLightDir, 0.0f, 0.0f, 0.0f);
	VEC_SET(g_pClientMgr->m_GlobalLightColor, 0.0f, 0.0f, 0.0f);
	g_pClientMgr->m_hFileMgr = cf_Init();

	// Initialize all the object banks.
	om_Init(&g_pClientMgr->m_ObjectMgr, LTTRUE);

	sb_Init2(&g_pClientMgr->m_FileIDInfoBank, sizeof( FileIDInfo ), 32, 32);

	g_pClientMgr->m_SharedTextureBank.Init(64, 64);

	// Initialize all the lists.
	dl_InitList(&g_pClientMgr->m_SharedTextures);

	input_GetManager(&g_pClientMgr->m_InputMgr);

	// Init the net manager.
	g_pClientMgr->m_NetMgr.Init("Player");
	g_pClientMgr->m_bRendering = LTFALSE;

	g_pClientMgr->m_nSkyObjects = 0;
	g_pClientMgr->m_pSurfaceEffects = LTNULL;
	g_pClientMgr->m_CurTime = g_pClientMgr->m_LastTime = g_pClientMgr->m_fLastPingTime = 0.0f;
	g_pClientMgr->m_iCurInputSlot = 0;
	g_pClientMgr->m_nLastCommandsOn = 0;
	g_pClientMgr->m_ExceptionFlags = 0;
	g_pClientMgr->m_bInputState = LTFALSE;
	g_pClientMgr->m_bTrackingInputDevices = LTFALSE;
	g_pClientMgr->m_ModelHookFn = LTNULL;
	g_pClientMgr->m_ModelHookUser = LTNULL;
	g_pClientMgr->m_nResTrees = 0;

	g_pClientMgr->m_pModelPlaceholder = new Model;
	g_pClientMgr->m_pModelPlaceholder->AddRef();
	g_pClientMgr->m_MoveAbstract = new CMoveAbstract();
	g_pClientMgr->m_pPacketHelper = new CLMessageHelper();

	// Check everything..
    if (!g_pClientMgr->m_pPacketHelper 
        || !g_pClientMgr->m_pModelPlaceholder 
        || !g_pClientMgr->m_MoveAbstract
        || (loader_thread->LoaderThread()->Start(LTPRI_NORMAL) != LT_OK)
        )
	{
		delete g_pClientMgr;
	}
}


CClientMgr::~CClientMgr()
{
	int i;


	// Stop loading stuff.
    loader_thread->LoaderThread()->Terminate(LTTRUE);
    CheckThreads();


	m_bNotifyRemoves = LTFALSE;

	TermSpriteSurfaces();

	// Tell the client shell we're going away.
	if(GetClientShell())
	{
		GetClientShell()->OnEngineTerm();
	}

	TermClientShellDE();

	// Get rid of ALL objects.  Should be done before closing the shell (and world)
	// so WorldModel's WorldData pointers are valid.
	for(i=0; i < NUM_OBJECTTYPES; i++) {
		RemoveObjectsInList(&m_ObjectMgr.m_ObjectLists[i], LTFALSE);
	}

	// Kill all the shells.  This must be before saving the config so the console state can
	// be restored for any demo playbacks.
	EndShell();

	SaveAutoConfig("autoexec.cfg");

	// Stop getting input.
	m_InputMgr->Term(m_InputMgr);

	ee_TermSurfaceEffects();

	memset(m_SkyObjects, 0xFF, sizeof(m_SkyObjects));

	m_SoundMgr.Term( );

	// Kill the music driver...
	AppTermMusic( );

	// Kill the graph manager
    graph_mgr->Mgr()->Term();

	// Free models.
	FreeModelList(&m_Models);
	FreeSpriteList(&m_Sprites);

	con_Term(LTTRUE);

	m_NetMgr.Term();


	r_TermRender(2, true);
	FreeSharedTextures();	// This must come after r_TermRender because
									// r_TermRender accesses the shared textures.
									// ALSO: this MUST come after TermClientShellDE because
									// if it comes before, the client shell could have HTEXTUREs
									// (SharedTexture*'s) to invalid data.

	c_TermConsoleCommands();


	om_Term(&m_ObjectMgr);


	// Cleanup the object map.
	if(m_ObjectMap)
	{
		dfree(m_ObjectMap);
		m_ObjectMap = LTNULL;
	}

	m_ObjectMapSize = 0;


	sb_Term(&m_FileIDInfoBank);

	// Kill the ClientDE interface..
	ci_Term();
	str_ShowAllStringsAllocated(ClientStringWhine, LTNULL);

	// Kill the file manager.
	if(m_hFileMgr)
	{
		cf_Term(m_hFileMgr);
	}


	TermErrorLog();

	if(m_pClientDE)
	{
		delete m_pClientDE;
		m_pClientDE = LTNULL;
	}

	if(m_MoveAbstract)
	{
		delete m_MoveAbstract;
		m_MoveAbstract = LTNULL;
	}

	if(m_pPacketHelper)
	{
		delete m_pPacketHelper;
		m_pPacketHelper = LTNULL;
	}

	if(m_pModelPlaceholder)
	{
		m_pModelPlaceholder->DecRef();
		delete m_pModelPlaceholder;
		m_pModelPlaceholder = LTNULL;
	}
}


CPacket* CClientMgr::GetPacket()
{
	SPPACKET pPacket;

	pPacket = packet_Get();
	pPacket->AddRef();
	pPacket->GetLMessage()->m_pHelper = m_pPacketHelper;
	pPacket->m_dwFlags |= PACKETFLAG_AUTORESET;

	return pPacket;
}


void CClientMgr::SetupPacket(CPacket *pPacket)
{
	pPacket->GetLMessage()->m_pHelper = m_pPacketHelper;
}


uint16 CClientMgr::IncCurTextureFrameCode()
{
	LTLink *pListHead, *pCur;

	if(m_CurTextureFrameCode == 0xFFFF)
	{
		m_CurTextureFrameCode = 1;

		pListHead = &m_SharedTextures.m_Head;
		for(pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
		{
			((SharedTexture*)pCur->m_pData)->SetCurFrameCode(0);
		}
	}
	else
	{
		m_CurTextureFrameCode++;
	}

	return m_CurTextureFrameCode;
}

LTRESULT CClientMgr::FreeUnusedModels()
{
	LTLink *pListHead, *pCur;

	// Go through the list and increment the reference counts so they don't get removed from the list
	// without our knowledge & clear their touch flags so this is effective..
	pListHead = &m_Models;
	for(pCur=pListHead->m_pNext; pCur != pListHead; pCur = pCur->m_pNext)
	{
		Model *pModel = (Model*)pCur->m_pData;

		pModel->AddRef();
		pModel->m_Flags &= ~MODELFLAG_TOUCHED;
	}

	// Mark all client side model objects as touched so we don't unload them
	// behind the game's back.
	pListHead = &m_ObjectMgr.m_ObjectLists[OT_MODEL].m_Head;
	for(pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
	{
		ModelInstance *pInstance = (ModelInstance*)pCur->m_pData;

		if(pInstance->m_ObjectID == (uint16)-1)
		{
			pInstance->GetModel()->m_Flags |= MODELFLAG_TOUCHED;
		}
	}

	// Get rid of any untouched resources.
	pListHead = &m_Models;
	for(pCur=pListHead->m_pNext; pCur != pListHead; )
	{
		LTLink *pNext = pCur->m_pNext;

		Model *pModel = (Model*)pCur->m_pData;
		// Dec the ref count of this model (see above loop)
		pModel->DecRef();
		if(pModel->IsFreeable() && !(pModel->m_Flags & MODELFLAG_TOUCHED))
		{
			// Actually delete the model
			dl_Remove(&pModel->m_Link);
			delete pModel;
		}

		pCur = pNext;
	}

	return LT_OK;
}

void CClientMgr::OnEnterServer() {
	ASSERT(m_pCurShell);

    cf_OnConnect(m_hFileMgr, m_pCurShell->m_HostID);

	// Init the server mirror state.
	memset(&m_ServerConsoleMirror, 0, sizeof(m_ServerConsoleMirror));
	m_ServerConsoleMirror.Alloc = dalloc;
	m_ServerConsoleMirror.Free = dfree;
	cc_InitState(&m_ServerConsoleMirror);
	m_nLastCommandsOn = 0;
}


void CClientMgr::OnExitServer(CClientShell *pShell) {
	if(pShell != m_pCurShell)
		return;

	memset(m_SkyObjects, 0xFF, sizeof(m_SkyObjects));

	// Tell the file manager.
	cf_OnDisconnect(m_hFileMgr);

	// Shutdown the server mirror console state.
	cc_TermState(&m_ServerConsoleMirror);
}


void CClientMgr::RunCommandLine(uint32 flags)
{
	int i;
	char fullCmd[512];

	for( i=0; i < command_line_args->Argc()-1; i++ )
	{
        if(command_line_args->Argv(i)[0]=='+'){
			sprintf( fullCmd, "(%s) (%s)",
                     &(command_line_args->Argv(i)[1]),
                     command_line_args->Argv(i+1));
			cc_HandleCommand2(&g_ClientConsoleState, fullCmd, flags);
		}
	}
}


void CClientMgr::RunAutoConfig(const char *pFilename, uint32 flags) {
	cc_RunConfigFile(&g_ClientConsoleState, pFilename, flags, VARFLAG_SAVE);
}


void CClientMgr::SaveAutoConfig(const char *pFilename) {
	if(m_bCanSaveConfigFile)
	{
		cc_SaveConfigFile(&g_ClientConsoleState, pFilename);
	}
}


LTRESULT CClientMgr::StartRenderFromGlobals() {
	RMode mode;

	_GetRModeFromConsoleVariables(&mode);
	return r_InitRender(&mode);
}



LTBOOL CClientMgr::Render(CameraInstance *pCamera, int drawMode, LTObject **pObjects, int nObjects) {
	uint32 width, height, i;
	SceneDesc *pDesc;
	int renderStatus;
	RenderStruct *pRenderStruct;
	Counter renderTicks;
	SceneDesc sceneDesc;
	LTObject *skyObjects[MAX_SKYOBJECTS];

	if(!r_IsRenderInitted() || m_bRendering)
		return LTFALSE;

	pDesc = &sceneDesc;
	pRenderStruct = r_GetRenderStruct();

	pDesc->m_DrawMode = drawMode;

	// Clear the background.
	width = pRenderStruct->m_Width;
	height = pRenderStruct->m_Height;

	// Get stuff from the camera object.
	if(m_pCurShell)
		pDesc->m_hRenderContext = (HRENDERCONTEXT)world_client->RenderContext();
	else
		pDesc->m_hRenderContext = LTNULL;

	pDesc->m_Pos = pCamera->GetPos();
	pDesc->m_Rotation = pCamera->m_Rotation;
	pDesc->m_xFov = pCamera->m_xFov;
	pDesc->m_yFov = pCamera->m_yFov;

	if(pCamera->m_bFullScreen)
	{
		pDesc->m_Rect.left = pDesc->m_Rect.top = 0;
		pDesc->m_Rect.right = width;
		pDesc->m_Rect.bottom = height;
	}
	else
	{
		pDesc->m_Rect.left = pCamera->m_Left;
		pDesc->m_Rect.top = pCamera->m_Top;
		pDesc->m_Rect.right = pCamera->m_Right;
		pDesc->m_Rect.bottom = pCamera->m_Bottom;
	}

	pDesc->m_FarZ = g_FarZ;

	pDesc->m_GlobalLightColor = m_GlobalLightColor;
	pDesc->m_GlobalLightDir = m_GlobalLightDir;
	pDesc->m_GlobalLightScale = m_GlobalLightScale;
	pDesc->m_GlobalLightAdd = pCamera->m_LightAdd;

	pDesc->m_GlobalModelLightAdd = g_ConsoleModelAdd;
	pDesc->m_GlobalModelLightDirAdd = g_ConsoleModelDirAdd;

	pDesc->m_pObjectList = pObjects;
	pDesc->m_ObjectListSize = nObjects;

	// Sky info.
	memcpy(&pDesc->m_SkyDef, &m_SkyDef, sizeof(pDesc->m_SkyDef));

	pDesc->m_nSkyObjects = 0;
	pDesc->m_SkyObjects = skyObjects;
	for(i=0; i < MAX_SKYOBJECTS; i++)
	{
		skyObjects[pDesc->m_nSkyObjects] = FindObject(m_SkyObjects[i]);
		if(skyObjects[pDesc->m_nSkyObjects])
			++pDesc->m_nSkyObjects;
	}

	// Model hook stuff.
	pDesc->m_ModelHookFn = m_ModelHookFn;
	pDesc->m_ModelHookUser = m_ModelHookUser;

	m_bRendering = LTTRUE;

	uint32 Cnter_AllRender = AddandStartProfileCounter('BASC',"RenderScene");

	CountAdder cntAdd(&g_PD_Render);

	if (g_CV_RenderEnable) {
		renderStatus = pRenderStruct->RenderScene(&sceneDesc); }

	StopProfileCounter(Cnter_AllRender);

	if (g_CV_PerfCounterDisplay) { DrawProfileCounters(); }
	if (g_CV_DrawDebugGeometry || g_CV_PerfCounterDisplay) {
		drawAllDebugGeometry(); }						// Draw all debug geometry here...

	m_bRendering = LTFALSE;

	pRenderStruct->m_ChangeFlags = 0;  // Reset change flags...
	return LTTRUE;
}


void CClientMgr::RebindTextures() {
	UnbindSharedTextures(false);
	BindSharedTextures();
}


LTRESULT CClientMgr::AddObjectToClientWorld(uint16 objectID,
	InternalObjectSetup *pSetup, LTObject **ppObject, LTBOOL bMove, LTBOOL bRotate )
{
	LTObject *pObject;
	LTRESULT dResult;

	*ppObject = LTNULL;

	// Create the main object.
	dResult = om_CreateObject(&m_ObjectMgr, pSetup->m_pSetup, &pObject);
	if(dResult != LT_OK)
		return dResult;

	// Init the object and client-specific data.
	dResult = so_ExtraInit(pObject, pSetup, LTFALSE);
	if(dResult != LT_OK)
	{
		om_DestroyObject(&m_ObjectMgr, pObject);
		return dResult;
	}

	// Setup its object ID and other data.
	pObject->m_ObjectID = objectID;
	if(pObject->m_ObjectID != OBJID_CLIENTCREATED)
	{
		// If it's a server object, add it to the server object list.
		AddToObjectMap(pObject->m_ObjectID);
		m_ObjectMap[pObject->m_ObjectID].m_nRecordType = RECORDTYPE_LTOBJECT;
		m_ObjectMap[pObject->m_ObjectID].m_pRecordData = pObject;
	}

	// Now put it where they specified.
	if( bMove && bRotate )
		MoveAndRotateObject(pObject, &pSetup->m_pSetup->m_Pos, &pSetup->m_pSetup->m_Rotation);
	else if( bMove )
		MoveObject(pObject, &pSetup->m_pSetup->m_Pos, LTTRUE );
	else if( bRotate )
		RotateObject(pObject, &pSetup->m_pSetup->m_Rotation );

	// Possibly scale it.
	if(pSetup->m_pSetup->m_Scale.x != 1.0f || pSetup->m_pSetup->m_Scale.y != 1.0f || pSetup->m_pSetup->m_Scale.z != 1.0f)
		ScaleObject(pObject, &pSetup->m_pSetup->m_Scale);

	if(HasWorldModel(pObject))
	{
		InitialWorldModelRotate(ToWorldModel(pObject));
	}

	*ppObject = pObject;

	r_GetRenderStruct()->m_ChangeFlags |= RSCHANGE_ADDOBJECT;
	return LT_OK;
}


LTRESULT CClientMgr::RemoveObjectFromClientWorld(LTObject *pObject) {
	IClientShell *pClientShell;

	// Detach it from whatever it's standing on.
	DetachObjectStanding(pObject);
	DetachObjectsStandingOn(pObject);

	// Remove the object from the world
	obj_RemoveNodeObjFromBsp(pObject);

	// Notify them if they want notification..
	if(pObject->cd.m_ClientFlags & CF_NOTIFYREMOVE)
	{
		pClientShell = GetClientShell();
		if(pClientShell)
			pClientShell->OnObjectRemove((HLOCALOBJ)pObject);
	}

	// Call the object's termination function
	so_ExtraTerm(pObject);

	// Remove it from the object map.
	if(pObject->m_ObjectID != (uint16)-1) {
		ClearObjectMapEntry(pObject->m_ObjectID);
	}

	// Unload un-used model textures..
	if (pObject->m_ObjectType == OT_MODEL) {
		FreeUnusedModelTextures(pObject);
	}

	om_DestroyObject(&m_ObjectMgr, pObject);

	r_GetRenderStruct()->m_ChangeFlags |= RSCHANGE_REMOVEOBJECT;
	return LT_OK;
}


#ifdef USE_LOCAL_STUFF
	void clienthack_AddObject(LTObject *pObject, void *pSpecialEffect_void, ObjectCreateStruct *pStruct)
	{
		InternalObjectSetup setup;
		LTRESULT dResult;
		CPacket *pSpecialEffect;

		setup.m_pSetup = pStruct;
		setup.m_Filename.m_pFilename = pStruct->m_Filename;
		setup.m_Filename.m_FileType = FILE_CLIENTFILE;
		setup.m_SkinName.m_pFilename = pStruct->m_SkinName;
		setup.m_SkinName.m_FileType = FILE_CLIENTFILE;

		dResult = so_ExtraInit(pObject, &setup, LTTRUE);
		if(dResult == LT_OK)
		{
			g_pClientMgr->AddToObjectMap(pObject->m_ObjectID);
			m_ObjectMap[pObject->m_ObjectID].m_nRecordType = RECORDTYPE_DOBJECT;
			m_ObjectMap[pObject->m_ObjectID].m_pRecordData = pObject;
		}

		// Do its special effect message.
		if(pSpecialEffect_void)
		{
			pSpecialEffect = (CPacket*)pSpecialEffect_void;
			pSpecialEffect->ResetRead();

			SetupPacket(pSpecialEffect);
			m_pClientShellDE->SpecialEffectNotify(m_pClientShellDE,
				(HLOCALOBJ)pObject, pSpecialEffect->GetLMessage());
		}
	}


	void clienthack_RemoveObject(LTObject *pObject)
	{
		ILTClientShell *pClientShell;

		// Notify them if they want notification..
		if(pObject->cd.m_ClientFlags & CF_NOTIFYREMOVE)
		{
			pClientShell = m_pClientShellDE;
			if(pClientShell)
				pClientShell->OnObjectRemove(pClientShell, (HLOCALOBJ)pObject);
		}

		g_pClientMgr->ClearObjectMapEntry(pObject->m_ObjectID);
	}
#endif


void CClientMgr::RemoveObjectsInList(LTList *pList, LTBOOL bServerOnly) {
	LTLink *pCur, *pNext, *pListHead;
	LTObject *pObj;

	pListHead = &pList->m_Head;
	pCur = pListHead->m_pNext;
	while(pCur != pListHead)
	{
		pNext = pCur->m_pNext;

		// Only remove objects that came from the server.
		pObj = (LTObject*)pCur->m_pData;

		if(bServerOnly) {
			if(pObj->m_ObjectID != (uint16)-1) {
				RemoveObjectFromClientWorld(pObj);
			}
		}
		else {
			RemoveObjectFromClientWorld(pObj);
		}

		pCur = pNext;
	}
}

void CClientMgr::TermSpriteSurfaces() {
	SpriteSurf *pSurf, *pNext;

	// Free the sprite surfaces.
	pSurf = m_SpriteSurfaces;
	while(pSurf)
	{
		pNext = pSurf->m_pNext;
		dfree(pSurf);
		pSurf = pNext;
	}

	m_SpriteSurfaces = LTNULL;
}


// ---------------------------------------------------------------------- //
// Leeches.
// ---------------------------------------------------------------------- //

// Called when a model hooked to a FileIdentifier gets destroyed.
static LTRESULT ModelInheritDestroy(Nexus *pNexus, Leech *pLeech, int msg, void *pExtraParam)
{
	FileIdentifier *pIdent;
	Model *pModel;

	if(msg == NEXUS_NEXUSDESTROY)
	{
		pIdent = (FileIdentifier*)pLeech->m_pUserData;
		if(pIdent && pIdent->m_pData)
		{
			pModel = (Model*)pIdent->m_pData;

			g_pClientMgr->UnbindModel(pModel, pIdent);
		}
	}

	return LT_OK;
}

LeechDef g_ModelInheritLeechDef =
{
	ModelInheritDestroy,
	&g_BaseLeech
};


// This function is used to update things in the middle of a frame.  This function is called after file IO.
// Streaming sounds need constant attention, so the sounds are updated.
void CClientMgr::Ping() {

}

























