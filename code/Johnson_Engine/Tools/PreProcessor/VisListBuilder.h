//------------------------------------------------------------------
//
//	FILE	  : VisListBuilder.h
//
//	PURPOSE	  : Defines the CVisListBuilder class .. this class
//              actually builds the whole visibility list.
//
//	CREATED	  : 2nd May 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __VISLISTBUILDER_H__
	#define __VISLISTBUILDER_H__


	// Includes....
	#include "bdefs.h"
	#include "editobjects.h"

	// Defines....
	class CPreLeaf;
	class CHullPoly;
	class CPreWorld;

	typedef void (*VisCallback)( CReal percent );
	
	enum VLBStatus
	{
		VLB_Ok,
		VLB_Error,
		VLB_OutOfMemory
	};

	
	class CZoneInfo
	{
	public:

		CBaseEditObj	*m_pObject;

		PVector			m_BoundMin;
		PVector			m_BoundMax;

		CMoArray<CPreLeaf*>	m_Leaves; // Leaves in this zone.
	};


	class CVisListBuilder
	{
		public:

			// Specifies whether or not distance based visibility culling is enabled
			BOOL					m_bDistanceCull;

			//if distance culling is enabled, this value holds the distance to
			//cull at, otherwise the value is undefined
			PReal					m_CullDistance;

			// Fill this stuff in when calling MakeVisList.
			uint32					m_CurrentLeaf;
			uint32					m_iFirst, m_iLast;
			BOOL					m_bFast;		// Fast vis?
			VisCallback				m_Callback;
			VisCallback				m_FastCallback; // Callback while doing fast approximation.
			BOOL					m_bCleanupPortals; // Cleanup portal lists?
			
			CPreWorld				*m_pWorld;
			uint32					m_nThreads;


			uint32					m_nThreadsDone;
			uint32					m_nLeavesDone;
			uint32					m_nLeavesToDo;
			uint32					m_nPortals;


			// Private member variables
			
			CMoArray<CHullPoly*>	m_Portals;
			
			uint32					m_LeafListSize;
			uint32					m_LeafListDWordSize;
			uint32					m_PortalListSize;


			// Used during the quick approximation.
			uint8					*m_pCurLeafList;
			CMoByteArray			m_PortalsVisible;

			CMoArray<CZoneInfo*>		m_Zones;
	};


	VLBStatus MakeVisList(CVisListBuilder *pBuilder);


#endif







