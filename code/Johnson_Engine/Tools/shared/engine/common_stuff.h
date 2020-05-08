
// Defines most of the console variables and some helper stuff.

#ifndef __COMMON_STUFF_H__
#define __COMMON_STUFF_H__

//#include "renderstruct.h"
	
class LMPage;
class MainWorld;

struct ObjectDraw {
	LTLink		m_Link;
	LTObject*	m_pObject; };

// Both renderers use this for the render contexts.
struct RenderContext {
	LMPage*		m_LMPages;		// Lightmap pages for d3drender.
	uint32		m_nLMPages;
	MainWorld*	m_pWorld;
	uint16		m_CurFrameCode; };

typedef void* HLTPARAM;

class ConVar
{
public:
	ConVar(char* pName, float defaultVal = 0.0f, int* pIntVal = LTNULL, float* pFloatVal = LTNULL);

	int			m_IntVal;
	float		m_FloatVal;
	float		m_DefaultVal;

	
	int*		m_pIntVal;		// Old-style ones that point to a global variable.
	float*		m_pFloatVal;

	char*		m_pName;
	HLTPARAM	m_hParam;
	ConVar*		m_pNext;
};

#define MAX_TEXTURE_GROUPS 10

extern bool		g_bWarbleScaleInitted;
extern float	g_CurrentWarbleScale;
extern bool		g_bFogInitted;
extern int		g_CurrentFogEnable, g_CurrentFogColor[3];
extern float	g_CurrentFogNearZ, g_CurrentFogFarZ;

extern int		g_CurrentTableFog;
extern ConVar	g_CV_TableFog;

extern bool		g_bDitherInitted;
extern int		g_CurrentDither;

extern bool		g_bBilinearInitted;
extern int		g_CurrentBilinear;
extern int		g_CV_Bilinear;

extern int		g_CV_EnvMapEnable;

extern float	g_CV_WarbleSpeed;
extern float	g_CV_WarbleScale, g_CV_OldWarbleScale;

extern float	g_CV_LightSaturate, g_CV_OldLightSaturate;
extern bool		g_bLightSaturateInitted;

extern int		g_CV_FixTJunc;
extern float	g_CV_EnvPanSpeed;
extern float	g_CV_EnvScale;
extern int		g_CV_EnvMapAll;

extern float	g_CV_SkyScale;
extern float	g_CV_ShadowZRange;

extern int		g_CV_OptimizeSurfaces;
extern int		g_CV_ShowSkySplits;

extern int		g_CV_DrawWorld;
extern int		g_CV_GroupOffsets[MAX_TEXTURE_GROUPS];
extern int		g_CV_TripleBuffer;
extern int		g_CV_UseDX6Commands;
extern int		g_CV_InvertHack;
extern int		g_CV_Force1Pass;

extern float	g_CV_Gamma, g_CV_OldGamma;
extern int		g_CV_LightAddPoly;
extern int		g_CV_ModelWarble;
extern int		g_CV_PaletteHackTest;

extern bool		g_CV_Force2Pass;
extern bool		g_bRunWindowed;

extern bool		g_bDrawToSystemMemory;

//extern RenderStruct* g_pStruct;
extern uint32	g_ScreenWidth, g_ScreenHeight;
extern HWND		g_hWnd;

extern int		g_ScreenScaleX, g_ScreenScaleY;
extern int		g_MipmapOffset;

extern float	g_ModelDists[3];
extern float	g_SpriteDists[3];

extern int		g_CV_ShowFillInfo;

// Turn on/off rendering of certain objects.
extern int		g_CV_DrawAll;

extern int		g_CV_FogColor[3];		// RGB (0-255)
extern float	g_fFogColor[3];			// RGB (0-1)
extern float	g_SpriteFogFactor[3];	// RGB (-255 to 255)

extern int		g_CV_FogEnable;
extern float	g_CV_FogNearZ, g_CV_FogFarZ;
extern float	g_CV_SkyFogNearZ, g_CV_SkyFogFarZ;
extern float	g_FogDistance;			// (g_CV_FogFarZ - g_CV_FogNearZ).
extern float	g_InvFogDist255;		// (1.0f / g_FogDistance) * 255

extern int		g_CV_LockPVS;
extern int		g_CV_ShowFullbriteModels;	
extern int		g_CV_BumpMap;
extern int		g_CV_DrawSky;
extern int		g_CV_TextureModels; // Should models use texturing?
extern int		g_CV_DrawModels;
extern int		g_CV_DrawLineSystems;
extern int		g_CV_DrawParticles;
extern int		g_CV_DrawSprites;
extern int		g_CV_DrawPolyGrids;

extern int		g_CV_ShowSplits;
extern int		g_CV_DynamicLight;
extern int		g_CV_FastLight;
extern int		g_CV_LightModels;
extern int		g_CV_ModelFullbrite;
extern int		g_CV_ShowTextureCounts;
extern int		g_CV_ShadowLodOffset;
extern int		g_CV_MaxModelShadows;
extern float	g_CV_LodScale;
extern int		g_CV_LodOffset;

extern int		g_CV_Wireframe;
extern int		g_CV_ModelBoxes;
extern int		g_CV_LightMap;
extern int		g_CV_DrawFlat;
extern int		g_CV_ShowPolyCounts;
extern int		g_CV_Dither;
extern int		g_CV_LightmapsOnly;
extern int		g_CV_RefRast;
extern int		g_CV_SWRast;

extern uint32	g_nFramesDrawn;

void*	dalloc(unsigned long size);
void*	dalloc_z(unsigned long size);
void	dfree(void *ptr);
void	AddDebugMessage(int debugLevel, char *pMsg, ...);
void	r_CreateConsoleVariables();
void	r_ReadConsoleVariables();
void	r_Toupper(char *pStr);
uint16	r_GetBitCount(uint32 mask);

inline void r_GetPolyVertices(WorldPoly *pPoly, PolyVertex **pPts, int *nPts) {
	if (g_CV_FixTJunc) {
		*pPts = pPoly->m_TVertices;
		*nPts = pPoly->m_nTVertices; }
	else {
		*pPts = pPoly->m_Vertices;
		*nPts = pPoly->m_nVertices; }
}			

// So the profiling can be compiled out...
#ifndef NO_PROFILE
#define AddandStartProfileCounter(X,Y)	g_pStruct->AddandStartProfileCounter(X,Y)
#define AddProfileCounter(X,Y)			g_pStruct->AddProfileCounter(X,Y)
#define StartProfileCounter(X)			g_pStruct->StartProfileCounter(X)
#define StopProfileCounter(X)			g_pStruct->StopProfileCounter(X)
#else 
#define AddandStartProfileCounter(X,Y)	0
#define AddProfileCounter(X,Y)			0
#define StartProfileCounter(X)
#define StopProfileCounter(X)
#endif

#endif  // __COMMON_STUFF_H__




