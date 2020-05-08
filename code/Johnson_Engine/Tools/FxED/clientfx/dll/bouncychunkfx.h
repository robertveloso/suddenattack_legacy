//------------------------------------------------------------------
//
//   MODULE  : BOUNCYCHUNKFX.H
//
//   PURPOSE : Defines class CBouncyChunkFX
//
//   CREATED : On 12/3/98 At 6:34:45 PM
//
//------------------------------------------------------------------

#ifndef __BOUNCYCHUNKFX__H_
	#define __BOUNCYCHUNKFX__H_

	// Includes....

	#include "basefx.h"

	class CBouncyChunkFX : public CBaseFX
	{
		public :

			// Constuctor

											CBouncyChunkFX();

			// Destructor

										   ~CBouncyChunkFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps);
			bool							Update(float tmCur);
			void							Term();

			// Accessors

		protected :

			// Member Variables

			HOBJECT							m_hBouncyChunk;
			
			char							m_sModelName[128];
			char							m_sSkinName[128];
			
			bool							m_bPlayImpactSound;
			char							m_sImpactSound[128];
			HLTSOUND						m_hImpactSound;

			LTVector						m_vChunkDir;
			float							m_fChunkSpeed;
			float							m_fChunkSpread;
			
			LTVector						m_vVel;
			float							m_fMultiplier;

			LTVector						m_vGravity;
			float							m_fGravityAmount;
	};

#endif