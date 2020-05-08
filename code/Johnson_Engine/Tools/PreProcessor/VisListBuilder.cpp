//------------------------------------------------------------------
//
//	FILE	  : VisListBuilder.cpp
//
//	PURPOSE	  : Implementation for the CVisListBuilder class.
//
//	CREATED	  : 2nd May 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

// Includes....
#include "bdefs.h"
#include "vislistbuilder.h"

#include "preleaf.h"
#include "hullpoly.h"
#include "processing.h"
#include "threads.h"
#include "pregeometry.h"
#include "preportal.h"
#include "preworld.h"
#include "geometry.h"


//----------------------------------------------------
// Macros
//----------------------------------------------------
#define NUM_VLB_VERTS	100

// Gives the byte or bit given a leaf index.
#define NUM_BYTE(_iNum)	(_iNum>>3)
#define NUM_BIT(_iNum)	(_iNum&7)


//----------------------------------------------------
// CRecurseData
//----------------------------------------------------
class CRecurseData
{
	public:
	
		char m_LookFromData[2][HULLPOLY_SIZE(NUM_VLB_VERTS)];
		char m_LookAtData[2][HULLPOLY_SIZE(NUM_VLB_VERTS)];
};


//----------------------------------------------------
// CPortalSequence
//----------------------------------------------------
class CPortalSequence
{
	public:
								~CPortalSequence()
								{
									DeleteAndClearArray(m_RecurseData);
								}

		CMoArray<CRecurseData*>	m_RecurseData;
		uint32					m_CurRecurseData;

		CMoByteArray			m_LeafTouch;
		CMoByteArray			m_VisibleLeaves;
		CPreLeaf				*m_pLeaf;	// The leaf that this sequence originates from.
};


//----------------------------------------------------
// Globals
//----------------------------------------------------
static CHullPoly		*g_pCurrentPortal	= NULL;
static CVisListBuilder	*g_pBuilder			= NULL;
uint32					g_nLeavesTagged		= 0;
void					*g_IncrementCS		= NULL;
void					*g_NextLeafCS		= NULL;


static inline void SetLeafTouch( CPortalSequence *pSeq, CPreLeaf *pLeaf )
{
	pSeq->m_LeafTouch[NUM_BYTE(pLeaf->m_Index)] |= (1 << NUM_BIT(pLeaf->m_Index));
}

static inline void ClearLeafTouch( CPortalSequence *pSeq, CPreLeaf *pLeaf )
{
	pSeq->m_LeafTouch[NUM_BYTE(pLeaf->m_Index)] &= ~(1 << NUM_BIT(pLeaf->m_Index));
}

static inline bool GetLeafTouch( CPortalSequence *pSeq, CPreLeaf *pLeaf )
{
	return !!(pSeq->m_LeafTouch[NUM_BYTE(pLeaf->m_Index)] & (1 << NUM_BIT(pLeaf->m_Index)));
}

static inline void SetLeafVisible(CMoByteArray &leafList, CPreLeaf *pLeaf, bool bVisible)
{
	if (bVisible)
		leafList[NUM_BYTE(pLeaf->m_Index)] |= 1 << NUM_BIT(pLeaf->m_Index);
	else
		leafList[NUM_BYTE(pLeaf->m_Index)] &= ~(1 << NUM_BIT(pLeaf->m_Index));
}

static inline bool IsLeafVisible(CMoByteArray &leafList, CPreLeaf *pLeaf)
{
	return !!(leafList[NUM_BYTE(pLeaf->m_Index)] & (1 << NUM_BIT(pLeaf->m_Index)));
}

static inline bool IsLeafVisible(uint8 *leafList, CPreLeaf *pLeaf)
{
	return !!(leafList[NUM_BYTE(pLeaf->m_Index)] & (1 << NUM_BIT(pLeaf->m_Index)));
}


	

// ----------------------------------------------------------------------- //
//  Gets the visible portions of pLookAt, assuming
//  you were looking from pLookFrom through pLookThru.
// ----------------------------------------------------------------------- //

static bool GetVisiblePortions(CHullPoly *pLookFrom, CHullPoly *pLookThru, CHullPolySwapper &polies, bool bReverseWinding)
{
	PVector *pts[3];
	CPolyVert *pCurPt;
	PReal mag;
	uint32 i, j, k;
	CPrePlane plane;


	// Snip off piece by piece...
	for( i=0; i < pLookThru->NumVerts(); i++ )
	{
		pts[bReverseWinding ? 1 : 0] = &pLookThru->Pt(i);
		pts[bReverseWinding ? 0 : 1] = &pLookThru->NextPt(i);

		for( j=0; j < pLookFrom->NumVerts(); j++ )
		{
			pts[2] = &pLookFrom->Pt(j);
			
			plane.m_Normal = (*pts[2] - *pts[0]).Cross( *pts[1] - *pts[0] );

			// Make sure it's valid.
			mag = plane.m_Normal.Mag();
			if( mag < 0.1f )
				continue;

			plane.m_Normal /= mag;
			plane.m_Dist = plane.m_Normal.Dot( *pts[0] );

			// Make sure pLookFrom is on the backside...
			pCurPt = pLookFrom->VertList();
			for( k=0; k < pLookFrom->NumVerts(); k++ )
			{
				if( (plane.m_Normal.Dot(pCurPt->m_Vec) - plane.m_Dist) > POINT_SIDE_EPSILON )
					goto LOOPEND;

				++pCurPt;
			}


			// Make sure pLookThru is on the frontside...
			pCurPt = pLookThru->VertList();
			for( k=0; k < pLookThru->NumVerts(); k++ )
			{
				if( (plane.m_Normal.Dot(pCurPt->m_Vec) - plane.m_Dist) < -POINT_SIDE_EPSILON )
					goto LOOPEND;

				++pCurPt;
			}


			if( !GetFrontSide(&plane, polies) )
				return false;

			LOOPEND:;
		}
	}

	return true;
}



// ----------------------------------------------------------------------- //
// The main recursive routine that goes thru all the portals.
// It tries to minimize the number of portals that it actually visits.
// ----------------------------------------------------------------------- //

