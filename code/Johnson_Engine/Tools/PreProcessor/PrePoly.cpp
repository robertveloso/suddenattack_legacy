//------------------------------------------------------------------
//
//	FILE	  : PrePoly.cpp
//
//	PURPOSE	  : Implements the CPrePoly class.
//
//	CREATED	  : 2nd May 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------



// Includes....
#include "bdefs.h"
#include "geometry.h"
#include "geomroutines.h"
#include "prepoly.h"
#include "pregeometry.h"
#include "prepolyfragments.h"


// ----------------------------------------------------------------------- //
// Helpers.
// ----------------------------------------------------------------------- //

CPrePoly::CPrePoly()
{
	m_PPFlags				= 0;
	m_LMWidth				= 0;
	m_LMHeight				= 0;
	m_pReplacement			= NULL;
	m_pOriginalBrushPoly	= NULL;

	m_pFragments			= 0;

	m_nName					= 0;

#ifdef DIRECTEDITOR_BUILD
	m_pUser = m_pUser2 = NULL;
#endif

}


CPrePoly::~CPrePoly()
{
	Term();
}


// ----------------------------------------------------------------------- //
// Clears out the polygon's data.
// ----------------------------------------------------------------------- //
void CPrePoly::Term()
{
	delete m_pFragments;
	m_pFragments = 0;
}


// ----------------------------------------------------------------------- //
// Removes collinear vertices.
// ----------------------------------------------------------------------- //
void CPrePoly::RemoveCollinearVertices()
{
	CPrePlane edgePlane;
	PReal dot;

	for(uint32 i = 0; i < NumVerts(); i++)
	{
		PVector &prev = PrevPt(i);
		PVector &next = NextPt(i);

		edgePlane.m_Normal = (next - prev).Cross(Normal());
		edgePlane.m_Normal.Norm();
		edgePlane.m_Dist = edgePlane.m_Normal.Dot(prev);

		dot = edgePlane.DistTo(Pt(i));
		if(fabs(dot) < 0.0001f)
		{
			RemoveVert(i);
			--i;
		}
	}
}


BOOL CPrePoly::PostJoinFixup(PReal convexThreshold)
{
	RemoveCollinearVertices();
	
	if(!IsConvex(this, convexThreshold))
		return FALSE;

	if(NumVerts() < 3)
		return FALSE;

	return TRUE;
}


// ----------------------------------------------------------------------- //
// Finds the texture origin.  This is basically the poly origin, adjusted
// for hardware interpolation and -(textureOrigin - O())'s fraction.
// (That basically makes it lie exactly on the first pixel).
// ----------------------------------------------------------------------- //

static float MinDistToPoints(CPrePoly *pPoly, PVector *pNormal)
{
	CPrePlane thePlane;
	PReal minDist, testDist;

	thePlane.m_Normal = *pNormal;
	thePlane.m_Normal.Norm();
	thePlane.m_Dist = thePlane.m_Normal.Dot(pPoly->m_PolyO);

	minDist = 100000.0f;
	for(uint32 i = 0; i < pPoly->NumVerts(); i++)
	{
		testDist = thePlane.m_Normal.Dot(pPoly->Pt(i)) - thePlane.m_Dist;

		if(fabs(testDist) < fabs(minDist))
			minDist = testDist;
	}

	return (float)minDist;
}
	

void AdjustForVec(CPrePoly *pPoly, PVector &dir, PVector &inverseDir, float lmGridSize)
{
	PReal dot;
	PVector normDir;

	normDir = dir;
	normDir.Norm();

	dot = (PReal)fmod((PReal)normDir.Dot(pPoly->m_PolyO), (PReal)lmGridSize);
	if(dot >= 0.0f)
		pPoly->m_PolyO -= normDir * dot;
	else
		pPoly->m_PolyO -= normDir * (lmGridSize+dot);
}


void CPrePoly::FindTextureOrigin(PVector *pPolyO, BOOL bAdjust, float lmGridSize)
{
	PReal dot;
	PVector cur = Pt(0);
	PVector unitP, unitQ;

	unitP = P();
	unitP.Norm();

	unitQ = Q();
	unitQ.Norm();

	// Get the main origin.
	for(uint32 i = 1; i < NumVerts(); i++ )
	{
		dot = unitP.Dot( Pt(i) - cur );
		if(dot < 0)
			cur += unitP * dot;

		dot = unitQ.Dot( Pt(i) - cur );
		if(dot < 0)
			cur += unitQ * dot;
	}

	if(pPolyO)
		*pPolyO = cur;
	else
		m_PolyO = cur;

	// Align it to a LIGHTMAP_GRID_SIZE_I-pixel grid to help avoid edges showing up.
	if(bAdjust)
	{
		AdjustForVec(this, P(), InverseP(), lmGridSize);
		AdjustForVec(this, Q(), InverseQ(), lmGridSize);
	}
}


// ----------------------------------------------------------------------- //
//      ROUTINE:        CPrePoly::CalculateTextureSize
//      PURPOSE:        Finds the texture dimensions for this polygon.
// ----------------------------------------------------------------------- //

void CPrePoly::CalculateTextureSize(BOOL bCapSizes, 
	float maxLightmapSize, float lmGridSize,
	uint16 &xPixels, uint16 &yPixels)
{
	PReal dot;
	PReal maxX, maxY;
	int bErr;
	uint16 wGridSize;

	maxX = maxY = (PReal)-MAX_CREAL;
	for(uint32 i = 0; i < NumVerts(); i++ )
	{
		dot = m_pSurface->P.Dot( Pt(i) - PolyO() );
		if( dot > maxX )
			maxX = dot;

		dot = m_pSurface->Q.Dot( Pt(i) - PolyO() );
		if( dot > maxY )
			maxY = dot;
	}

	xPixels = (uint16)ceil(maxX);
	yPixels = (uint16)ceil(maxY);

	if(bCapSizes)
	{
		if(xPixels > (lmGridSize*maxLightmapSize))
		{
			bErr = 1;
		}

		if(yPixels > (lmGridSize*maxLightmapSize))
		{
			bErr = 1;
		}
	}

	wGridSize = (uint16)lmGridSize;
	if(xPixels % wGridSize)
		xPixels = xPixels + (wGridSize - (xPixels % wGridSize));

	if(yPixels % wGridSize)
		yPixels = yPixels + (wGridSize - (yPixels % wGridSize));
}

CPrePoly* CPrePoly::Clone() const
{
	CPrePoly *pRet;

	pRet = CreatePoly(CPrePoly, NumVerts(), false);
	if(!pRet)
		return NULL;

	//make sure that this new polygon shares the same surface as the 
	//original before we copy over the verts, since we need to use
	//the surface normal for the vertices
	pRet->CopySplitAttributes(this);

	for(uint32 i = 0; i < NumVerts(); i++)
	{
		pRet->AddVert(Pt(i));
		pRet->Alpha(i) = Alpha(i);
	}

	pRet->m_nName = m_nName;

	return pRet;
}




