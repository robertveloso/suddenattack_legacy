
// This module defines lots of the main 3d operations like transformation,
// clipping, projection, rejection tests, etc..

#ifndef __3D_OPS_H__
#define __3D_OPS_H__

#include "common_stuff.h"
//#include "fixedpoint.h"

inline int RoundFloatToInt(float f)
{
	int nResult;

	__asm
	{
		fld f
		fistp nResult
	}
	return nResult;
}



// DEFINES
#define ROTATION_MAX	100000.0f

#define CLIP_EPSILON	0.00001f

// Clipping planes.  RenderStruct::m_ClipMask uses these.
#define CPLANE_NEAR				1
#define CPLANE_FAR				2
#define CPLANE_LEFT				4
#define CPLANE_TOP				8
#define CPLANE_RIGHT			16
#define CPLANE_BOTTOM			32

#define CPLANE_NEAR_INDEX		0
#define CPLANE_FAR_INDEX		1
#define CPLANE_LEFT_INDEX		2
#define CPLANE_TOP_INDEX		3
#define CPLANE_RIGHT_INDEX		4
#define CPLANE_BOTTOM_INDEX		5

#define CPLANE_ALL				0x3F
#define NUM_CLIPPLANES			6

// The formats for the vertices.
#define BASE_TLVERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR)
#define TLVERTEX_FORMAT (BASE_TLVERTEX_FORMAT | D3DFVF_TEX1)
#define LMVERTEX_FORMAT (BASE_TLVERTEX_FORMAT | D3DFVF_TEX2)

// Near Z is always this.
#define NEARZ			7.0f
#define MAX_FARZ		500000.0f

// Used for really close models.
#define REALLYCLOSE_NEARZ	0.01f

#define NEARZ_TEST(pt)			((pt).z >= g_ViewParams.m_NearClipZ)
#define FARZ_TEST(pt)			((pt).z <= g_ViewParams.m_FarClipZ)
#define LEFT_TEST(pt)			((pt).x > -(pt).z)
#define RIGHT_TEST(pt)			((pt).x < (pt).z)
#define TOP_TEST(pt)			((pt).y < (pt).z)
#define BOTTOM_TEST(pt)			((pt).y > -(pt).z)

#define TEST_TRICLIP(the_Test, newMask) {												\
		added = (int)the_Test((*pV1)) + (int)the_Test((*pV2)) + (int)the_Test((*pV3));	\
		if(added == 0) {																\
			return 1; }																	\
		else if(added != 3) {															\
			*outClipMask = (newMask) & clipMask;										\
			return 2; } }

#define NEARZ_CLIP(pt1, pt2)	t = (g_ViewParams.m_NearClipZ - pt1.z) / (pt2.z - pt1.z); \
								pOut->m_Vec.x = pt1.x + ((pt2.x - pt1.x) * t);\
								pOut->m_Vec.y = pt1.y + ((pt2.y - pt1.y) * t);\
								pOut->m_Vec.z = g_ViewParams.m_NearClipZ;

#define FARZ_CLIP(pt1, pt2)		t = (g_ViewParams.m_FarClipZ - pt1.z) / (pt2.z - pt1.z); \
								pOut->m_Vec.x = pt1.x + ((pt2.x - pt1.x) * t);\
								pOut->m_Vec.y = pt1.y + ((pt2.y - pt1.y) * t);\
								pOut->m_Vec.z = g_ViewParams.m_FarClipZ;

#define LEFT_CLIP(pt1, pt2)		t = -(pt1.x + pt1.z) / (pt2.x - pt1.x + pt2.z - pt1.z); \
								pOut->m_Vec.y = pt1.y + ((pt2.y - pt1.y) * t);\
								pOut->m_Vec.z = pt1.z + ((pt2.z - pt1.z) * t);\
								pOut->m_Vec.x = -pOut->m_Vec.z;

#define RIGHT_CLIP(pt1, pt2)	t = -(pt1.x - pt1.z) / (pt2.x - pt1.x - pt2.z + pt1.z);\
								pOut->m_Vec.y = pt1.y + ((pt2.y - pt1.y) * t);\
								pOut->m_Vec.z = pt1.z + ((pt2.z - pt1.z) * t);\
								pOut->m_Vec.x = pOut->m_Vec.z;

#define TOP_CLIP(pt1, pt2)		t = -(pt1.y - pt1.z) / (pt2.y - pt1.y - pt2.z + pt1.z);\
								pOut->m_Vec.x = pt1.x + ((pt2.x - pt1.x) * t);\
								pOut->m_Vec.z = pt1.z + ((pt2.z - pt1.z) * t);\
								pOut->m_Vec.y = pOut->m_Vec.z;

#define BOTTOM_CLIP(pt1, pt2)	t = -(pt1.y + pt1.z) / (pt2.y - pt1.y + pt2.z - pt1.z);\
								pOut->m_Vec.x = pt1.x + ((pt2.x - pt1.x) * t);\
								pOut->m_Vec.z = pt1.z + ((pt2.z - pt1.z) * t);\
								pOut->m_Vec.y = -pOut->m_Vec.z;