static void RecurseThruPortals(
	CPortalSequence *pSeq, 
	CHullPoly *pLookFrom, 
	CHullPoly *pLookThru, 
	uint8 *pInLeaves,
	bool bCheckCanSeeThru)
{
	CPreLeaf			*pLeaf;
	CPreLeaf			*pDestLeaf;

	CHullPoly			*pPortal;
	uint32				iPortal, i;
	uint32				moreMask;
	CPrePlane			plane;
	CMoByteArray		visibleLeaves;
	CHullPolySwapper	lookFroms, lookAts;
	CRecurseData		*pRecurseData;


	// Can we see through this one?
	if(	bCheckCanSeeThru &&
		pLookThru->m_pAdjacent && 
		(pLookThru->m_pAdjacent->m_Flags & HULLPOLY_CANTSEETHRUPORTAL))
	{
		return;
	}

	// Get some data for the stack.
	if(pSeq->m_CurRecurseData == pSeq->m_RecurseData.GetSize())
		pSeq->m_RecurseData.Append(new CRecurseData);

	pRecurseData = pSeq->m_RecurseData[pSeq->m_CurRecurseData];
	++pSeq->m_CurRecurseData;
	lookFroms.Init((CHullPoly*)pRecurseData->m_LookFromData[0], (CHullPoly*)pRecurseData->m_LookFromData[1]);
	lookAts.Init((CHullPoly*)pRecurseData->m_LookAtData[0], (CHullPoly*)pRecurseData->m_LookAtData[1]);

	pLeaf = pLookThru->m_pLeaf;
	
	visibleLeaves.SetSize(g_pBuilder->m_LeafListSize);

	// Give some time up..
	if(g_pBuilder->m_nThreads > 1)
	{
		thd_Sleep(0);
	}

	
	// Mark this leaf as touched.
	SetLeafTouch(pSeq, pLeaf);
	
	// Mark this leaf as visible in the portal sequence.	
	SetLeafVisible(pSeq->m_VisibleLeaves, pLeaf, TRUE);

	
	for(iPortal=0; iPortal < pLeaf->m_VLPortals; iPortal++)
	{
		pPortal = pLeaf->m_VLPortals[iPortal];
		pDestLeaf = pPortal->m_pLeaf;


		// Test to see if we can avoid going into this portal.
		{
			// Don't go into this leaf if it's currently on the stack.
			if( GetLeafTouch(pSeq, pDestLeaf) )
				continue;

			// Don't go into this leaf if it's not possibly visible.
			if(!IsLeafVisible(pInLeaves, pDestLeaf))
				continue;

			// Don't go through this portal if it can never see the sequence's base leaf.
// DOESN'T WORK BECAUSE PORTALS ALWAYS FACE AWAY FROM THE ORIGINATING LEAF.
//			if( !(pPortal->m_VisibleLeaves[pSeq->m_BaseLeafIndex] & pSeq->m_BaseLeafMask) )
//				continue;

			// Merge the input visible leaf's list with this portal's list.
			moreMask = 0;
			
			uint32	*pInLeavesList = (uint32*)pInLeaves;
			uint32	*pPortalLeavesList = (uint32*)pPortal->m_VisibleLeaves.GetArray();
			uint32	*pVisibleLeavesList = (uint32*)visibleLeaves.GetArray();
			uint32	*pSeqLeavesList = (uint32*)pSeq->m_VisibleLeaves.GetArray();

			for(i=0; i < g_pBuilder->m_LeafListDWordSize; i++)
			{
				pVisibleLeavesList[i] =  pInLeavesList[i] & pPortalLeavesList[i];
				
				// See if there's anything new in these leaves.
				moreMask |= (pVisibleLeavesList[i] & ~pSeqLeavesList[i]);
			}
		}

		// Stop if all the potentially visible leaves have been marked.
		if(moreMask == 0)
			continue;


		// Chop the polies up.
		{
			CopyHullPolyDirect(pPortal, lookAts.Cur());
			CopyHullPolyDirect(pLookFrom, lookFroms.Cur());

			if( !GetFrontSide(pLookFrom->GetPlane(), lookAts) )
				continue;

			if( pLookFrom != pLookThru )
			{
				// Chop the pLookAt poly on pLookFrom and pLookThru.
				if( !GetFrontSide(pLookFrom->GetPlane(), lookAts) )
					continue;

				if( !GetFrontSide(pLookThru->GetPlane(), lookAts) )
					continue;


				// Make the reverse plane of pLookAt.
				plane.m_Normal = -lookAts.Cur()->Normal();
				plane.m_Dist = -lookAts.Cur()->Dist();

				// Clip pLookFrom on that plane.
				if( !GetFrontSide(&plane, lookFroms) )
					continue;

				// Minimize the visible portions of pLookAt.
				if( !GetVisiblePortions(lookFroms.Cur(), pLookThru, lookAts, false) )
					continue;

				// Minimize the visible portions of pLookFrom.
				if( !GetVisiblePortions(lookAts.Cur(), pLookThru, lookFroms, true) )
					continue;
			}

			lookAts.Cur()->m_pLeaf = pPortal->m_pLeaf;
		}

		// Ok .. recurse through.
		RecurseThruPortals(
			pSeq, 
			lookFroms.Cur(), 
			lookAts.Cur(), 
			visibleLeaves.GetArray(),
			true);
	}


	// Mark this leaf as untouched.
	ClearLeafTouch( pSeq, pLeaf );

	--pSeq->m_CurRecurseData;
}
	
	

// ----------------------------------------------------------------------- //
//      ROUTINE:   MakePortalVisList
//      PURPOSE:   Finds out what is visible from the portal.
// ----------------------------------------------------------------------- //

