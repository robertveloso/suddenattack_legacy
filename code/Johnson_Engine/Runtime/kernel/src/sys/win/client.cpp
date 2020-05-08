#ifdef LITHTECH_ESD
#include "ltrealaudio_impl.h"
#include "ltrealvideo_impl.h"
#endif // LITHTECH_ESD

#include "bdefs.h"

#include "clientmgr.h"
#include "sysdebugging.h"
#include "input.h"
#include "render.h"
#include <crtdbg.h>
#include <io.h>
#include "iltclient.h"
#include "GameUtil.h"

// Begin minjin. for MS Windows XP SP2 Firewall Setting
#include "NFMngr.h"
// End minjin. 2005. 02. 11.

//------------------------------------------------------------------
//------------------------------------------------------------------
// Holders and their headers.
//------------------------------------------------------------------
//------------------------------------------------------------------

//holder for command line argument mgr interface.
#include "icommandlineargs.h"
static ICommandLineArgs *command_line_args;
define_holder(ICommandLineArgs, command_line_args);

//IClientShell game client shell object.
#include "iclientshell.h"
static IClientShell *i_client_shell;
define_holder(IClientShell, i_client_shell);

//ILTCursor game interface.
#include "iltcursor.h"
static ILTCursor *ilt_cursor;
define_holder(ILTCursor, ilt_cursor);



// Used for finding memory leaks.
int g_iStopAllocCount = -1;


extern char g_SSFile[];
extern LTBOOL g_bNullRender;
extern int32 g_CV_CursorCenter;
extern int32 g_nConsoleLines;
extern LTBOOL g_bConsoleEnable;
extern int32 g_bShowRunningTime;
extern LTBOOL g_CV_HighPriority;
extern int32 g_CV_PlayDemoReps;
extern int32 g_CV_NoDefaultEngineRez;
extern uint32 g_CV_ValidWndMode;
extern int32 g_ClientSleepMS;

uint32 g_CurRunIteration=0;

//
//Stuff for using Launch.DLL
//

#define MAX_NUM_ARGS 150
#define MAX_ARG_LENGTH 200

typedef int (*GetLithtechCommandLineFn)(int32 argc, char **argv, 
    int32 *pOutArgc, char **pOutArgv, int32 maxNumOutArgs, int32 maxOutArgLength);


#define SERVICE_GAMEHI	0
#define SERVICE_CJ		1
#define SERVICE_7ON		2
#define SERVICE_MORIA	3
///////////////////////////////////////////////////
//  Globals.

ClientGlob	g_ClientGlob;
uint32		g_EngineStartMS;
char		g_szRezDir[MAX_RESTREES][128];
int32		g_nService	= SERVICE_GAMEHI;
bool		g_bRunWindowMode = false;


///////////////////////////////////////////////////
//  Functions.


//[MURSUM]
void SetCurrentDirectoryThis( HINSTANCE hInstance )
{
	TCHAR	szModuleName[MAX_PATH];
	TCHAR	szDrive[_MAX_DRIVE];
	TCHAR	szDir[_MAX_DIR];
	TCHAR	szCurrentDir[_MAX_DIR];

	GetModuleFileName( hInstance, szModuleName, MAX_PATH );
	_tsplitpath( szModuleName, szDrive, szDir, LTNULL, LTNULL );

	wsprintf( szCurrentDir, "%s%s", szDrive, szDir );

	SetCurrentDirectory( szCurrentDir );
}

