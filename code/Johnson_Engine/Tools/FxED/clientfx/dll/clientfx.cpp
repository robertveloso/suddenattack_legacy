//------------------------------------------------------------------
//
//   MODULE  : CLIENTFX.CPP
//
//   PURPOSE : Implements client FX
//
//   CREATED : On 10/6/98 At 9:21:36 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "resource.h"
#include "ClientFX.h"
#include "LightningFX.h"
#include "SpriteSystemFX.h"
#include "SpriteFX.h"
#include "ModelFX.h"
#include "DynaLightFX.h"
#include "PlaySoundFX.h"
#include "CamJitterFX.h"
#include "CamWobbleFX.h"
#include "BouncyChunkFX.h"
#include "NullFX.h"
#include "FallingStuffFX.h"
#include "PolyTubeFX.h"
#include "PlayRandomSoundFX.h"
#include "PolyFanFX.h"
#include "LTBModelFX.h"
#include "LTBBouncyChunkFX.h"

#ifdef _WIN32
	#include "windows.h"
#endif

HINSTANCE g_hInst;

// Globals....

LT_POLYGT3						g_pTris[MAX_TRIS];
LTVector							g_pVerts[MAX_VERTS];

int								g_nTris			= 0;
int								g_nVerts		= 0;

bool							g_bMultiplay	= false;
float							g_fDetail		= 1.0f;
int								g_nDetail		= 2;

HOBJECT							g_hPlayer;

bool							g_bAppFocus;

// Extern....