static void MakePortalVisList(CPreLeaf *pLeaf, CHullPoly *pPortal)
{
	CPortalSequence seq;


	// Don't do this stuff if it's only fast approx.	
	if(!(pLeaf->m_Flags & LEAF_FASTAPPROX))
	{
		// Setup the initial sequence.	
		seq.m_LeafTouch.SetSize(g_pBuilder->m_LeafListSize);
		memset(seq.m_LeafTouch.GetArray(), 0, g_pBuilder->m_LeafListSize);
		
		// Setup the final visible leaves list.
		seq.m_VisibleLeaves.SetSize(g_pBuilder->m_LeafListSize);
		memset(seq.m_VisibleLeaves.GetArray(), 0, g_pBuilder->m_LeafListSize);
		seq.m_pLeaf = pLeaf;
		seq.m_CurRecurseData = 0;
		
		// Recurse in..
		RecurseThruPortals(
			&seq, 
			pPortal, 
			pPortal, 
			pPortal->m_VisibleLeaves.GetArray(),
			false);

		// Set the portal's new list.
		memcpy(pPortal->m_VisibleLeaves.GetArray(), seq.m_VisibleLeaves.GetArray(), g_pBuilder->m_LeafListSize);
	}
}


static inline void EncodeBit(CMoByteArray &out, uint32 &curBit, bool bSet)
{
	if((curBit>>3) >= out.GetSize())
		out.Append(0);

	if(bSet)
	{
		out[curBit>>3] |= (1 << (curBit&7));
	}

	++curBit;
}


static void EncodePVSData(CNode *pRoot, CMoByteArray &out, uint32 &curBit)
{
	CNode *pBack, *pFront;

	if(!IsValidNode(pRoot) || !(pRoot->m_Flags & NODEFLAG_VISIBLE))
		return;

	// If there's only one way we can possibly go, we don't need to encode anything.
	// We do encode a bit if this node has a leaf.
	if(pRoot->m_pLeaf)
	{
		EncodeBit(out, curBit, !!(pRoot->m_pLeaf->m_Flags & LEAF_VISITED));
	}
		
	pBack = pRoot->m_Sides[BackSide];
	pFront = pRoot->m_Sides[FrontSide];

	// Only encode if there's a node there..
	if(IsValidNode(pBack))
	{
		EncodeBit(out, curBit, !!(pBack->m_Flags & NODEFLAG_VISIBLE));
	}
	EncodePVSData(pBack, out, curBit);

	if(IsValidNode(pFront))
	{
		EncodeBit(out, curBit, !!(pFront->m_Flags & NODEFLAG_VISIBLE));
	}
	EncodePVSData(pFront, out, curBit);
}


static void CompressBits(CMoByteArray &in, CMoByteArray &out, uint32 numBits)
{
	uint32 i, bit, byte;
	uint32 curState, testState, stateCount;
	
	out.SetCacheSize( 100 );

	curState = 0;
	stateCount = 0;

	for( i=0; i < numBits; i++ )
	{
		byte = i >> 3;
		bit = i & 7;

		// Did it switch states?
		testState = !!(in[byte] & (1 << bit));
		if( testState != curState )
		{
			out.Append((uint8)stateCount);
			stateCount = 0;
			curState = !curState;
		}

		// Make sure it doesn't exceed a byte.
		if( stateCount == 255 )
		{
			out.Append(255);
			out.Append(0);
			stateCount = 0;
		}

		// One more in this state...
		++stateCount;
	}

	if(stateCount > 0)
	{
		out.Append((uint8)stateCount);
	}
}


static void ClearNodeFlagVisible(CNode *pNode)
{
	if(!IsValidNode(pNode))
		return;

	pNode->m_Flags &= ~NODEFLAG_VISIBLE;
	if (pNode->m_Sides[FrontSide])
		ClearNodeFlagVisible(pNode->m_Sides[FrontSide]);
	if (pNode->m_Sides[BackSide])
		ClearNodeFlagVisible(pNode->m_Sides[BackSide]);
}


static void CompressVisList(CMoByteArray &visList, CMoByteArray &out, uint32 maxLeaves)
{
	uint32 i, curBit;
	CNode *pCurNode;

	
	// Clear the flags.
	ClearNodeFlagVisible(g_pBuilder->m_pWorld->m_RootNode);

	// Tag the used nodes and leaves.
	for(i=0; i < g_pBuilder->m_pWorld->m_Leafs; i++)
		g_pBuilder->m_pWorld->m_Leafs[i]->m_Flags &= ~LEAF_VISITED;

	for(i=0; i < maxLeaves; i++)
	{
		if(visList[i>>3] & (1 << (i&7)))
		{
			g_pBuilder->m_pWorld->m_Leafs[i]->m_Flags |= LEAF_VISITED;
			
			pCurNode = g_pBuilder->m_pWorld->m_Leafs[i]->m_pNode;
			ASSERT(pCurNode);
			while(pCurNode)
			{
				pCurNode->m_Flags |= NODEFLAG_VISIBLE;
				pCurNode = pCurNode->m_pParentNode;
			}
		}
	}

	// Encode.
	curBit = 0;
	EncodePVSData(g_pBuilder->m_pWorld->m_RootNode, out, curBit);

	// This method compresses to about half size.. might be useful at some point.
	//CMoByteArray temp;
	//curBit = 0;
	//EncodePVSData(g_pBuilder->m_pWorld->m_FastRootNode, temp, curBit);
	//CompressBits(temp, out, curBit);
}


CMoByteArray *g_VerifyData;
uint32 g_VerifyBit;
CMoArray<CPreLeaf*> g_TestLeaves;

static inline uint8 ReadTraverseBit()
{
	uint8 ret;
	ret = g_VerifyData->Get(g_VerifyBit>>3) & (1 << (g_VerifyBit&7));
	++g_VerifyBit;
	return ret;
}


void RecurseAndVerify(CNode *pNode)
{
	ASSERT(pNode->m_Flags & NODEFLAG_VISIBLE);

	if(pNode->m_pLeaf)
	{
		if(ReadTraverseBit())
		{
			ASSERT(pNode->m_pLeaf->m_Flags & LEAF_VISITED);
			g_TestLeaves.Append(pNode->m_pLeaf);
		}
	}

	if(IsValidNode(pNode->m_Sides[BackSide]))
	{
		if(ReadTraverseBit())
		{
			RecurseAndVerify(pNode->m_Sides[BackSide]);
		}
	}
	
	if(IsValidNode(pNode->m_Sides[FrontSide]))
	{
		if(ReadTraverseBit())
		{
			RecurseAndVerify(pNode->m_Sides[FrontSide]);
		}
	}
}


