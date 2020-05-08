//------------------------------------------------------------------
//
//	FILE	  : BspHullMaker.h
//
//	PURPOSE	  : Defines the CBspHullMaker class, which generates
//              the convex hulls for all the leaf nodes in the BSP tree.
//
//	CREATED	  : 2nd May 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __BSP_HULL_MAKER_H__
	#define __BSP_HULL_MAKER_H__


	// Includes....
	#include "bdefs.h"
	#include "geometry.h"
	#include "prepoly.h"


	// Defines....
	class CPreLeaf;
	class CEditPoly;

	
	enum BHMStatus
	{		
		BHM_Ok,
		BHM_InvalidInput,
		BHM_OutOfMemory
	};

	typedef void (*HullCallback)( CReal percentage );

	class CBspHullMaker
	{
	// Input to MakeHulls..
	public:

		HullCallback	m_Callback;
		uint32			m_nNodesVisited, m_nTotalVisits;

	
	// Output from MakeHulls.
	public:
		
		// MakeHulls fills this out with any polies that caused problems while splitting.
		CMoArray<CEditPoly*>	m_ProblemPolies;
	};

	BHMStatus MakeHulls(CBspHullMaker *pHullMaker, 
						CPreWorld *pWorld, 
						HullCallback callback);


#endif



