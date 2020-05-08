//------------------------------------------------------------------
//
//   MODULE  : CLIENTFX.H
//
//   PURPOSE : Defines client fx stuff
//
//   CREATED : On 10/6/98 At 9:21:50 PM
//
//------------------------------------------------------------------

#ifndef __CLIENTFX_H_
	#define __CLIENTFX_H_

	// Includes....

	#include "basefx.h"
	#include "linklist.h"
	#include "iltclient.h"
	#include "iltdrawprim.h"
	#include "iltmath.h"
	#include "iltphysics.h"
	#include "ltvector.h"

	// Defines....

	#define NUM_FX								15

	extern "C"
	{

		__declspec(dllexport) int				fxGetNum();
		__declspec(dllexport) FX_REF			fxGetRef(int nFx);
	
		__declspec(dllexport) void				fxDelete(CBaseFX *pDeleteFX);

		__declspec(dllexport) int				fxGetNumAnims();
		__declspec(dllexport) char*				fxGetAnimName(int nAnim);

		__declspec(dllexport) void				fxSetMultiplay(bool bMultiplay);

		__declspec(dllexport) void				fxSetDetail(float fDetail);

		__declspec(dllexport) void				fxSetPlayer(HOBJECT hPlayer);

		__declspec(dllexport) void				fxSetParams(FX_PARAMS *pParams);

		// Some useful functions....

		inline int Min(int a, int b) { return (a < b) ? a : b; }
	}

	// FX Creation functions

	void									AddBaseProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateLightning();
	void									fxGetLightningProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateSpriteSystem();
	void									fxGetSpriteSystemProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateSprite();
	void									fxGetSpriteProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateModel();
	void									fxGetModelProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateDynaLight();
	void									fxGetDynaLightProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreatePlaySound();
	void									fxGetPlaySoundProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateCamJitter();
	void									fxGetCamJitterProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateCamWobble();
	void									fxGetCamWobbleProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateBouncyChunk();
	void									fxGetBouncyChunkProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateLTBBouncyChunk();
	void									fxGetLTBBouncyChunkProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateNull();
	void									fxGetNullProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateFallingStuff();
	void									fxGetFallingStuffProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreatePolyTube();
	void									fxGetPolyTubeProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreatePlayRandomSound();
	void									fxGetPlayRandomSoundProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreatePolyFan();
	void									fxGetPolyFanProps(CFastList<FX_PROP> *pList);

	CBaseFX*								fxCreateLTBModel();
	void									fxGetLTBModelProps(CFastList<FX_PROP> *pList);

	// Externs....

	#define MAX_TRIS						8192
	#define MAX_VERTS						MAX_TRIS * 3
	
	extern LT_POLYGT3 g_pTris[MAX_TRIS];
	extern int g_nTris;

	extern LTVector g_pVerts[MAX_VERTS];
	extern int g_nVerts;

	extern float g_fDetail;
	extern int   g_nDetail;

	extern bool g_bMultiplay;

	extern bool g_bAppFocus;

	// Helper functions

	inline void SetupVert(LT_POLYGT3 *pPoly, int nIndex, float x, float y, float z, uint8 r, uint8 g, uint8 b, uint8 a, float u, float v)
	{
		pPoly->verts[nIndex].x = x;
		pPoly->verts[nIndex].y = y;
		pPoly->verts[nIndex].z = z;
		pPoly->verts[nIndex].rgba.r = r;
		pPoly->verts[nIndex].rgba.g = g;
		pPoly->verts[nIndex].rgba.b = b;
		pPoly->verts[nIndex].rgba.a = a;
		pPoly->verts[nIndex].u = u;
		pPoly->verts[nIndex].v = v;
	}

	inline LTMatrix GetCamTransform(ILTClient *pClientDE, HOBJECT hCamera)
	{
		LTVector3f vPos;
		LTVector3f vRight, vUp, vForward;
		LTOrientation orient;
	
		pClientDE->Physics()->GetPosition(hCamera, &vPos);
		pClientDE->Physics()->GetRotation(hCamera, &orient);
		
		pClientDE->GetMathLT()->GetRotationVectors(orient, vUp, vRight, vForward);

		vPos.x = -vPos.x;
		vPos.y = -vPos.y;
		vPos.z = -vPos.z;

		LTMatrix mTran, mRot, mFull;
		
		LTVector r = vRight;
		LTVector u = vUp;
		LTVector f = vForward;

		Mat_SetBasisVectors(&mRot, &r, &u, &f);
		MatTranspose3x3(&mRot);

		Mat_Identity(&mTran);
		mTran.m[0][3] = vPos.x;
		mTran.m[1][3] = vPos.y;
		mTran.m[2][3] = vPos.z;

		MatMul(&mFull, &mRot, &mTran);

		return mFull;
	}

	inline void CalcColour(CLinkList<FX_COLOURKEY> *pList, float tmStart, float tmCur, float tmLifespan, float *pRed, float *pGreen, float *pBlue, float *pAlpha)
	{
		float r, g, b, a;		
		float tmActual;
		CLinkListNode<FX_COLOURKEY> *pNode;
			
		if (tmLifespan == 0.0f) tmLifespan = 0.01f;
		tmActual = (tmCur - tmStart) / tmLifespan;
		if (tmActual >= tmLifespan)
		{
			pNode = pList->GetTail();
			
			r = pNode->m_Data.m_red / 255.0f;
			g = pNode->m_Data.m_green / 255.0f;
			b = pNode->m_Data.m_blue / 255.0f;
			a = pNode->m_Data.m_alpha / 255.0f;

			if (r < 0.0f) r = 0.0f;
			if (r > 1.0f) r = 1.0f;
			if (g < 0.0f) g = 0.0f;
			if (g > 1.0f) g = 1.0f;
			if (b < 0.0f) b = 0.0f;
			if (b > 1.0f) b = 1.0f;

			a = 1.0f - a;
			if (a < 0.0f) a = 0.0f;
			if (a > 1.0f) a = 1.0f;

			*pRed   = r;
			*pGreen = g;
			*pBlue  = b;
			*pAlpha = a;

			return;
		}

		// Locate the keyframe

		pNode = pList->GetHead();

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



#endif