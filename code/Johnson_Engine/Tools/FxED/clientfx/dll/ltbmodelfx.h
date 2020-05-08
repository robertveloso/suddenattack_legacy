//------------------------------------------------------------------
//
//   MODULE  : LTBMODELFX.H
//
//   PURPOSE : Defines class CLTBModelFX
//
//   CREATED : On 12/3/98 At 6:34:45 PM
//
//------------------------------------------------------------------

#ifndef __LTBMODELFX__H_
	#define __LTBMODELFX__H_

	// Includes....

	#include "basefx.h"

	class CLTBModelFX : public CBaseFX
	{
		public :

			// Constuctor

											CLTBModelFX();

			// Destructor

										   ~CLTBModelFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps);
			bool							Update(float tmCur);
			void							Term();

			bool							IsFinished() { return true; }

			// Accessors

		protected :

			// Member Variables

			char							m_sModelName[128];
			char							m_sSkinName[128];
			LTVector							m_vFacing;
			bool							m_bFinished;
			bool							m_bShuttingDown;
			bool							m_bEnvironmentMap;
			bool							m_bNoBackfaceCulling;
			float							m_tmShutdown;
			float							m_fBaseAlpha;
			bool							m_bSmoothFade;
			bool							m_bFadingOut;
			float							m_tmFade;
			float							m_tmFadeStart;
			bool							m_bShadow;
	};

#endif