static bool StartClient(ClientGlob *pGlob) 
{
    uint32 initStartTime;
    const TCHAR *resTrees[MAX_RESTREES];
    TCHAR strVersion[32];
    uint32 nResTrees;

	//[MURSUM]
	uint32	i;
	
    short control;
    _asm {  
        fstcw   control      // Get FPU control word
        and control, 0xfcff  // PC field = 00 for single precision  
        fldcw   control 
    }

    // Init the client mugger.
    pGlob->m_bHost = command_line_args->FindArgDash("host") != NULL;
    pGlob->m_pWorldName = command_line_args->FindArgDash("world");

    // Find all the res trees.
    nResTrees = 0;

    for (i = 0; i < command_line_args->Argc() - 1; i++) {
        //check if this argument is -rez
        if (_tcsicmp(command_line_args->Argv(i), "-rez") == 0) {
            //save this parameter.
            resTrees[nResTrees++] = command_line_args->Argv(i + 1);
            if (nResTrees+1 >= MAX_RESTREES) {
                break;
            }
        }
    }

	// Add the default engine resource...
    if (!g_CV_NoDefaultEngineRez) 
	{
        resTrees[nResTrees++] = "engine.rez";
    }

	//[MURSUM]==========================================================
	if( g_nService == SERVICE_CJ )
	{
		//·±Ã³°¡ ¾È ³Ñ°ÜÁÖ´Â rez °­Á¦·Î ·Îµù.
		resTrees[nResTrees++] = "game";
		uint8			nCount = 0;
		_tfinddata_t	fileinfo;
		TCHAR			szFindDir[_MAX_PATH];

		GetCurrentDirectory( _MAX_PATH, szFindDir );
		_tcscat( szFindDir, TEXT("\\game\\*.rez") );

		long nFindHandle = _tfindfirst( szFindDir, &fileinfo );
		
		if( nFindHandle >= 0 )
		{
			do
			{
				if( _tcscmp( fileinfo.name, TEXT(".") ) == 0 ) continue;
				if( _tcscmp( fileinfo.name, TEXT("..") ) == 0 ) continue;
				if( _tcslen( fileinfo.name ) == 0 ) continue;
				if( (fileinfo.attrib&_A_SUBDIR) == _A_SUBDIR ) break;

				wsprintf( g_szRezDir[nCount], TEXT("game\\%s"), fileinfo.name );

				resTrees[nResTrees++] = g_szRezDir[nCount];

				if( nResTrees+1 >= MAX_RESTREES )
				{
					MessageBox( LTNULL, "Rez Overflow!!!!!!!1", "ERROR", 10L );
					break;
				}
				++nCount;

			} while( _tfindnext( nFindHandle, &fileinfo ) == 0 );
		}
	}

	const char*	lpszRezDir = command_line_args->FindArgDash( "rez_dir" );
	if( lpszRezDir )
	{
		uint8			nCount = 0;
		_tfinddata_t	fileinfo;
		TCHAR			szFindDir[_MAX_PATH];

		GetCurrentDirectory( _MAX_PATH, szFindDir );
		_tcscat( szFindDir, TEXT("\\") );
		_tcscat( szFindDir, lpszRezDir );
		_tcscat( szFindDir, TEXT("\\*.rez") );

		long nFindHandle = _tfindfirst( szFindDir, &fileinfo );
		
		if( nFindHandle >= 0 )
		{
			do
			{
				if( _tcscmp( fileinfo.name, TEXT(".") ) == 0 ) continue;
				if( _tcscmp( fileinfo.name, TEXT("..") ) == 0 ) continue;
				if( _tcslen( fileinfo.name ) == 0 ) continue;
				if( (fileinfo.attrib&_A_SUBDIR) == _A_SUBDIR ) break;

				wsprintf( g_szRezDir[nCount], TEXT("%s\\%s"), lpszRezDir, fileinfo.name );

				resTrees[nResTrees++] = g_szRezDir[nCount];

				if( nResTrees+1 >= MAX_RESTREES )
				{
					MessageBox( LTNULL, "Rez Overflow!!!!!!!1", "ERROR", 10L );
					break;
				}
				++nCount;

			} while( _tfindnext( nFindHandle, &fileinfo ) == 0 );
		}
	}
	//===================================================================


    if (command_line_args->FindArgDash("noinput")) 
	{
        pGlob->m_bInputEnabled = LTFALSE;
    }

    const TCHAR *pDir = command_line_args->FindArgDash("workingdir");
    if (pDir) 
	{
        SetCurrentDirectory(pDir);
    }

	//the configuration files that we need to load (they are loaded in order, so the later in
	//the lists override the earlier in the lists)
	static const uint32 knMaxConfigFiles = 16;
	const TCHAR* pszConfigFiles[knMaxConfigFiles];
	uint32 nNumConfigFiles = 0;

	// ±¤½Ä
	//see what the name of the autoexec configuration file is
    const TCHAR* pszAutoExecFileName = command_line_args->FindArgDash("config");
    pszConfigFiles[nNumConfigFiles] = (pszAutoExecFileName) ? pszAutoExecFileName : "autoexec.cfg";
	nNumConfigFiles++;
	
	//see what the name of the display configuration file is
	const TCHAR* pszDisplayFileName = command_line_args->FindArgDash("display");
    pszConfigFiles[nNumConfigFiles] = (pszDisplayFileName) ? pszDisplayFileName : "display.cfg";
	nNumConfigFiles++;

#ifdef USE_ABSTRACT_SOUND_INTERFACES

    //  =======================================================================

    const TCHAR* pcSoundDriverName = command_line_args->FindArg(SOUND_DRIVER_NAME_ARG);
    if (pcSoundDriverName != LTNULL) {
        for (int i = 0; i < SOUND_DRIVER_NAME_LEN && pcSoundDriverName[ i ] != 0; i++) {
            pGlob->m_acSoundDriverName[ i ] = pcSoundDriverName[ i ];
        }
    }
    else {
        pGlob->m_acSoundDriverName[0] = 0;
    }

    //  =======================================================================

#endif  // USE_ABSTRACT_SOUND_INTERFACES

    initStartTime = timeGetTime();
    if (g_pClientMgr->Init(resTrees, nResTrees, nNumConfigFiles, pszConfigFiles) != LT_OK) {
        return LTFALSE;
    }
  
    g_pClientMgr->m_VersionInfo.GetString(strVersion, sizeof(strVersion));
    DebugOut("LithTech build %s initialized in %.2f seconds.\n", 
        strVersion, (float)(timeGetTime() - initStartTime) / 1000.0f);

    return LTTRUE;
}


