//------------------------------------------------------------------
//
//	FILE	  : HullPoly.h
//
//	PURPOSE	  : Defines the CHullPoly class.
//
//	CREATED	  : September 3 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __HULLPOLY_H__
	#define __HULLPOLY_H__


	// Includes....
	#include "bdefs.h"

	#ifndef __SWAPPER_H__
	#	include "swapper.h"
	#endif

	#ifndef __PREPLANE_H__
	#	include "preplane.h"
	#endif

	#ifndef __PREBASEPOLY_H__
	#	include "prebasepoly.h"
	#endif


	// Defines....
	class CPreLeaf;
	class CNode;
	class CPrePoly;
	

	#define CHullPolyList		CMultiLinkList<CHullPoly*>
	#define CHullPolyArray		CMoArray<CHullPoly*>
	#define CHullPolySwapper	CSwapper<CHullPoly*>


	// Used for terrain occluders. Means you can't see through this portal (you may
	// be able to see through its other side though)!
	#define HULLPOLY_CANTSEETHRUPORTAL	(1<<0)


	class CHullPoly 
	{
		public:

								CHullPoly()
								{
									m_pAdjacent = NULL;
									m_pLeaf = NULL;
									m_pNode = NULL;
									m_OuterShellID = (WORD)-1;
									m_Flags = 0;
								}

								~CHullPoly()
								{
									if(m_pAdjacent)
									{
										m_pAdjacent->m_pAdjacent = NULL;
									}
								}

			static bool				MakeCubeHull( CHullPolyArray &polies, PVector &min, PVector &max );
			void					Flip();

			CPrePlane*				GetPlane()		{ return &m_Plane; }
			
			PVector&				Normal()		{ return m_Plane.m_Normal; }
			PReal&					Dist()			{ return m_Plane.m_Dist; }
			
			void CopySplitAttributes(CHullPoly *pPoly)
			{
				m_pNode = pPoly->m_pNode;
				m_Plane = pPoly->m_Plane;
				m_OuterShellID = pPoly->m_OuterShellID;
				m_Flags = pPoly->m_Flags;
				m_pAdjacent = pPoly->m_pAdjacent;
			}

		public:

			// Combination of HULLPOLY_ flags.
			uint32					m_Flags;

			CMLLNode				m_LLNode;

			CNode					*m_pNode;  // The node from whence it came.

			// Is this one of the hull polies that makes up the outer shell (the
			// box initially put around the level in BspHullMaker)? If this is
			// -1, then the hull poly is not an outer shell poly.
			uint16					m_OuterShellID;
			
			CPrePlane				m_Plane;
			CPreLeaf				*m_pLeaf;

			// The hull poly adjacent to this one.
			// If this is set, it's a portal poly.
			CHullPoly				*m_pAdjacent;

			PVector					m_CenterPoint;

			// Used in the visibility list builder .. which leaves this portal can possibly see.
			CMoByteArray			m_VisibleLeaves;

			//this must come last
			BASEPOLY_MEMBER()
	};


	CHullPoly*	CopyHullPoly( CHullPoly *pPoly );
	void		CopyHullPolyDirect( CHullPoly *pSrc, CHullPoly *pDest );

	bool		GetFrontSide( CPrePlane *pSplitter, CHullPolySwapper &polies );


	#define HULLPOLY_SIZE(nVerts)	(sizeof(CHullPoly)+(sizeof(CPolyVert)*((nVerts)-3)))


#endif  // __HULLPOLY_H__

			