#define TEMPVERT_COUNT	128

extern uint32 g_nClipTests;

class TLVertex;
class LMVertex;

extern void *g_pClipVerts;
extern uint32 g_ClipVertsBufferSize;
extern uint32 g_MipmapTable[2];
extern ConVar g_CV_UseD3DClip;

typedef bool (__fastcall *ClipPolyFn)(uint32 clipMask, void **ppVerts, int *pnVerts);

// STRUCTURES
struct TLRGB    { uint8 b; uint8 g; uint8 r; uint8 a; };
struct RGBColor {
	union {
		TLRGB rgb;
		uint32 color; }; };

class FogHandlerNone
{
public:
	template<class V>
		static inline void HandleFog(V *pVert) { }
};

class TCoordSet
{
public:
	float	tu, tv;
};

// Our internal structure for a D3DTLVERTEX which makes it easier to
// access the color values.
class TLVertex
{
	public:
		static TLVertex* GetClipVerts() {
			return (TLVertex*)g_pClipVerts; }

			static void ClipExtra(TLVertex *pPrev, TLVertex *pCur, TLVertex *pOut, float t)
			{
				pOut->tu = pPrev->tu + t * (pCur->tu - pPrev->tu);
				pOut->tv = pPrev->tv + t * (pCur->tv - pPrev->tv);
				pOut->rgb.r = (uint8)RoundFloatToInt(pPrev->rgb.r + t * (pCur->rgb.r - pPrev->rgb.r));  
				pOut->rgb.g = (uint8)RoundFloatToInt(pPrev->rgb.g + t * (pCur->rgb.g - pPrev->rgb.g));  
				pOut->rgb.b = (uint8)RoundFloatToInt(pPrev->rgb.b + t * (pCur->rgb.b - pPrev->rgb.b)); 
				pOut->rgb.a = (uint8)RoundFloatToInt(pPrev->rgb.a + t * (pCur->rgb.a - pPrev->rgb.a)); 
				pOut->specular.rgb.a = (uint8)RoundFloatToInt(pPrev->specular.rgb.a + t * (pCur->specular.rgb.a - pPrev->specular.rgb.a)); 
			}

		LTVector m_Vec;
		float rhw;

		union
		{
			TLRGB rgb;
			uint32 color;
		};

		RGBColor specular;

		#include "vertextcoord.h"

		union
		{
			struct
			{
				float tu, tv;
			};
			TCoordSet m_TCoords[1];
		};


		#ifdef LGLIDE_COMPILE
			float tutv_oow;
		#endif
};


class LMVertex
{
	public:

		static LMVertex* GetClipVerts() 
		{
			return (LMVertex*)g_pClipVerts;
		}

		static void ClipExtra(LMVertex *pPrev, LMVertex *pCur, LMVertex *pOut, float t)
		{
			pOut->tu = pPrev->tu + t * (pCur->tu - pPrev->tu);
			pOut->tv = pPrev->tv + t * (pCur->tv - pPrev->tv);
			pOut->lm_tu = pPrev->lm_tu + t * (pCur->lm_tu - pPrev->lm_tu);
			pOut->lm_tv = pPrev->lm_tv + t * (pCur->lm_tv - pPrev->lm_tv);
			pOut->rgb.r = (uint8)RoundFloatToInt(pPrev->rgb.r + t * (pCur->rgb.r - pPrev->rgb.r));  
			pOut->rgb.g = (uint8)RoundFloatToInt(pPrev->rgb.g + t * (pCur->rgb.g - pPrev->rgb.g));  
			pOut->rgb.b = (uint8)RoundFloatToInt(pPrev->rgb.b + t * (pCur->rgb.b - pPrev->rgb.b)); 
			pOut->rgb.a = (uint8)RoundFloatToInt(pPrev->rgb.a + t * (pCur->rgb.a - pPrev->rgb.a)); 
			pOut->specular.rgb.a = (uint8)RoundFloatToInt(pPrev->specular.rgb.a + t * (pCur->specular.rgb.a - pPrev->specular.rgb.a)); 
		}

		LTVector m_Vec;
		float rhw;

		union
		{
			TLRGB rgb;
			uint32 color;
		};

		RGBColor specular;

		float tu, tv;
		float lm_tu, lm_tv;

		#include "vertextcoord.h"
};


class TLVertexProjTCoords
{
	public:

		static TLVertexProjTCoords* GetClipVerts() 
		{
			return (TLVertexProjTCoords*)g_pClipVerts;
		}

