
// This module defines all the engine's console variables.
// NOTE: the reason some of the variables are extern'd is because certain modules don't
// compile with engine_vars.cpp so they 'own' the variable.

#include "bdefs.h"
#include "ltbasetypes.h"
#include "concommand.h"
#include "packetdefs.h"




int32	g_CV_RenderEnable = LTTRUE;

int32	g_CV_PlayDemoReps = 0;	// How many times to repeat the PlayDemo.

int32	g_CV_ForceNoSound = LTFALSE;

int32	g_CV_CursorCenter = LTTRUE;	// Automatically resets the cursor to the center 
									// of the window/screen each frame

int32	g_CV_VideoDebug = LTFALSE;

int32	g_CV_DebugLoaders = LTFALSE; // Debug output for loader threads?


int32	g_CV_STracePackets = LTFALSE;
int32	g_CV_DelimitPackets = LTTRUE;

char	*g_CV_BindIP = LTNULL;
char	*g_CV_IP = LTNULL;

char*	g_CV_Console_FontTexFile = NULL;
int32	g_CV_NoDefaultEngineRez = 0;

int32	g_CV_ForceConsole = 0;
int32	g_CV_UDPDebug = LTFALSE;

float	g_CV_IPQueryTimeout = 30.0f;
float	g_CV_MaxFPS = 0.0f;

int32	g_CV_InputDebug = LTFALSE;
int32	g_CV_JoystickDisable = LTTRUE;

int32	g_CV_UpdateRate = DEFAULT_CLIENT_UPDATE_RATE;

int32	g_CV_TraceConsole = LTFALSE;

int32	g_CV_ShowFileAccess = LTFALSE;

int32	g_CV_ShowConnStats = LTFALSE;

int32	g_CV_FullLightScale = LTFALSE;

int32	g_CV_ForceClear = LTFALSE;	// Force the engine to clear the screen.

float	g_CV_LatencySim = 0.0f;	// Simulate latency.
float	g_CV_DropRate = 0.0f;   // Simulate packet drops - affects both client and server at the same time

int32	g_CV_InputRate=0;		// The amount of time an input sample is spread over in milliseconds


int32	g_bForceRemote = LTFALSE;	// Force remote connection even if it's local.. less efficient.
int32	g_bShowRunningTime = LTFALSE;	// Shows how long the engine has been running.
LTBOOL	g_bNullRender = LTFALSE;	// Set this when using NullRender.. the engine
									// will do things to make it work better. 

LTBOOL	g_CV_CollideParticles = LTTRUE;	// Should marked particle systems handle collisions with other objects?

LTBOOL	g_CV_HighPriority = LTFALSE;	// Should the process be set into high priority?

int32	g_bLocalDebug = LTFALSE;
int32	g_bPrediction = LTTRUE;
int32	g_nPredictionLines = 0; // Draw lines showing the results of client-side prediction
  								// 0 = no lines   1 = track prediction   >1 = stop 
int32	g_TransportDebug = 0;
int32	g_CV_ParseNet_Incoming = 0; // Parse incoming network packets
int32	g_CV_ParseNet_Outgoing = 0; // Parse outgoing network packets
int32   g_CV_ParseNet = 0; // Parse network packets going either direction
int32	g_ClientSleepMS = 0;
int32	g_ShowTickCounts = 0;
int32	g_bShowMemStats = LTFALSE;
int32	g_bDebugStrings = LTFALSE;
int32	g_bDebugPackets = LTFALSE;

float	g_CV_MaxExtrapolateTime = 0.5f;  // Maximum amount of time to extrapolate a position

LTVector	g_ConsoleModelAdd(0.0f, 0.0f, 0.0f);
LTVector	g_ConsoleModelDirAdd(0.0f, 0.0f, 0.0f);
LTBOOL		g_bUpdateServer = LTTRUE;

int32	g_nSoundDebugLevel = 0;
LTBOOL	g_bSoundShowCounts = LTFALSE;

int32	g_bErrorLog = LTTRUE;
int32	g_bAlwaysFlushLog = LTFALSE;

int32	g_CV_BitDepth = 32;
LTBOOL  g_bHWTnLDisabled = LTFALSE;
int32	g_bSoftware = LTFALSE;
int32	g_ScreenWidth = 640, g_ScreenHeight = 480;
int32	g_CV_ShowFrameRate = LTFALSE;
int32	g_nConsoleLines = 6;
LTBOOL  g_bConsoleEnable = LTTRUE;
char	g_SSFile[_MAX_PATH + 1] = "SA_";

LTBOOL	g_bMusicEnable = LTTRUE;
LTBOOL	g_bSoundEnable = LTTRUE;
int32	g_CV_ForceSoundDisable = LTFALSE; // Force it to not initialize music or sound (behind the game's back).