static long WINAPI MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    ClientGlob *pGlob;
	/*
	char fileName[_MAX_PATH + 1];
    int i;
    FILE *fp;
	*/
    PAINTSTRUCT ps;
    WPARAM wSysParam;

    pGlob = &g_ClientGlob;
        
    if (pGlob->m_bProcessWindowMessages && g_pClientMgr) 
	{
        switch(message) 
		{
            case WM_PAINT: 
			{
                BeginPaint(hwnd, &ps);
                EndPaint(hwnd, &ps);
                return 0;
            }
			break;

#ifndef _FINAL
			case WM_SYSKEYDOWN:
			{
				//	if the console is disabled allow it to come down if the user hits alt-tilde and
				//	if the Developer cvar exists and is == 1
				if (wParam != 192 || ((lParam & (1<<29)) == 0)) { }
				else if (g_ClientGlob.m_bIsConsoleUp == FALSE) {
					g_ClientGlob.m_bIsConsoleUp = !g_ClientGlob.m_bIsConsoleUp;
					if (!g_ClientGlob.m_bIsConsoleUp) {
						if (g_pClientMgr && g_pClientMgr->m_InputMgr)
							g_pClientMgr->m_InputMgr->FlushInputBuffers(g_pClientMgr->m_InputMgr); } }
			}
			break;
#endif

            case WM_KEYDOWN: 
			{
#ifndef _FINAL
                if (wParam == 192)
				{
					// Tilde key..
					if (dsi_IsConsoleEnabled () == FALSE)
					{
						//	if the console is disabled, but it's up, hitting tilde will turn it off
						if (g_ClientGlob.m_bIsConsoleUp == TRUE)
							g_ClientGlob.m_bIsConsoleUp = FALSE;

						//	if the lithtech console is disabled, just add tilde as a normal keypress
						if( g_ClientGlob.m_nKeyDowns < MAX_KEYBUFFER )
						{
							g_ClientGlob.m_KeyDowns[g_ClientGlob.m_nKeyDowns] = wParam;
							g_ClientGlob.m_KeyDownReps[g_ClientGlob.m_nKeyDowns] = lParam & 0x7FFF;
							++g_ClientGlob.m_nKeyDowns;
						}
					}
					else
					{
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_CONTROL) & 0x8000) 
						{
							g_nConsoleLines = 0;
						}
						else if (GetAsyncKeyState(VK_SHIFT) & 0x8000) 
						{
							++g_nConsoleLines;
						}
						else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) 
						{
							--g_nConsoleLines;
						}
						else 
						{
							if (g_bConsoleEnable || g_ClientGlob.m_bIsConsoleUp) 
							{
								g_ClientGlob.m_bIsConsoleUp = !g_ClientGlob.m_bIsConsoleUp;
								if (!g_ClientGlob.m_bIsConsoleUp) 
								{
									if (g_pClientMgr && g_pClientMgr->m_InputMgr) 
									{
										g_pClientMgr->m_InputMgr->FlushInputBuffers(g_pClientMgr->m_InputMgr);
									}
								}
							}
						}
					}

					g_nConsoleLines = LTCLAMP(g_nConsoleLines, 0, 90);
                }
				else if(wParam == VK_F9 )
				{
					if( g_pClientMgr ) g_pClientMgr->ToggleFullScreen();
				}
                else 