void VerifyVisList(CPreWorld *pWorld, CNode *pRoot, CMoByteArray &data)
{
	uint32 i;
	CPreLeaf *pLeaf;

	g_VerifyData = &data;
	g_VerifyBit = 0;
	g_TestLeaves.SetSize(0);
	g_TestLeaves.SetCacheSize(100);

	RecurseAndVerify(pRoot);

	for(i=0; i < pWorld->m_Leafs; i++)
	{
		pLeaf = pWorld->m_Leafs[i];

		if(pLeaf->m_Flags & LEAF_VISITED)
		{
			ASSERT(g_TestLeaves.FindElement(pLeaf) != BAD_INDEX);
		}
		else
		{
			ASSERT(g_TestLeaves.FindElement(pLeaf) == BAD_INDEX);
		}
	}
}


// ----------------------------------------------------------------------- //
// Gets all the portals we might be able to see thru with the leaves in
// the specified visibility list.
// ----------------------------------------------------------------------- //
static void GetVisListPortals(CPreWorld *pWorld, CMoByteArray &visList, CMoArray<PrePortal*> &portals)
{
	uint32 i, j, k, iCurLeaf;
	CPreLeaf *pLeaf;
	CHullPoly *pHullPoly;
	PrePortal *pPortal;

	iCurLeaf = 0;
	for(i=0; i < visList; i++)
	{
		for(j=0; j < 8; j++)
		{
			if(visList[i] & (1 << j))
			{
				pLeaf = pWorld->m_Leafs[iCurLeaf];

				for(k=0; k < pLeaf->m_VLPortals; k++)
				{
					pHullPoly = pLeaf->m_VLPortals[k];
				
					if(pHullPoly->m_pNode)
					{
						pPortal = pHullPoly->m_pNode->m_pPoly->m_pSurface->m_pPortal;
						if(pPortal)
						{
							if(portals.FindElement(pPortal) == BAD_INDEX)
							{
								portals.Append(pPortal);
							}
						}
					}
				}
			}
			
			++iCurLeaf;
		}
	}
}


// ----------------------------------------------------------------------- //
// Builds a visible leaf list into 'out' by flowing thru the visible leaves
// in 'in'.  It will only travel thru the portal specified.
// ----------------------------------------------------------------------- //
void FlowThruLeaves(CMoByteArray &visitedLeaves, CMoByteArray &in, CMoByteArray &out, uint32 iPortal, CPreLeaf *pLeaf)
{
	uint32 i;
	CHullPoly *pHullPoly;
	CPreLeaf *pTestLeaf;
	PrePortal *pPortal;

	// Don't visit a leaf twice.
	if(visitedLeaves[NUM_BYTE(pLeaf->m_Index)] & (1 << NUM_BIT(pLeaf->m_Index)))
		return;

	visitedLeaves[NUM_BYTE(pLeaf->m_Index)] |= (1 << NUM_BIT(pLeaf->m_Index));
	out[NUM_BYTE(pLeaf->m_Index)] |= (1 << NUM_BIT(pLeaf->m_Index));

	for(i=0; i < pLeaf->m_VLPortals; i++)
	{
		pHullPoly = pLeaf->m_VLPortals[i];
		pTestLeaf = pHullPoly->m_pLeaf;

		// Should we even try?
		if(in[NUM_BYTE(pTestLeaf->m_Index)] & (1 << NUM_BIT(pTestLeaf->m_Index)))
		{
			// Is this a user portal?
			if(pHullPoly->m_pNode && (pPortal = pHullPoly->m_pNode->m_pPoly->m_pSurface->m_pPortal))
			{
				// Only go thru it if it's the one they specified.
				if(pPortal->m_Index == iPortal)
				{
					FlowThruLeaves(visitedLeaves, in, out, iPortal, pHullPoly->m_pLeaf);				
				}
			}
			else
			{
				// Nope.. just go thru it.
				FlowThruLeaves(visitedLeaves, in, out, iPortal, pHullPoly->m_pLeaf);				
			}
		}
	}
}


static CHullPoly* FindPortalPoly(CMoByteArray &visitedLeaves, CMoByteArray &in, CPreLeaf *pLeaf, 
									uint32 iPortal, bool bGoThruPortals)
{
	CHullPoly *pHullPoly, *pTest;
	PrePortal *pPortal;
	uint32 i;

	// Don't visit a leaf twice.
	if(visitedLeaves[NUM_BYTE(pLeaf->m_Index)] & (1 << NUM_BIT(pLeaf->m_Index)) ||
		!(in[NUM_BYTE(pLeaf->m_Index)] & (1 << NUM_BIT(pLeaf->m_Index))))
		return NULL;
	visitedLeaves[NUM_BYTE(pLeaf->m_Index)] |= (1 << NUM_BIT(pLeaf->m_Index));

	for(i=0; i < pLeaf->m_VLPortals; i++)
	{
		pHullPoly = pLeaf->m_VLPortals[i];

		// Is this a user portal?
		if(pHullPoly->m_pNode && (pPortal = pHullPoly->m_pNode->m_pPoly->m_pSurface->m_pPortal))
		{
			if(pPortal->m_Index == iPortal)
			{
				// Ok we found the one they want.
				return pHullPoly;
			}
			else
			{
				if(!bGoThruPortals)
					continue;
			}
		}

		// Nope.. just go thru it.
		if(pHullPoly->m_pLeaf)
		{
			pTest = FindPortalPoly(visitedLeaves, in, pHullPoly->m_pLeaf, iPortal, bGoThruPortals);
			if(pTest)
				return pTest;
		}
	}

	return NULL;
}