		static void ClipExtra(TLVertexProjTCoords *pPrev, TLVertexProjTCoords *pCur, TLVertexProjTCoords *pOut, float t)
		{
			pOut->tu = pPrev->tu + t * (pCur->tu - pPrev->tu);
			pOut->tv = pPrev->tv + t * (pCur->tv - pPrev->tv);
			pOut->tw = pPrev->tw + t * (pCur->tw - pPrev->tw);
			pOut->rgb.r = (uint8)RoundFloatToInt(pPrev->rgb.r + t * (pCur->rgb.r - pPrev->rgb.r));  
			pOut->rgb.g = (uint8)RoundFloatToInt(pPrev->rgb.g + t * (pCur->rgb.g - pPrev->rgb.g));  
			pOut->rgb.b = (uint8)RoundFloatToInt(pPrev->rgb.b + t * (pCur->rgb.b - pPrev->rgb.b)); 
			pOut->rgb.a = (uint8)RoundFloatToInt(pPrev->rgb.a + t * (pCur->rgb.a - pPrev->rgb.a)); 
			pOut->specular.rgb.a = (uint8)RoundFloatToInt(pPrev->specular.rgb.a + t * (pCur->specular.rgb.a - pPrev->specular.rgb.a)); 
		}

		LTVector m_Vec;
		float rhw;

		union
		{
			TLRGB rgb;
			uint32 color;
		};

		RGBColor specular;

		float tu, tv, tw;

		#include "vertextcoord.h"
};

// Use this macro when you have a buffer and you're not sure what kind of vertex it is.  This
// will make enough space to hold any type of vertex.
#define LARGEST_VERTEX	LMVertex




// This structure holds the info that is used to precalculate a bunch
// of view frustum stuff.  All its member are specified in camera
// space (translated and rotated so the camera is looking down Z).
struct ViewBoxDef
{
	LTVector	m_COP;				// Center of projection.
	float		m_WindowSize[2];	// Half-width and half-height of view window.
	float		m_NearZ, m_FarZ;	// Near and far Z (in relation to m_COP.z).
};


// The global viewing parameters.
class ViewParams
{
public:

	void		SetParity(int parity)
	{
		m_Parity = parity;
		m_ParityMul = m_Parity ? -1.0f : 1.0f;
	}
	float		UseParity(float x) {return m_Parity ? -x : x;}

	const LTVector&	GetFogRefPos() {return m_FogRefPos;}
	void			SetFogRefPos(LTVector pos);

	int				GetViewerVFogSide()		{return m_ViewerVFogSide;}
	float			GetViewerVFogDensity()	{return m_ViewerVFogDensity;}


public:

	ViewBoxDef	m_ViewBox; // Just copied from the input to r_Initfrustum.
	
	LTRect		m_Rect;

	// Particle min/max coordinates.
	float		m_ParticleMinX;
	float		m_ParticleMaxX;
	float		m_ParticleMinY;
	float		m_ParticleMaxY;
	int			m_iParticleMinX;
	int			m_iParticleMaxX;
	int			m_iParticleMinY;
	int			m_iParticleMaxY;

	// Screen/projection info.  
	
	// Field of view in radians.  These don't come in directly, they are generated
	// from the projection window.
	float		m_xFov, m_yFov;
	
	float		m_fScreenWidth, m_fScreenHeight;
	float		m_fHalfWidth, m_fHalfHeight;
	float		m_fInvHalfWidth, m_fInvHalfHeight;
	float		m_fCenterX, m_fCenterY;

	// For software texture mapper equations.
	float		m_XD, m_YD;
	float		m_InvXD, m_InvYD;
	float		m_XD_Times_YD;

	float		m_NearClipZ;	// Near clip Z.
	float		m_FarZ;			// Maximum far Z.
	float		m_FarClipZ;		// Current far Z clipping plane.

	// The scale to scale world coordinates into a 90 degree FOV.
	float		m_FovXScale;
	float		m_FovYScale;

	// Viewer matrix (passed into r_Initfrustum).
	LTMatrix	m_mViewer;
	
	// World space to camera space (puts camera at origin and aligns to XYZ axes).
	// (D3DTRANSFORMSTATE_VIEW)
	LTMatrix	m_mView;

	// These are the same as the d3d matrices.		
	// (D3DTRANSFORMSTATE_PROJECTION)
	LTMatrix	m_mProjection;


	// m_ClipTransform * m_WorldTransform.
	LTMatrix	m_ClipTimesWorld;

	// m_DeviceTransform * mProjectionTransform.
	LTMatrix	m_DeviceTimesProjection;

	// All the above matrices put together.. use this when drawing unclipped things.
	LTMatrix	m_FullTransform;

	// Identity matrix.
	LTMatrix	m_mIdentity;

	// Shearing matrix (changes the direction of projection)
	LTMatrix	m_mShear;

	// Basically m_ClipTimesWorld without FOV squash.
	LTMatrix	m_mShearTimesView;

	// Camera space to clip space transformation (shear * FOV)
	LTMatrix	m_mCameraToClip;


	// These are extracted from m_DeviceTimesProjection for faster manual projection.
	float		m_Projection00, m_Projection02;
	float		m_Projection11, m_Projection12;
	float		m_Projection22, m_Projection23;


	// World space view frustum points.
	LTVector	m_ViewPoints[5];

	// Planes defining the view frustum in camera space.	
	LTPlane		m_CSClipPlanes[NUM_CLIPPLANES];
	
