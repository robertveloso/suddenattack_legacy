//------------------------------------------------------------------
//
//	FILE	  : ConsoleCommands.cpp
//
//	PURPOSE	  : Implements the console command handler.
//
//	CREATED	  : May 7 1997
//
//	COPYRIGHT : LithTech Inc., 1996-2000
//
//------------------------------------------------------------------

// Includes....
#include "bdefs.h"

#include "concommand.h"
#include "sysconsole_impl.h"
#include "clientshell.h"
#include "client_filemgr.h"
#include "clientmgr.h"
#include "sysinput.h"
#include "consolecommands.h"
#include "iltinfo.h"
#include "iltclient.h"
#include "dhashtable.h"

#ifndef __PS2
    #include "render.h"
#endif



#define INPUTMGR g_pClientMgr->m_InputMgr


// The main console state.
ConsoleState g_ClientConsoleState;

// The client console iterator
//	Note : This iterator is forward only
class CClientConIterator : public CConIterator
{
protected:
	enum EState {
		STATE_COMMAND = 0,
		STATE_CONVAR = 1,
		STATE_ENGINEVAR = 2
	};
	EState m_eState;
	int m_iCommandIndex;
	HHashIterator *m_hVarIndex;
	HHashElement *m_hCurVar;
	int m_iEngineIndex;

	virtual bool Begin();
	virtual bool NextItem();

public:
	CClientConIterator();
	virtual ~CClientConIterator();

	virtual const char *Get() const;
};
CClientConIterator	g_ClientConIterator;

// From engine_vars.cpp.
extern LTEngineVar* GetEngineVars();
extern int GetNumEngineVars();
extern char g_SSFile[];
extern LTBOOL g_bUpdateServer;
extern int32 g_ScreenWidth, g_ScreenHeight;
extern int32 g_CV_ConsoleLeft;
extern int32 g_CV_ConsoleTop;
extern int32 g_CV_ConsoleRight;
extern int32 g_CV_ConsoleBottom;


//------------------------------------------------------------------
//------------------------------------------------------------------
// All the command handlers.
//------------------------------------------------------------------
//------------------------------------------------------------------

extern int32 g_DebugLevel;  // From bdefs.cpp.
extern LTVector g_ConsoleModelAdd;
extern LTVector g_ConsoleModelDirAdd;
extern float g_CV_DropChance;




//------------------------------------------------------------------
static void con_BlastServer(int argc, char *argv[])
{
#ifdef _DEBUG
	int i, count;
	CPacket packet;

	if(g_pClientMgr && g_pClientMgr->m_pCurShell)
	{
		if(argc >= 1)
			count = atoi(argv[0]);
		else
			count = 500;

		for(i=0; i < count; i++)
		{
			packet.SetPacketID(CMSG_TEST);
			g_pClientMgr->m_NetMgr.SendPacket(&packet,
				g_pClientMgr->m_pCurShell->m_HostID, MESSAGE_GUARANTEED);
			packet.ResetWrite();
		}
		dsi_ConsolePrint("Blasted server with %d guaranteed packets", count);
	}


#endif	
}


//------------------------------------------------------------------
static void con_ModelAdd(int argc, char *argv[])
{
	if(argc >= 3)
	{
		g_ConsoleModelAdd.x = (float)atof(argv[0]);
		g_ConsoleModelAdd.y = (float)atof(argv[1]);
		g_ConsoleModelAdd.z = (float)atof(argv[2]);

		g_ConsoleModelAdd.x = LTCLAMP(g_ConsoleModelAdd.x, 0.0f, 250.0f);
		g_ConsoleModelAdd.y = LTCLAMP(g_ConsoleModelAdd.y, 0.0f, 250.0f);
		g_ConsoleModelAdd.z = LTCLAMP(g_ConsoleModelAdd.z, 0.0f, 250.0f);
	}
}


//------------------------------------------------------------------
static void con_ModelDirAdd(int argc, char *argv[])
{
	if(argc >= 3)
	{
		g_ConsoleModelDirAdd.x = (float)atof(argv[0]);
		g_ConsoleModelDirAdd.y = (float)atof(argv[1]);
		g_ConsoleModelDirAdd.z = (float)atof(argv[2]);

		g_ConsoleModelDirAdd.x = LTCLAMP(g_ConsoleModelDirAdd.x, 0.0f, 250.0f);
		g_ConsoleModelDirAdd.y = LTCLAMP(g_ConsoleModelDirAdd.y, 0.0f, 250.0f);
		g_ConsoleModelDirAdd.z = LTCLAMP(g_ConsoleModelDirAdd.z, 0.0f, 250.0f);
	}
}