// ----------------------------------------------------------------------- //
// Removes anything in list2 from list1.  (Result goes in list1).  The
// lists must be the same size.
// ----------------------------------------------------------------------- //
static void SubtractFromList(CMoByteArray &list1, CMoByteArray &list2)
{
	uint32 i, j;

	ASSERT(list1.GetSize() == list2.GetSize());

	for(i=0; i < list2; i++)
	{
		for(j=0; j < 8; j++)
		{
			if(list2[i] & (1 << j))
			{
				list1[i] &= ~(1 << j);
			}
		}
	}
}


// ----------------------------------------------------------------------- //
// Returns TRUE if there are any visible leaves in the list.
// ----------------------------------------------------------------------- //
static bool AnyVisibleLeaves(CMoByteArray &theList)
{
	for(uint32 i=0; i < theList; i++)
		if(theList[i] != 0)
			return true;
	
	return false;
}


// ----------------------------------------------------------------------- //
// Are the two VisLists the same?
// ----------------------------------------------------------------------- //
static bool MatchLists(VisList *pList1, VisList *pList2)
{
	PreLeafList *pLeafList1, *pLeafList2;
	uint32 i, j;

	if(pList1->m_LeafLists.GetSize() != pList2->m_LeafLists.GetSize())
		return FALSE;

	for(i=0; i < pList1->m_LeafLists; i++)
	{
		pLeafList1 = pList1->m_LeafLists[i];

		for(j=0; j < pList2->m_LeafLists; j++)
		{
			pLeafList2 = pList2->m_LeafLists[j];
			
			if(pLeafList1->m_iPortal == pLeafList2->m_iPortal)
			{
				if(pLeafList1->m_LeafList.GetSize() == pLeafList2->m_LeafList.GetSize())
				{
					if(memcmp(pLeafList1->m_LeafList.GetArray(), pLeafList2->m_LeafList.GetArray(), pLeafList1->m_LeafList.GetSize()) == 0)
					{
						break;
					}
				}
			}
		}

		// If we didn't find a match, return..
		if(j == pList2->m_LeafLists)
			return FALSE;
	}

	return TRUE;
}


// ----------------------------------------------------------------------- //
// Looks for a leaf with the exact same PVS.
// ----------------------------------------------------------------------- //
static CPreLeaf* FindMatchingList(CPreWorld *pWorld, VisList *pList, uint32 iEndLeaf)
{
	uint32 i;
	CPreLeaf *pLeaf;

	for(i=0; i < iEndLeaf; i++)
	{
		pLeaf = pWorld->m_Leafs[i];
	
		if(MatchLists(&pLeaf->m_VisList, pList))
			return pLeaf;
	}

	return NULL;
}


// ----------------------------------------------------------------------- //
// Given the (uncompressed) vislist, it figures out how far the leaf can see.
// ----------------------------------------------------------------------- //
void CalcLeafVisDist(CPreWorld *pWorld, CPreLeaf *pMainLeaf, CMoByteArray &visList)
{
	uint32		i;
	PReal		testDist, maxDist;
	CPreLeaf	*pLeaf;

	maxDist = 0.0f;
	for(i=0; i < pWorld->m_Leafs; i++)
	{
		pLeaf = pWorld->m_Leafs[i];

		if(IsLeafVisible(visList, pLeaf))
		{
			testDist = (pLeaf->m_Center - pMainLeaf->m_Center).Mag() + pLeaf->m_Radius;
			maxDist = LTMAX(testDist, maxDist);
		}
	}

	pMainLeaf->m_MaxVisDist = maxDist + pMainLeaf->m_Radius;
}


