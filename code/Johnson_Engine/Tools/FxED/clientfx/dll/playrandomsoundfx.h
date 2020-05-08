//------------------------------------------------------------------
//
//   MODULE  : PLAYRANDOMSOUNDFX.H
//
//   PURPOSE : Defines class CPlayRandomSoundFX
//
//   CREATED : On 12/15/98 At 5:06:02 PM
//
//------------------------------------------------------------------

#ifndef __PLAYRANDOMSOUNDFX__H_
	#define __PLAYRANDOMSOUNDFX__H_

	// Includes....

	#include "basefx.h"

	class CPlayRandomSoundFX : public CBaseFX
	{
		public :

			// Constuctor

											CPlayRandomSoundFX();

			// Destructor

										   ~CPlayRandomSoundFX();
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
			int								m_nRand;
	};

#endif