static void con_DoWorldCommand(char *pWorldName, char *pRecordFilename)
{
	CClientShell *pShell;
	char str[200], testName[200];
	FileRef ref;
	StartGameRequest request;


	memset(&request, 0, sizeof(request));

	// If the world doesn't exist, prefix it with 'worlds\\'.

	if (pWorldName[strlen(pWorldName)-4] != '.') 
	{
#ifdef __PS2
		sprintf(testName, "%s.ltb", pWorldName);
#else
		sprintf(testName, "%s.dat", pWorldName);
#endif
	}
	else {
		sprintf(testName, "%s", pWorldName);
	}
	
	ref.m_pFilename = testName;
	ref.m_FileType = FILE_ANYFILE;
	if(!cf_GetFileIdentifier(g_pClientMgr->m_hFileMgr, &ref, TYPECODE_WORLD))
	{
		if (pWorldName[strlen(pWorldName)-4] != '.') 
		{
#ifdef __PS2
			sprintf(testName, "%s.ltb", pWorldName);
#else
			sprintf(testName, "%s.dat", pWorldName);
#endif
		}
		else {
			sprintf(testName, "%s", pWorldName);
		}
		//sprintf(testName, "worlds\\%s.ltb", pWorldName);
		ref.m_pFilename = testName;
		if(cf_GetFileIdentifier(g_pClientMgr->m_hFileMgr, &ref, TYPECODE_WORLD))
		{
			sprintf(testName, "worlds\\%s", pWorldName);
			pWorldName = testName;
		}
	}

	// Add the record info..
	if(pRecordFilename)
	{
		SAFE_STRCPY(request.m_RecordFilename, pRecordFilename);
	}

	pShell = g_pClientMgr->m_pCurShell;
	if(pShell && (pShell->m_ShellMode == STARTGAME_NORMAL || pShell->m_ShellMode == STARTGAME_HOST) &&
		request.m_RecordFilename[0] != 0)
	{
		sprintf(str, "world %s", pWorldName);
		pShell->SendCommandToServer(str);
	}
	else
	{
		request.m_Type = STARTGAME_NORMAL;
		strncpy(request.m_WorldName, pWorldName, MAX_SGR_STRINGLEN);
		g_pClientMgr->StartShell(&request);
	}
}


//------------------------------------------------------------------
static void con_World(int argc, char *argv[])
{
	if(argc >= 1)
	{
		con_DoWorldCommand(argv[0], LTNULL);
	}
	else
	{
		dsi_ConsolePrint("World <world name>");
	}
}


//------------------------------------------------------------------
static void con_TimeDemo(int argc, char *argv[])
{
	char str[256];
	StartGameRequest request;

	if(argc >= 1)
	{
		if(!g_pClientMgr->m_pCurShell)
		{
			// Start a local server to run it.
			memset(&request, 0, sizeof(request));
			request.m_Type = STARTGAME_NORMAL;
			g_pClientMgr->StartShell(&request);

			if(!g_pClientMgr->m_pCurShell)
			{
				dsi_ConsolePrint("Unable to start a server to run demo.");
				return;
			}
		}

		sprintf(str, "timedemo %s", argv[0]);
		g_pClientMgr->m_pCurShell->SendCommandToServer(str);
	}
	else
	{
		dsi_ConsolePrint("TimeDemo <record filename (w/o extension)>");
	}
}




//------------------------------------------------------------------
static void con_EnableDevice(int argc, char *argv[])
{
	if( argc >= 1 )
	{
		if( !INPUTMGR->EnableDevice(INPUTMGR, argv[0]) )
			con_Printf( CONRGB(255,255,255), 1, "Error enabling device: %s", argv[0] );
	}
}


//------------------------------------------------------------------
static void con_Bind( int argc, char *argv[] )
{
	int i;
	
	if( argc >= 2 )
	{
		INPUTMGR->ClearBindings(INPUTMGR, argv[0], argv[1]);

		for(i=2; i < argc; i++)
		{
			if(!INPUTMGR->AddBinding(INPUTMGR, argv[0], argv[1], argv[i], 0.0f, 0.0f))
			{
				con_Printf(CONRGB(255,255,255), 1, "Error binding device: %s, trigger: %s",
					argv[0], argv[1]);
			}
		}
	}
}


//------------------------------------------------------------------
static void con_RangeBind( int argc, char *argv[] )
{
	int i, nActions;
	
	if( argc >= 4 )
	{
		INPUTMGR->ClearBindings(INPUTMGR, argv[0], argv[1]);

		nActions = (argc - 2) / 3;
		for(i=0; i < nActions; i++)
		{
			if(!INPUTMGR->AddBinding(INPUTMGR, argv[0], argv[1], 
				argv[i*3+4], (float)atof(argv[i*3+2]), (float)atof(argv[i*3+3])))
			{
				con_Printf(CONRGB(255,255,255), 1, "Error binding device: %s, trigger: %s",
					argv[0], argv[1]);
			}
		}
	}
}


//------------------------------------------------------------------
static void con_Scale( int argc, char *argv[] )
{
	if( argc >= 3 )
	{
		if( !INPUTMGR->ScaleTrigger(INPUTMGR, argv[0], argv[1], (float)atof(argv[2]), 0.0f, 0.0f, 0.0f))
			con_Printf( CONRGB(255,255,255), 1, "Error finding device: %s, trigger: %s",
				argv[0], argv[1] );
	}
}


//------------------------------------------------------------------
static void con_RangeScale( int argc, char *argv[] )
{
	if( argc >= 5 )
	{
		if( !INPUTMGR->ScaleTrigger(INPUTMGR, argv[0], argv[1], (float)atof(argv[2]), 
				(float)atof(argv[3]), (float)atof(argv[4]), (float)atof(argv[5]) ))
			con_Printf( CONRGB(255,255,255), 1, "Error finding device: %s, trigger: %s",
				argv[0], argv[1] );
	}
}


//------------------------------------------------------------------
static void con_AddAction(int argc, char *argv[])
{
	if(argc >= 2)
	{
		INPUTMGR->AddAction(INPUTMGR, argv[0], atoi(argv[1]));
	}
}


//------------------------------------------------------------------
static void con_SSFile( int argc, char *argv[] )
{
	if(argc >= 1)
	{
		strcpy(g_SSFile, argv[0]);
	}
}


//------------------------------------------------------------------
static void con_UpdateServer(int argc, char *argv[])
{
	// This is a command because you never really want to save this variable in the config file.
	if(argc >= 1)
	{
		g_bUpdateServer = (LTBOOL)atoi(argv[0]);
	}
}


