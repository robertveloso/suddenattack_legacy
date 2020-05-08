//------------------------------------------------------------------
//
//   MODULE  : PLAYSOUNDFX.H
//
//   PURPOSE : Defines class CPlaySoundFX
//
//   CREATED : On 12/15/98 At 5:06:02 PM
//
//------------------------------------------------------------------

#ifndef __PLAYSOUNDFX__H_
	#define __PLAYSOUNDFX__H_

	// Includes....

	#include "basefx.h"

	class CPlaySoundFX : public CBaseFX
	{
		public :

			// Constuctor

											CPlaySoundFX();

			// Destructor

										   ~CPlaySoundFX();
			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pData, CLinkList<FX_PROP> *pList);
			void							Term();
			bool							Update(float tmCur);

			// Accessors

		protected :

			// Member Variables

			char							m_sSoundName[128];
			char							m_sMultiplaySound[128];
			HLTSOUND						m_hSound;
			int								m_nPriority;
			float							m_fOuterRadius;
			float							m_fInnerRadius;
			bool							m_bLoop;
			bool							m_bPlayLocal;
			int								m_nVolume;
			float							m_fPitch;
			bool							m_bUseMarkers;
	};

#endif