	// Planes defining the view frustum in world space.	
	LTPlane		m_ClipPlanes[NUM_CLIPPLANES];
	LTPlane		m_ReallyCloseClipPlanes[NUM_CLIPPLANES];

	// Viewer orientation and vectors.
	LTVector	m_Up, m_Right, m_Forward;

	// Viewer position.
	LTVector	m_Pos;
	
	// Sky viewer position.
	LTVector	m_SkyViewPos;

	// 0 = normal parity, 1 = flipped on an axis.
	int			m_Parity;
	float		m_ParityMul;  // if m_Parity==0, this is 1, otherwise -1.

	// Are we looking thru a portal?
	bool		m_bPortal;


protected:

	// Where fog is calculated from.  Usually the same as m_Pos, but for world models this
	// is set to (0,0,0).
	LTVector	m_FogRefPos;

	// Precalculated when you set m_FogRefPos.
	float		m_ViewerVFogDensity; // (r_FogDensity at viewer position).
	int			m_ViewerVFogSide;

};


// Lighting info for polies.
struct LitPoly
{
	WorldPoly *m_pPoly;
	LitPoly *m_pNext;
};

struct LightLink
{
	LightLink		*m_pNext;
	DynamicLight	*m_pLight;
	LTVector		m_tLightPos;	// The light position transformed into the worldmodel's space.
};


// ---------------------------------------------------------------- //
// Externs.
// ---------------------------------------------------------------- //

extern ViewParams g_ViewParams;
extern uint32 g_ClipMask;


// ---------------------------------------------------------------- //
// Functions.
// ---------------------------------------------------------------- //

// Setup the transformation for an object.
void r_SetupTransformation(LTVector *pPos, float *pRotation, LTVector *pScale, LTMatrix *pMat);

// Calculate the sum of the lights touching this leaf.
void r_CalcLightAdd(LTObject *pObject, LTVector *pLightAdd);

		// Visibility position used for portals
//		LTVector	m_vPortalViewPos;


// ---------------------------------------------------------------- //
// Inline functions.
// ---------------------------------------------------------------- //

inline bool r_CheckVertCount(int nVerts)
{
	if(nVerts <= TEMPVERT_COUNT)
	{
		return true;
	}
	else
	{
		dsi_ConsolePrint("Error: vertex buffer overflow");
		return false;
	}
}

inline void r_RGBSet(TLRGB *pRGB, uint8 r, uint8 g, uint8 b)
{
	pRGB->r = r;
	pRGB->g = g;
	pRGB->b = b;
}


// 0 = fully inside
// 1 = fully outside
// 2 = clipped
// If it returns 2, it sets outClipMask to a 'smaller' clipmask than clipMask.
inline int r_GetTriClipStatus(uint32 clipMask, uint32 *outClipMask, LTVector *pV1, LTVector *pV2, LTVector *pV3)
{
	int added;

	if(clipMask & CPLANE_NEAR)
		TEST_TRICLIP(NEARZ_TEST, CPLANE_NEAR|CPLANE_LEFT|CPLANE_TOP|CPLANE_RIGHT|CPLANE_BOTTOM|CPLANE_FAR)

	if(clipMask & CPLANE_LEFT)
		TEST_TRICLIP(LEFT_TEST, CPLANE_LEFT|CPLANE_TOP|CPLANE_RIGHT|CPLANE_BOTTOM|CPLANE_FAR)

	if(clipMask & CPLANE_TOP)
		TEST_TRICLIP(TOP_TEST, CPLANE_TOP|CPLANE_RIGHT|CPLANE_BOTTOM|CPLANE_FAR)

	if(clipMask & CPLANE_RIGHT)
		TEST_TRICLIP(RIGHT_TEST, CPLANE_RIGHT|CPLANE_BOTTOM|CPLANE_FAR)

	if(clipMask & CPLANE_BOTTOM)
		TEST_TRICLIP(BOTTOM_TEST, CPLANE_BOTTOM|CPLANE_FAR)

	if(clipMask & CPLANE_FAR)
		TEST_TRICLIP(FARZ_TEST, CPLANE_FAR)

	// Ok.. it's fully inside since it was never outside or clipped.
	return 0;
}

// Returns false if the object is completely outside.
inline bool r_GetSphereClipMask(LTVector *pCenter, float radius, LTPlane *pPlanes, uint32 *clipMask)
{
	int i;
	LTPlane *pPlane;
	float dot;


	// All models/sprites must start with this since they CAN lie
	// outside of their node's box!!!
	*clipMask = CPLANE_ALL;	

	pPlane = pPlanes;
	for(i=0; i < NUM_CLIPPLANES; i++)
	{
		dot = pPlane->DistTo(*pCenter);
		
		if(dot < -radius)
		{
			// Not visible.
			return false;
		}
		else if(dot > radius)
		{
			*clipMask &= ~(1 << i);	// Don't need to clip on this particular plane.
		}

		++pPlane;
	}

	return true;
}
	