//------------------------------------------------------------------
static void con_RenderCommand(int argc, char *argv[])
{
	#ifdef __PS2
	#else
	if(r_GetRenderStruct())
	{
		r_GetRenderStruct()->RenderCommand(argc, argv);
	}
	#endif
}


//------------------------------------------------------------------
static void con_RestartConsole(int argc, char *argv[])
{
	g_pClientMgr->InitConsole();
}


//------------------------------------------------------------------
static void con_RestartRender(int argc, char *argv[])
{
	char str[245];
	LTRESULT dResult;

	// Set renderdll automatically.
	if(argc > 0)
	{
		sprintf(str, "renderDLL %s", argv[0]);
		c_CommandHandler(str);
	}
		
	#ifdef __PS2
	g_Render->Term();
	#else
	r_TermRender(1, false);
	#endif
	if((dResult = g_pClientMgr->StartRenderFromGlobals()) != LT_OK)
	{
		g_pClientMgr->ProcessError(dResult | ERROR_SHUTDOWN);
	}
}


//------------------------------------------------------------------
static void con_ResizeScreen(int argc, char *argv[])
{
	uint32 oldScreenWidth, oldScreenHeight;
	LTRESULT dResult;

	if(argc >= 2)
	{
		oldScreenWidth = g_ScreenWidth;
		oldScreenHeight = g_ScreenHeight;

		g_ScreenWidth = atoi(argv[0]);
		g_ScreenHeight = atoi(argv[1]);

		con_Printf(CONRGB(0,255,0), 1, "Setting screen to %dx%d", g_ScreenWidth, g_ScreenHeight);
		
		#ifdef __PS2
		#else
		r_TermRender(1, false);
		#endif
		
		if(g_pClientMgr->StartRenderFromGlobals() != LT_OK)
		{
			g_ScreenWidth = oldScreenWidth;
			g_ScreenHeight = oldScreenHeight;
	
			if((dResult = g_pClientMgr->StartRenderFromGlobals()) != LT_OK)
			{
				g_pClientMgr->ProcessError(dResult | ERROR_SHUTDOWN);
			}
		}
	}
}


//------------------------------------------------------------------
static void con_ServerCommand(int argc, char *argv[])
{
	char tempStr[300], fullCommand[500];
	int i;

	if(argc >= 1)
	{
		if(g_pClientMgr->m_pCurShell)
		{
			// 'unparse' the string to send it to the server.
			fullCommand[0] = 0;
			for(i=0; i < argc; i++)
			{
				sprintf(tempStr, "\"%s\" ", argv[i]);
				strcat(fullCommand, tempStr);
			}

			g_pClientMgr->m_pCurShell->SendCommandToServer(fullCommand);
		}
	}
}


//------------------------------------------------------------------
static void con_Quit(int argc, char *argv[])
{
	dsi_OnClientShutdown( LTNULL );
}



//------------------------------------------------------------------
static void con_ListInputDevices(int argc, char *argv[])
{
	INPUTMGR->ListDevices(INPUTMGR);
}



//------------------------------------------------------------------
static void con_EnvMap(int argc, char *argv[])
{
	FileRef ref;

	#ifdef __PS2
	#else
	if(g_pClientMgr && argc >= 1)
	{
		ref.m_FileType = FILE_CLIENTFILE;
		ref.m_pFilename = argv[0];
		g_Render.m_pEnvMap = g_pClientMgr->AddSharedTexture(&ref);
		if(g_Render.m_pEnvMap)
		{
			dsi_ConsolePrint("Environment map set to %s.", argv[0]);
		}
		else
		{
			dsi_ConsolePrint("Couldn't find texture %s.", argv[0]);
		}	
	}
	#endif
}


//------------------------------------------------------------------
static void con_RebindTextures(int argc, char *argv[])
{
	if(g_pClientMgr)
	{
		g_pClientMgr->RebindTextures();
	}
}


void dm_HeapCompact();
static void con_HeapCompact(int argc, char *argv[])
{
	dm_HeapCompact();
}

// Manipulate the console's history
static void con_ConsoleHistory(int argc, char *argv[])
{
	// Get the history iterator
	CConsole *pConsole = GETCONSOLE();
	CConHistory *pHistory = pConsole->GetCommandHistory();

	// List the current history
	if ( !pHistory->First() )
		return;

	int iCount = 0;
	do
	{
		const char *pLine = pHistory->Get();
		if ( !pLine )
			break;
		
		dsi_ConsolePrint( (char *)pLine );
		iCount++;
	} while ( pHistory->Next() );
	dsi_ConsolePrint( "%d commands", iCount );
}

// Clear the console's history list
static void con_ClearHistory(int argc, char *argv[])
{
	GETCONSOLE()->GetCommandHistory()->Clear();
	dsi_ConsolePrint( "Command history cleared." );
}

// Write the console history to a file
static void con_WriteHistory(int argc, char *argv[])
{
	// Argument checking
	if ( argc < 1 )
	{
		dsi_ConsolePrint( "Specify a filename." );
		return;
	}

	// Start the history iterator
	CConsole *pConsole = GETCONSOLE();
	CConHistory *pHistory = pConsole->GetCommandHistory();

	if ( !pHistory->First() )
	{
		dsi_ConsolePrint( "No commands in history." );
		return;
	}

	// Open the file
	FILE *fOutput;
	if ( (fOutput = fopen( argv[0], "wt" )) == LTNULL )
	{
		dsi_ConsolePrint( "Error opening file %s.", argv[0] );
		return;
	}

	int iCount = 0;

	do
	{
		// Write this history line to the file
		const char *pLine = pHistory->Get();
		if ( !pLine )
			break;

		fputs( pLine, fOutput );
		fputc( '\n', fOutput );
		iCount++;
	} while ( pHistory->Next() );

	// Close the file
	fclose( fOutput );

	dsi_ConsolePrint( "Successfully wrote %d commands to %s.", iCount, argv[0] );
}