float	g_fLodScale = 1.0f;

// Framerate for server logic.
float	g_ServerFPS = 10.0f;
#ifdef LOCK_SERVER_FPS
int32	g_LockServerFPS = 1;
#endif

// Scale the time to make things go slower or faster.
float	g_ServerTimeScale = 1.0f;

// If an object moves to a position with coord larger than this, it'll complain in the console.
float	g_DebugMaxPos = 0.0f;

int32	g_CV_ShowGameTime = LTFALSE;

int32	g_CV_ShowClassTicks = LTFALSE;
char	*g_CV_ShowClassTicksSpecific = LTNULL;

int32	g_CV_ShowSphereFindTicks = LTFALSE;

// Console attributes
int32	g_CV_ConsoleHistoryLen = 20;
int32	g_CV_ConsoleBufferLen = 500;
float	g_CV_ConsoleAlpha = 1.0f;		// Note : OptimizeSurfaces must be 1 for this to have any effect
int32	g_CV_ConsoleLeft = -1;	// Note : Negative values move the edge away from the screen edge by
int32	g_CV_ConsoleTop = -1;	//	that positive fraction of the screen dimension.  (-1 = no offset)
int32	g_CV_ConsoleRight = -1;
int32	g_CV_ConsoleBottom = -2;

// The LithTech DirectMusic console output level
int32	g_CV_LTDMConsoleOutput = 2;

int32	g_CV_DrawDebugGeometry		 = 0;
int32	g_CV_DebugModelRez           = 0;


//Texture groups, used to control offsetting of Mip Maps when they are loaded, thus allowing
//us to not waste any memory loading mips higher than those that we are going to use. Note that
//this define needs to match the one in the texture loading code. This is ugly, but there is no
//shared header.
#define MAX_TEXTURE_GROUPS 4
int32	g_CV_TextureMipMapOffset	= 0;
int32	g_CV_TextureGroupOffset[MAX_TEXTURE_GROUPS];

#ifdef LITHTECH_ESD
int32 g_CV_LTRConsoleOutput = 1;3
#endif // LITHTECH_ESD

//int32 g_CV_ShowFrameRate = 0;

// default client port number, range: 0 or (recommended) 1024 - 65535.
// [default] value of zero means "let system choose the port number".
int32 g_CV_IPClientPort = 0;			// base of IPClientPort range.  See additional description info in udpdriver.cpp
int32 g_CV_IPClientPortRange = 1;		// client port range
int32 g_CV_IPClientPortMRU = 0;			// most-recently-used client port

int32 g_CV_BandwidthTargetClient = 256000; // client send bandwidth target in bits-per-second (n/a for local unless "ForceRemote" is set)
int32 g_CV_BandwidthTargetServer = 256000; // server send bandwidth target in bits-per-second (n/a for local unless "ForceRemote" is set)

int32 g_CV_NewPlayerPhysics = 1;	// Use the new player physics

int32 g_CV_UDPSimulatePacketLoss = 0;
int32 g_CV_UDPSimulateCorruption = 0;


// [NOVICE] BEGIN ------------
#ifndef _FINAL
int32 g_CV_OutputS2C = 0;
int32 g_CV_OutputC2S = 0;

int32	g_CV_INSize	= 0;
uint32	g_CV_In_UDPSize = 0;
uint32	g_CV_In_GSize = 0;
uint32	g_CV_In_UGSize = 0;

LTBOOL	g_CV_NoisyText = LTFALSE;

uint32	g_CV_SndCntPerSec		= 0;
uint32	g_CV_SndCntAtSameTime	= 0;
LTBOOL	g_CV_SndTestEnable		= LTFALSE;
uint32	g_CV_SndTestHeardOffset	= 0;
char*	g_CV_SndTestFileName	= LTNULL;
#endif

uint32	g_CV_ParseRecv = 0;
uint32	g_CV_ParseSend = 0;
LTBOOL	g_CV_SavePacketStaistics = LTFALSE;
// [NOVICE] END --------------



LTFLOAT	g_CV_LockSendUUF	= 0.1f;
LTFLOAT	g_CV_LockFPS		= 0.014f;
uint32	g_CV_ValidWndMode = 1;

#ifndef _FINAL
uint32	g_CV_TestG	= 0;
uint32	g_CV_TestUG	= 0;
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
// The main table of command variables
//------------------------------------------------------------------
//------------------------------------------------------------------