#endif // #ifndef __FINAL
				{
                    if (g_ClientGlob.m_nKeyDowns < MAX_KEYBUFFER) 
					{
                        g_ClientGlob.m_KeyDowns[g_ClientGlob.m_nKeyDowns] = wParam;
                        g_ClientGlob.m_KeyDownReps[g_ClientGlob.m_nKeyDowns] = lParam & 0x7FFF;
                        ++g_ClientGlob.m_nKeyDowns;
                    }
                }
                
                return 0;
            }
            
            case WM_KEYUP: 
			{
				/*
                if (wParam == VK_F8) 
				{
                    if (r_GetRenderStruct()) 
					{
                        // Get a free filename.
                        for (i=0; i < 3000; i++) 
						{
                            LTSNPrintF(fileName, sizeof(fileName), "%s%d.bmp", g_SSFile, i);
                            fp = fopen(fileName, "rb");
                            if (fp) 
							{
                                fclose(fp);
                            }
                            else 
							{
                                r_GetRenderStruct()->MakeScreenShot(fileName);
                                break;
                            }
                        }
                    }
                }
                else 
				*/
				{
                    if (g_ClientGlob.m_nKeyUps < MAX_KEYBUFFER) 
					{
                        g_ClientGlob.m_KeyUps[g_ClientGlob.m_nKeyUps++] = wParam;
                    }
                }
                
                return 0;
            }
			break;

            case WM_CHAR: 
			{
                return 0;
            }
			break;
            
            case WM_ERASEBKGND: 
			{
                // Ignore this message if the renderer is initialized.
                if (r_IsRenderInitted())
                    return 0;
            }
            break;

            case WM_ACTIVATEAPP: 
			{
				if( g_CV_ValidWndMode ) 
				{
					if(wParam) 
					{
						g_ClientGlob.m_bClientActive = LTTRUE;
#ifdef _FINAL
						g_ClientSleepMS = 0;
#else
						g_ClientSleepMS = 1;
#endif

						if (g_ClientGlob.m_bLostFocus) 
						{
							ShowWindow( g_ClientGlob.m_hMainWnd, SW_SHOWNORMAL );
							RECT Rect;
							GetWindowRect( g_ClientGlob.m_hMainWnd, &Rect );
							SetWindowPos(g_ClientGlob.m_hMainWnd, HWND_TOP, 0, 0,
								Rect.right-Rect.left, 
								Rect.bottom-Rect.top,
								SWP_NOMOVE); 
							ShowCursor(FALSE);						
							if( false == g_bRunWindowMode ) ClipCursor( &Rect );

							if (ilt_cursor != NULL) 
							{
								ilt_cursor->RefreshCursor();
							}
						}

						g_pClientMgr->ClearInput();
						g_ClientGlob.m_bLostFocus = LTFALSE;						
					}
					else if( !g_ClientGlob.m_bLostFocus )
					{
						g_ClientGlob.m_bLostFocus = LTTRUE;	
						g_ClientSleepMS = 10;
						
						g_pClientMgr->ClearInput();

						if( false == g_bRunWindowMode ) ShowWindow( g_ClientGlob.m_hMainWnd, SW_MINIMIZE );	
						ShowCursor(TRUE);
						ClipCursor( NULL );
					}
				}
				else
				{
                    if (wParam) 
					{
                        if (g_ClientGlob.m_bLostFocus) 
						{
                            // Tell the client
                            i_client_shell->OnEvent(LTEVENT_GAINEDFOCUS, 0);

                            if (!g_ClientGlob.m_bRendererShutdown) 
							{
                                // Restore the video mode.
                                OutputDebugString("Regained focus.. initializing renderer.\n");

                                if (r_InitRender(&g_RMode) != LT_OK) 
								{
                                    dsi_OnClientShutdown("Init Render Error");
                                }
#ifdef LITHTECH_ESD
								if (g_pClientMgr->m_pRealAudioMgr)
									g_pClientMgr->m_pRealAudioMgr->AppFocus(LTTRUE);
								if (g_pClientMgr->m_pRealVideoMgr)
									g_pClientMgr->m_pRealVideoMgr->AppFocus(LTTRUE);
#endif // LITHTECH_ESD
                            }

                            if (GetClientILTSoundMgrImpl()->IsValid()) 
							{
                                GetClientILTSoundMgrImpl()->ReacquireDigitalHandle();
                            }
                        }

                        g_pClientMgr->ClearInput();
                        g_ClientGlob.m_bLostFocus = LTFALSE;
                        g_ClientGlob.m_bClientActive = LTTRUE;		
#ifdef _FINAL
						g_ClientSleepMS = 0;
#else
						g_ClientSleepMS = 1;
#endif

                        if (ilt_cursor != NULL) 
						{
                            ilt_cursor->RefreshCursor();
                        }
                    }
                    else if( !g_ClientGlob.m_bLostFocus )
					{
                        g_ClientGlob.m_bLostFocus = LTTRUE;
                        g_ClientGlob.m_bClientActive = LTFALSE;
						g_ClientSleepMS = 10;

                        // Tell the client
                        i_client_shell->OnEvent(LTEVENT_LOSTFOCUS, 0);

                        OutputDebugString("Losing focus.. shutting down renderer.\n");
                        r_TermRender(1, false);

                        g_pClientMgr->ClearInput();

                        if (!g_ClientGlob.m_bAppClosing && GetClientILTSoundMgrImpl()->IsValid()) 
						{
                            GetClientILTSoundMgrImpl()->ReleaseDigitalHandle();
                        }
                    }
                }            
                return (message == WM_NCACTIVATE) ? TRUE : 0;
            }
			break;

            case WM_SYSCOMMAND: 
			{
                wSysParam = wParam & 0xFFF0;

                if (wSysParam == SC_KEYMENU)
				{
                    return LTTRUE;
                }

                if (wSysParam == SC_SCREENSAVE || (wParam & 0xFFF0) == SC_MONITORPOWER) {
                    if (!IsIconic(hwnd)) 
					{
                        return LTTRUE;
                    }
                }

                if (wSysParam == SC_CLOSE) 
				{   
                    g_ClientGlob.m_bAppClosing = LTTRUE;

                    // Kill the music and sound drivers...
                    if (GetClientILTSoundMgrImpl()->IsValid())
                        GetClientILTSoundMgrImpl()->StopAllSounds();
                    if (GetMusicMgr() && GetMusicMgr()->m_bValid)
                        GetMusicMgr()->Stop(MUSIC_IMMEDIATE);
                }
            }
			break;

			case WM_DESTROY: 
			{
                dsi_OnClientShutdown(LTNULL);
				ShowCursor(TRUE);
				ClipCursor( NULL );
                return 0;
            }
			break;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}


int LTAllocHook(int allocType, void *userData, size_t size, int blockType, 
   long requestNumber, const unsigned char *filename, int lineNumber)
{
    static int allocCount=0;

    if (allocType == _HOOK_ALLOC) {
        ++allocCount;
    }

    if (allocCount == g_iStopAllocCount) {
        ASSERT(LTFALSE);
    }

    return LTTRUE;
}