// Read a file into the console history
static void con_ReadHistory(int argc, char *argv[])
{
	#ifndef __PS2
	// Argument checking
	if ( argc < 1 )
	{
		dsi_ConsolePrint( "Specify a filename." );
		return;
	}

	// Open the file
	FILE *fInput;
	if ( (fInput = fopen( argv[0], "rt" )) == LTNULL )
	{
		dsi_ConsolePrint( "Error opening file %s.", argv[0] );
		return;
	}

	// Get the history iterator
	CConsole *pConsole = GETCONSOLE();
	CConHistory *pHistory = pConsole->GetCommandHistory();

	int iCount = 0;

	char aBuffer[MAX_CONSOLE_TEXTLEN];
	while ( !feof( fInput ) )
	{
		// Read a line from the file
		if ( fgets( aBuffer, MAX_CONSOLE_TEXTLEN, fInput ) > 0 )
		{
			int iLength = strlen( aBuffer );
			// Remove trailing newlines
			if ( (iLength > 0) && (aBuffer[iLength - 1] == '\n') )
				aBuffer[--iLength] = 0;

			// Skip blank lines
			if ( !iLength )
				continue;

			// Add the line to the history
			pHistory->Add( aBuffer );
			iCount++;
		}
	} 

	// Close the file
	fclose( fInput );

	dsi_ConsolePrint( "Successfully read %d commands from %s.", iCount, argv[0] );
	#endif
}

// Execute a file
static void con_Exec(int argc, char *argv[])
{
	// Argument checking
	if ( argc < 1 )
	{
		dsi_ConsolePrint( "Specify a filename." );
		return;
	}

	// Execute the file
	if ( cc_RunConfigFile(&g_ClientConsoleState, argv[0], 0, VARFLAG_SAVE) )
		dsi_ConsolePrint( "Successfully executed %s.", argv[0] );
	else
		dsi_ConsolePrint( "Error executing %s.", argv[0] );
}

// Show version information
static void con_ShowVersionInfo(int argc, char *argv[])
{
	// Sanity checks
	ASSERT(g_pClientMgr->m_pClientDE);

	// Just to make things a little more readable
	ILTClient* pClient = g_pClientMgr->m_pClientDE;
	LTVERSIONINFOEXT info;

	// Initialize our structure
	memset(&info, 0, sizeof(LTVERSIONINFOEXT));
	info.m_dwSize = sizeof(LTVERSIONINFOEXT);
	
	// Get the actual information
	LTRESULT ltResult = pClient->GetVersionInfoExt(&info);

	// Display the info
	if (LT_OK == ltResult)
	{
		// Display results
		dsi_ConsolePrint("%s: %d.%d, %s: %d, %s: %d/%d/%d, %s, %s: %s, %s: %s",
							"Version",
							info.m_dwMajorVersion,
							info.m_dwMinorVersion,
							"Build Number",
							info.m_dwBuildNumber,
							"Build Date",
							info.m_dwBuildMonth,
							info.m_dwBuildDay,
							info.m_dwBuildYear,
							info.m_sBuildName,
							"Language",
							"English",
							"Platform",
							"PC/Windows");
	}
	else
		dsi_ConsolePrint("Error attempting to show version info.");
}

// Show performance information
static void con_ShowPerformanceInfo(int argc, char *argv[])
{
	// Sanity checks
	ASSERT(g_pClientMgr->m_pClientDE);

	// Just to make things a little more readable
	ILTClient* pClient = g_pClientMgr->m_pClientDE;
	LTPERFORMANCEINFO info;

	// Initialize our structure
	memset(&info, 0, sizeof(LTPERFORMANCEINFO));
	info.m_dwSize = sizeof(LTPERFORMANCEINFO);

	// Get the actual information
	LTRESULT ltResult = pClient->GetPerformanceInfo(&info);

	if (LT_OK == ltResult)
	{
		// Display results
		dsi_ConsolePrint("%s: %dx%d   %s: %d   %s: %d   %s: %d   %s: %d",
					"Resolution",
					info.m_dwScreenWidth,
					info.m_dwScreenHeight,
					"Frame Rate",
					info.m_dwFps,
					"World Polys",
					info.m_dwNumWorldPolys,
					"World Polys Proc",
					info.m_dwNumWorldPolysProcessed,
					"Model Polys",
					info.m_dwNumModelPolys);
	}
	else
		dsi_ConsolePrint("Error attempting to show performance info.");
}

#ifdef LITHTECH_ESD
extern ILTRealAudioPlayer* g_pRealAudioPlayer;
extern ILTRealVideoPlayer* g_pRealVideoPlayer;

