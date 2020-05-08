//------------------------------------------------------------------
//
//   MODULE  : DYNALIGHTFX.H
//
//   PURPOSE : Defines class CDynaLightFX
//
//   CREATED : On 12/14/98 At 5:43:43 PM
//
//------------------------------------------------------------------

#ifndef __DYNALIGHTFX__H_
	#define __DYNALIGHTFX__H_

	// Includes....

	#include "basefx.h"

	class CDynaLightFX : public CBaseFX
	{
		public :

			// Constuctor

											CDynaLightFX();

			// Destructor

										   ~CDynaLightFX();
			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pData, CLinkList<FX_PROP> *pList);
			void							Term();
			bool							Update(float tmCur);

			// Accessors

		protected :

			// Member Variables

			bool							m_bFlicker;
	};

#endif