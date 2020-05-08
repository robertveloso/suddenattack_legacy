//------------------------------------------------------------------
//
//   MODULE  : FALLINGSTUFFFX.H
//
//   PURPOSE : Defines class CFallingStuffFX
//
//   CREATED : On 10/26/98 At 4:00:00 PM
//
//------------------------------------------------------------------

#ifndef __FALLINGSTUFFFX__H_
	#define __FALLINGSTUFFFX__H_

	// Includes....

	#include "basefx.h"
	#include "linklist.h"
	#include "spritesystemfx.h"

	// Structures....

	struct FALLING_THING
	{
		float								m_tmStart;
		LTVector								m_vLastPos;
		LTVector								m_vPos;
		LTVector								m_vVel;
		HLOCALOBJ							m_hObject;
		bool								m_bSplash;
	};

	struct SPLASH 
	{
		HOBJECT								m_hObject;
		float								m_tmStart;
		float								m_scale;
	};

	// Defines....

//	#define SS_POINT						0
//	#define SS_PLANEUP						1
//	#define SS_SPHERE						2
	#define FS_PLANEUP						3
//	#define SS_CONE							4

	#define FSPT_NONE						0
	#define FSPT_SINE						1
	#define FSPT_PENDULUM					2

	// Classes....

	class CFallingStuffFX : public CBaseFX
	{
		public :

			// Constuctor

											CFallingStuffFX();

			// Destructor

										   ~CFallingStuffFX();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pData, CLinkList<FX_PROP> *pList);
			void							Term();
			bool							Update(float tmCur);
			bool							IsFinished() { return (m_collSprites.GetSize() == 0) ? true : false; }

			void							Unfreeze(float tmUnfreeze)
											{
												if (m_bFrozen)
												{
													float tmFreezeDelta = tmUnfreeze - m_tmFreeze;
													m_tmLastEmission += tmFreezeDelta;
												}

												// Call through to base class

												CBaseFX::Unfreeze(tmUnfreeze);
											}

			// Accessors

		protected :

			// Member Variables

			char							m_sSpriteName[128];
			CLinkList<FALLING_THING *>		m_collSprites;
			uint32							m_nFallingStuffFXEmission;
			float							m_tmLastEmission;
			float							m_tmFallingStuffFXEmission;
			float							m_tmSpriteLifespan;
			int								m_nEmissionType;
			float							m_fRadius;
			LTVector						m_vPlaneDir;
			LTVector						m_vUp;
			LTVector						m_vRight;
			float							m_fVel;
			LTVector						m_vWind;
			float							m_fWindAmount;
			LTMatrix						m_mSpin;
			bool							m_bUseSpin;
			float							m_xRot;
			float							m_yRot;
			float							m_zRot;
			float							m_fStretchMul;
			char							m_sImpactSpriteName[128];
			CLinkList<SPLASH *>				m_collSplashes;
			float							m_tmImpactLifespan;
			float							m_fImpactScale1;
			float							m_fImpactScale2;
			int								m_nImpactCreate;
			int								m_nImpactPerturbType;
	};

#endif