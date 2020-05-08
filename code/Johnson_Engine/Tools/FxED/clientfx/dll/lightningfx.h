//------------------------------------------------------------------
//
//   MODULE  : LIGHTNINGFX.H
//
//   PURPOSE : Defines class CLightningFX
//
//   CREATED : On 10/12/98 At 5:07:15 PM
//
//------------------------------------------------------------------

#ifndef __LIGHTNINGFX__H_
	#define __LIGHTNINGFX__H_

	// Includes....

	#include "screentrail.h"

	// Defines....

	#define LT_SINGLE							0
	#define LT_MEGAZAP							1
	#define LT_BALL								2

	struct LIGHTNING
	{
											LIGHTNING()
											{
												memset(this, 0, sizeof(LIGHTNING));
											}

		bool								m_bActive;
		LTVector							m_vNorm;
		LTVector							m_vTarget;
		float								m_tmLifespan;
		float								m_tmStart;
	};

	class CLightningFX : public CBaseFX
	{
		public :

			// Constuctor

											CLightningFX();

			// Destructor

										   ~CLightningFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pData, CLinkList<FX_PROP> *pList);
			bool							Update(float tmCur);
			void							OnRendererShutdown();
			void							Term();

			void							DrawLightning(LTVector vFrom, LTVector vTo, float tmStart, float tmLifespan);

			// Accessors

		protected :

			// Member Variables

			int								m_nType;
			LTVector						m_vTarget;
			float							m_fRadius;
			float							m_fWidth;
			float							m_fRandomness;

			CLinkList<TRAIL_SECTION>		m_collLightning;

			LIGHTNING					   *m_pLightning;
			uint32							m_nLightning;
			float							m_tmLastEmission;
			float							m_tmLightningEmission;
			float							m_tmLightningLifespan;
			LTVector					   *m_pBreaks;
			uint32							m_dwBreaks;		

			LTVector					   *m_pTmpBuf;

			float							m_fTrailWidth;
			float							m_fPerturb;
			int								m_nFlash;
			LTVector						m_vDir;
			char							m_sTextureName[128];

			HTEXTURE						m_hTexture;
			bool							m_bLoadFailed;
	};

#endif