// Real Audio playback
static void con_RealAudio(int argc, char *argv[])
{
	// Does the player exist?
	if (!g_pRealAudioPlayer)
	{
		dsi_ConsolePrint("RealAudio player does not exist.");
		return;
	}

	// Right arg count?
	if (argc > 0 && argc < 4)
	{
		// Should we play?
		if (0 == _stricmp(argv[0], "play"))
		{
			if (2 == argc || 3 == argc)
			{
				bool bIsDone = LTFALSE;
				g_pRealAudioPlayer->IsDone(&bIsDone);
				if (!bIsDone)
				{
					dsi_ConsolePrint("Player is already playing.", argv[1]);
					return;
				}

				// Loop?
				bool bLoop = LTFALSE;
				if (3 == argc)
					bLoop = atoi(argv[2]) ? LTTRUE : LTFALSE;

				if (LT_OK == g_pRealAudioPlayer->Open(argv[1]))
				{
					if (LT_OK != g_pRealAudioPlayer->Play(bLoop))
						dsi_ConsolePrint("Error attempting to play %s.", argv[1]);
				}
				else
					dsi_ConsolePrint("Error attempting to open %s.", argv[1]);
				return;
			}
		}

		// Should we stop?
		if (0 == _stricmp(argv[0], "stop"))
		{
			if (LT_OK != g_pRealAudioPlayer->Stop())
				dsi_ConsolePrint("Error: RealAudio stop failed.");
			return;
		}

		// Should we pause?
		if (0 == _stricmp(argv[0], "pause"))
		{
			if (LT_OK != g_pRealAudioPlayer->Pause())
				dsi_ConsolePrint("Error: RealAudio pause failed.");
			return;
		}

		// Should we resume?
		if (0 == _stricmp(argv[0], "resume"))
		{
			if (LT_OK != g_pRealAudioPlayer->Resume())
				dsi_ConsolePrint("Error: RealAudio resume failed.");
			return;
		}
	}

	// Display usage
	dsi_ConsolePrint("Usage: RealAudio [play <filename> <loop 0,1> | stop | pause | resume]");
}

// Real Video playback
static void con_RealVideo(int argc, char *argv[])
{
	// Does the player exist?
	if (!g_pRealVideoPlayer)
	{
		dsi_ConsolePrint("RealVideo player does not exist.");
		return;
	}

	// Right arg count?
	if (argc > 0 && argc < 4)
	{
		// Should we play?
		if (0 == _stricmp(argv[0], "play"))
		{
			if (2 == argc || 3 == argc)
			{
				bool bIsDone = LTFALSE;
				g_pRealVideoPlayer->IsDone(&bIsDone);
				if (!bIsDone)
				{
					dsi_ConsolePrint("Player is already playing.", argv[1]);
					return;
				}

				// Loop?
				bool bLoop = LTFALSE;
				if (3 == argc)
					bLoop = atoi(argv[2]) ? LTTRUE : LTFALSE;

				if (LT_OK == g_pRealVideoPlayer->Open(argv[1]))
				{
					if (LT_OK != g_pRealVideoPlayer->Play(bLoop))
						dsi_ConsolePrint("Error attempting to play %s.", argv[1]);
				}
				else
					dsi_ConsolePrint("Error attempting to open %s.", argv[1]);
				return;
			}
		}

		// Should we stop?
		if (0 == _stricmp(argv[0], "stop"))
		{
			if (LT_OK != g_pRealVideoPlayer->Stop())
				dsi_ConsolePrint("Error: RealVideo stop failed.");
			return;
		}

		// Should we pause?
		if (0 == _stricmp(argv[0], "pause"))
		{
			if (LT_OK != g_pRealVideoPlayer->Pause())
				dsi_ConsolePrint("Error: RealVideo pause failed.");
			return;
		}

		// Should we resume?
		if (0 == _stricmp(argv[0], "resume"))
		{
			if (LT_OK != g_pRealVideoPlayer->Resume())
				dsi_ConsolePrint("Error: RealVideo resume failed.");
			return;
		}
	}

	// Display usage
	dsi_ConsolePrint("Usage: RealVideo [play <filename> <loop 0,1> | stop | pause | resume]");
}

// Real Console Manager status
static void con_RealConsole(int argc, char *argv[])
{
	// Sanity checks
	if (!g_pClientMgr) return;
	if (!g_pClientMgr->m_pClientDE) return;

	// Get the RealConsoleMgr
	ILTRealConsoleMgr* pRealConsoleMgr = g_pClientMgr->m_pClientDE->GetRealConsoleMgr();

	// Does the player exist?
	if (!pRealConsoleMgr)
	{
		dsi_ConsolePrint("RealConsole manager does not exist.");
		return;
	}

	// Determine if we are initialized
	BOOL bInit = FALSE;
	HRESULT hr = pRealConsoleMgr->IsInitialized(&bInit);
	if (!bInit || hr != LT_OK)
	{
		dsi_ConsolePrint("RealConsole manager is not initialized.");
		return;
	}

	// Get number of messages in queue
	long lNumMsgs = 0;
	hr = pRealConsoleMgr->GetNumMessages(&lNumMsgs);
	if (hr == LT_OK)
	{
		dsi_ConsolePrint("RNGC: Num messages in queue = %i", lNumMsgs);
	}

	// Get last error info
	HRESULT hrLastError = S_OK;
	hr = pRealConsoleMgr->GetLastErrorResult(&hrLastError);
	if (hr == LT_OK)
	{
		dsi_ConsolePrint("RNGC: Last error result = %lu", hrLastError);
	}

	char sText[256] = { "" };
	UINT32 uSize = 255;
	hr = pRealConsoleMgr->GetLastErrorDescription(sText, &uSize);
	if (hr == LT_OK)
	{
		dsi_ConsolePrint("RNGC: Last error desc = %s", sText);
	}
}

#endif // LITHTECH_ESD

