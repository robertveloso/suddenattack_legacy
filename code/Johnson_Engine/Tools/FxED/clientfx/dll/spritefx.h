//------------------------------------------------------------------
//
//   MODULE  : SPRITEFX.H
//
//   PURPOSE : Defines class CSpriteFX
//
//   CREATED : On 11/23/98 At 6:21:38 PM
//
//------------------------------------------------------------------

#ifndef __SpriteFX__H_
	#define __SpriteFX__H_

	// Includes....

	#include "basefx.h"

	class CSpriteFX : public CBaseFX
	{
		public :

			// Constuctor

											CSpriteFX();

			// Destructor

										   ~CSpriteFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps);
			bool							Update(float tmCur);
			void							Term();

			// Accessors

		protected :

			// Member Variables

			char							m_sSpriteName[128];
			LTVector						m_vRot;
			bool							m_bAlongNormal;
			bool							m_bParentRotate;
			float							m_fYaw;
	};

#endif