// Just transforms the list of points with the given matrix..
template<class T>
inline void r_TransformPoints(T *pVertices, int nVertices, LTMatrix *pTransform)
{
	while(nVertices)
	{
		MatVMul_InPlace_H(pTransform, &pVertices->m_Vec);
		--nVertices;
		++pVertices;
	}
}		  


#define MM g_ViewParams.m_DeviceTimesProjection.m


template<class V>
inline void r_ProjectWithBias(V *pOut, LTVector *pIn, float bias)
{
	float rhw, newZ;
	LTVector vec;

	rhw = 1.0f / (pIn->x*MM[3][0] + pIn->y*MM[3][1] + pIn->z*MM[3][2] + MM[3][3]);
	vec.x = rhw * (pIn->x*MM[0][0] + pIn->y*MM[0][1] + pIn->z*MM[0][2] + MM[0][3]);
	vec.y = rhw * (pIn->x*MM[1][0] + pIn->y*MM[1][1] + pIn->z*MM[1][2] + MM[1][3]);

	newZ = pIn->z + bias;
	rhw = 1.0f / (pIn->x*MM[3][0] + pIn->y*MM[3][1] + newZ*MM[3][2] + MM[3][3]);
	vec.z = rhw * (pIn->x*MM[2][0] + pIn->y*MM[2][1] + newZ*MM[2][2] + MM[2][3]);

	pOut->rhw = rhw;
	pOut->m_Vec = vec;
}

// Tells if the point is on the screen.
inline bool r_IsPointOnScreen(float x, float y)
{
	return x > 0.0f && y > 0.0f && 
		(int)x <= g_ViewParams.m_Rect.right && 
		(int)y <= g_ViewParams.m_Rect.bottom;
}

template<class T>
inline void r_ProjectPoint2(T *pVert, ViewParams *pParams)
{
	// Fast version!
	// Note: in here, it just barely turns out that we don't need a temporary vector to
	// do the matrix multiply in place.. it's _close_!
	pVert->rhw = 1.0f / pVert->m_Vec.z;
	pVert->m_Vec.x = pVert->rhw * (pVert->m_Vec.x*pParams->m_Projection00 + pVert->m_Vec.z*pParams->m_Projection02);
	pVert->m_Vec.y = pVert->rhw * (pVert->m_Vec.y*pParams->m_Projection11 + pVert->m_Vec.z*pParams->m_Projection12);
	pVert->m_Vec.z = pVert->rhw * (pVert->m_Vec.z*pParams->m_Projection22 + pParams->m_Projection23);
	 
	// Slow version.
	//pVert->rhw = MatVMul_InPlace_H(&pParams->m_DeviceTimesProjection, &pVert->m_Vec);

	//ASSERT((int)pVert->m_Vec.x >= 0.0f);
	//ASSERT((int)pVert->m_Vec.y >= 0.0f);
	//ASSERT((int)pVert->m_Vec.x <= pParams->m_Rect.right);
	//ASSERT((int)pVert->m_Vec.y <= pParams->m_Rect.bottom);
}

template<class T>
inline void r_ProjectPoint(T *pVert)
{
	r_ProjectPoint2(pVert, &g_ViewParams);
}


template<class T>	
inline void r_ProjectPoints2(T *pVerts, int nVerts, ViewParams *pParams)
{
	while(nVerts)
	{
		nVerts--;
		r_ProjectPoint2(pVerts++, pParams);
	}
}


template<class T>	
inline void r_ProjectPoints(T *pVerts, int nVerts)
{
	while(nVerts)
	{
		nVerts--;
		r_ProjectPoint(pVerts);
		++pVerts;
	}
}


template<class T>
inline void r_ProjectPointsNoAssert(T *pVerts, int nVerts)
{
	while(nVerts)
	{
		--nVerts;
		pVerts->rhw = MatVMul_InPlace_H(&g_ViewParams.m_DeviceTimesProjection, &pVerts->m_Vec);
		++pVerts;
	}
}


inline void r_TransformVec(LTVector *pVec, LTMatrix *pMat)
{
	float tempX, tempY;

	tempX = pVec->x * pMat->m[0][0] + pVec->y * pMat->m[0][1] + pVec->z * pMat->m[0][2] + pMat->m[0][3];
	tempY = pVec->x * pMat->m[1][0] + pVec->y * pMat->m[1][1] + pVec->z * pMat->m[1][2] + pMat->m[1][3];
	pVec->z = pVec->x * pMat->m[2][0] + pVec->y * pMat->m[2][1] + pVec->z * pMat->m[2][2] + pMat->m[2][3];
	pVec->x = tempX;
	pVec->y = tempY;
}


template<class T>
inline void r_TransformPoint(T *pVert, ViewParams *pParams)
{
	r_TransformVec(&pVert->m_Vec, &pParams->m_ClipTimesWorld);
}