// Does a cpu benchmark
static void con_DoCPUBenchmarking(int argc, char *argv[])
{
	ASSERT(g_pClientMgr->m_pBenchmarkMgr);

	// Right arg count?
	if (0 == argc || 1 == argc)
	{
		// Default to 1 second
		float fTestDuration = 1.0f;
		LTBENCH_CPU_TEST	test;
		LTBENCH_CPU_RESULT	result;

		// Use a different duration?
		if (1 == argc)
			fTestDuration = (float)atof(argv[0]);

		// Initialize structures
		memset(&test, 0, sizeof(LTBENCH_CPU_TEST));
		test.m_dwSize = sizeof(LTBENCH_CPU_TEST);
		test.m_dwMaxPerformanceIndex = MAX_DWORD;

		memset(&result, 0, sizeof(LTBENCH_CPU_RESULT));
		result.m_dwSize = sizeof(LTBENCH_CPU_RESULT);

		// Do the test...
		((ILTBenchmarkMgr*)g_pClientMgr->m_pBenchmarkMgr)->DoCPUBenchmarking(&test, &result, fTestDuration, true);

		// Display results
		dsi_ConsolePrint("%s: %d   %s: %f",
							"CPU Rating",
							result.m_dwPerformanceIndex,
							"Test Duration",
							result.m_fTestDuration);

		return;
	}

	// Display usage
	dsi_ConsolePrint("Usage: DoCPUBenchmarking [seconds]");
}

// Starts graphics benchmarking
static void con_StartGraphicsBenchmarking(int argc, char *argv[])
{
	ASSERT(g_pClientMgr->m_pBenchmarkMgr);

	// Default to 1 second
	float fTestDuration = 0.0f;

	// Use a different duration?
	if (1 == argc)
		fTestDuration = (float)atof(argv[0]);

	// Start benchmarking
	LTRESULT ltResult = ((ILTBenchmarkMgr*)g_pClientMgr->m_pBenchmarkMgr)->StartGraphicsBenchmarking(fTestDuration);

	if (LT_OK != ltResult)
		dsi_ConsolePrint("Error: StartGraphicsBenchmarking failed.");
}

// Stops graphics benchmarking
static void con_StopGraphicsBenchmarking(int argc, char *argv[])
{
	ASSERT(g_pClientMgr->m_pBenchmarkMgr);

	LTBENCH_GRAPHICS_TEST test;

	// Initialize structures
	memset(&test, 0, sizeof(LTBENCH_GRAPHICS_TEST));
	test.m_dwSize = sizeof(LTBENCH_GRAPHICS_TEST);

	// Stop benchmarking
	LTRESULT ltResult = ((ILTBenchmarkMgr*)g_pClientMgr->m_pBenchmarkMgr)->StopGraphicsBenchmarking(&test);

	if (LT_OK == ltResult)
	{
		// Display results
		dsi_ConsolePrint("%s: %d   %s: %d   %s: %d",
							"MinFrameRate",
							test.m_dwMinFrameRate,
							"MaxFrameRate",
							test.m_dwMaxFrameRate,
							"AvgFrameRate",
							test.m_dwAverageFrameRate);
		dsi_ConsolePrint("%s: %d   %s: %d   %s: %d   %s: %f",
							"MinPolyCount",
							test.m_dwMinPolyCount,
							"MaxPolyCount",
							test.m_dwMaxPolyCount,
							"AvgPolyCount",
							test.m_dwAveragePolyCount,
							"TestTime",
							test.m_fTestDuration);
	}
	else
		dsi_ConsolePrint("Error: StopGraphicsBenchmarking failed.");
}

// Gets current benchmarking info
static void con_GetGraphicsBenchmarkingInfo(int argc, char *argv[])
{
	ASSERT(g_pClientMgr->m_pBenchmarkMgr);

	LTBENCH_GRAPHICS_TEST test;

	// Initialize structures
	memset(&test, 0, sizeof(LTBENCH_GRAPHICS_TEST));
	test.m_dwSize = sizeof(LTBENCH_GRAPHICS_TEST);

	// Get benchmark info
	LTRESULT ltResult = ((ILTBenchmarkMgr*)g_pClientMgr->m_pBenchmarkMgr)->GetGraphicsBenchmarkingInfo(&test);

	if (LT_OK == ltResult)
	{
		// Display results
		dsi_ConsolePrint("%s: %d   %s: %d   %s: %d",
							"MinFrameRate",
							test.m_dwMinFrameRate,
							"MaxFrameRate",
							test.m_dwMaxFrameRate,
							"AvgFrameRate",
							test.m_dwAverageFrameRate);
		dsi_ConsolePrint("%s: %d   %s: %d   %s: %d   %s: %f",
							"MinPolyCount",
							test.m_dwMinPolyCount,
							"MaxPolyCount",
							test.m_dwMaxPolyCount,
							"AvgPolyCount",
							test.m_dwAveragePolyCount,
							"TestTime",
							test.m_fTestDuration);
	}
	else
		dsi_ConsolePrint("Error: GetGraphicsBenchmarkingInfo failed.");
}

// Clears the current benchmarking info
static void con_ClearGraphicsBenchmarkingInfo(int argc, char *argv[])
{
	ASSERT(g_pClientMgr->m_pBenchmarkMgr);

	// Clear benchmark info
	LTRESULT ltResult = ((ILTBenchmarkMgr*)g_pClientMgr->m_pBenchmarkMgr)->ClearGraphicsBenchmarkingInfo();
	
	if (LT_OK != ltResult)
		dsi_ConsolePrint("Error: ClearGraphicsBenchmarkingInfo failed.");
}

// Determines if we are currently benchmarking
static void con_IsGraphicsBenchmarking(int argc, char *argv[])
{
	ASSERT(g_pClientMgr->m_pBenchmarkMgr);

	LTBOOL bIsBenchmarking = LTFALSE;
	LTRESULT ltResult = ((ILTBenchmarkMgr*)g_pClientMgr->m_pBenchmarkMgr)->IsGraphicsBenchmarking(&bIsBenchmarking);

	if (LT_OK == ltResult)
		dsi_ConsolePrint("%s", bIsBenchmarking ? "Benchmarking..." : "Not Benchmarking.");
	else
		dsi_ConsolePrint("Error: IsGraphicsBenchmarking failed.");
}

