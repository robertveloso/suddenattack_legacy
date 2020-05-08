#include "bdefs.h"

#include <stddef.h>
#include "render.h"
#include "consolecommands.h"
#include "dsys_interface.h"
#include "clientmgr.h"
#include "sysclientde_impl.h"
#include "varsetter.h"
#include "videomgr.h"
#include "sysconsole_impl.h"
#include "DDPerfCounterManager.h"
#include "dtxmgr.h"
#include "iltclient.h"

// ------------------------------------------------------------ //
// Globals..
// ------------------------------------------------------------ //
SysCache g_SysCache;

extern int32 g_CV_CursorCenter;


char *g_CurrentRenderDLL = LTNULL;
RMode g_RMode;


// ------------------------------------------------------------ //
// Internal functions.
// ------------------------------------------------------------ //

void r_UnloadSystemTexture(TextureData *pTexture)
{
	SysCache *pCache;

	pCache = &g_SysCache;
	ASSERT(pCache->m_CurMem >= pTexture->m_AllocSize);
	pCache->m_CurMem -= pTexture->m_AllocSize;

	if (pTexture) 
	{
		if (pTexture->m_pDataBuffer) { 
			delete[] pTexture->m_pDataBuffer; 
			pTexture->m_pDataBuffer = NULL; }
	
		if (pTexture->m_pSharedTexture) 
			pTexture->m_pSharedTexture->m_pEngineData = LTNULL; 
	}

	dl_RemoveAt(&pCache->m_List, &pTexture->m_Link);
	dfree(pTexture);
}

// Makes room for the given texture.  Returns the number of textures it unloaded
// or -1 if it can't make room.
static int r_MakeRoomForTexture(SysCache *pCache, uint32 allocSize)
{
	LTLink *pListHead; int count = 0;

	pListHead = &pCache->m_List.m_Head;
	while ((pCache->m_CurMem + allocSize) > pCache->m_MaxMem) {
		if (pListHead->m_pNext == pListHead) {
			// This should NEVER happen.. this means all the textures have
			// been freed and there still isn't enough texture memory.
			return -1; }
		else {
			// Get rid of the least recently used texture.
			r_UnloadSystemTexture((TextureData*)pListHead->m_pPrev->m_pData);
			++count; } }
	return count;
}

// Loads the texture and installs it.
LTRESULT r_LoadSystemTexture(SharedTexture *pSharedTexture, TextureData* &pTextureData, LTBOOL bBind)
{
	FileIdentifier *pIdent;
	LThreadMessage msg;
	ILTStream *pStream;
	LTRESULT dResult;
	ConParse parse;
	FileRef ref;

	pTextureData = LTNULL;
	pIdent = pSharedTexture->m_pFile;
	if (!pIdent) return LT_NOTINITIALIZED;

	pStream = cf_OpenFileIdentifier(g_pClientMgr->m_hFileMgr, pIdent);
	if (pStream) {
		dResult = dtx_Create(pStream, &pTextureData, LTFALSE, LTFALSE, true);
		pStream->Release();

		if (dResult != LT_OK) return dResult; }
	else {
		RETURN_ERROR_PARAM(1, r_LoadSystemTexture, LT_MISSINGFILE, pIdent->m_Filename); }

	// Add the new texture to the MRU list.
	dl_AddHead(&g_SysCache.m_List, &pTextureData->m_Link, pTextureData);
	g_SysCache.m_CurMem += pTextureData->m_AllocSize;

	// Store its pointer in the SharedTexture.
	pSharedTexture->m_pEngineData = pTextureData;
	pTextureData->m_pSharedTexture = pSharedTexture;

	if (bBind) {
		r_BindTexture(pSharedTexture, LTFALSE); }

	// Load its detail texture?
	parse.Init(pTextureData->m_Header.m_CommandString);
	if (parse.ParseFind("DetailTex", LTFALSE, 1)) {
		ref.m_FileType = FILE_CLIENTFILE;
		ref.m_pFilename = parse.m_Args[1];
		pSharedTexture->m_pDetailTexture = g_pClientMgr->AddSharedTexture(&ref);
		pSharedTexture->m_EnvMap = ENVMAP_NONE; }
	else  {
		parse.Init(pTextureData->m_Header.m_CommandString);
		if (parse.ParseFind("EnvMap", LTFALSE, 1)) {
			ref.m_FileType = FILE_CLIENTFILE;
			ref.m_pFilename = parse.m_Args[1];
			pSharedTexture->m_pDetailTexture = g_pClientMgr->AddSharedTexture(&ref);
			pSharedTexture->m_EnvMap = ENVMAP_SIGNED; }
		parse.Init(pTextureData->m_Header.m_CommandString);
		if (parse.ParseFind("EnvMapAlpha", LTFALSE, 1)) {
			ref.m_FileType = FILE_CLIENTFILE;
			ref.m_pFilename = parse.m_Args[1];
			pSharedTexture->m_pDetailTexture = g_pClientMgr->AddSharedTexture(&ref);
			pSharedTexture->m_EnvMap = ENVMAP_ALPHA; } }
	return LT_OK;
}

