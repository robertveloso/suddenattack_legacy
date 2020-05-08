//------------------------------------------------------------------
//
//	FILE	  : PreLeaf.h
//
//	PURPOSE	  : Defines the CPreLeaf class.  This contains all the 
//              information for a leaf node in the BSP tree.
//
//	CREATED	  : September 3 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __PRELEAF_H__
	#define __PRELEAF_H__

	
	// Includes....
	#include "bdefs.h"

	// Defines....
	class CPrePoly;
	class CHullPoly;
	class CNode;

	
	// Leaf flags.
	#define LEAF_VISIBLE		1	// Used by Unseen.
	#define LEAF_VISITED		2	// Used by LeakFinders and VLB.
	#define LEAF_FASTAPPROX		4	// Used by VisListBuilder.


	class CTempTouchInfo
	{
	public:
		uint16	m_WorldIndex;
		uint16	m_PolyIndex;
	};


	class PreLeafList
	{
		public:

			CMoByteArray	m_LeafList;	// (Compressed) list of leaves.
			uint32			m_iPortal;

	};

	class VisList
	{
		public:

							VisList()
							{
								m_iDuplicateLeaf = 0xFFFF;
							}

							~VisList()
							{
								DeleteAndClearArray(m_LeafLists);
							}

			// The first one is the 'certain' list (always there).  The rest are ones that you can
			// only see if a portal is open.
			CMoArray<PreLeafList*>	m_LeafLists;

			// If the leaf contains its own PVS, this is 0xFFFF, otherwise, it's the index of a leaf 
			// whose PVS it's using.
			uint16					m_iDuplicateLeaf;

	};


	class CPreLeaf
	{
		public:

									CPreLeaf();
									~CPreLeaf();

		
		public:

			// All the portals for this leaf.
			// Precalculated at the start of the visibility list builder.
			// This is just a list of m_HullPolies[X]->m_pAdjacent.
			CMoArray<CHullPoly*>		m_VLPortals;

			
			// The hull for this node.
			CMoArray<CHullPoly*>		m_HullPolies;

			// All the regular polies touching this node's hull.
			CLinkedList<CPrePoly*>		m_Touching;

			// Only used while loading the file.
			CMoArray<CTempTouchInfo>	m_TempTouchInfo;

			// All the other nodes visible from this one.
			// One bit for each node.
			VisList						m_VisList;

			CNode						*m_pNode;

			// Flags on this leaf.
			uint16						m_Flags;

			// This leaf's index into the world's arrays.
			uint16						m_Index;

			// Boundaries of the touching polies.  These should be regenerated
			// with CPreWorld::CalcLeafExtents before they're used.
			PVector						m_Center;
			PReal						m_Radius;

			// How far its vislist extends (used in the heirarchical BSP culling).
			// Initialized to 0.  
			PReal						m_MaxVisDist;
	};


#endif