// C and asm version of these useful routines.
#ifdef USE_ALL_C
	static inline void DWordClear(void *pDest, uint32 val, int nBlocks)
	{
		uint32 *pDWords;

		pDWords = (uint32*)pDest;
		while(nBlocks--)
		{
			*pDWords++ = val;
		}
	}

	static inline void DWordCopy(void *pDest, void *pSrc, int nBlocks)
	{
		uint32 *dwDest, *dwSrc;

		dwDest = (uint32*)pDest;
		dwSrc = (uint32*)pSrc;
		while(nBlocks)
		{
			*dwDest++ = *dwSrc++;
			--nBlocks;
		}
	}
#else
	static inline void DWordClear(void *pDest, uint32 val, int nBlocks)
	{
		__asm
		{
			mov ecx, nBlocks
			mov edi, pDest
			mov eax, val
			repnz stosd
		}
	}

	static inline void DWordCopy(void *pDest, void *pSrc, int nBlocks)
	{
		__asm
		{
			mov ecx, nBlocks
			mov edi, pDest
			mov esi, pSrc
			repnz movsd
		}
	}
#endif

class NearZClip
{
	public:
		static int		Test(LTVector &pt) {return pt.z >= g_ViewParams.m_NearClipZ;}
		static float	Clip(LTVector &pt1, LTVector &pt2, LTVector &out)
		{
			float t;
			float div = (pt2.z - pt1.z);
			
			if(div < -CLIP_EPSILON || div > CLIP_EPSILON)
				t = (g_ViewParams.m_NearClipZ - pt1.z) / div; 
			else
				t = 0.0f;

			out.x = pt1.x + ((pt2.x - pt1.x) * t);
			out.y = pt1.y + ((pt2.y - pt1.y) * t);
			out.z = g_ViewParams.m_NearClipZ;
			return t;
		}
};

class LeftClip
{
	public:
		static int		Test(LTVector &pt) {return pt.x > -pt.z;}
		static float	Clip(LTVector &pt1, LTVector &pt2, LTVector &out)
		{
			float div = (pt2.x - pt1.x + pt2.z - pt1.z);
			float t;
			
			if(div < -CLIP_EPSILON || div > CLIP_EPSILON)
				t = -(pt1.x + pt1.z) / div;
			else
				t = 0.0f;

			out.y = pt1.y + ((pt2.y - pt1.y) * t);
			out.z = pt1.z + ((pt2.z - pt1.z) * t);
			out.x = -out.z;
			return t;
		}
};

class TopClip
{
	public:
		static int		Test(LTVector &pt) {return pt.y < pt.z;}
		static float	Clip(LTVector &pt1, LTVector &pt2, LTVector &out)
		{
			float div = (pt2.y - pt1.y - pt2.z + pt1.z);
			float t;

			if(div < -CLIP_EPSILON || div > CLIP_EPSILON)
				t = -(pt1.y - pt1.z) / div;
			else
				t = 0.0f;

			out.x = pt1.x + ((pt2.x - pt1.x) * t);
			out.z = pt1.z + ((pt2.z - pt1.z) * t);
			out.y = out.z;
			return t;
		}
};

class RightClip
{
	public:
		static int		Test(LTVector &pt) {return pt.x < pt.z;}
		static float	Clip(LTVector &pt1, LTVector &pt2, LTVector &out)
		{
			float div = (pt2.x - pt1.x - pt2.z + pt1.z);
			float t;
			
			if(div < -CLIP_EPSILON || div > CLIP_EPSILON)
				t = -(pt1.x - pt1.z) / div;
			else
				t = 0.0f;

			out.y = pt1.y + ((pt2.y - pt1.y) * t);
			out.z = pt1.z + ((pt2.z - pt1.z) * t);
			out.x = out.z;
			return t;
		}
};

class BottomClip
{
	public:
		static int		Test(LTVector &pt) {return pt.y > -pt.z;}
		static float	Clip(LTVector &pt1, LTVector &pt2, LTVector &out)
		{
			float div = (pt2.y - pt1.y + pt2.z - pt1.z);
			float t;

			if(div < -CLIP_EPSILON || div > CLIP_EPSILON)
				t = -(pt1.y + pt1.z) / div;
			else
				t = 0.0f;

			out.x = pt1.x + ((pt2.x - pt1.x) * t);
			out.z = pt1.z + ((pt2.z - pt1.z) * t);
			out.y = -out.z;
			return t;
		}
};

class FarZClip
{
	public:
		static int		Test(LTVector &pt) {return pt.z <= g_ViewParams.m_FarClipZ;}
		static float	Clip(LTVector &pt1, LTVector &pt2, LTVector &out)
		{
			float div = (pt2.z - pt1.z);
			float t;
			
			if(div < -CLIP_EPSILON || div > CLIP_EPSILON)
				t = (g_ViewParams.m_FarClipZ - pt1.z) / div;
			else
				t = 0.0f;

			out.x = pt1.x + ((pt2.x - pt1.x) * t);
			out.y = pt1.y + ((pt2.y - pt1.y) * t);
			out.z = g_ViewParams.m_FarClipZ;
			return t;
		}
};

class GeneralClip
{
public:

	int		Test(LTVector &pt) {return m_pPlane->DistTo(pt) > 0.0f;}

	float	Clip(LTVector &pt1, LTVector &pt2, LTVector &out)
	{
		float dist1, dist2, t, div;

		dist1 = m_pPlane->DistTo(pt1);
		dist2 = m_pPlane->DistTo(pt2);
		div = (dist1 - dist2);
		if(div < -CLIP_EPSILON || div > CLIP_EPSILON)
			t = dist1 / div;
		else
			t = 0.0f;

		out = pt1 + (pt2 - pt1) * t;
		return t;
	}

public:

	LTPlane		*m_pPlane;
};

template<class C, class V>
int DoClip(C &clipper, V* &pVerts, int &nVerts, V* &pOut)
{
	int bInside[50], *pInside;
	int nInside=0; 
	V *pPrev, *pCur, *pEnd, *pOldOut; 
	int iPrev, iCur; 
	float t; 

	++g_nClipTests;
	pCur = pVerts;
	pEnd = pCur + nVerts;
	pInside = bInside;
	while(pCur != pEnd)
	{
		*pInside = clipper.Test(pCur->m_Vec);
		nInside += *pInside;
		++pInside;
		++pCur;
	} 

	if( nInside == 0 ) 
	{ 
		return false; 
	} 
	else if( nInside != nVerts ) 
	{       
		pOldOut = pOut; 

		iPrev = nVerts - 1; 
		pPrev = pVerts + iPrev; 
		for( iCur=0; iCur < nVerts; iCur++ ) 
		{ 
			pCur = pVerts + iCur; 

			if( bInside[iPrev] ) 
				*pOut++ = *pPrev; 

			if( bInside[iPrev] != bInside[iCur] ) 
			{ 
				t = clipper.Clip(pPrev->m_Vec, pCur->m_Vec, pOut->m_Vec);
				V::ClipExtra(pPrev, pCur, pOut, t);
			 
				++pOut; 
			} 
		 
			iPrev = iCur; 
			pPrev = pCur; 
		} 

		nVerts = pOut - pOldOut; 
		pVerts = pOldOut; 
		pOut += nVerts;
	} 

	ASSERT((uint32)((uint8*)pOut - (uint8*)g_pClipVerts) < g_ClipVertsBufferSize);
	return true;
}

template<class T>
inline bool r_ClipPolyGeneral(T **ppVerts, int *pnVerts, LTPlane *pPlanes, uint32 nPlanes)
{
	T *pOut = T::GetClipVerts();
	T *pVerts = *ppVerts;
	int nVerts = *pnVerts;
	GeneralClip gClip;


	while(nPlanes)
	{
		--nPlanes;
	
		gClip.m_pPlane = pPlanes;
		if(!DoClip(gClip, pVerts, nVerts, pOut))
			return false;

		++pPlanes;
	}				

	*ppVerts = pVerts;
	*pnVerts = nVerts;
	return true;
}

template<class T>
inline bool r_ClipPoly(uint32 clipMask, T **ppVerts, int *pnVerts)
{
	if (g_CV_UseD3DClip.m_IntVal)
	{
		clipMask &= CPLANE_NEAR;
		if (!clipMask)
			return true;
	}

	T *pOut = T::GetClipVerts();
	T *pVerts = *ppVerts;
	int nVerts = *pnVerts;
	NearZClip nearZ;
	LeftClip left;
	TopClip top;
	RightClip right;
	BottomClip bottom;
	FarZClip farZ;
	
	if(clipMask & CPLANE_NEAR)
	{
		if(!DoClip(nearZ, pVerts, nVerts, pOut))
			return false;
	}

	if(clipMask & CPLANE_LEFT)
	{
		if(!DoClip(left, pVerts, nVerts, pOut))
			return false;
	}

	if(clipMask & CPLANE_TOP)
	{
		if(!DoClip(top, pVerts, nVerts, pOut))
			return false;
	}

	if(clipMask & CPLANE_RIGHT)
	{
		if(!DoClip(right, pVerts, nVerts, pOut))
			return false;
	}

	if(clipMask & CPLANE_BOTTOM)
	{
		if(!DoClip(bottom, pVerts, nVerts, pOut))
			return false;
	}

	if(clipMask & CPLANE_FAR)
	{
		if(!DoClip(farZ, pVerts, nVerts, pOut))
			return false;
	}

	*ppVerts = pVerts;
	*pnVerts = nVerts;
	return true;
}