// ------------------------------------------------------------ //
// RenderStruct function implementations.
// ------------------------------------------------------------ //
LTObject* r_ProcessAttachment(LTObject *pParent, Attachment *pAttachment)
{
	LTObject *pChild;
	LTransform tAttachment;
	LTRESULT dResult;

	// Use CommonLT::GetAttachmentTransform.
	dResult = g_pClientMgr->m_pClientDE->Common()->GetAttachmentTransform((HATTACHMENT)pAttachment, tAttachment, LTTRUE);
	if (dResult != LT_OK) return LTNULL;

	pChild = g_pClientMgr->FindObject(pAttachment->m_ChildID);
	if (!pChild) return LTNULL;

	g_pClientMgr->MoveObject(pChild, &tAttachment.m_Pos, LTFALSE);
	g_pClientMgr->RotateObject(pChild, &tAttachment.m_Rot);
	return pChild;
}

static SharedTexture* r_GetSharedTexture(const char *pFilename)
{
	FileRef ref;

	ref.m_FileType = FILE_ANYFILE;
	ref.m_pFilename = (char *)pFilename; // Note : I hate when things aren't const, and they should be..

	return g_pClientMgr->AddSharedTexture(&ref);
}


static TextureData*	r_GetTexture(SharedTexture *pTexture, int *nUnloaded)
{
	TextureData *pRet;

	// Is it already loaded?
	if(pTexture->m_pEngineData)
	{
		// Put it at the start of the MRU.
		pRet = (TextureData*)pTexture->m_pEngineData;
		return pRet;
	}
	else
	{
		r_LoadSystemTexture(pTexture, pRet, LTFALSE);
		return pRet;
	}
}


static void r_FreeTexture(SharedTexture *pTexture)
{
	if (pTexture->m_pEngineData) {
		TextureData* pTextureData = (TextureData*)pTexture->m_pEngineData;
		r_UnloadSystemTexture(pTextureData); }
}


static void r_RunConsoleString(char *pStr)
{
	cc_HandleCommand(&g_ClientConsoleState, pStr);
}

static void r_ConsolePrint(char *pMsg, ...)
{
	char msg[300];
	va_list marker;

	va_start(marker, pMsg);
	_vsnprintf(msg, 299, pMsg, marker);
	va_end(marker);

	con_WhitePrintf(msg);
}

void r_GenericTextPrint(char *pMsg, const LTRect *pRect, uint32 textColor)
{
	GETCONSOLE()->DrawTextLine(pMsg,pRect,textColor);
}

static HLTPARAM r_GetParameter(char *pName)
{
	return (HLTPARAM)cc_FindConsoleVar(&g_ClientConsoleState, pName);
}

static float r_GetParameterValueFloat(HLTPARAM hParam)
{
	if(hParam)
	{
		return ((LTCommandVar*)hParam)->floatVal;
	}
	else
	{
		return 0.0f;
	}
}

static char* r_GetParameterValueString(HLTPARAM hParam)
{
	if(hParam)
	{
		return ((LTCommandVar*)hParam)->pStringVal;
	}
	else
	{
		return LTNULL;
	}
}

static void r_RendererPing()
{
}

static uint32 r_GetObjectFrameCode()
{
	return g_pClientMgr->m_ObjectMgr.GetFrameCode();
}

static uint32 r_IncObjectFrameCode()
{
	return g_pClientMgr->m_ObjectMgr.IncFrameCode();
}

static uint16 r_IncCurTextureFrameCode()
{
	return g_pClientMgr->IncCurTextureFrameCode();
}

static void* r_Alloc(uint32 size)
{
	return dalloc(size);
}

static void r_Free(void *ptr)
{
	dfree(ptr);
}

static void r_InitSysCache(SysCache *pCache, uint32 maxMem)
{
	dl_InitList(&pCache->m_List);
	pCache->m_CurMem = 0;
	pCache->m_MaxMem = maxMem;
}


// ------------------------------------------------------------ //
// The global RenderStruct.
// ------------------------------------------------------------ //

RenderStruct g_Render;


// ------------------------------------------------------------ //
// Interface functions.
// ------------------------------------------------------------ //

// For MemoryWatch.
unsigned long GetInternalTextureMemory()
{
	return g_SysCache.m_CurMem;
}

