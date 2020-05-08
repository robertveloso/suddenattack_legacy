
// This file defines DirectEngine's interface to the renderer and holds
// the global RenderStruct used to talk to it.

#ifndef __RENDER_H__
#define __RENDER_H__

    struct RenderStruct;
    struct RMode;

    //  Functions that used to be extracted from the render DLL with GetProcAddress
    //  but are now staticly linked.
	extern RMode* rdll_GetSupportedModes();
	extern void rdll_FreeModeList(RMode *pModes);
	extern void rdll_RenderDLLSetup(RenderStruct *pStruct);

    //  End.


    #ifndef __RENDERSTRUCT_H__
	#include "renderstruct.h"
    #endif


	extern RenderStruct g_Render;
	extern RMode		g_RMode;				// The current (or last successful) config for the renderer.


	class CClientMgr;

	
	// Render initialization status codes.
	#define R_OK					0
	#define R_CANTLOADLIBRARY		1
	#define R_INVALIDRENDERDLL		2
	#define R_INVALIDRENDEROPTIONS	3


	// Returns the RenderStruct it's using to talk to the driver.
	// Better be initialized...
	inline RenderStruct* r_GetRenderStruct() {return &g_Render;}


	inline LTBOOL r_IsRenderInitted() {return g_Render.m_bInitted;}

	// Called right at the beginning by the client.. initializes the RenderStruct
	// data members.
	void r_InitRenderStruct(LTBOOL bFullClear);

	// Only implemented for client.exe.. initializes the renderer.
	LTRESULT r_InitRender(RMode *pMode);

	// surfaceHandling
	//    0 = leave surfaces alone
	//    1 = backup surfaces
	//    2 = delete surfaces
	LTRESULT r_TermRender(int surfaceHandling, bool bUnLoadDLL);

	LTRESULT r_LoadSystemTexture(SharedTexture *pSharedTexture, 
		TextureData* &pData, LTBOOL bBind);

	void r_UnloadSystemTexture(TextureData *pTexture);

	// Each texture must be bound and unbound.
	void r_BindTexture(SharedTexture *pSharedTexture, LTBOOL bTextureChanged);
	void r_UnbindTexture(SharedTexture *pSharedTexture, bool bUnLoad_EngineData);
	void r_DisableTextures();

	// Called by the renderer and ILTClient::ProcessAttachments.
	LTObject* r_ProcessAttachment(LTObject *pParent, Attachment *pAttachment);

	// One-time init and term..
	void r_OneTimeInit();
	void r_OneTimeTerm();

	struct SysCache {
		uint32	m_MaxMem;
		uint32	m_CurMem;	// How much memory currently used?

		// All the loaded TextureDatas, in an MRU (most recently used are at
		// the start of the list).
		LTList	m_List; };
	extern SysCache g_SysCache;


#endif  // __RENDER_H__