// ----------------------------------------------------------------------- //
// Collects the visibility lists for portals into the
// visibility lists for leaves.
// ----------------------------------------------------------------------- //
static void CollectLeafLists()
{
	uint32					iLeaf, iPortal, i;
	CPreLeaf				*pLeaf, *pDuplicateLeaf;
	CHullPoly				*pPortal, *pHullPoly;
	CMoByteArray			visList, certainList, extraList, visitedLeaves;
	PreLeafList				*pLeafList;
	CMoArray<PrePortal*>	portals;
	PVector					center;


	// Make sure CPreLeaf::m_Center and m_Radius are valid.
	g_pBuilder->m_pWorld->CalcLeafExtents();
	g_pBuilder->m_pWorld->m_RootNode->SetParenthood();


	visList.SetSize(g_pBuilder->m_LeafListSize);
	certainList.SetSize(g_pBuilder->m_LeafListSize);
	extraList.SetSize(g_pBuilder->m_LeafListSize);
	visitedLeaves.SetSize(g_pBuilder->m_LeafListSize);
	
	g_pBuilder->m_pWorld->SetLeafIndices();

	for( iLeaf=g_pBuilder->m_iFirst; iLeaf < g_pBuilder->m_iLast; iLeaf++)
	{
		pLeaf = g_pBuilder->m_pWorld->m_Leafs[iLeaf];
		
		// Create the visibility list.
		memset( visList.GetArray(), 0, g_pBuilder->m_LeafListSize );

		// Merge the lists from all the leaf's portals.
		for( iPortal=0; iPortal < pLeaf->m_VLPortals; iPortal++ )
		{
			pPortal = pLeaf->m_VLPortals[iPortal];
			
			for(i=0; i < g_pBuilder->m_LeafListSize; i++)
				visList[i] |= pPortal->m_VisibleLeaves[i];
		}

		// Set the leaf visible to itself..
		SetLeafVisible(visList, pLeaf, TRUE);


		// Figure out how far it can see (assuming all of its portals are open)..
		CalcLeafVisDist(g_pBuilder->m_pWorld, pLeaf, visList);


		// Create the 'certain' leaf list (what we can see if all the portals are closed).
		pLeafList = new PreLeafList;
		pLeaf->m_VisList.m_LeafLists.Append(pLeafList);

		memset(certainList.GetArray(), 0, g_pBuilder->m_LeafListSize);
		memset(visitedLeaves.GetArray(), 0, g_pBuilder->m_LeafListSize);
		FlowThruLeaves(visitedLeaves, visList, certainList, 0xFFFFFFFF, pLeaf);
		pLeafList->m_iPortal = 0xFFFFFFFF;
		
		CompressVisList(certainList, pLeafList->m_LeafList, g_pBuilder->m_pWorld->m_Leafs.GetSize());
		VerifyVisList(g_pBuilder->m_pWorld, g_pBuilder->m_pWorld->m_RootNode, pLeafList->m_LeafList);

		// Get a list of portals we might be able to see thru.
		GetVisListPortals(g_pBuilder->m_pWorld, visList, portals);

		
		// Create an 'extra' list for each portal.
		for(i=0; i < portals; i++)
		{
			memset(extraList.GetArray(), 0, extraList.GetSize());

			ASSERT(visitedLeaves.GetSize()==certainList.GetSize() && certainList.GetSize()==g_pBuilder->m_LeafListSize);
			//memcpy(visitedLeaves.GetArray(), certainList.GetArray(), g_pBuilder->m_LeafListSize);


			//Old way, can't handle looking through multiple portals.
			//FlowThruLeaves(visitedLeaves, visList, extraList, portals[i]->m_Index, pLeaf);

			// Figure out which side of the portal pLeaf can be reached through.  If both sides,
			// then don't add this portal.  If one side, then extraList is what we can reach
			// through the portal.
			
			// Find a hull poly on pLeaf's side for this poly (doesn't matter which one, we're
			// only interested in the leaf on the other side).
			// It tries it the first time without going thru portals in case you can get the portal
			// from both sides.  If it can't find it that way (ie: you see this portal thru another
			// portal), then it just lets it flow thru portals.
			memset(visitedLeaves.GetArray(), 0, g_pBuilder->m_LeafListSize);
			pHullPoly = FindPortalPoly(visitedLeaves, visList, pLeaf, portals[i]->m_Index, false);
			if(!pHullPoly)
			{
				memset(visitedLeaves.GetArray(), 0, g_pBuilder->m_LeafListSize);
				pHullPoly = FindPortalPoly(visitedLeaves, visList, pLeaf, portals[i]->m_Index, true);
			}

			// Make sure it faces the leaf.  Sometimes we'll get the wrong side in poorly
			// optimized levels.
			/*
			if(pHullPoly)
			{
				center = pLeaf->m_Center;
				if(DIST_TO_PLANE(center, *pHullPoly->GetPlane()) > 0.0f)
				{
					if(pHullPoly->m_pAdjacent && pHullPoly->m_pAdjacent->m_pLeaf)
					{
						pHullPoly = pHullPoly->m_pAdjacent;
					}
				}
			}
			*/			
			
			// Tag all the leaves on the other side.
			if(pHullPoly && pHullPoly->m_pLeaf)
			{
				ASSERT(pHullPoly->m_pNode->m_pPoly->m_pSurface->m_pPortal == portals[i]);
				memset(visitedLeaves.GetArray(), 0, g_pBuilder->m_LeafListSize);
				FlowThruLeaves(visitedLeaves, visList, extraList, 0xFFFFFFFF, pHullPoly->m_pLeaf);
			}
			
			SubtractFromList(extraList, certainList); // Get rid of the ones in the certain list.

			// Encode it if there are any visible.
			if(AnyVisibleLeaves(extraList))
			{
				pLeafList = new PreLeafList;
				pLeaf->m_VisList.m_LeafLists.Append(pLeafList);

				pLeafList->m_iPortal = portals[i]->m_Index;
				CompressVisList(extraList, pLeafList->m_LeafList, g_pBuilder->m_pWorld->m_Leafs.GetSize());
				VerifyVisList(g_pBuilder->m_pWorld, g_pBuilder->m_pWorld->m_RootNode, pLeafList->m_LeafList);
			}
		}

		if(iLeaf > 0)
		{
			if(pDuplicateLeaf = FindMatchingList(g_pBuilder->m_pWorld, &pLeaf->m_VisList, iLeaf-1))
			{
				pLeaf->m_VisList.m_iDuplicateLeaf = pDuplicateLeaf->m_Index;
				DeleteAndClearArray(pLeaf->m_VisList.m_LeafLists);
			}
		}
	}
}


static bool QuickCanSeeThru(CHullPoly *pLookFrom, CHullPoly *pLookThru)
{
	uint32			i, j;
	CPrePlane		*pPlane;


	// If pLookThru has nothing on the frontside of pLookFrom, then you can't see thru it.
	pPlane = pLookFrom->GetPlane();
	for( i=0; i < pLookThru->NumVerts(); i++ )
	{
		if( (pPlane->Normal().Dot(pLookThru->Pt(i)) - pPlane->Dist()) > POINT_SIDE_EPSILON )
		{
			// If pLookThru has nothing on the backside of pLookFrom, then you can't see thru it.
			pPlane = pLookThru->GetPlane();
			for( j = 0; j < pLookFrom->NumVerts(); j++ )
			{
				if( (pPlane->Normal().Dot(pLookFrom->Pt(j)) - pPlane->Dist()) < -POINT_SIDE_EPSILON )
					return true;
			}

			return false;
		}
	}
	
	return false;
}


static void QuickPortalVis(CPreLeaf *pLeaf, CHullPoly *pFrom, PReal curDist)
{
	CHullPoly		*pPortal;
	uint32			bit = NUM_BIT( pLeaf->m_Index );
	uint32			byte = NUM_BYTE( pLeaf->m_Index );
	uint32			i;
	bool			bVisible;
	PReal			dist;


	// Has it already visited here?
	if(g_pBuilder->m_pCurLeafList[byte] & (1 << bit))
		return;

	//see if we want to do distance based visibility culling
	if(g_pBuilder->m_bDistanceCull)
	{
		//we are doing distance culling, lets see if we have crossed
		//that distance
		if(curDist >= g_pBuilder->m_CullDistance)
		{
			return;
		}
	}

	// Mark this leaf possibly visible.
	g_pBuilder->m_pCurLeafList[byte] |= 1 << bit;

	// Go through all its portals.
	for( i=0; i < pLeaf->m_VLPortals; i++ )
	{
		pPortal = pLeaf->m_VLPortals[i];
	
		dist = (pPortal->m_CenterPoint - pFrom->m_CenterPoint).MagSqr();
		
		bVisible = QuickCanSeeThru(g_pCurrentPortal, pPortal);
		
		if(bVisible)
			QuickPortalVis(pPortal->m_pLeaf, pPortal, curDist+dist);
	}
}