unsigned long GetRendererTextureMemory()
{
	return g_Render.m_SystemTextureMemory;
}

void r_InitRenderStruct(LTBOOL bFullClear)
{
	if(bFullClear)
		memset(&g_Render, 0, sizeof(g_Render));
	else
		memset(&g_Render, 0, offsetof(RenderStruct, m_DontClearMarker));
	
	g_Render.ProcessAttachment = r_ProcessAttachment;	
	g_Render.GetSharedTexture = r_GetSharedTexture;
	g_Render.GetTexture = r_GetTexture;
	g_Render.FreeTexture = r_FreeTexture;
	g_Render.RunConsoleString = r_RunConsoleString;
	g_Render.ConsolePrint = r_ConsolePrint;
	g_Render.GetParameter = r_GetParameter;
	g_Render.GetParameterValueFloat = r_GetParameterValueFloat;
	g_Render.GetParameterValueString = r_GetParameterValueString;
	g_Render.Ping = r_RendererPing;
	g_Render.IncObjectFrameCode = r_IncObjectFrameCode;
	g_Render.GetObjectFrameCode = r_GetObjectFrameCode;
	g_Render.IncCurTextureFrameCode = r_IncCurTextureFrameCode;
	g_Render.AddandStartProfileCounter = r_AddandStartProfileCounter;
	g_Render.AddProfileCounter = r_AddProfileCounter;
	g_Render.StartProfileCounter = r_StartProfileCounter;
	g_Render.StopProfileCounter = r_StopProfileCounter;
	g_Render.Alloc = r_Alloc;
	g_Render.Free = r_Free;

	g_Render.m_GlobalLightDir.Init(0.0f, -2.0f, -1.0f);
    g_Render.m_GlobalLightDir.Norm();
	g_Render.m_AmbientLight = 0;
	g_Render.m_ChangeFlags = 0;
}

bool g_bFirstTimeInit = true;
LTRESULT r_InitRender(RMode *pMode)
{
	RenderStructInit init;
	int initStatus;
	HWND hWnd;
	char cmd[200];


	// Don't get in here recursively.
	if(g_ClientGlob.m_bInitializingRenderer)
		return LT_OK;


	VarSetter<BOOL> setter(&g_ClientGlob.m_bInitializingRenderer, LTTRUE, LTFALSE);

	
	r_TermRender(0, false);


	hWnd = (HWND)dsi_GetMainWindow();
	ShowWindow(hWnd, SW_RESTORE);
	
	
	// This is hard coded for now..
	if (g_bFirstTimeInit) 
		r_InitSysCache(&g_SysCache, 10000*1000); // 10M for textures.

	
	// Setup the init request.
	memset(&init, 0, sizeof(init));
	init.m_hWnd = (void*)hWnd;
	memcpy(&init.m_Mode, pMode, sizeof(RMode));


	// Set up the RenderStruct.
	rdll_RenderDLLSetup(&g_Render);

	// Store these.. the renderer may change them for pixel doubling.
	g_Render.m_Width = pMode->m_Width;
	g_Render.m_Height = pMode->m_Height;

	// Try to initialize the renderer for the requested mode.
	initStatus = g_Render.Init(&init);
	if(initStatus != RENDER_OK || init.m_RendererVersion != LTRENDER_VERSION)
	{
		g_pClientMgr->SetupError(LT_ERRORLOADINGRENDERDLL, "Init Failed");
		RETURN_ERROR_PARAM(1, r_InitRender, LT_ERRORLOADINGRENDERDLL, "Init Failed");
	}

	// Init the console.
	g_pClientMgr->InitConsole();

	// Restore interface surfaces that were backed up.
	if(!cis_RendererIsHere(r_GetRenderStruct()))
	{
		g_pClientMgr->SetupError(LT_UNABLETORESTOREVIDEO);
		RETURN_ERROR(1, r_InitRender, LT_UNABLETORESTOREVIDEO);
	}

	g_Render.m_bInitted = LTTRUE;

	// Let the game do a loading screen or something.
	if(g_pClientMgr->m_pClientShell)
	{
		g_pClientMgr->m_pClientShell->OnEvent(
			LTEVENT_RENDERALMOSTINITTED, 0);
	}

	// Bind any open worlds.
	if(!g_pClientMgr->BindClientShellWorlds())
	{
		g_Render.m_bInitted = LTFALSE;
		RETURN_ERROR(1, r_InitRender, LT_ERRORBINDINGWORLD);
	}

	// Set focus and capture the mouse.  We leave things like resizing the window to the render DLL.
	SetFocus(hWnd);

	// Clip the cursor so they don't hit the task bar.		
	/*
	if(!g_bNullRender)
	{
		GetWindowRect(GetDesktopWindow(), &wndRect);
		xBorder = (wndRect.right - wndRect.left) / 6;
		yBorder = (wndRect.bottom - wndRect.top) / 6;
		wndRect.left += xBorder;
		wndRect.top += yBorder;
		wndRect.right -= xBorder;
		wndRect.bottom -= yBorder;
		ClipCursor(&wndRect);
	}
	*/

	// Bind any open textures.
	g_pClientMgr->BindSharedTextures();

	// Store this config..
	memcpy(&g_RMode, &init.m_Mode, sizeof(RMode));

	// Set the console variables.
	sprintf(cmd, "CardDesc %s", init.m_Mode.m_InternalName);
	c_CommandHandler(cmd);

	sprintf(cmd, "ScreenWidth %d", init.m_Mode.m_Width);
	c_CommandHandler(cmd);

	sprintf(cmd, "ScreenHeight %d", init.m_Mode.m_Height);
	c_CommandHandler(cmd);

	sprintf(cmd, "BitDepth %d", init.m_Mode.m_BitDepth);
	c_CommandHandler(cmd);

	// The console can load its background now that we're initialized.
	con_LoadBackground();

	// Tell the game the renderer has initialized.
	if(g_pClientMgr->m_pClientShell)
	{
		g_pClientMgr->m_pClientShell->OnEvent(
			LTEVENT_RENDERINIT, 0);
	}
	
	g_bFirstTimeInit = false;
	return LT_OK;
}

