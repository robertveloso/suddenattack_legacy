//------------------------------------------------------------------
//
//   MODULE  : CAMJITTERFX.H
//
//   PURPOSE : Defines class CCamJitterFX
//
//   CREATED : On 12/30/98 At 3:28:28 PM
//
//------------------------------------------------------------------

#ifndef __CAMJITTERFX__H_
	#define __CAMJITTERFX__H_

	// Includes....

	#include "basefx.h"

	class CCamJitterFX : public CBaseFX
	{
		public :

			// Constuctor

											CCamJitterFX();

			// Destructor

										   ~CCamJitterFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pData, CLinkList<FX_PROP> *pList);
			void							Term();
			bool							PreUpdate(float tmCur);

			// Accessors

		protected :

			// Member Variables
	};

#endif