static void SetupPortalLeafList( CHullPoly *pPortal )
{
	// Setup the initial list.
	pPortal->m_VisibleLeaves.SetSize(g_pBuilder->m_LeafListSize);
	memset( pPortal->m_VisibleLeaves.GetArray(), 0, g_pBuilder->m_LeafListSize );

	memset(g_pBuilder->m_PortalsVisible.GetArray(), 0, g_pBuilder->m_PortalListSize);

	g_pCurrentPortal = pPortal;
	
	// Flow through the portal graph and mark visible leaves.
	g_pBuilder->m_pCurLeafList = pPortal->m_VisibleLeaves.GetArray();
	QuickPortalVis(pPortal->m_pLeaf, pPortal, 0.0f);
}


// ----------------------------------------------------------------------- //
//      ROUTINE:   SetupPortalLeafLists
//      PURPOSE:   Sets up potential visibility lists in each portal.
// ----------------------------------------------------------------------- //
static void SetupPortalLeafLists()
{
	uint32		iLeaf, iPortal;
	CPreLeaf	*pLeaf;
	CPreWorld	*pWorld = g_pBuilder->m_pWorld;
	CHullPoly	*pPortal;

	
	g_pBuilder->m_PortalsVisible.SetSize(g_pBuilder->m_PortalListSize);

	for( iLeaf=0; iLeaf < pWorld->m_Leafs.GetSize(); iLeaf++ )
	{
		pLeaf = pWorld->m_Leafs[iLeaf];

		for( iPortal=0; iPortal < pLeaf->m_VLPortals; iPortal++ )
		{
			pPortal = pLeaf->m_VLPortals[iPortal];
			SetupPortalLeafList(pPortal);
		}

		if(g_pBuilder->m_FastCallback)
			g_pBuilder->m_FastCallback( (CReal)iLeaf / pWorld->m_Leafs.GetSize() );
	}

	g_pBuilder->m_PortalsVisible.Term();
}


// ----------------------------------------------------------------------- //
//      ROUTINE:   CVisListBuilder::PrecalculatePortals
//      PURPOSE:   Precalculates portals for the given nodes.
// ----------------------------------------------------------------------- //

static void PrecalculatePortals()
{
	CPreLeaf				*pLeaf;
	CHullPoly				*pHullPoly, *pPortal;

	CHullPolySwapper		polies;
	uint32					iLeaf, curPortal, i, j;

	char					data1[ HULLPOLY_SIZE(100) ], data2[ HULLPOLY_SIZE(100) ];
	CPreWorld				*pWorld = g_pBuilder->m_pWorld;



	g_pBuilder->m_nPortals = 0;
	polies.Init( (CHullPoly*)data1, (CHullPoly*)data2 );


	// Fill in the leaf indices.
	g_pBuilder->m_pWorld->SetLeafIndices();
	
	
	// Init the precalculated portals in the leaves.
	for( iLeaf=0; iLeaf < pWorld->m_Leafs.GetSize(); iLeaf++ )
	{
		pLeaf = pWorld->m_Leafs[iLeaf];

		pLeaf->m_VLPortals.SetSize(0);
		
		for( i=0; i < pLeaf->m_HullPolies; i++ )
		{
			pHullPoly = pLeaf->m_HullPolies[i];

			if( pHullPoly->m_pAdjacent && pHullPoly->m_pAdjacent->m_pAdjacent == pHullPoly )
			{
				pLeaf->m_VLPortals.Append( pHullPoly->m_pAdjacent );
				g_pBuilder->m_nPortals++;
			}
		}
	}


	// Setup the internal list of portals (this is done after the loop above so it 
	// doesn't have to repeatedly append to the array).
	g_pBuilder->m_Portals.SetSize(g_pBuilder->m_nPortals);
	
	curPortal = 0;
	for( iLeaf=0; iLeaf < pWorld->m_Leafs.GetSize(); iLeaf++ )
	{
		pLeaf = pWorld->m_Leafs[iLeaf];

		for( i=0; i < pLeaf->m_VLPortals; i++ )
		{
			g_pBuilder->m_Portals[curPortal] = pLeaf->m_VLPortals[i];
			curPortal++;
		}
	}

	ASSERT(curPortal == g_pBuilder->m_nPortals);


	// Get the portal centerpoints.
	for(i=0; i < g_pBuilder->m_Portals; i++)
	{
		pPortal = g_pBuilder->m_Portals[i];

		pPortal->m_CenterPoint.Init(0, 0, 0);
		for(j=0; j < pPortal->NumVerts(); j++)
		{
			pPortal->m_CenterPoint += pPortal->Pt(j);
		}

		pPortal->m_CenterPoint /= (PReal)pPortal->NumVerts();
	}

	g_pBuilder->m_PortalListSize = (g_pBuilder->m_nPortals / 8) + 1;
}



// ----------------------------------------------------------------------- //
//      ROUTINE:   CVisListBuilder::MakeVisList
//      PURPOSE:   THE function you call to make the visibility list.
// ----------------------------------------------------------------------- //

static void IncrementLeavesDone()
{
	thd_EnterCriticalSection(g_IncrementCS);
		++g_pBuilder->m_nLeavesDone;

		if(g_pBuilder->m_Callback)
			g_pBuilder->m_Callback( (float)g_pBuilder->m_nLeavesDone / g_pBuilder->m_nLeavesToDo );
	thd_LeaveCriticalSection(g_IncrementCS);
}

// Gets the next leaf to process.  Returns -1 if there are no more.
static int32 GetNextLeafJob()
{
	int32 ret;

	thd_EnterCriticalSection(g_NextLeafCS);
		if(g_pBuilder->m_CurrentLeaf == g_pBuilder->m_iLast)
		{
			ret = -1;
		}
		else
		{
			ret = (int32)g_pBuilder->m_CurrentLeaf;
			++g_pBuilder->m_CurrentLeaf;
		}
	thd_LeaveCriticalSection(g_NextLeafCS);

	return ret;
}