extern "C"
{

bool APIENTRY DllMain( HANDLE hModule, 
                       uint32  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	// Save the module instance

    g_hInst = (HINSTANCE)hModule;
	
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetNum()
//
//   PURPOSE  : Returns the number of special fx in this dll
//
//------------------------------------------------------------------

__declspec(dllexport) int fxGetNum()
{
	// Success !!

	return NUM_FX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetRef()
//
//   PURPOSE  : Returns a reference structure for a given fx
//
//------------------------------------------------------------------

__declspec(dllexport) FX_REF fxGetRef(int nFx)
{
	FX_REF fxRef;

	switch (nFx)
	{
		case 0 :
		{
			strcpy(fxRef.m_sName, "Lightning");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY | FX_NEEDMOTIONKEY;
			fxRef.m_pfnCreate	= fxCreateLightning;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetLightningProps;
		}
		break;

		case 1 :
		{
			strcpy(fxRef.m_sName, "SpriteSystem");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY | FX_NEEDMOTIONKEY;
			fxRef.m_pfnCreate	= fxCreateSpriteSystem;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetSpriteSystemProps;
		}
		break;

		case 2 :
		{
			strcpy(fxRef.m_sName, "Sprite");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY | FX_NEEDMOTIONKEY;
			fxRef.m_pfnCreate	= fxCreateSprite;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetSpriteProps;
		}
		break;

		case 3 :
		{
			strcpy(fxRef.m_sName, "Model");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY | FX_NEEDMOTIONKEY;
			fxRef.m_pfnCreate	= fxCreateModel;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetModelProps;
		}
		break;

		case 4 :
		{
			strcpy(fxRef.m_sName, "LTBModel");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY | FX_NEEDMOTIONKEY;
			fxRef.m_pfnCreate	= fxCreateLTBModel;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetLTBModelProps;
		}
		break;

		case 5 :
		{
			strcpy(fxRef.m_sName, "DynaLight");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY | FX_NEEDMOTIONKEY;
			fxRef.m_pfnCreate	= fxCreateDynaLight;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetDynaLightProps;
		}
		break;

		case 6 :
		{
			strcpy(fxRef.m_sName, "PlaySound");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDMOTIONKEY | FX_NEEDVOLUMEKEY;
			fxRef.m_pfnCreate	= fxCreatePlaySound;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetPlaySoundProps;
		}
		break;

		case 7 :
		{
			strcpy(fxRef.m_sName, "CamJitter");
			fxRef.m_dwType		= FX_NEEDCAMERA | FX_NEEDSCALEKEY;
			fxRef.m_pfnCreate	= fxCreateCamJitter;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetCamJitterProps;
		}
		break;

		case 8 :
		{
			strcpy(fxRef.m_sName, "WonkyFX");
			fxRef.m_dwType		= FX_NEEDCAMERA | FX_NEEDSCALEKEY;
			fxRef.m_pfnCreate	= fxCreateCamWobble;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetCamWobbleProps;
		}
		break;

		case 9 :
		{
			strcpy(fxRef.m_sName, "BouncyChunk");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY;
			fxRef.m_pfnCreate	= fxCreateBouncyChunk;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetBouncyChunkProps;
		}
		break;

		case 10 :
		{
			strcpy(fxRef.m_sName, "LTBBouncyChunk");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY;
			fxRef.m_pfnCreate	= fxCreateLTBBouncyChunk;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetLTBBouncyChunkProps;
		}
		break;

		case 11 :
		{
			strcpy(fxRef.m_sName, "Null");
			fxRef.m_dwType		= FX_NEEDOBJECT;
			fxRef.m_pfnCreate	= fxCreateNull;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetNullProps;
		}
		break;

		case 12 :
		{
			strcpy(fxRef.m_sName, "FallingStuff");
			fxRef.m_dwType		= FX_NEEDCAMERA | FX_NEEDOBJECT | FX_NEEDMOTIONKEY | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY;
			fxRef.m_pfnCreate	= fxCreateFallingStuff;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetFallingStuffProps;
		}
		break;

		case 13 :
		{
			strcpy(fxRef.m_sName, "PolyTrail");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDMOTIONKEY | FX_NEEDCOLOURKEY | FX_NEEDSCALEKEY;
			fxRef.m_pfnCreate	= fxCreatePolyTube;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetPolyTubeProps;
		}
		break;

		case 14 :
		{
			strcpy(fxRef.m_sName, "PlayRandomSound");
			fxRef.m_dwType		= FX_NEEDOBJECT | FX_NEEDMOTIONKEY;
			fxRef.m_pfnCreate	= fxCreatePlayRandomSound;
			fxRef.m_pfnDelete	= fxDelete;
			fxRef.m_pfnGetProps = fxGetPlayRandomSoundProps;
		}
		break;
	}
	
	// Success !!
	
	return fxRef;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetNumTypes()
//
//   PURPOSE  : Returns the different 'types' of FX for a given FX
//
//------------------------------------------------------------------

__declspec(dllexport) int fxGetNumTypes(char *sFx)
{
	if (!stricmp(sFx, "ParticleFlurry"))
	{
		return 1;
	}
	else if (!stricmp(sFx, "Lightning"))
	{
		return 2;
	}
	else if (!stricmp(sFx, "SpriteSystem"))
	{
		return 1;
	}
	
	// Failure !!!
	
	return 0;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetType()
//
//   PURPOSE  : Returns a string name type for a given 'type'
//
//------------------------------------------------------------------

__declspec(dllexport) char* fxGetType(char *sFx, int nFx)
{
	if (!stricmp(sFx, "ParticleFlurry"))
	{
		// Particle Flurry
	}
	else if (!stricmp(sFx, "Lightning"))
	{
		// Lightning
		
		switch (nFx)
		{
			case 0 : return "Lightning Ball"; break;
			case 1 : return "Lightning Bolt"; break;
		}
	}
	else if (!stricmp(sFx, "SpriteSystem"))
	{
		// Sprite System
	}

	// Failure !!!

	return NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxDelete()
//
//   PURPOSE  : Deletes the FX
//
//------------------------------------------------------------------

__declspec(dllexport) void fxDelete(CBaseFX *pDeleteFX)
{
	delete pDeleteFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetNumAnims()
//
//   PURPOSE  : returns the number of preset animations
//
//------------------------------------------------------------------

__declspec(dllexport) int fxGetNumAnims()
{
	return 6;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetAnimName()
//
//   PURPOSE  : returns the name of the nth preset animation
//
//------------------------------------------------------------------

#define AN(a, b) case (a) : return (b);

__declspec(dllexport) char* fxGetAnimName(int nAnim)
{
	switch (nAnim)
	{
		AN(0, "Small Spin X/Y")
		AN(1, "Small Spin Y/Z")
		AN(2, "Small Spin X/Z")
		AN(3, "Large Spin X/Y")
		AN(4, "Large Spin Y/Z")
		AN(5, "Large Spin X/Z")

		default : return "Unknown Animation";
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : fxSetMultiplay()
//
//   PURPOSE  : Sets multiplayer mode
//
//------------------------------------------------------------------

__declspec(dllexport) void fxSetMultiplay(bool bMultiplay)
{
	g_bMultiplay = bMultiplay;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxSetDetail()
//
//   PURPOSE  : Sets detail
//
//------------------------------------------------------------------

__declspec(dllexport) void fxSetDetail(float fDetail)
{
	g_fDetail = fDetail;

	if (g_fDetail == 1.0f)
	{
		g_nDetail = 2;
	}
	else if (g_fDetail = 0.5f)
	{
		g_nDetail = 1;
	}
	else
	{
		g_nDetail = 0;
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : fxSetPlayer()
//
//   PURPOSE  : Sets the current player (client) object
//
//------------------------------------------------------------------

__declspec(dllexport) void fxSetPlayer(HOBJECT hPlayer)
{
	g_hPlayer = hPlayer;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxSetParams()
//
//   PURPOSE  : Sets up parameters for FX engine
//
//------------------------------------------------------------------

__declspec(dllexport) void fxSetParams(FX_PARAMS *pParams)
{
	if (!pParams) return;

	g_bAppFocus = pParams->m_bAppFocus;
}

// End extern "C"
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateLightning()
//
//   PURPOSE  : Creates a lightning bolt FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateLightning()
{
	return new CLightningFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateSpriteSystem()
//
//   PURPOSE  : Creates a sprite system FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateSpriteSystem()
{
	return new CSpriteSystem;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateSprite
//
//   PURPOSE  : Creates a sprite FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateSprite()
{
	return new CSpriteFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateModel
//
//   PURPOSE  : Creates a model FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateModel()
{
	return new CModelFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateLTBModel
//
//   PURPOSE  : Creates a model FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateLTBModel()
{
	return new CLTBModelFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateDynaLight
//
//   PURPOSE  : Creates a dynamic light FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateDynaLight()
{
	return new CDynaLightFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreatePlaySound
//
//   PURPOSE  : Creates a play sound FX
//
//------------------------------------------------------------------

CBaseFX* fxCreatePlaySound()
{
	return new CPlaySoundFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateCamJitter
//
//   PURPOSE  : Creates a camera jitter FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateCamJitter()
{
	return new CCamJitterFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateCamWobble
//
//   PURPOSE  : Creates a camera wobble FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateCamWobble()
{
	return new CCamWobbleFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateBouncyChunk
//
//   PURPOSE  : Creates a bouncy chunk FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateBouncyChunk()
{
	return new CBouncyChunkFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateLTBBouncyChunk
//
//   PURPOSE  : Creates a bouncy chunk FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateLTBBouncyChunk()
{
	return new CLTBBouncyChunkFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateNumbers
//
//   PURPOSE  : Creates a null FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateNull()
{
	return new CNullFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreateFallingStuff
//
//   PURPOSE  : Creates a falling stuff FX
//
//------------------------------------------------------------------

CBaseFX* fxCreateFallingStuff()
{
	return new CFallingStuffFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreatePolyTube
//
//   PURPOSE  : Creates a poly tube FX
//
//------------------------------------------------------------------

CBaseFX* fxCreatePolyTube()
{
	return new CPolyTubeFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreatePlayRandomSound
//
//   PURPOSE  : Creates a play random sound FX
//
//------------------------------------------------------------------

CBaseFX* fxCreatePlayRandomSound()
{
	return new CPlayRandomSoundFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxCreatePolyFan()
//
//   PURPOSE  : Creates a polyfan FX
//
//------------------------------------------------------------------

CBaseFX* fxCreatePolyFan()
{
	return new CPolyFanFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : AddBaseProps()
//
//   PURPOSE  : Adds base properties
//
//------------------------------------------------------------------

void AddBaseProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;
	float fVec[3];
	fVec[0] = 0.0f;
	fVec[1] = 0.0f;
	fVec[2] = 0.0f;

	fxProp.Combo("UpdatePos","1,Fixed,Follow,LeftHand,RightHand,LeftFoot,RightFoot,Head,Tail,u1,u2,u3,u4,u5,u6,u7,u8,u9,u10");
	pList->AddTail(fxProp);

	fxProp.Combo("RandGen", "0,None,BoundingBox");
	pList->AddTail(fxProp);

	fxProp.Combo("Interpolate", "0,Yes,No");
	pList->AddTail(fxProp);

	fxProp.Vector("Offset", fVec);
	pList->AddTail(fxProp);

	fxProp.Vector("RotateAdd", fVec);
	pList->AddTail(fxProp);
}

//------------------------------------------------------------------
//
//   FUNCTION : CalcColour()
//
//   PURPOSE  : Calculates the current colour based on parms
//
//------------------------------------------------------------------

void CBaseFX::CalcColour(float tmStart, float tmCur, float tmLifespan, float *pRed, float *pGreen, float *pBlue, float *pAlpha)
{
	float r, g, b, a;

	
	float tmActual;
	if (tmLifespan)
		tmActual = (float)fmod((tmCur - tmStart), tmLifespan) / tmLifespan;//((tmCur - tmStart) / tmLifespan) * m_tmLifespan;

	// Locate the keyframe

	CLinkListNode<FX_COLOURKEY> *pNode = m_collColourKeys.GetHead();

	while (pNode->m_pNext)
	{
		if ((tmActual >= pNode->m_Data.m_tmKey) && (tmActual < pNode->m_pNext->m_Data.m_tmKey))
		{
			FX_COLOURKEY startKey = pNode->m_Data;
			FX_COLOURKEY endKey = pNode->m_pNext->m_Data;

			// Use this and the previous key to compute the colour

			float tmDist = endKey.m_tmKey - startKey.m_tmKey;
			float tmKey  = tmActual - startKey.m_tmKey;
			
			if (tmDist > 0.0f)
			{
				float ratio = tmKey / tmDist;
				
				r = (startKey.m_red == endKey.m_red) ? startKey.m_red / 255.0f : (startKey.m_red + ((endKey.m_red - startKey.m_red) * ratio)) / 255.0f;
				g = (startKey.m_green == endKey.m_green) ? startKey.m_green / 255.0f : (startKey.m_green + ((endKey.m_green - startKey.m_green) * ratio)) / 255.0f;
				b = (startKey.m_blue == endKey.m_blue) ? startKey.m_blue / 255.0f : (startKey.m_blue + ((endKey.m_blue - startKey.m_blue) * ratio)) / 255.0f;
				a = (startKey.m_alpha == endKey.m_alpha) ? startKey.m_alpha / 255.0f : (startKey.m_alpha + (endKey.m_alpha - startKey.m_alpha) * ratio) / 255.0f;
			}
			else
			{
				r = endKey.m_red / 255.0f;
				g = endKey.m_green / 255.0f;
				b = endKey.m_blue / 255.0f;
				a = endKey.m_alpha / 255.0f;
			}

			if (r < 0.0f) r = 0.0f;
			if (r > 1.0f) r = 1.0f;
			if (g < 0.0f) g = 0.0f;
			if (g > 1.0f) g = 1.0f;
			if (b < 0.0f) b = 0.0f;
			if (b > 1.0f) b = 1.0f;
			if (a < 0.0f) a = 0.0f;
			if (a > 1.0f) a = 1.0f;
			a = 1.0f - a;

			*pRed   = r;
			*pGreen = g;
			*pBlue  = b;
			*pAlpha = a;
		}

		pNode = pNode->m_pNext;
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : CalcScale()
//
//   PURPOSE  : Calulates object scale
//
//------------------------------------------------------------------

void CBaseFX::CalcScale(float tmStart, float tmCur, float tmLifespan, float *pScale)
{
	float tmActual = ((tmCur - tmStart) / tmLifespan) * m_tmLifespan;
	tmActual = (float)fmod(tmActual, m_tmLifespan);

	// Locate the keyframe

	CLinkListNode<FX_SCALEKEY> *pNode = m_collScaleKeys.GetHead();

	while (pNode->m_pNext)
	{
		if ((tmActual >= pNode->m_Data.m_tmKey) && (tmActual < pNode->m_pNext->m_Data.m_tmKey))
		{
			FX_SCALEKEY startKey = pNode->m_Data;
			FX_SCALEKEY endKey = pNode->m_pNext->m_Data;

			// Use this and the previous key to compute the colour

			float tmDist = endKey.m_tmKey - startKey.m_tmKey;
			float tmKey  = tmActual - startKey.m_tmKey;
			
			if (tmDist > 0.0f)
			{
				float rat = (endKey.m_scale - startKey.m_scale) / tmDist;

				*pScale = startKey.m_scale + (rat * tmKey);
			}
			else
			{
				*pScale = startKey.m_scale;
			}
		}

		pNode = pNode->m_pNext;
	}	
}
