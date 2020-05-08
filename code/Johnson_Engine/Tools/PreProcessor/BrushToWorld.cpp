// ------------------------------------------------------------------ //
//	FILE	  : BrushToWorld.cpp
//	CREATED	  : February 5, 1997
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
// ------------------------------------------------------------------ //

// Includes....
#include "bdefs.h"
#include "preworld.h"
#include "btwpoly.h"
#include "brushtoworld.h"
#include "processing.h"
#include "SetupPolyAlpha.h"

// TODO : Get rid of this class.  It's no longer necessary.  It now only
// acts as glue between the old functions and the new functions.  The biggest
// thing that would have to be fixed is the splitting of an EditPoly into multiple
// PrePoly's due to vertex count, which converting to a TempBrush handles.
class TempBrush
{
public:
	TempBrush() {}

	~TempBrush()
	{
		Term();
	}

private:
	void Term()
	{
		for(GPOS pos=m_Polies; pos; )
		{
			DeletePoly(m_Polies.GetNext(pos));
		}

		m_Polies.RemoveAll();
	}

public:

	CGLinkedList<CBTWPoly*> m_Polies;

	// Cosine of the crease angle
	float				m_fCreaseAngle;
};

uint32 GenerateBrushPolygonName(const char *pName)
{
	if (!pName)
		return 0;

	uint32 nResult = 0;
	for (; *pName; ++pName)
	{
		nResult *= 29;
		nResult += toupper(*pName) - 'A';
	}

	return nResult;
}

// This generates the polies for a TempBrush given a CEditBrush.
TempBrush* EditToTempBrush(CEditBrush *pBrush)
{
	const uint32 k_nMaxNumVerts = 30;

	// Create the brush
	TempBrush *pRet = new TempBrush;

	uint32 nBrushName = GenerateBrushPolygonName(pBrush->GetName());

	for(uint32 i = 0; i < pBrush->m_Polies; i++)
	{
		CEditPoly *pEditPoly = pBrush->m_Polies[i];

		// Get the first k_nMaxNumVerts verts from the poly
		uint32 nStartVert = 0;
		uint32 nEndVert = LTMIN(k_nMaxNumVerts, pEditPoly->NumVerts());
		while (nStartVert != nEndVert)
		{
			uint32 nVertCount = nEndVert - nStartVert;
			if (nStartVert)
				nVertCount += 2;

			CBTWPoly *pBTWPoly = CreatePoly(CBTWPoly, nVertCount, false);

			pBTWPoly->m_pSurface = (CPreSurface*)pEditPoly->m_pUser1;

			// If this is an overflow poly, we need the base point and the last point from the previous poly
			if (nStartVert)
			{
				pBTWPoly->AddVert( pEditPoly->Pt(0) );
				pBTWPoly->AddVert( pEditPoly->Pt(nStartVert - 1) );
			}

			for(uint32 j = nStartVert; j < nEndVert; j++)
				pBTWPoly->AddVert( pEditPoly->Pt(j) );

			CopyPolyColors(pEditPoly, pBTWPoly, nStartVert, nEndVert);
			SetupPolyAlpha(pBrush, pBTWPoly, nVertCount);

			pBTWPoly->m_nName = nBrushName;

			pRet->m_Polies.AddTail(pBTWPoly);

			// Move to the next k_nMaxNumVerts verts
			nStartVert = nEndVert;
			nEndVert = LTMIN(nEndVert + k_nMaxNumVerts, pEditPoly->NumVerts());
			
		}
	}

	return pRet;
}

// ------------------------------------------------------------------ //
// Converts pBrush to a TempBrush and all its polies to CPrePolys.
// ------------------------------------------------------------------ //

static CPrePoly* CreatePrePolyFromBTWPoly(CBTWPoly *pPoly)
{
	CPrePoly *pPrePoly;

	pPrePoly = CreatePoly(CPrePoly, pPoly->NumVerts(), true);
	memcpy(pPrePoly->VertList(), pPoly->VertList(), sizeof(CPolyVert)*pPoly->NumVerts());

	pPrePoly->m_pSurface = pPoly->m_pSurface;

	pPrePoly->m_nName = pPoly->m_nName;

	return pPrePoly;
}


static TempBrush* CreateTempBrush(CPreWorld *pWorld, CEditBrush *pBrush)
{
	TempBrush *pRet;
	CBTWPoly *pBTWPoly;
	CPrePoly *pPrePoly;
	PVector vec;
	GPOS pos;

	
	// Setup the TempBrush.
	pRet = EditToTempBrush(pBrush);

	// Create the CPrePoly for each CBTWPoly.
	for(pos=pRet->m_Polies.GetHeadPosition(); pos; )
	{
		pBTWPoly = pRet->m_Polies.GetNext(pos);

		pPrePoly = CreatePrePolyFromBTWPoly(pBTWPoly);
		pBTWPoly->m_pPrePoly = pPrePoly;

		pWorld->m_OriginalBrushPolies.Append(pPrePoly);
	}

	// Get the crease angle
	CRealProp *pCreaseProp = (CRealProp *)pBrush->m_PropList.GetMatchingProp("CreaseAngle", PT_REAL);
	if (pCreaseProp)
	{
		pRet->m_fCreaseAngle = fabsf(cosf(MATH_DEGREES_TO_RADIANS(pCreaseProp->m_Value)));
	}
	else
		pRet->m_fCreaseAngle = 0.707f;

	return pRet;
}