// Move the console window
static void con_MoveConsole(int argc, char *argv[])
{
	// Report the console location
	if (!argc)
	{
		dsi_ConsolePrint("Console position : (%d,%d, %d,%d)", g_CV_ConsoleLeft, g_CV_ConsoleTop, g_CV_ConsoleRight, g_CV_ConsoleBottom);
		return;
	}

	// Change the console size
	LTRect rect;
	if (stricmp(argv[0], "Top") == 0)
	{
		rect.left = -1;
		rect.top = -1;
		rect.right = -1;
		rect.bottom = -2;
	}
	else if (stricmp(argv[0], "Bottom" ) == 0)
	{
		rect.left = -1;
		rect.top = -2;
		rect.right = -1;
		rect.bottom = -1;
	}
	else if (stricmp(argv[0], "Middle") == 0)
	{
		rect.left = -4;
		rect.top = -4;
		rect.right = -4;
		rect.bottom = -4;
	}
	else if (stricmp(argv[0], "Full") == 0)
	{
		rect.left = -1;
		rect.top = -1;
		rect.right = -1;
		rect.bottom = -1;
	}
	else
	{
		if (argc < 4)
		{
			dsi_ConsolePrint("Specify Top, Bottom, Full, Middle, or 4 screen coordinates");
			return;
		}

		// Use the coordinates entered by the user
		rect.left = atoi(argv[0]);
		rect.top = atoi(argv[1]);
		rect.right = atoi(argv[2]);
		rect.bottom = atoi(argv[3]);
	}

	// Write the new console rectangle to the console variables

	char cmd[200];
	sprintf(cmd, "ConsoleLeft %d", rect.left);
	c_CommandHandler(cmd);
	sprintf(cmd, "ConsoleTop %d", rect.top);
	c_CommandHandler(cmd);
	sprintf(cmd, "ConsoleRight %d", rect.right);
	c_CommandHandler(cmd);
	sprintf(cmd, "ConsoleBottom %d", rect.bottom);
	c_CommandHandler(cmd);
}

// These ones are implemented below the static table definitions.
//------------------------------------------------------------------
static void con_ListCommands(int argc, char *argv[]);
static void con_Set(int argc, char *argv[]);




// ------------------------------------------------------------------ //
// Save functions
// ------------------------------------------------------------------ //

static void SaveModelAdd(FILE *fp)
{
	fprintf(fp, "ModelAdd %f %f %f\n", g_ConsoleModelAdd.x, g_ConsoleModelAdd.y, g_ConsoleModelAdd.z);
	fprintf(fp, "ModelDirAdd %f %f %f\n", g_ConsoleModelDirAdd.x, g_ConsoleModelDirAdd.y, g_ConsoleModelDirAdd.z);
}

static LTSaveFn g_SaveFns[] =
{
	input_SaveBindings,
	SaveModelAdd
};

#define NUM_SAVEFNS	(sizeof(g_SaveFns) / sizeof(LTSaveFn))




//------------------------------------------------------------------
//------------------------------------------------------------------
// The main table of command handlers.
//------------------------------------------------------------------
//------------------------------------------------------------------

static LTCommandStruct g_LTCommandStructs[] =
{
	"BlastServer", con_BlastServer, 0,
	"ListInputDevices", con_ListInputDevices, 0,
	"quit", con_Quit, 0,
	"serv", con_ServerCommand, 0,
	"ModelAdd", con_ModelAdd, 0,
	"ModelDirAdd", con_ModelDirAdd, 0,
	"World", con_World, 0,
	"TimeDemo", con_TimeDemo, 0,
	"EnableDevice", con_EnableDevice, 0,
	"RangeBind", con_RangeBind, 0,
	"Bind", con_Bind, 0,
	"Scale", con_Scale, 0,
	"RangeScale", con_RangeScale, 0,
	"AddAction", con_AddAction, 0,
	"SSFile", con_SSFile, 0,
	"UpdateServer", con_UpdateServer, 0,
	"RenderCommand", con_RenderCommand, 0,
	"RCom", con_RenderCommand, 0,
	"Set", con_Set, 0,
	"ListCommands", con_ListCommands, 0,
	"RestartConsole", con_RestartConsole, 0,
	"RestartRender", con_RestartRender, 0,
	"ResizeScreen", con_ResizeScreen, 0,
	"EnvMap", con_EnvMap, 0,
	"RebindTextures", con_RebindTextures, 0,
	"HeapCompact", con_HeapCompact, 0,
	"ConsoleHistory", con_ConsoleHistory, 0,
	"ClearHistory", con_ClearHistory, 0,
	"WriteHistory", con_WriteHistory, 0,
	"ReadHistory", con_ReadHistory, 0,
	"Exec", con_Exec, 0,
	"ShowVersionInfo", con_ShowVersionInfo, 0,
	"ShowPerformanceInfo", con_ShowPerformanceInfo, 0,
	#ifdef LITHTECH_ESD
	"RealAudio", con_RealAudio, 0,
	"RealVideo", con_RealVideo, 0,
	"RealConsole", con_RealConsole, 0,
	#endif // LITHTECH_ESD
	"DoCPUBenchmarking", con_DoCPUBenchmarking, 0,
	"StartGraphicsBenchmarking", con_StartGraphicsBenchmarking, 0,
	"StopGraphicsBenchmarking", con_StopGraphicsBenchmarking, 0,
	"GetGraphicsBenchmarkingInfo", con_GetGraphicsBenchmarkingInfo, 0,
	"ClearGraphicsBenchmarkingInfo", con_ClearGraphicsBenchmarkingInfo, 0,
	"IsGraphicsBenchmarking", con_IsGraphicsBenchmarking, 0,
	"MoveConsole", con_MoveConsole, 0,
};	