static void DoThreadJob(void *pData)
{
	int iLeaf;
	uint32 iPortal;
	CPreLeaf *pLeaf;

	while(1)
	{
		iLeaf = GetNextLeafJob();
		if(iLeaf == -1)
		{
			++g_pBuilder->m_nThreadsDone;
			return;
		}

		pLeaf = g_pBuilder->m_pWorld->m_Leafs[iLeaf];

		// Do all portals in this leaf.
		for(iPortal=0; iPortal < pLeaf->m_VLPortals; iPortal++)
			MakePortalVisList(pLeaf, pLeaf->m_VLPortals[iPortal]);

		IncrementLeavesDone();
	}
}


static void FlowAndMarkLeaves(CPreLeaf *pLeaf)
{
	uint32 i, flags;
	CHullPoly *pPoly;

	if(pLeaf->m_Flags & LEAF_FASTAPPROX)
		return;

	pLeaf->m_Flags |= LEAF_FASTAPPROX;
	++g_nLeavesTagged;

	for(i=0; i < pLeaf->m_VLPortals; i++)
	{
		pPoly = pLeaf->m_VLPortals[i];

		// Don't flow through hull makers.
		if(pPoly->m_pNode && pPoly->m_pNode->m_pPoly && 
			pPoly->m_pNode->m_pPoly->m_pSurface)
		{
			flags = pPoly->m_pNode->m_pPoly->m_pSurface->m_Flags;

			if(!(flags & SURF_HULLMAKER))
			{
				if(pPoly->m_pLeaf)
				{
					ASSERT(pPoly->m_pLeaf != pLeaf);
					FlowAndMarkLeaves(pPoly->m_pLeaf);
				}
			}
		}
	}
}


static void MarkFastApproxLeaves(CPreWorld *pWorld)
{
	uint32 i;
	CPreLeaf *pLeaf;
	PVector pt;
	CBaseEditObj *pObj;


	g_nLeavesTagged = 0;
	for(i=0; i < pWorld->m_Leafs; i++)
		pWorld->m_Leafs[i]->m_Flags &= ~LEAF_FASTAPPROX;

	for(i=0; i < pWorld->m_pMainWorld->m_Objects; i++)
	{
		pObj = pWorld->m_pMainWorld->m_Objects[i];

		if(strcmp(pObj->GetClassName(), "FastApproxArea") == 0)
		{
			pt = pObj->GetPos();
			if(pLeaf = FindPointLeaf(pWorld, pt, pWorld->m_RootNode))
			{
				FlowAndMarkLeaves(pLeaf);
			}
		}
	}

	DrawStatusText(eST_Normal, "Marked %d out of %d leaves as FastApprox", 
		g_nLeavesTagged, pWorld->m_Leafs.GetSize());
}


VLBStatus MakeVisList(CVisListBuilder *pBuilder)
{
	uint32		i;
	THREAD_ID	threadIDs[MAX_THREADS];


	// Validate stuff..
	pBuilder->m_nThreads = CLAMP(pBuilder->m_nThreads, 1, MAX_THREADS);
	if( (pBuilder->m_iFirst == 0) && (pBuilder->m_iLast == 0) )
		pBuilder->m_iLast = pBuilder->m_pWorld->m_Leafs.GetSize();

	g_IncrementCS = thd_CreateCriticalSection();
	g_NextLeafCS = thd_CreateCriticalSection();

	g_pBuilder = pBuilder;

	//keep track of counts of the work progress
	g_pBuilder->m_nLeavesDone = 0;
	g_pBuilder->m_nLeavesToDo = pBuilder->m_iLast - pBuilder->m_iFirst;
	
	pBuilder->m_pWorld->SetPolyIndices();
	
	//set up the size of the size of the leaf lists in both bytes and dwords
	g_pBuilder->m_LeafListSize			= (pBuilder->m_pWorld->m_Leafs.GetSize() / 8) + 1;
	g_pBuilder->m_LeafListSize		   += 4 - (g_pBuilder->m_LeafListSize%4);
	g_pBuilder->m_LeafListDWordSize		= g_pBuilder->m_LeafListSize / 4;

	// Setup the initial visible approximation for each portal.  We must do this
	// for every portal in the world since the results are used in the full vis.
	PrecalculatePortals();
	SetupPortalLeafLists();

	if(!g_pBuilder->m_bFast)
	{
		// Mark up the leaves so we know which ones are fast approx only.
		MarkFastApproxLeaves(pBuilder->m_pWorld);
		
		g_pBuilder->m_CurrentLeaf = pBuilder->m_iFirst;
		g_pBuilder->m_nThreadsDone = 0;

		// Start all the threads.
		if(g_pBuilder->m_nThreads == 1)
		{
			// Special case if there's only one thread.  We run in the same thread as our caller
			// so we pickup its thread priority.
			DoThreadJob(NULL);
		}
		else
		{
			for(i=0; i < g_pBuilder->m_nThreads; i++)
			{
				threadIDs[i] = thd_BeginThread(DoThreadJob, NULL);
			}
		}

		// Wait till they're done.
		//thd_WaitForMultipleToFinish(threadIDs, pBuilder->m_nThreads);
		// Loop around, broadcasting the finished leaves until we're done.
		while(1)
		{
			if(g_pBuilder->m_nThreadsDone == g_pBuilder->m_nThreads)
				break;

			thd_Sleep(100);
		}
	}

	// Collect everything into the leaves' visibility lists.
	CollectLeafLists();

	// Get rid of the portals.
	if(g_pBuilder->m_bCleanupPortals)
	{
		for( i=0; i < pBuilder->m_pWorld->m_Leafs; i++ )
			pBuilder->m_pWorld->m_Leafs[i]->m_VLPortals.SetSize(0);
	}

	thd_DeleteCriticalSection(g_IncrementCS);
	thd_DeleteCriticalSection(g_NextLeafCS);
	g_IncrementCS = NULL;

	return VLB_Ok;
}




