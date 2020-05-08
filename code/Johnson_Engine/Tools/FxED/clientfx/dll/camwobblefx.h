//------------------------------------------------------------------
//
//   MODULE  : CAMWOBBLEFX.H
//
//   PURPOSE : Defines class CCamWobbleFX
//
//   CREATED : On 12/30/98 At 3:28:28 PM
//
//------------------------------------------------------------------

#ifndef __CAMWOBBLEFX__H_
	#define __CAMWOBBLEFX__H_

	// Includes....

	#include "basefx.h"

	class CCamWobbleFX : public CBaseFX
	{
		public :

			// Constuctor

											CCamWobbleFX();

			// Destructor

										   ~CCamWobbleFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pData, CLinkList<FX_PROP> *pList);
			void							Term();
			bool							PreUpdate(float tmCur);

			// Accessors

		protected :

			// Member Variables
			
			float							m_fPeriod;
			uint32							m_dwType;
			float							m_xFovAnchor;
			float							m_yFovAnchor;
			float							m_xMultiplier;
			float							m_yMultiplier;
			float							m_tmStarted;
	};

#endif