// Clip a line segment.
inline bool r_ClipLine(TLVertex *pVerts, uint32 clipMask)
{
	TLVertex *pOut;
	float t;

	if(clipMask & CPLANE_NEAR)
	{
		#define CLIPTEST NEARZ_TEST
		#define DOCLIP NEARZ_CLIP
		#include "clipline.h"
		#undef CLIPTEST
		#undef DOCLIP
	}

	if(clipMask & CPLANE_LEFT)
	{
		#define CLIPTEST LEFT_TEST
		#define DOCLIP LEFT_CLIP
		#include "clipline.h"
		#undef CLIPTEST
		#undef DOCLIP
	}

	if(clipMask & CPLANE_TOP)
	{
		#define CLIPTEST TOP_TEST
		#define DOCLIP TOP_CLIP
		#include "clipline.h"
		#undef CLIPTEST
		#undef DOCLIP
	}

	if(clipMask & CPLANE_RIGHT)
	{
		#define CLIPTEST RIGHT_TEST
		#define DOCLIP RIGHT_CLIP
		#include "clipline.h"
		#undef CLIPTEST
		#undef DOCLIP
	}

	if(clipMask & CPLANE_BOTTOM)
	{
		#define CLIPTEST BOTTOM_TEST
		#define DOCLIP BOTTOM_CLIP
		#include "clipline.h"
		#undef CLIPTEST
		#undef DOCLIP
	}

	if(clipMask & CPLANE_FAR)
	{
		#define CLIPTEST FARZ_TEST
		#define DOCLIP FARZ_CLIP
		#include "clipline.h"
		#undef CLIPTEST
		#undef DOCLIP
	}

	return true;
}


template<class T, class F>
inline bool r_FullSetupPoly3(T **ppVerts, int *pnVerts, 
	ViewParams *pParams, 
	F *fogHandler)
{
	T *pVerts;
	int nVerts;

	if(g_ClipMask == 0)
	{
		// Just do the FULL thing all at once!
		pVerts = *ppVerts;
		nVerts = *pnVerts;

		while(nVerts)
		{
			pVerts->rhw = MatVMul_InPlace_H(&pParams->m_FullTransform, &pVerts->m_Vec);
			F::HandleFog(pVerts);

			//ASSERT((int)pVerts->m_Vec.x >= 0.0f);
			//ASSERT((int)pVerts->m_Vec.y >= 0.0f);
			//ASSERT((int)pVerts->m_Vec.x <= pParams->m_Rect.right);
			//ASSERT((int)pVerts->m_Vec.y <= pParams->m_Rect.bottom);

			++pVerts;
			--nVerts;
		}
	}
	else
	{
		// Get them into clip space.  Doesn't use the standard matrix multiply here 
		// because the bottom row of the matrix is 0001 and we can take advantage of it.
		pVerts = *ppVerts;
		nVerts = *pnVerts;
		while(nVerts)
		{
			// Fast version (1/w is 1 so no need to use it).
			r_TransformPoint(pVerts, pParams);
			F::HandleFog(pVerts);

			// Slow version.			
			//MatVMul_InPlace_H(&pParams->m_ClipTimesWorld, &pVerts->m_Vec);
			
			++pVerts;
			--nVerts;
		}

		// Clip.
		if(!r_ClipPoly(g_ClipMask, ppVerts, pnVerts))
			return false;

		// Project.
		pVerts = *ppVerts;
		nVerts = *pnVerts;
		while(nVerts)
		{
			r_ProjectPoint2(pVerts, pParams);
			
			++pVerts;
			--nVerts;
		}
	}

	return true;
}

// Transforms, clips, and maps to the viewport.
template<class T>
inline bool r_FullSetupPoly2(T **ppVerts, int *pnVerts, ViewParams *pParams)
{
	return r_FullSetupPoly3(ppVerts, pnVerts, pParams, (FogHandlerNone*)LTNULL);
}

template<class T>
inline bool r_FullSetupPoly(T **ppVerts, int *pnVerts)
{
	return r_FullSetupPoly2(ppVerts, pnVerts, &g_ViewParams);
}

inline void r_SetupTransformation(LTVector *pPos, float *pRotation, LTVector *pScale, LTMatrix *pMat)
{
	if(pRotation[0] > ROTATION_MAX || pRotation[0] < -ROTATION_MAX)
		pRotation[0] = 0.0f;
	
	if(pRotation[1] > ROTATION_MAX || pRotation[1] < -ROTATION_MAX) 
		pRotation[1] = 0.0f;
	
	if(pRotation[2] > ROTATION_MAX || pRotation[2] < -ROTATION_MAX) 
		pRotation[2] = 0.0f;
	
	if(pRotation[3] > ROTATION_MAX || pRotation[3] < -ROTATION_MAX) 
		pRotation[3] = 1.0f;
	
	quat_ConvertToMatrix(pRotation, pMat->m);

	pMat->m[0][0] *= pScale->x;
	pMat->m[1][0] *= pScale->x;
	pMat->m[2][0] *= pScale->x;

	pMat->m[0][1] *= pScale->y;
	pMat->m[1][1] *= pScale->y;
	pMat->m[2][1] *= pScale->y;

	pMat->m[0][2] *= pScale->z;
	pMat->m[1][2] *= pScale->z;
	pMat->m[2][2] *= pScale->z;

	pMat->m[0][3] = pPos->x;
	pMat->m[1][3] = pPos->y;
	pMat->m[2][3] = pPos->z;
	
	//	MatTranspose3x3(pMat);
}

#endif