LTRESULT r_TermRender(int surfaceHandling, bool bUnLoadDLL)
{
	LTLink *pCur, *pNext, *pListHead;

	if(g_Render.m_bInitted)
	{
		// Tell the game the renderer has initialized.
		if(g_pClientMgr->m_pClientShell)
		{
			g_pClientMgr->m_pClientShell->OnEvent(
				LTEVENT_RENDERTERM, 0);
		}

		// Notify the video stuff.
		if (g_pClientMgr->m_pVideoMgr)
		{
			g_pClientMgr->m_pVideoMgr->OnRenderTerm();
		}

		// Notify the client interface system-dependent module..
		if(!cis_RendererGoingAway())
		{
			// If the surface backup failed and we care, give a message.
			if(surfaceHandling == 1)
			{
				g_pClientMgr->SetupError(LT_UNABLETORESTOREVIDEO);
				RETURN_ERROR(1, r_TermRender, LT_UNABLETORESTOREVIDEO);
			}
		}
		
		// Unbind everybody from the renderer.
		g_pClientMgr->TermConsole();
		g_pClientMgr->UnbindSharedTextures(bUnLoadDLL);
		g_pClientMgr->UnbindClientShellWorlds();

		g_Render.Term(bUnLoadDLL);
		
		// Get rid of all the loaded textures.
		if (bUnLoadDLL) {
			pListHead = &g_SysCache.m_List.m_Head;
			pCur = pListHead->m_pNext;
			while(pCur != pListHead) {
				pNext = pCur->m_pNext;
				r_UnloadSystemTexture((TextureData*)pCur->m_pData);
				pCur = pNext; }
			ASSERT(g_SysCache.m_CurMem == 0); }

		// Un-clip the cursor.
		if (g_CV_CursorCenter) {
			ClipCursor(LTNULL); }

		g_Render.m_bInitted = LTFALSE;
		r_InitRenderStruct(LTFALSE); // Reinitialize the renderstruct.
	}

	return LT_OK;
}


void r_BindTexture(SharedTexture *pSharedTexture, LTBOOL bTextureChanged)
{
	if (g_Render.m_bInitted) {
		g_Render.BindTexture(pSharedTexture, bTextureChanged ? true : false); }
}

void r_DisableTextures()
{
	if(g_Render.m_bInitted) {
		g_Render.DisableTextures(); }
}

void r_UnbindTexture(SharedTexture *pSharedTexture, bool bUnLoad_EngineData)
{
	if (g_Render.m_bInitted) {
		g_Render.UnbindTexture(pSharedTexture); }

	if (bUnLoad_EngineData && pSharedTexture->m_pEngineData) {
		r_UnloadSystemTexture((TextureData*)pSharedTexture->m_pEngineData);
		pSharedTexture->m_pEngineData = LTNULL; }
}

void r_OneTimeInit()
{
	memset(&g_RMode, 0, sizeof(g_RMode));
}


void r_OneTimeTerm()
{
	if(g_CurrentRenderDLL)
	{
		dfree(g_CurrentRenderDLL);
		g_CurrentRenderDLL = LTNULL;
	}
}





