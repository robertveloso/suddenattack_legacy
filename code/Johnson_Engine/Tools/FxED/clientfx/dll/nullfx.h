//------------------------------------------------------------------
//
//   MODULE  : NULLFX.H
//
//   PURPOSE : Defines class NullFX
//
//   CREATED : On 12/3/98 At 6:34:45 PM
//
//------------------------------------------------------------------

#ifndef __NULLFX__H_
	#define __NULLFX__H_

	// Includes....

	#include "basefx.h"

	class CNullFX : public CBaseFX
	{
		public :

			// Constuctor

											CNullFX();

			// Destructor

										   ~CNullFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps);
			bool							Update(float tmCur);
			void							Term();

			// Accessors

		protected :
	};

#endif