static void TransferBrushPolys(CEditBrush *pEditBrush, CPreWorld *pWorld)
{
	// Note : If we change the rest of this function to operate on CEditBrush, we can
	// get rid of the TempBrush usage.
	TempBrush *pBrush = CreateTempBrush(pWorld, pEditBrush);

	// Add the polies
	for(GPOS iPoly=pBrush->m_Polies; iPoly; )
	{
		CBTWPoly *pPoly = pBrush->m_Polies.GetNext(iPoly);
		
		CPrePoly *pPrePoly = CreatePrePolyFromBTWPoly(pPoly);
		
		// Remember where it came from.. this CPrePoly will eventually be replaced by
		// its m_pOriginalBrushPoly.
		pPrePoly->m_pOriginalBrushPoly = pPoly->m_pPrePoly;

		pPrePoly->SetPPFlags(pPrePoly->GetPPFlags() | PP_DONOTREMOVE);
		
		// Generate smoothed vertex normals if they want
		// Note : This goes into the "original" poly since the new one's going to get clobbered..
		CPrePoly *pNormalPoly = ((pPrePoly->GetSurfaceFlags() & SURF_NONEXISTENT) == 0) ? pPoly->m_pPrePoly : pPrePoly;
		for (uint32 nCurVert = 0; nCurVert < pNormalPoly->NumVerts(); ++nCurVert)
		{
			LTVector vNormal(0.0f, 0.0f, 0.0f);
			LTVector vCurPos = pNormalPoly->Pt(nCurVert);

			for (GPOS posFindPolys = pBrush->m_Polies; posFindPolys; )
			{
				CBTWPoly *pFindPoly = pBrush->m_Polies.GetNext(posFindPolys);
				// Skip polys with normals that will cause a crease
				float fPolyAngle = pNormalPoly->Normal().Dot(pFindPoly->Normal());
				if (fPolyAngle < pBrush->m_fCreaseAngle)
					continue;
				for (uint32 nFindVert = 0; nFindVert < pFindPoly->NumVerts(); ++nFindVert)
				{
					// We found a match, so add in the normal...
					if (vCurPos.DistSqr(pFindPoly->Pt(nFindVert)) < 0.1f)
					{
						vNormal += pFindPoly->Normal();
						// Stop looking in this poly...
						break;
					}
				}
			}

			// Store the normal back in the vertex normal
			vNormal.Norm();
			pNormalPoly->PtNormal(nCurVert) = vNormal;
		}

		pWorld->m_Polies.Append(pPrePoly);
	}
	
	// Get rid of the temp brush.
	delete pBrush;
}


CPrePoly* EditPolyToPrePoly(CEditPoly *pEditPoly)
{
	CPrePoly *pPrePoly;
	uint32 i;

	pPrePoly = CreatePoly(CPrePoly, pEditPoly->NumVerts(), false);
	if(!pPrePoly)
		return NULL;

	pPrePoly->m_pSurface = (CPreSurface*)pEditPoly->m_pUser1;

	for(i=0; i < pEditPoly->NumVerts(); i++)
	{
		pPrePoly->AddVert( pEditPoly->Pt(i) );
	}


	CopyPolyColors(pEditPoly, pPrePoly, 0, pPrePoly->NumVertsAndTVerts());
	SetupPolyAlpha(pEditPoly->m_pBrush, pPrePoly, pPrePoly->NumVertsAndTVerts());

	return pPrePoly;
}


bool BrushToWorld(
	GenList<CEditBrush*> &brushes, 
	CPreWorld *pWorld, 
	BrushToWorldCallback callback)
{
	// Create the TempBrushes.
	if (!brushes.GenGetSize())
	{
		DrawStatusText(eST_Error, "Empty world model BSP found (%s).", pWorld->m_WorldName);

		//display a bit more information about it if it is the main visibility BSP
		//since this is commonly caused by the artist not having any non-detail brushes
		//in their level
		if(pWorld->m_WorldInfoFlags & WIF_VISBSP)
		{
			DrawStatusText(eST_Error, "The main visibility BSP needs to have some brushes that are not detail brushes. Ensure that there are some brushes not bound to world models and that at least one has detail set to false. --");
		}
		return pWorld->m_Polies.GetSize() > 0;
	}

	// Move the brushes into the world
	DrawStatusTextIfMainWorld(eST_Normal, "Transferring brush polygons to world");

	uint32		nProcessCount = 0, nProcessTotal;
	nProcessTotal = brushes.GenGetSize();
	for(GenListPos iBrush = brushes.GenBegin(); brushes.GenIsValid(iBrush); )
	{
		TransferBrushPolys(brushes.GenGetNext(iBrush), pWorld);
		callback((float)nProcessCount / nProcessTotal);
		++nProcessCount;
	}

	// Setup all the vertex indices
	pWorld->InitPolyVertexIndices();

	// Set up the LM surface vectors
	pWorld->SetupSurfaceLMVectors();

	return true;
}