int RunClientApp(HINSTANCE hInstance) {
    MSG         msg;
    WNDCLASS    wndclass;
    ClientGlob  *pGlob;
    const char      *pArg;
    RECT screenRect, wndRect;
    int status, nExitValue;
    LTRESULT dResult;
    LTBOOL bOutOfMemory, bPrevHighPriority;
	const char* lpszService;

    _CrtSetAllocHook(LTAllocHook);

    nExitValue = 0;

    pGlob = &g_ClientGlob;

    // Init the globals.
    memset(pGlob, 0, sizeof(*pGlob));
	pGlob->m_bConsoleEnabled = true;

    if (command_line_args->FindArgDash("DebugStructBanks")) {
        g_bDebugStructBanks = LTTRUE;
    }

    // Set the jump-to position for memory errors...
    bOutOfMemory = LTFALSE;
    if (setjmp(pGlob->m_MemoryJmp) != 0) {
        bOutOfMemory = LTTRUE;
        goto END_MAINLOOP;
    }

    // Set the working directory.
    pArg = command_line_args->FindArgDash("workingdir");
    if (pArg) {
        SetCurrentDirectory(pArg);
    }

    pGlob->m_bInputEnabled = LTTRUE;

    // Initialize the system-dependent modules.
    status = dsi_Init();
    if (status != 0)
	{
        if (status == 1)
		{
            MessageBox(LTNULL, "COM°´Ã¼ ÃÊ±âÈ­¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù.", "Error", MB_OK);
            dsi_Term();
            return -1;
        }
        else if( status == 2 )
		{
			//±×³É ¹«½Ã?
		}
		else
		{
            MessageBox(LTNULL, "Unknown error initializing engine.", "Error", MB_OK);
            dsi_Term();
            return -1;
        }
    }

    // Initialize the client.
    pGlob->m_hInstance = hInstance;
    pGlob->m_WndClassName = "__GH_Sudden_Attack__";
    cm_Init();
    pGlob->m_bClientActive = LTTRUE;

    pGlob->m_WndCaption = command_line_args->FindArgDash("windowtitle");
    if (!pGlob->m_WndCaption) {
        pGlob->m_WndCaption = "SuddenAttack";
    }

    pGlob->m_bBreakOnError = command_line_args->FindArgDash("breakonerror") != NULL;


    // Create the main window.
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndclass.lpfnWndProc   = MainWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = pGlob->m_hInstance;
    wndclass.hIcon         = LoadIcon (LTNULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (LTNULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject (BLACK_BRUSH);
    wndclass.lpszMenuName  = LTNULL;
    wndclass.lpszClassName = pGlob->m_WndClassName;

    RegisterClass (&wndclass);

    GetWindowRect(GetDesktopWindow(), &screenRect);

	pGlob->m_hMainWnd = CreateWindow (pGlob->m_WndClassName,			// window class name
                pGlob->m_WndCaption,									// window caption
                WS_POPUP,		// window style
				0,		// initial x position
                0,		// initial y position
                800,													// initial x size
                600,													// initial y size
                LTNULL,													// parent window handle
                LTNULL,													// window menu handle
                pGlob->m_hInstance,										// program instance handle
                LTNULL);

	lpszService = command_line_args->FindArgPlus("service");
	if( lpszService )
	{
		if( stricmp( lpszService, "cj_internet" ) == 0 )
		{
			if( g_pClientMgr ) g_pClientMgr->GetCookieFromClipboard( pGlob->m_hMainWnd );
			g_nService = SERVICE_CJ;
		}
	}

#ifndef _FINAL
	lpszService = command_line_args->FindArgPlus("windowed");
	if( lpszService )
	{
		if( stricmp( lpszService, "1" ) == 0 )
		{
			g_bRunWindowMode = true;
		}
	}
#endif

    bPrevHighPriority = LTFALSE;
    if (StartClient(pGlob)) 
	{
		pGlob->m_bProcessWindowMessages = LTTRUE;

		if (g_CV_HighPriority)
		{
			SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
		}

        for (;;) {
            dResult = g_pClientMgr->Update();
            if (dResult != LT_OK) {
                goto END_MAINLOOP;
            }

			/*
            // Give our process high priority?
            if (g_CV_HighPriority != bPrevHighPriority) 
			{
                if (g_CV_HighPriority)
				{
                    dsi_ConsolePrint("Setting process to high priority");
                    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
                }
                else 
				{
                    dsi_ConsolePrint("Setting process to normal priority");
                    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
                }
            }
			*/

            if (g_bShowRunningTime) {
                dsi_PrintToConsole("Running for %.1f seconds", (float)(timeGetTime() - g_EngineStartMS) / 1000.0f);
            }

            // Center the mouse in the window.
            if (g_CV_CursorCenter && !g_ClientGlob.m_bLostFocus) {
                GetWindowRect(pGlob->m_hMainWnd, &wndRect);
                // [BL 2/2/2000] - center the cursor in the SCREEN, not the window -- otherwise it breaks
                // when you play in a small window on a big screen
				SetCursorPos((wndRect.right - wndRect.left) / 2, (wndRect.bottom - wndRect.top) / 2);
            }
            
            while (PeekMessage(&msg, LTNULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    nExitValue = msg.wParam;
                    goto END_MAINLOOP;
                }
                
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

END_MAINLOOP:;

	/*
    // Check for error message to post...
    if (pGlob->m_ExitMessage[0]) {
        r_TermRender(2, true);
        dsi_MessageBox(pGlob->m_ExitMessage, pGlob->m_WndCaption);
    }
	*/

    pGlob->m_bProcessWindowMessages = LTFALSE;
    
    // For some odd reason we have to do this here or it crashes in 
    // IDirectDraw::RestoreDisplayMode if the user didn't alt+f4 out.
    g_pClientMgr->Term();
    delete g_pClientMgr;
    g_pClientMgr = LTNULL;

    DestroyWindow(pGlob->m_hMainWnd);

    dsi_Term();

    if (bOutOfMemory) {
        ShowWindow(pGlob->m_hMainWnd, SW_HIDE);
        MessageBox(GetDesktopWindow(), "Out of memory", pGlob->m_WndCaption, MB_OK);
    }

    LTBOOL bShowMemLeaks = LTFALSE;
    if (bShowMemLeaks) {
        _CrtDumpMemoryLeaks();
    }

    return nExitValue;
}

//----------------------------------------------------------------------------------
// Command line parsing: This uses the following rules as taken from the MSDN
// guidelines for parsing command line parameters
//
//Arguments are delimited by white space, which is either a space or a tab.
//
//The caret character (^) is not recognized as an escape character or delimiter. The character is 
//handled completely by the command-line parser in the operating system before being passed to the 
//argv array in the program.
// - Note, this is not handled
//
//A string surrounded by double quotation marks ("string") is interpreted as a single argument, 
//regardless of white space contained within. A quoted string can be embedded in an argument.
//
//A double quotation mark preceded by a backslash ( \") is interpreted as a literal double quotation 
//mark character (").
//
//Backslashes are interpreted literally, unless they immediately precede a double quotation mark.
//
//If an even number of backslashes is followed by a double quotation mark, one backslash is placed 
//in the argv array for every pair of backslashes, and the double quotation mark is interpreted as 
//a string delimiter.
//
//If an odd number of backslashes is followed by a double quotation mark, one backslash is placed 
//in the argv array for every pair of backslashes, and the double quotation mark is “escaped” by the 
//remaining backslash, causing a literal double quotation mark (") to be placed in argv. 
//

//this will find the beginning of an argument. If it cannot find another one, it will return the
//length of the string
static bool FindArgumentStart(const char* pszCommandLine, uint32& nStrOffset)
{
	//just eat whitespace until we hit a non-whitespace character or the end of the string
	while((pszCommandLine[nStrOffset] != '\0') && isspace(pszCommandLine[nStrOffset]))
		nStrOffset++;

	return (pszCommandLine[nStrOffset] != '\0');
}

//this will find the end of an argument, converting it into a string
static void AddArgumentChar(char* pszBuffer, uint32 nBufferLen, uint32 nBufferPos, char ch)
{
	if(nBufferLen == 0)
		return;

	if(nBufferLen == 1)
	{
		pszBuffer[0] = '\0';
		return;
	}

	if(nBufferPos < nBufferLen - 1)
	{
		pszBuffer[nBufferPos]		= ch;
		pszBuffer[nBufferPos + 1]	= '\0';
	}
}

//this will find the end of an argument, converting it into a string
static void RepAddArgumentChar(char* pszBuffer, uint32 nBufferLen, uint32 nBufferPos, char ch, uint32 nNumReps)
{
	for(uint32 nRep = 0; nRep < nNumReps; nRep++)
		AddArgumentChar(pszBuffer, nBufferLen, nBufferPos + nRep, ch);
}


//this will find the end of an argument, converting it into a string, and returning how long the string
//actually is along with whether or not it hit the end of the file
static bool ParseArgument(const char* pszCommandLine, char* pszBuffer, uint32 nBufferLen, uint32& nActualBufferLen, uint32& nStrOffset)
{
	//keep track of our parsing status
	uint32	nSlashCount			= 0;
	bool	bInQuotes			= false;

	//keep track of how large our string is
	nActualBufferLen = 0;

	while(1)
	{
		//see what our current character is
		char ch = pszCommandLine[nStrOffset];

		//see if we have a slash, we have to hold off processing these until we hit a non-slash
		//character, we can then decide what to do
		if(ch == '\\')
		{
			//slashes cannot be output directly because of their odd rules
			nSlashCount++;
			nStrOffset++;
			continue;
		}

		//lets handle quotes now
		if(ch == '\"')
		{
			//output one slash for every two slashes
			uint32 nNumOutputSlashes = nSlashCount / 2;
			RepAddArgumentChar(pszBuffer, nBufferLen, nActualBufferLen, '\\', nNumOutputSlashes);
			nActualBufferLen += nNumOutputSlashes;

			//only interpret it as a string delimiter if the slashes are even
			if((nSlashCount % 2) == 0)
			{
				bInQuotes = !bInQuotes;
				nStrOffset++;
				nSlashCount = 0;
				continue;
			}
			nSlashCount = 0;
		}
		else
		{
			//just output the slashes normally
			RepAddArgumentChar(pszBuffer, nBufferLen, nActualBufferLen, '\\', nSlashCount);
			nActualBufferLen += nSlashCount;
			nSlashCount = 0;
		}

		//have we hit whitespace?
		if(isspace(ch) && !bInQuotes)
			return true;

		//have we hit the end of the string?
		if(ch == '\0')
			return false;

		//alright, we have a character we want to output to the string, do so
		AddArgumentChar(pszBuffer, nBufferLen, nActualBufferLen, ch);
		nActualBufferLen++;

		nStrOffset++;
	}

	return false;
}

//---------------------------------------------------------------------------------------------

//Given the name of a file, and an existing command line, it will remove the text chunk in the passed
//in range and replace it with the file. It will then return an orphaned pointer to that string
char* InsertCommandFile(const char* pszCmdFile, const char* pszCurrCmdLine, uint32 nArgBegin, uint32 nArgEnd)
{
	//open up the file and read in its contents
	FILE* fp = fopen(pszCmdFile, "rb");
	if ( !fp )
	{
		char pszErrorMsg[512];
		LTSNPrintF(pszErrorMsg, sizeof(pszErrorMsg), "Error opening the command file %s for reading", pszCmdFile);
		MessageBox(LTNULL, pszErrorMsg, "Error", MB_OK);
		return NULL;
	}

	//determine the size of the file
	fseek(fp, 0, SEEK_END);
	uint32 nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//allocate a new buffer to hold this file
	uint32 nNewBufferSize = nFileSize;

	//add in the old string
	uint32 nCurrCmdLineLen = strlen(pszCurrCmdLine);
	nNewBufferSize += nCurrCmdLineLen;

	//and remove the section that held the -cmdfile
	nNewBufferSize -= nArgEnd - nArgBegin;

	//allocate a new buffer
	char* pszBuffer;
	LT_MEM_TRACK_ALLOC(pszBuffer = new char[nNewBufferSize + 1], LT_MEM_TYPE_MISC);

	//check the allocation
	if(!pszBuffer)
	{
		fclose(fp);
		return NULL;
	}

	//now copy over the beginning of the old command line
	memcpy(pszBuffer, pszCurrCmdLine, nArgBegin);

	//and read in our data from the file
	fread(&pszBuffer[nArgBegin], nFileSize, 1, fp);
	fclose(fp);

	//and finally, copy the rest of the data from the old command string
	memcpy(&pszBuffer[nArgBegin + nFileSize], &pszCurrCmdLine[nArgEnd], nCurrCmdLineLen - nArgEnd);

	//make sure to end this string
	pszBuffer[nNewBufferSize] = '\0';

	//success, return our buffer
	return pszBuffer;
}


//called to run through, build up the final command string and setup the global command line
//argument object
bool SetupArgs(const char* pszCommandLine) 
{
	//copy the command line parameters since we assume we are working with an internally allocated
	//string
	char* pszCurrCmdLine = NULL;
	LT_MEM_TRACK_ALLOC(pszCurrCmdLine = new char[strlen(pszCommandLine) + 1], LT_MEM_TYPE_MISC);

	if(pszCurrCmdLine)
		strcpy(pszCurrCmdLine, pszCommandLine);
	else
		return false;

	//we first need to build up the command line, this is done by going through, looking for
	//-cmdfile, and upon finding this, expanding the file into the command string

	//this is the number of arguments that we have
	uint32 nArgCount = 0;

	//this is the total string length of the arguments
	uint32 nTotalStrLen = 0;

	//flag indicating we want to continue parsing, we always want to go at least once
	bool bContinueParsing = true;

	//we always want to take at least one pass through
	while(bContinueParsing)
	{
		//disable parsing, if something wants to continue it can
		bContinueParsing = false;

		//run through and try and find a -cmdfile
		uint32 nCurrPos = 0;

		//reset our counts again
		nArgCount = 0;
		nTotalStrLen = 0;

		//go through all of our arguments (the loop will bail when no more arguments are found)
		while(1)
		{
			if(!FindArgumentStart(pszCurrCmdLine, nCurrPos))
				break;

			//save the starting position so we can remove it later
			uint32 nArgStart = nCurrPos;


			//see what this command is
			char pszArgBuffer[_MAX_PATH + 1];
			uint32 nActualLen;

			ParseArgument(pszCurrCmdLine, pszArgBuffer, sizeof(pszArgBuffer), nActualLen, nCurrPos);
			
			//update our argument count
			nArgCount++;
			nTotalStrLen += nActualLen + 1;

			//see if we have hit a command file argument
			if(stricmp(pszArgBuffer, "-cmdfile") == 0)
			{
				//we have, see what the file is
				if(!FindArgumentStart(pszCurrCmdLine, nCurrPos))
					break;

				ParseArgument(pszCurrCmdLine, pszArgBuffer, sizeof(pszArgBuffer), nActualLen, nCurrPos);

				//update our argument count
				nArgCount++;
				nTotalStrLen += nActualLen + 1;

				//alright, we have a command file and a command line, so we need to remove the old
				//-cmdfile [] and replace it with the contents of the file
				char* pszNewCmdLine = InsertCommandFile(pszArgBuffer, pszCurrCmdLine, nArgStart, nCurrPos);

				//free the old command line
				delete [] pszCurrCmdLine;

				//assign it over
				pszCurrCmdLine = pszNewCmdLine;

				//check the allocation
				if(!pszCurrCmdLine)
					return false;

				//we need to restart parsing since everything is invalidated, but we need to run
				//through again to get accurate counts and all -cmdfile
				bContinueParsing = true;

				break;
			}
		}
	}
	
	//alright, we need to allocate our buffers, and take one more pass through building our final
	//parameter list
	char* pszArgBuffer;
	char** pszArgV;
	LT_MEM_TRACK_ALLOC(pszArgBuffer = new char[nTotalStrLen], LT_MEM_TYPE_MISC);
	LT_MEM_TRACK_ALLOC(pszArgV = new char*[nArgCount], LT_MEM_TYPE_MISC);

	//check the allocations
	if(!pszArgBuffer || !pszArgV)
	{
		delete [] pszArgBuffer;
		delete [] pszArgV;
		delete [] pszCurrCmdLine;
		return false;
	}

	//alright, now parse through building our final lists
	uint32 nCurrPos = 0;
	uint32 nFinalBufferPos = 0;
	uint32 nCurrArg = 0;

	//go through all of our arguments (the loop will bail when no more arguments are found)
	while(1)
	{
		if(!FindArgumentStart(pszCurrCmdLine, nCurrPos))
			break;

		//see what this command is
		uint32 nActualLen;
		ParseArgument(pszCurrCmdLine, &pszArgBuffer[nFinalBufferPos], nTotalStrLen - nFinalBufferPos, nActualLen, nCurrPos);

		//now update our buffer position
		pszArgV[nCurrArg] = &pszArgBuffer[nFinalBufferPos];
		nFinalBufferPos += nActualLen + 1;
		nCurrArg++;
	}

	//sanity check
	assert(nFinalBufferPos == nTotalStrLen);
	assert(nCurrArg == nArgCount);
	
    //set up the command line arg mgr with the default arguements passed into the exe.
    command_line_args->Init(nArgCount, pszArgV);

	//and now clean up
	delete [] pszArgBuffer;
	delete [] pszArgV;
	delete [] pszCurrCmdLine;

    //success.
    return true;
}



extern "C" int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) 
{
	SetCurrentDirectoryThis( hInstance );
	
	LTMemInit();

    g_EngineStartMS = timeGetTime();
    
    // Setup our command line args.
    if (SetupArgs(lpszCmdParam) == false) 
	{
        return -1;
    }
	
	BOOL bInitialized = FALSE;

	const char* lpTemp = command_line_args->FindArg( "+UserID" );
	if( lpTemp )
	{
		bInitialized = MursumCrashReport_Install( lpTemp, "mursum@gamehi.net" );
	}
	else
	{
		char	szTemp[32];
		sprintf( szTemp, "%04d_Sudden", GetTickCount()%10000 );
		bInitialized = MursumCrashReport_Install( szTemp, "mursum@gamehi.net" );
	}
	
	MursumUT_SetDumpDir( "EngineInitErr" );
	//////////////////////////////////////////////////////////////////////////
	

	// ÀÎ¶óÀÎ ÇÔ¼ö·Î Á¦°øµÇ´Â, DLL ¹× ÇÔ¼öÆ÷ÀÎÅÍ ÃÊ±âÈ­
	NF_Initialize();
	
	WCHAR strGameExeFullPath[MAX_PATH];
	WCHAR* strFriendlyAppName = _FIREWALL_EXCEPTION_LIST_NAME_;
	GetModuleFileNameW( NULL, /*(char*)*/strGameExeFullPath, MAX_PATH ); 
	
	NF_Start( strGameExeFullPath, strFriendlyAppName );		// Firewall ¸ðµâ »ý¼º
	//////////////////////////////////////////////////////////////////////////
	

    //run the client once.
    int ret = RunClientApp(hInstance);

    //check if we should run more than once.
    if (g_CV_PlayDemoReps > 1) {
        //decrement the count.
        ++g_CurRunIteration;

        //keep running it.
        while (g_CurRunIteration < (uint32)g_CV_PlayDemoReps) {
            //decrement the count.
            ++g_CurRunIteration;

            //run the client again.
            ret = RunClientApp(hInstance);
        }
    }

	//////////////////////////////////////////////////////////////////////////
	NF_End( strGameExeFullPath );							// Firewall ¿¹¿Ü ¸ñ·Ï¿¡¼­ ÇÁ·Î¼¼½º Á¦°Å
	NF_Release();											// DLL Release
	//////////////////////////////////////////////////////////////////////////

	MursumCrashReport_UnInstall();

    return ret;
}
