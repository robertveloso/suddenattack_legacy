//------------------------------------------------------------------
//
//   MODULE  : POLYTUBEFX.H
//
//   PURPOSE : Defines class CPolyTubeFX
//
//   CREATED : On 3/22/99 At 4:00:54 PM
//
//------------------------------------------------------------------

#ifndef __POLYTUBEFX_H_
	#define __POLYTUBEFX_H_

	// Includes....

	#include "screentrail.h"
	#include "clientfx.h"

	struct PT_TRAIL_SECTION
	{
		LTVector			m_vPos;
		LTVector			m_vTran;
		LTVector			m_vBisector;
		uint8				m_red;
		uint8				m_blue;
		uint8				m_green;
		uint8				m_alpha;
		float				m_fScale;
		float				m_uVal;
		float				m_tmStart;
	};

	class CPolyTubeFX : public CBaseFX
	{
		public :

			// Constuctor

											CPolyTubeFX();

			// Destructor

										   ~CPolyTubeFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps);
			bool							Update(float tmCur);
			void							Term();

			void							OnRendererShutdown();

			bool							IsFinished() { return (m_collPathPts.GetSize() == 0) ? true : false; }

			void							CreateNewSegment(LTVector vPos);

			// Accessors

		protected :

			CLinkList<PT_TRAIL_SECTION>		m_collPathPts;
			float							m_tmLastAddPt;
			float							m_tmAddPtInterval;
			float							m_tmSectionLifespan;
			uint32							m_nMaxTrailLength;
			HTEXTURE						m_hTexture;			
			uint32							m_dwWidth;
			uint32							m_dwHeight;
			float							m_fTrailWidth;
			float							m_uOffset;
			float							m_uAdd;
			char							m_sPath[128];
			bool							m_bLoadFailed;
	};

#endif