#define NUM_COMMANDSTRUCTS	(sizeof(g_LTCommandStructs) / sizeof(LTCommandStruct))






//------------------------------------------------------------------
static void con_ListCommands(int argc, char *argv[])
{
	int i;

	for(i=0; i < g_ClientConsoleState.m_nCommandStructs; i++)
	{
		con_WhitePrintf(g_ClientConsoleState.m_pCommandStructs[i].pCmdName);
	}
}


//------------------------------------------------------------------
static void con_Set(int argc, char *argv[])
{
	LTCommandVar *pCurVar;
	HHashIterator *hIterator;
	HHashElement *hElement;
	hIterator = hs_GetFirstElement( g_ClientConsoleState.m_VarHash );
	while(hIterator)
	{
		hElement = hs_GetNextElement(hIterator);
		if( !hElement )
			continue;
		
		pCurVar = ( LTCommandVar * )hs_GetElementUserData( hElement );
		cc_PrintVarDescription(&g_ClientConsoleState, pCurVar);
	}
}


//------------------------------------------------------------------
//------------------------------------------------------------------
// Main parsing / command handler
//------------------------------------------------------------------
//------------------------------------------------------------------

void* c_Alloc(uint32 size)
{
	return dalloc(size);
}

void c_Free(void *ptr)
{
	dfree(ptr);
}

void c_InitConsoleCommands()
{
	memset(&g_ClientConsoleState, 0, sizeof(g_ClientConsoleState));

	g_ClientConsoleState.m_SaveFns = g_SaveFns;
	g_ClientConsoleState.m_nSaveFns = NUM_SAVEFNS;

	g_ClientConsoleState.m_pEngineVars = GetEngineVars();
	g_ClientConsoleState.m_nEngineVars = GetNumEngineVars();

	g_ClientConsoleState.m_pCommandStructs = g_LTCommandStructs;
	g_ClientConsoleState.m_nCommandStructs = NUM_COMMANDSTRUCTS;

	g_ClientConsoleState.ConsolePrint = con_WhitePrintf;
	
	g_ClientConsoleState.Alloc = c_Alloc;
	g_ClientConsoleState.Free = c_Free;

	cc_InitState(&g_ClientConsoleState);

	// Set up the completion iterator
	GETCONSOLE()->SetCompletionIterator( &g_ClientConIterator );
}


void c_TermConsoleCommands()
{
	cc_TermState(&g_ClientConsoleState);
}


void c_CommandHandler(char *pCommand)
{
	cc_HandleCommand(&g_ClientConsoleState, pCommand);
}	



CClientConIterator::CClientConIterator() :
	m_eState(STATE_COMMAND),
	m_iCommandIndex(0),
	m_hVarIndex(0),
	m_hCurVar(0),
	m_iEngineIndex(0)
{
}

CClientConIterator::~CClientConIterator()
{
	// Nothing to destruct...
}

bool CClientConIterator::Begin()
{
	// Go to the beginning of the command list
	m_eState = STATE_COMMAND;
	m_iCommandIndex = 0;

	// This isn't guaranteed to be true, but I'm pretty sure it will be...
	return TRUE;
}

bool CClientConIterator::NextItem()
{
	bool bEnd = LTFALSE;

	switch (m_eState)
	{
		case STATE_COMMAND :
		{
			// Move to the next command
			m_iCommandIndex++;
			// Overflow to the variable list
			if ( m_iCommandIndex >= g_ClientConsoleState.m_nCommandStructs )
			{
				// Go to variable mode
				m_eState = STATE_CONVAR;
				m_hVarIndex = 0;
				bEnd = !NextItem();
			}
			break;
		}
		case STATE_CONVAR :
		{
			if (!m_hVarIndex)
			{
				m_hVarIndex = hs_GetFirstElement( g_ClientConsoleState.m_VarHash );
				m_hCurVar = LTNULL;
			}

			// find the next variable
			do { 
				m_hCurVar = hs_GetNextElement(m_hVarIndex);
			} while ( m_hVarIndex && !m_hCurVar );

			// Overflow to the engine variable list
			if (!m_hVarIndex)
			{
				m_eState = STATE_ENGINEVAR;
				m_iEngineIndex = -1;
				bEnd = !NextItem();
			}
			break;
		}
		case STATE_ENGINEVAR :
		{
			if (m_iCommandIndex >= GetNumEngineVars())
			{
				// Nothing to overflow to, so we're at the end
				bEnd = TRUE;
			}
			else
			{
				m_iEngineIndex++;
			}
		}
	}

	return !bEnd;
}

const char *CClientConIterator::Get() const
{
	const char *pResult = LTNULL;

	switch (m_eState)
	{
		// Get a command
		case STATE_COMMAND :
		{
			// Make sure we're not past the end of the list
			if ( m_iCommandIndex < g_ClientConsoleState.m_nCommandStructs )
				pResult = g_ClientConsoleState.m_pCommandStructs[m_iCommandIndex].pCmdName;
			break;
		}
		// Get a console variable
		case STATE_CONVAR :
		{
			// Make sure we're not past the end of the list
			if ( m_hCurVar )
				pResult = (( LTCommandVar * )hs_GetElementUserData( m_hCurVar ))->pVarName;
			break;
		}
		// Get an engine variable
		case STATE_ENGINEVAR :
		{
			// Make sure we're not past the end of the list
			if ( m_iEngineIndex < GetNumEngineVars() )
				pResult = GetEngineVars()[m_iEngineIndex].pVarName;
			break;
		}
	}

	return pResult;
}

