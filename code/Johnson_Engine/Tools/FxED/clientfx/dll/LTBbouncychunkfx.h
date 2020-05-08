//------------------------------------------------------------------
//
//   MODULE  : LTBBOUNCYCHUNKFX.H
//
//   PURPOSE : Defines class CLTBBouncyChunkFX
//
//   CREATED : On 12/3/98 At 6:34:45 PM
//
//------------------------------------------------------------------

#ifndef __LTBBOUNCYCHUNKFX__H_
	#define __LTBBOUNCYCHUNKFX__H_

	// Includes....

	#include "basefx.h"

	class CLTBBouncyChunkFX : public CBaseFX
	{
		public :

			// Constuctor

											CLTBBouncyChunkFX();

			// Destructor

										   ~CLTBBouncyChunkFX();

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