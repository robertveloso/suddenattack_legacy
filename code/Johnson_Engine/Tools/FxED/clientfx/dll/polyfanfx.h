//------------------------------------------------------------------
//
//   MODULE  : POLYFANFX.H
//
//   PURPOSE : Defines class CPolyFanFX
//
//   CREATED : On 11/23/98 At 6:21:38 PM
//
//------------------------------------------------------------------

#ifndef __POLYFANFX__H_
	#define __POLYFANFX__H_

	// Includes....

	#include "basefx.h"

	class CPolyFanFX : public CBaseFX
	{
		public :

			// Constuctor

											CPolyFanFX();

			// Destructor

										   ~CPolyFanFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps);
			bool							Update(float tmCur);
			void							Term();

			// Accessors

		protected :

			// Member Variables

			char							m_sPolyFanName[128];
			LTVector							m_vRot;
			int								m_bAlongNormal;
			bool							m_bParentRotate;
			float							m_fYaw;
	};

#endif