static LTEngineVar g_LTEngineVars[] =
{
#ifndef _FINAL
	EV_LONG("TestG", &g_CV_TestG ),
	EV_LONG("TestUG", &g_CV_TestUG ),
#endif
	EV_STRING("BindIP", &g_CV_BindIP),
	EV_STRING("IP", &g_CV_IP),

	EV_STRING("Console_FontTexFile", &g_CV_Console_FontTexFile),
	EV_LONG("NoDefaultEngineRez", &g_CV_NoDefaultEngineRez),

	EV_LONG("RenderEnable", &g_CV_RenderEnable),
	EV_LONG("PlayDemoReps", &g_CV_PlayDemoReps),
	EV_LONG("ForceNoSound", &g_CV_ForceNoSound),
	
	EV_LONG("CursorCenter", &g_CV_CursorCenter),
	

	EV_LONG("VideoDebug", &g_CV_VideoDebug),
	EV_LONG("DebugLoaders", &g_CV_DebugLoaders),
	
	EV_LONG("STracePackets", &g_CV_STracePackets),
	EV_LONG("DelimitPackets", &g_CV_DelimitPackets),
	EV_LONG("ForceConsole", &g_CV_ForceConsole),
	EV_LONG("UDPDebug", &g_CV_UDPDebug),
	EV_LONG("InputDebug", &g_CV_InputDebug),
	EV_LONG("UpdateRate", &g_CV_UpdateRate),
	EV_LONG("InputRate", &g_CV_InputRate),
	EV_LONG("ForceRemote", &g_bForceRemote),
	EV_LONG("DebugLevel", &g_DebugLevel),
	EV_LONG("ShowRunningTime", &g_bShowRunningTime),
	EV_LONG("NullRender", &g_bNullRender),
	EV_LONG("LocalDebug", &g_bLocalDebug),
	EV_LONG("TransportDebug", &g_TransportDebug),
	EV_LONG("ParseNet_Incoming", &g_CV_ParseNet_Incoming),
	EV_LONG("ParseNet_Outgoing", &g_CV_ParseNet_Outgoing),
	EV_LONG("ParseNet", &g_CV_ParseNet),
	EV_LONG("ClientSleepMS", &g_ClientSleepMS),
	EV_LONG("ShowTickCounts", &g_ShowTickCounts),
	EV_LONG("ShowMemStats", &g_bShowMemStats),
	EV_LONG("Prediction", &g_bPrediction),
	EV_LONG("PredictionLines", &g_nPredictionLines),
	EV_LONG("DebugStrings", &g_bDebugStrings),
	EV_LONG("DebugPackets", &g_bDebugPackets),
	EV_LONG("CollideParticles", &g_CV_CollideParticles),
	EV_LONG("SoundShowCounts", &g_bSoundShowCounts),
	EV_LONG("SoundDebugLevel", &g_nSoundDebugLevel),
	EV_LONG("ErrorLog", &g_bErrorLog),
	EV_LONG("AlwaysFlushLog", &g_bAlwaysFlushLog),
	EV_LONG("ShowFrameRate", &g_CV_ShowFrameRate),
	EV_LONG("NumConsoleLines", &g_nConsoleLines),
	EV_LONG("ConsoleEnable", &g_bConsoleEnable),
	EV_LONG("ScreenWidth", &g_ScreenWidth),
	EV_LONG("ScreenHeight", &g_ScreenHeight),
	EV_LONG("Software", &g_bSoftware),
	EV_LONG("BitDepth", &g_CV_BitDepth),
	EV_LONG("HWTnLDisabled", &g_bHWTnLDisabled),
	EV_LONG("MusicEnable", &g_bMusicEnable),
	EV_LONG("SoundEnable", &g_bSoundEnable),
	EV_LONG("ForceSoundDisable", &g_CV_ForceSoundDisable),
	EV_LONG("ShowSphereFindTicks", &g_CV_ShowSphereFindTicks),
	EV_LONG("ShowClassTicks", &g_CV_ShowClassTicks),
	EV_STRING("ShowClassTicksSpecific", &g_CV_ShowClassTicksSpecific),
	EV_LONG("ShowGameTime", &g_CV_ShowGameTime),
	EV_LONG("JoystickDisable", &g_CV_JoystickDisable),
	EV_LONG("TraceConsole", &g_CV_TraceConsole),
	EV_LONG("ShowFileAccess", &g_CV_ShowFileAccess),
	EV_LONG("FullLightScale", &g_CV_FullLightScale),
	EV_LONG("ForceClear", &g_CV_ForceClear),
	EV_LONG("HighPriority", &g_CV_HighPriority),
	EV_LONG("ShowConnStats", &g_CV_ShowConnStats),

	EV_LONG("LTDMConsoleOutput", &g_CV_LTDMConsoleOutput),

	EV_LONG("MipMapOffset", &g_CV_TextureMipMapOffset),
	EV_LONG("GroupOffset0", &g_CV_TextureGroupOffset[0]),
	EV_LONG("GroupOffset1", &g_CV_TextureGroupOffset[1]),
	EV_LONG("GroupOffset2", &g_CV_TextureGroupOffset[2]),
	EV_LONG("GroupOffset3", &g_CV_TextureGroupOffset[3]),

	#ifdef LITHTECH_ESD
	EV_LONG("LTRConsoleOutput", &g_CV_LTRConsoleOutput),
	#endif // LITHTECH_ESD

	EV_FLOAT("MaxFPS", &g_CV_MaxFPS),
	EV_FLOAT("LatencySim", &g_CV_LatencySim),
	EV_FLOAT("DropRate", &g_CV_DropRate),
	EV_FLOAT("LODScale", &g_fLodScale),
	EV_FLOAT("DebugMaxPos", &g_DebugMaxPos),
	EV_FLOAT("TimeScale", &g_ServerTimeScale),
	EV_FLOAT("MaxExtrapolateTime", &g_CV_MaxExtrapolateTime),

	EV_LONG("ConsoleHistoryLen", &g_CV_ConsoleHistoryLen),
	EV_LONG("ConsoleBufferLen", &g_CV_ConsoleBufferLen),
	EV_FLOAT("ConsoleAlpha", &g_CV_ConsoleAlpha),
	EV_LONG("ConsoleLeft", &g_CV_ConsoleLeft),
	EV_LONG("ConsoleTop", &g_CV_ConsoleTop),
	EV_LONG("ConsoleRight", &g_CV_ConsoleRight),
	EV_LONG("ConsoleBottom", &g_CV_ConsoleBottom),

	EV_LONG("DrawDebugGeometry", &g_CV_DrawDebugGeometry),
	
	//EV_LONG("ShowFrameRate", &g_CV_ShowFrameRate),

	// default client IP port number
	EV_LONG("IPClientPort", &g_CV_IPClientPort),				 // IP port number (numeric range: 0 - 65535)
	EV_LONG("IPClientPortRange", &g_CV_IPClientPortRange),		 // port number range (0 - N where N should be pretty small)
	EV_LONG("IPClientPortMRU", &g_CV_IPClientPortMRU),			 // MRU port number
		
	EV_LONG("BandwidthTargetClient", &g_CV_BandwidthTargetClient), // in bytes-per-sec
	EV_LONG("BandwidthTargetServer", &g_CV_BandwidthTargetServer), // in bytes-per-sec

	#ifdef LOCK_SERVER_FPS
	EV_FLOAT("ServerFPS", &g_ServerFPS),						 // server frames-per-second
	EV_LONG("LockServerFPS", &g_LockServerFPS),					 // force delay between frames to achieve lock rate if running too fast
	#endif

	EV_LONG("DebugModelRez", &g_CV_DebugModelRez),

	EV_LONG("NewPlayerPhysics", &g_CV_NewPlayerPhysics),

	EV_LONG("UDPSimulatePacketLoss", &g_CV_UDPSimulatePacketLoss),
	EV_LONG("UDPSimulateCorruption", &g_CV_UDPSimulateCorruption),
	
#ifndef _FINAL
	//[MURSUM]
	EV_LONG("OutputS2C", &g_CV_OutputS2C),
	EV_LONG("OutputC2S", &g_CV_OutputC2S),

	EV_LONG("ShowInSize", &g_CV_INSize),	
	
	// [NOVICE] BEGINE ---------------------
	EV_LONG( "NoisyText", &g_CV_NoisyText ),

	EV_LONG( "SndCntPerSec", &g_CV_SndCntPerSec ),
	EV_LONG( "SndCntAtSameTime", &g_CV_SndCntAtSameTime ),
	EV_LONG( "SndTestEnable", &g_CV_SndTestEnable ),
	EV_LONG( "SndTestHeardOffset", &g_CV_SndTestHeardOffset ),
	EV_STRING( "SndTestFileName", &g_CV_SndTestFileName ),
	
	// [NOVICE] END ------------------------
#endif
	EV_LONG( "ParseRecv", &g_CV_ParseRecv ),
	EV_LONG( "ParseSend", &g_CV_ParseSend ),
	EV_LONG( "SavePacketStaistics", &g_CV_SavePacketStaistics ),
	
	EV_FLOAT( "LockSendUUF", &g_CV_LockSendUUF ),
	EV_FLOAT( "LockFPS", &g_CV_LockFPS ),
	EV_LONG( "ValidWndMode", &g_CV_ValidWndMode ),
};


// Functions that other modules call.
LTEngineVar* GetEngineVars() {return g_LTEngineVars;}
int GetNumEngineVars() {return sizeof(g_LTEngineVars) / sizeof(LTEngineVar);}

