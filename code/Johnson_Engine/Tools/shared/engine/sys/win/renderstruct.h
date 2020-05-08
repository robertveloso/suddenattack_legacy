
#ifndef __RENDERSTRUCT_H__
#define __RENDERSTRUCT_H__

    #ifndef __PIXELFORMAT_H__
	#include "pixelformat.h"
    #endif

    #ifndef __D3D8_H__
	#include <d3d8.h>
    #define __D3D8_H__
    #endif

	#include "d3dddstructs.h"

    class SharedTexture;
    class TextureData;
	class Attachment;
	class MainWorld;
    struct RenderInfoStruct;

    #ifndef __RENDEROBJECT_H__
    #include "renderobject.h"
    #endif

	#ifndef NUM_MIPMAPS
		#define NUM_MIPMAPS 8	// Changed from 4 in winter. -JE
	#endif


	
	// Change flags.. helps the renderer decide whether or not to do a VisitPVS.
	#define RSCHANGE_ADDOBJECT		(1<<0)
	#define RSCHANGE_REMOVEOBJECT	(1<<1)


	// Version checking..
	#define LTRENDER_VERSION	3421


	// Used to prevent super small changes to lightmap animations from regenerating
	// a bunch of lightmaps.
	#define LM_CHANGE_RESOLUTION	32

	// How much the lightmap grid is stretched out.
	#define DEFAULT_LIGHTMAP_GRID_SIZE		20.0f
	
	// We can't have lightmap textures larger than this.
	#define LIGHTMAP_MAX_PIXELS		32.0f
	#define LIGHTMAP_MAX_PIXELS_I	32
	#define LIGHTMAP_MAX_TOTAL_PIXELS	(LIGHTMAP_MAX_PIXELS_I * LIGHTMAP_MAX_PIXELS_I)
	#define LIGHTMAP_MAX_DATA_SIZE		(LIGHTMAP_MAX_TOTAL_PIXELS * 2)
	
	#define DEFAULT_LIGHTMAP_PIXELS		32.0f

	inline int IsValidLightmapSize(int size)
	{
		return size==16 || size==32;
	}


	// Values.
	#define RENDER_OK			0
	#define RENDER_ERROR		1


	// Blit flags.
	#define BLIT_TRANSPARENT	1	// Transparent blit.


	// Different buffer formats.
	#define BF_RGB_565		0
	#define BF_RGB_555		1


	
	#define OPTIMIZE_NO_TRANSPARENCY	0xFFFFFFFF

	
	// Used when doing texture memory tracking.
	#define TT_WORLD		0
	#define TT_LIGHTMAP		1
	#define TT_PARTICLE		2
	#define TT_SPRITE		3
	#define TT_SKIN			4
	#define NUM_TEXTURETYPES	5

	struct RenderContext;
	typedef RenderContext* HRENDERCONTEXT;
	typedef void* HLTPARAM;
	typedef void* HLTBUFFER;

	// Draw modes.
	#define DRAWMODE_NORMAL		1	// Render normally.
	#define DRAWMODE_OBJECTLIST	2	// Only render the objects in m_pObjectList.



	#define NUM_SOFTSKYTEXTURES	6 // One for each side of the box.. indexed by the PLANE_ defines.

	
	// Renderer info flags.
	#define RI_PLACEHOLDER	1

	struct IDirect3DDevice8;

	// This is passed in when creating a world context.
	struct RenderContextInit
	{
		MainWorld	*m_pWorld;
	};


	// This is passed in when rendering a scene.
	struct SceneDesc
	{
		// How to draw.  One of the DRAWMODE_ defines above.
		int				m_DrawMode;
		
		LTVector		m_GlobalModelLightAdd;		// Global model light addition.
		LTVector		m_GlobalModelLightDirAdd;	// Global model light directional addition.
		
		// The context gotten from CreateContext.
		HRENDERCONTEXT	m_hRenderContext;

		// Global lighting.  (RGB 0-1)
		LTVector		m_GlobalLightColor;		// Not used currently.
		LTVector		m_GlobalLightDir;
		
		LTVector		m_GlobalLightScale;		// RGB 0-1, scale / darken the light.
		LTVector		m_GlobalLightAdd;		// RGB 0-1, add / brighten the light.  This
												// draws a poly over the screen if one of these
												// is nonzero (so it's slower than scale).

		float			m_FrameTime;			// Used for some special fx calculations.

		// Sky definition and objects.
		SkyDef			m_SkyDef;
		LTObject		**m_SkyObjects;
		int				m_nSkyObjects;

		// Viewport rectangle.
		LTRect			m_Rect;

		// Field of view.
		float			m_xFov, m_yFov;

		// Clipping distances.
		float			m_FarZ;
		
		// Position info.
		LTVector		m_Pos;
		LTRotation		m_Rotation;
		
		// Objects to draw, if mode is DRAWMODE_OBJECTSONLY.
		LTObject		**m_pObjectList;
		int				m_ObjectListSize;

		// If ModelHookFn is set, then the renderer will call it before drawing each model.
		void			(*m_ModelHookFn)(ModelHookData *pData, void *pUser);
		void			*m_ModelHookUser;
	};


	struct RenderStructInit
	{
		int		m_RendererVersion;	// The renderer MUST set this to LTRENDER_VERSION.
		RMode	m_Mode;		// What mode we want to use.
							// The renderer fills in the actual mode that it set.
		void	*m_hWnd;	// The main window.
	};


	// A blit command.
	class BlitRequest
	{
	public:

						BlitRequest()
						{
							m_hBuffer = NULL;
							m_BlitOptions = 0;
							m_pSrcRect = NULL;
							m_pDestRect = NULL;
							m_Alpha = 1.0f;
						}


	public:

		HLTBUFFER		m_hBuffer;			// The buffer to blit.
		uint32			m_BlitOptions;		// Combination of the BLIT_ flags above.
		GenericColor	m_TransparentColor;	// Transparent color.
		LTRect			*m_pSrcRect;		// Source (m_hBuffer) rectangle.
		LTRect			*m_pDestRect;		// Destination rectangle.
		float			m_Alpha;			// Alpha value (0-1).

		LTWarpPt		*m_pWarpPts;
		int				m_nWarpPts;
	};


	struct GlobalPanInfo
	{
		SharedTexture	*m_pTexture;
		float	m_Offset[2];
		float	m_Scale[2];
	};


	struct RenderStruct
	{
		// Functions LithTech implements.

			// Processes the attachment.  Returns the child object if it exists.
			LTObject*	(*ProcessAttachment)(LTObject *pParent, Attachment *pAttachment);

			// Get a shared texture from a file name  (returns NULL on failure)
			SharedTexture*	(*GetSharedTexture)(const char *pFilename);

			// Gets the texture in memory (guaranteed to be in memory until the next 
			// call to GetTexture).  If NULL is not returned, nUnloaded is set to
			// the number of textures it had to unload (in system memory) to make
			// space for the texture.
			TextureData*	(*GetTexture)(SharedTexture *pTexture, int *nUnloaded);

			// Force it to free this texture.
			void			(*FreeTexture)(SharedTexture *pTexture);

			// Runs a string in the console.  The render drivers usually use this
			// so they can get HLTPARAMs right away and not have to check for
			// them all the time.
			void			(*RunConsoleString)(char *pString);

			// Print a string in the console.			
			void			(*ConsolePrint)(char *pMsg, ...);

			// Gets a parameter from the game (something that can be set from the console).
			// Returns NULL if the parameter doesn't exist.
			HLTPARAM		(*GetParameter)(char *pName);
			
			// Gets the value of a parameter .. returns 0/NULL if you pass in NULL.
			float			(*GetParameterValueFloat)(HLTPARAM hParam);
			char*			(*GetParameterValueString)(HLTPARAM hParam);

			// The renderer should call this a few times as it renders.. gives the
			// client a chance to process input quickly and get it to the server
			// as fast as possible.
			void			(*Ping)();

			// Increments the object frame code.  This is needed for portals.
			uint32			(*IncObjectFrameCode)();
			uint32			(*GetObjectFrameCode)();
			
			// Returns a texture frame code that is guaranteed to not be set in 
			// any SharedTextures.  The renderer is expected to set this frame code on
			// any SharedTextures that it uses while rendering so the engine can know
			// if a texture was viewed or not.
			uint16			(*IncCurTextureFrameCode)();

			// Profile Manager (Performance counter) access..
			uint32			(*AddandStartProfileCounter)(uint32 dwCounterGroup, const char* szCounterName);
			uint32			(*AddProfileCounter)(uint32 dwCounterGroup, const char* szCounterName);
			void			(*StartProfileCounter)(uint32 iCounterID);
			void			(*StopProfileCounter)(uint32 iCounterID);
	
			// Allocators...
			void*			(*Alloc)(uint32 size);
			void			(*Free)(void *ptr);
	

		// The engine maintains these variables.

			uint32			m_Width;
			uint32			m_Height;
			int				m_bInitted;


		// The renderer maintains these.

			uint32	m_SceneTextureMemory;		// How much texture memory used in current scene.
			uint32	m_LightmapTextureMemory;
			uint32	m_SystemTextureMemory;		// How much memory the renderer is using for textures.
			uint32	m_MPMemorySaved;			// How much memory was saved by using master palettes?
	

			// Used for PlayDemoProfile.
			uint32			m_PD_TagVisibleLeaves;
			uint32			m_PD_FlushObjectQueues;
			uint32			m_PD_Models;
			uint32			m_PD_WorldModels;
			uint32			m_PD_Translucent;

		
		// Functions implemented by the render driver.

			int				(*Init)(RenderStructInit *pInit);	// Returns RENDER_OK for success, or an error code.
			void			(*Term)(bool bFullTerm);

			IDirect3DDevice8* (*GetD3DDevice)(); // Note: In spring the renderer will link directly with the engine.
								//	RenderStruct will go away - the renderer will be the only thing that
								//	needs d3d. The DrawPrim interface lives in the engine for now (and it needs the Device).

			// Any textures you expect the renderer to use must be bound and unbound.
			// If bTextureChanged is TRUE, the renderer should reinitialize its data for the texture
			// even if it's already bound.
			void			(*BindTexture)(SharedTexture *pTexture, bool bTextureChanged);
			void			(*UnbindTexture)(SharedTexture *pTexture);
			D3DFORMAT		(*GetTextureDDFormat1)(BPPIdent BPP, uint32 iFlags);
			bool			(*QueryDDSupport)(PFormat* Format);
			void			(*GetTextureDDFormat2)(BPPIdent BPP, uint32 iFlags, PFormat* pFormat);
			bool			(*ConvertTexDataToDD)(uint8* pSrcData, PFormat* SrcFormat, uint32 SrcWidth, uint32 SrcHeight, uint8* pDstData, PFormat* DstFormat, uint32 DstWidth, uint32 DstHeight);
			void			(*DisableTextures)();

			// You render through a context.  Note: LithTech frees all of its lightmap data
			// after calling this because it assumes you converted it all into a more suitable format.
			HRENDERCONTEXT	(*CreateContext)(RenderContextInit *pContext);
			void			(*DeleteContext)(HRENDERCONTEXT hContext);

			// Clear a section of the screen.  Flags are from CLEARSCREEN_ flags in de_codes.h.
			void			(*Clear)(LTRect *pRect, uint32 flags, LTRGBColor& ClearColor);

			// Used around render calls.
			bool			(*Start3D)();
			bool			(*End3D)();
			bool			(*IsIn3D)();

			bool			(*StartOptimized2D)();
			void			(*EndOptimized2D)();
			bool			(*IsInOptimized2D)();
			bool			(*SetOptimized2DBlend)(LTSurfaceBlend blend);
			bool			(*GetOptimized2DBlend)(LTSurfaceBlend &blend);
			bool			(*SetOptimized2DColor)(HLTCOLOR color);
			bool			(*GetOptimized2DColor)(HLTCOLOR &color);
			
			// Render a scene.
			int				(*RenderScene)(SceneDesc *pScene);

			// Handle a command from the console.
			void			(*RenderCommand)(int argc, char **argv);

			// Show the backbuffer.
			void			(*SwapBuffers)(uint flags = 0);

			uint32			(*GetInfoFlags)();	// Combination of the RI_ flags above.

			// Get the screen pixel format.
			void			(*GetScreenFormat)(PFormat *pFormat);

			
			HLTBUFFER		(*CreateSurface)(int width, int height);
			void			(*DeleteSurface)(HLTBUFFER hSurf);

			void			(*GetSurfaceInfo)(HLTBUFFER hSurf, uint32 *pWidth, uint32 *pHeight);
			
			void*			(*LockSurface)(HLTBUFFER hSurf, uint32& Pitch);
			void			(*UnlockSurface)(HLTBUFFER hSurf);

			// Set transparentColor to OPTIMIZE_NO_TRANSPARENCY to not use transparency.
			bool			(*OptimizeSurface)(HLTBUFFER hSurf, uint32 transparentColor);
			void			(*UnoptimizeSurface)(HLTBUFFER hSurf);

			// Note: whenever you do this, you stall all async performance!
			bool			(*LockScreen)(int left, int top, int right, int bottom, void **pData, long *pPitch);
			void			(*UnlockScreen)();

			// Blit a surface to the screen.
			// No clipping is performed so it WILL crash if you don't clip the coordinates!
			// This function CAN be NULL if the driver doesn't want to support this, in
			// which case the engine will just lock the screen and do it itself.
			void			(*BlitToScreen)(BlitRequest *pRequest);
			bool			(*WarpToScreen)(BlitRequest *pRequest);

			// Make a screenshot file.
			void			(*MakeScreenShot)(char *pFilename);

			// Reads in new console variable values.
			void			(*ReadConsoleVariables)();

			// Get the current render info
			void			(*GetRenderInfo)(RenderInfoStruct *pStruct);

			// Blit from the screen.

			void			(*BlitFromScreen)(BlitRequest *pRequest);
			
			// Creating RenderObjects...
			CRenderObject*	(*CreateRenderObject)(CRenderObject::RENDER_OBJECT_TYPES ObjectType);
			bool			(*DestroyRenderObject)(CRenderObject* pObject);

			// This stuff MUST come last so it doesn't get zeroed out when switching res.

			int				m_DontClearMarker;
			
			// Global environment map.
			SharedTexture	*m_pEnvMap;
			
			// Global panning things.  Indexed by GLOBALPAN_ defines in de_codes.
			GlobalPanInfo		m_GlobalPans[NUM_GLOBALPAN_TYPES];

			LTVector			m_GlobalLightDir;
			LTVector			m_GlobalLightColor;
			uint32			m_AmbientLight; // 0-255

			// Combination of RSCHANGE_ flags above.
			uint32			m_ChangeFlags;
	};



	// This is what you use to select how you want to initialize the renderer..  Get a list of
	// the modes it supports, then copy the desired mode and pass it into RenderStruct::Init().
	
	// Get the list with GetSupportedModes() and pass that into FreeModeList() to free it.
	typedef RMode* (*GetSupportedModesFn)();
	typedef void (*FreeModeListFn)(RMode *pHead);


	// To make a DirectEngine rendering DLL, make a DLL with the function 
	// "RenderDLLSetup" that looks like this.  This function should init all the
	// function pointers in the structure.
	typedef void (*RenderDLLSetupFn)(RenderStruct *pStruct);


#endif  // __RENDERSTRUCT_H__

